/*
** $Id: flood.c,v 1.22 2008-09-21 14:14:36 welle Exp $
*/


/*
 * This file is part of flood.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with GNU Emacs; see the file COPYING.  If not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 *
 */


#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <signal.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <arpa/inet.h>

#include "macros.h"
#include "jobdata.h"
#include "hash.h"
#include "patcher.h"
#include "datafile.h"
#include "flood.h"


static int isFlooding = 0;

#define BLKSIZE          64
#define MAXFIELDLENGTH 1024



/*
** Abort flooding the network if hit by SIGINT
*/
void
sig_int_handler( int s )
{
   
    isFlooding = 0;
  
} /* sig_int_handler */



/*
** Parse cmd line and fill job_data structure
*/
void
parse_cmd_line( int argc, char **argv, JOB_DATA *job_data )
{
    char o;
           
           
    init_job_data( job_data );


    while ( ( o = getopt( argc, argv, "bp:P:a:A:d:c:C:s:l:rtmvh" ) ) != EOF ) {
 
        switch ( o ) {

        case 'b':
            job_data->broadcast = 1;
            job_data->parmsset |= JD_BCAST;
            break;

        case 'p':
            job_data->dport = atoi( optarg );
            job_data->parmsset |= JD_DPORT;
            break;

        case 'P':
            job_data->sport = atoi( optarg );
            job_data->parmsset |= JD_SPORT;
            break;

        case 'a':
            /* daddr is initially malloced in init_job_data() */
            FREE( job_data->daddr );
            job_data->daddr = strdup( optarg );
            job_data->parmsset |= JD_DADDR;
            break;

        case 'A':
            /* saddr is initially malloced in init_job_data() */
            FREE( job_data->saddr );
            job_data->saddr = strdup( optarg );
            job_data->parmsset |= JD_SADDR;
            break;

        case 'd':
            job_data->delay = 1000 * atol( optarg );
            job_data->parmsset |= JD_DELAY;
            break;

        case 'c':
            job_data->cnt = atol( optarg );
            job_data->parmsset |= JD_CNT;
            break;

        case 'C':
            job_data->concurrency = MAX(1, MIN(atol( optarg ), 500));
            job_data->parmsset |= JD_CONCURCY;
            break;

        case 's':
            job_data->payloadsize = atoi( optarg );
            job_data->parmsset |= JD_PAYLOADSIZE;
            break;

        case 't':
            job_data->tcp = 1;
            job_data->parmsset |= JD_TCP;
            break;

        case 'm':
            job_data->mcast = 1;
            job_data->parmsset |= JD_MCAST;
            break;

        case 'l':
            job_data->datafile = strdup( optarg );
            break;

        case 'r':
            job_data->reactonerr = ERR_SOCK_CONT;
            job_data->parmsset |= JD_REACTONERR;
            break;

        case 'v':
            job_data->verbose = 1;
            job_data->parmsset |= JD_VERBOSE;
            break;

        case 'h':
            print_help();
            break;

        default:
            ERR( "Unknown option: %c at optind %d\n", o, optind );
            goto parse_error;
            break;

        } /* switch */

    } /* while */

    return;

 parse_error:
    print_help();

} /* parse_cmd_line */



int
append_to_payload( JOB_DATA *job_data, unsigned char *field, int len )
{
    unsigned char *tmp;

    
    LOG( "realloc to %zd bytes\n", job_data->payloadsize + len );

    tmp = realloc( job_data->payload,
                   job_data->payloadsize + len );
            
    if ( tmp == NULL )
        return -1;
            
    job_data->payload = tmp;

    memcpy( &job_data->payload[job_data->payloadsize], field, len );
    job_data->payloadsize += len;

    return 0;
    
} /* append_to_payload */



int
build_payload_from_template( JOB_DATA *job_data )
{
    FILE *tpl;


    if ( !job_data->templatefile ) 
        return -1;


    tpl = fopen( job_data->templatefile, "r" );
    if ( tpl == NULL ) {

        ERR( "Could not open template file '%s'\n", job_data->templatefile );
        return -1;

    } /* if */

    
    while ( !feof ( tpl ) ) {
        const char *delim = "=";
        char line[1024];
        char *k, *v;


        if ( fgets( line, sizeof( line ), tpl ) == NULL ) {

            if ( !feof( tpl ) ) {

                ERR( "Error reading from data file '%s'\n",
                     job_data->datafile );
                
                fclose( tpl );
                return -1;   

            } /* if */

            break;

        } /* if */

        strip_crlf( line );

        k = strtok( line, delim );
        v = strtok( NULL, delim );

        if ( k && v ) {
            unsigned char patched[MAXFIELDLENGTH];
            int           len;


            len = patch_line( k, v, patched, MAXFIELDLENGTH );
 
            if (  len > 0 ) {

                append_to_payload( job_data, patched, len );

            } else {

                ERR( "Ooops patch error: Param: '%s' value to patch in: '%s'.\n", k, v );

            } /* if */

        } /* if */

    } /* while */

    fclose( tpl );

    return 0;

} /* build_payload_from_template */



/*
** Load raw payload from a given file
*/
int
load_raw_payload_from_file( JOB_DATA *job_data )
{
    struct stat buf;
    size_t remainder;
    size_t ret;
    size_t size;
    int fd;



    /*
    ** First let us discover the payload's size
    */
    ret = stat( job_data->datafile, &buf );
        
    if ( ret != 0 ) {
                         
        ERR( "Couldn't stat() payload file '%s'.\n", job_data->datafile );
        return -1;
        
    } /* if */
    
    size = buf.st_size;



    /*
    ** Malloc() the buffer
    */
    if ( NULL == ( job_data->payload = malloc( size ) ) ) {
        
        ERR( "Couldn't malloc() buffer for payload.\n" );
        return -1;
        
    } /* if */

      
    /*
    ** Now we can try to read in the payload.
    */
    fd = open( job_data->datafile, O_RDONLY );

    if ( fd < 0 ) {

        ERR( "Couldn't load payload from '%s'.\n", job_data->datafile );

        FREE( job_data->payload );
        return -1;

    } /* if */

    remainder = size;
    ret = 0;

    do {

        ret = read( fd, job_data->payload+ret, remainder );
        remainder -= ret;

    } while ( remainder > 0 || errno == EINTR );

    close( fd );

    /*
    ** Oops, we could not read in the entire file...
    */
    if ( remainder > 0 ) {

        ERR( "Problem reading payload file '%s'.\n", job_data->datafile );
        
        FREE( job_data->payload );
        return -1;

    } /* if */

    job_data->payloadsize = size;

    return 0;

} /* load_raw_payload_from_file */



/*
** Fill the payload with job_data.payloadsize (or DEFAULTPAYLOADSIZE)
** byte with value of fill
*/ 
int
create_byte_filled_payload( JOB_DATA *job_data, unsigned char fill )
{

    if ( job_data->payloadsize == 0 ) 
        job_data->payloadsize = DEFAULTPAYLOADSIZE;

    ERR( "Creating payload filled with %zd bytes containing '0x%x'.\n",
         job_data->payloadsize, fill );

    if ( NULL == ( job_data->payload = malloc( job_data->payloadsize ) ) ) {

        ERR( "Couldn't malloc() buffer for payload.\n" );

        return -1;

    } /* if */

    memset( job_data->payload, fill, job_data->payloadsize );

    return 0;

} /* create_byte_filled_payload */



/*
** If we have a payload file name, we try to load the payload
** from that file. The file may be a Flood data file or it may
** contain raw payload data.
** If that crashes or if we only have a payload size, we create
** a (\0) byte filled payload of that (or of  DEFAULTPAYLOADSIZE)
** size. If we have neither a payload file name nor a payload size,
** we use an empty payload.
*/
int
create_payload( JOB_DATA *job_data )
{
    int ret;


    /*
    ** We have a payload file name, so we try to
    ** suck our payload from that file.
    */
    if ( job_data->datafile ) {

        ret = load_data_file( job_data );

        switch ( ret ) {

        case DF_FLOODDF:
            ret = build_payload_from_template( job_data );
            break;

        case DF_RAWDF:
            ret = load_raw_payload_from_file( job_data );
            break;

        case DF_ERROR:
            ret = -1;
            break;

        default:
            break;

        } /* switch */


        /*
        ** Got it.
        */
        if ( ret != -1 )
            return ret;

    } /* if */

    /*
    ** No payload file or trouble reading it...
    */
    return create_byte_filled_payload( job_data, 0 );

} /* create_payload */



/*
** Here we go...
*/
long long
flood( JOB_DATA job_data )
{
    long long cnt;
    int       s_fd;
    size_t    ret;


    s_fd = create_client_socket( job_data );

    if ( s_fd < 0 ) {

        ERR( "Problem creating socket. Exit.\n" );
        exit( -1 );

    } /* if */

    cnt = 0;
    isFlooding = 1;

    do {

        ret = send( s_fd,
                    job_data.payload, job_data.payloadsize,
                    MSG_NOSIGNAL );
        
        if ( ret != job_data.payloadsize ) {

            ERR( "Problem sending packet no. %lld: %s.\n",
                 cnt + 1, strerror( errno ) );

            /*
            ** If the user has choosen to ignore socket errors
            ** we try to create a new client socket and continue.
            ** The packet counter isn't resettet.
            */
            if ( ( job_data.reactonerr & ERR_SOCK_CONT ) > 0 ) {

                ERR( "User decided to ignore error. "
                     "Create new socket and continue.\n" );

                close( s_fd );

                s_fd = create_client_socket( job_data );

                if ( s_fd < 0 ) {

                    ERR( "Problem creating socket. Exit.\n" );
                    exit( -1 );
        
                } /* if */

            } else {

                LOG( "User decided to exit on socket error. "
                     "Quit sending loop.\n" );

                isFlooding = 0;

            } /* if */

        } else {

            cnt++;

        } /* if */

        if ( job_data.delay > 0 )
            usleep((useconds_t) job_data.delay );
    
    } while ( isFlooding == 1
              && ( job_data.cnt == 0 || job_data.cnt > cnt ) );

    close( s_fd );

    return cnt;

} /* flood */

long long flood_forked(JOB_DATA job_data)
{
	long long cnt = 0;
	int child_pid = 0;
	int wait_pid = 0;
	size_t i;
	int status = -1;
	for (i = 0; i < job_data.concurrency; ++i) {
		if ((child_pid = fork()) == 0) {
			cnt = flood(job_data);
			printf( "%s[%d]: Sent %lld packets.\n", APPNAME, (int)getpid(), cnt );
			exit(EXIT_SUCCESS);
		} else if (child_pid < 0) {
			printf("failed to fork");
			break;
		}
		/* parent will keep forking */
	}
	
	while ((wait_pid = wait(&status)) > 0);
	return cnt;
} /* flood_forked */


int
create_client_socket( JOB_DATA job_data )
{
    struct hostent     *host_ent;
    struct sockaddr_in clt; 
    int                socktype;
    int                s_fd;
    int                yes  = 1;
    

    if ( NULL == ( host_ent = gethostbyname( job_data.daddr ) ) ) {

        ERR( "Problem getting destination host address for name '%s'.\n",
             job_data.daddr );
        return -1;

    } /* if */

    memset( ( void * )&clt, 0, sizeof( clt ) );
    clt.sin_family = AF_INET; 
    clt.sin_addr = *(struct in_addr*)host_ent->h_addr;
    clt.sin_port = htons( job_data.dport );


    socktype = ( job_data.tcp ? SOCK_STREAM : SOCK_DGRAM );

    if ( -1 == ( s_fd = socket( PF_INET, socktype, 0 ) )) {
            
        if ( job_data.tcp ) 
            ERR( "Problem creating connection based socket: %s.\n",
                 strerror( errno ) );
        else 
            ERR( "Problem creating datagramm based socket: %s.\n",
                 strerror( errno ) );

        return -1;
        
    } /* if */

    if ( job_data.broadcast ) {

        if ( -1 == ( setsockopt( s_fd, SOL_SOCKET,
                                 SO_BROADCAST, &yes, sizeof( yes ) ) ) ) {

            ERR( "Problem setting socket broadcast option: %s.\n",
                 strerror( errno ) );
            return -1;

        } /* if */

    } /* if */

    if ( -1 == ( connect( s_fd, (struct sockaddr*)&clt, sizeof( clt) ) ) ) {

        ERR( "Problem connecting to socket: %s.\n", strerror( errno ) );
        return -1;

    } /* if */

    return s_fd;

} /* create_client_socket */



void
print_help()
{

    printf( "\n" );
    printf( "%s\n\n", APPNAMELONG );
    printf( "usage: flood [-b] [-A] [-a] [-P] [-p] [-d] [-l] [-s] [-c] [-C] [-t] [-m] [-v] [-h]\n" );
    printf( "-b\tUse broadcast flag\n" );
    printf( "-a\tDestination address\n" );
    printf( "-A\tSource address (not implemented yet)\n" );
    printf( "-p\tDestination port\n" );
    printf( "-P\tSource port (not implemented yet)\n" );
    printf( "-d\tDelay between packets in ms\n" );
    printf( "-l\tLoad %s data file or raw data file\n", APPNAME );
    printf( "-s\tSize of payload\n" );
    printf( "-c\tNumber of packets to send\n" );
    printf( "-C\tConcurrency, Total Packets = values of -c * -C\n" );
    printf( "-t\tUse TCP instead of UDP\n" );
    printf( "-m\tUse multicast (not implemented yet)\n" );
    printf( "-r\tContinue after socket error\n" );
    printf( "-v\tBe verbose\n" );
    printf( "-h\tShow this help\n" );
    printf( "\n" );

    exit( -1 );

} /* print_help */



int
main( int argc, char **argval )
{
    JOB_DATA  job_data;
    long long cnt = 0;


    signal( SIGINT, sig_int_handler );
    
#ifndef _HAVE_MSG_NOSIGNAL_
    signal( SIGPIPE, SIG_IGN );
#endif

    parse_cmd_line( argc, argval, &job_data );

    create_payload( &job_data );

    if ( job_data.verbose ) {

        print_job_data( job_data );

    } /* if */

    cnt = flood_forked( job_data );
    //printf( "%s[%d]: Sent %lld packets.\n\n", APPNAME, (int)getpid(), cnt );

    free_job_data( &job_data );
    drop_hash();

    return cnt > 0;

} /* main */
