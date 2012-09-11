/*
** $Id: datafile.c,v 1.6 2008-09-21 14:15:23 welle Exp $
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
#include <string.h>

#include "flood.h"
#include "jobdata.h"
#include "hash.h"
#include "macros.h"
#include "datafile.h"



/*
** Strip crlf or lf from the end of a buffer.
*/
void
strip_crlf( char *line )
{
    int len;

    
    if ( line ) { // Should not need this, cause line is a static buffer

        len = strlen( line );

        if ( ( len > 2 )
             && ( line[len - 2] == '\r' && line[len - 1] == '\n' ) )
            line[len - 2] = '\0';
        else if ( ( len > 1 )
                  && line[len - 1] == '\n' )
            line[len - 1] = '\0';

    } /* if */

} /* strip_crlf */



/*
**
*/
int
parse_cfg_parm( JOB_DATA *job_data, char *k, char *v )
{
    int ret;


    LOG( "Detected config param: '%s' = '%s'\n", k, v );

    ret = -1;
    
    if ( strcmp( k, "template" ) == 0 ) {

        LOG( "Use template file (fdf): '%s'\n", v );

        job_data->templatefile = strdup( v );
        ret = 0;
        
    } else if ( strcmp( k, "destinationaddress" ) == 0 
                || strcmp( k, "dstaddr" ) == 0 ) {

        LOG( "Use destination address (fdf): '%s'\n", v );

        if ( !( job_data->parmsset & JD_DADDR ) )
            job_data->daddr = strdup( v );

    } else if ( strcmp( k, "destinationport" ) == 0
                || strcmp( k, "dstport" ) == 0 ) {

        LOG( "Use destination port (fdf): '%s'\n", v );

        if ( !( job_data->parmsset & JD_DPORT ) )
            job_data->dport = atoi( v );

    } else if ( strcmp( k, "sourceaddress" ) == 0
                || strcmp( k, "srcaddr" ) == 0 ) {

        LOG( "Use sourcre address (fdf): '%s'\n", v );

        if ( !( job_data->parmsset & JD_SADDR ) )
            job_data->saddr = strdup( v );

    } else if ( strcmp( k, "sourceport" ) == 0
                || strcmp( k, "srcport" ) == 0 ) {

        LOG( "Use source port (fdf): '%s'\n", v );

        if ( !( job_data->parmsset & JD_SPORT ) )
            job_data->sport = atoi( v );

    } else if ( strcmp( k, "broadcast" ) == 0
                || strcmp( k, "bcast" ) == 0 ) {

        LOG( "Use broadcast (fdf): '%s'\n", v );

        if ( !( job_data->parmsset & JD_BCAST ) )
            job_data->broadcast = atoi( v );

    } else if ( strcmp( k, "multicast" ) == 0
                || strcmp( k, "mcast" ) == 0 ) {

        LOG( "Use multicast (fdf): '%s'\n", v );

        if ( !( job_data->parmsset & JD_MCAST ) )
            job_data->mcast = atoi( v );

    } else if ( strcmp( k, "tcp" ) == 0 ) {

        LOG( "Use tcp (fdf): '%s'\n", v );

        if ( !( job_data->parmsset & JD_TCP ) )
            job_data->tcp = atoi( v );

    } else if ( strcmp( k, "delay" ) == 0 ) {

        LOG( "Use delay (fdf): '%s'\n", v );

        if ( !( job_data->parmsset & JD_DELAY ) )
            job_data->delay = atol( v );

    } else if ( strcmp( k, "packets" ) == 0 ) {

        LOG( "Number of packets to send (fdf): '%s'\n", v );

        if ( !( job_data->parmsset & JD_CNT ) )
            job_data->cnt = atol( v );

    } else if ( strcmp( k, "verbose" ) == 0 ) {

        LOG( "Be verbose (fdf): '%s'\n", v );

        if ( !( job_data->parmsset & JD_VERBOSE ) )
            job_data->verbose = atoi( v );

    } else {
        
        ERR( "Unknown config parameter: %s\n", k );
        
    } /* if */

    return ret;

} /* parse_cfg_parm */



/*
**
*/
int
parse_tpl_parm( JOB_DATA *job_data, char *k, char *v )
{    
    int ret;


    LOG( "Detected tpl param: '%s' = '%s'\n", &k[4], v );

    ret = 0;

    if ( set_value( &k[4], v ) < 0 ) {

        ERR( "Oops, there are more than %d variables.\n"
             "Some variables may be discarded\n",
             get_max_size() );

        ret = -1;

    } /* if */

    return ret;

} /* parse_tpl_parm */



/*
**
*/
int
load_data_file( JOB_DATA *job_data )
{
    const char *delim = "=";
    char line[MAXLINELENGTH];
    FILE *data;


    /*
    ** First we try to discover if we are dealing
    ** with a Flood data file or with a file containing
    ** raw payload.
    */
    data = fopen( job_data->datafile, "r" );
    if ( data == NULL ) {

        ERR( "Could not open data file '%s'\n", job_data->datafile );
        return DF_ERROR;

    } /* if */


    memset( line, 0, MAXLINELENGTH );

    /*
    ** First line in data file should contain a mark 
    ** indicating a Flood data file.
    */
    if ( fgets( line, sizeof( line ), data ) == NULL ) {

        if ( !feof( data ) )                                     
            ERR( "Error reading from data file '%s'\n", job_data->datafile );
        fclose( data );
        return DF_ERROR;   
        
    } /* if */

    strip_crlf( line );

    if ( strcmp( line, FLOODDATAFILEMARK ) == 0 ) {

        LOG( "Detected Flood data file\n" );


        /*
        ** Remainding lines should contain the data
        ** values for patching into the template file
        ** and configuration directives.
        */
        init_hash();
    
        while ( !feof ( data ) ) {
            char *k, *v;
        
            
            memset( line, 0, MAXLINELENGTH );

            if ( fgets( line, sizeof( line ), data ) == NULL ) {

                if ( !feof( data ) ) {

                    ERR( "Error reading from data file '%s'\n",
                         job_data->datafile );
                    fclose( data );
                    return DF_ERROR;   

                } /* if */

                break;

            } /* if */

            strip_crlf( line );

            k = strtok( line, delim );
            v = strtok( NULL, delim );

            if ( k && v ) {

                if ( strncmp( k, PRF_CFG, strlen( PRF_CFG ) ) == 0 ) {

                    parse_cfg_parm( job_data, &k[strlen( PRF_CFG )], v );

                } else if ( strncmp( k, PRF_TPL, strlen( PRF_TPL ) ) == 0 ) {

                    parse_tpl_parm( job_data, k, v );

                } else if ( strncmp( k, PRF_CMT, strlen( PRF_CMT ) ) == 0 ) {

                    LOG( "Detected comment\n" );

                } else {

                    /* Should not reach here... */
                    LOG( "Detected unknown directive: '%s' = '%s'\n", k, v );

                } /* if */

            } /* if: k && v */        
        
        } /* while */

        fclose( data );

        return DF_FLOODDF;

    } else {

        fclose( data );
        return DF_RAWDF;

    } /* if */

} /* load_data_file */
