/*
** $Id: jobdata.c,v 1.4 2008-09-21 14:14:12 welle Exp $
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

#include "macros.h"
#include "flood.h"
#include "jobdata.h"


/*
** Fill yet unset attributes of the job_data structure with
** more or less meaningful default values. This defaults allow
** flood to start sending packets, even if some necessary values
** are not given via cmd line or the flood data file.
*/
void
init_job_data( JOB_DATA *job_data )
{
    const char *bcast = "255.255.255.255";


    memset( job_data, 0, sizeof( *job_data ) );

    job_data->broadcast    = 0;
    job_data->sport        = 4242;
    job_data->dport        = 4242;
    job_data->delay        = 1000 * 1000;
    job_data->cnt          = 0;
    job_data->verbose      = 0;
    job_data->tcp          = 0;
    job_data->mcast        = 0;
    job_data->templatefile = '\0';
    job_data->datafile     = '\0';
    job_data->payloadsize  = 0;
    job_data->payload      = '\0';
    job_data->reactonerr   = ERR_EXIT;
    job_data->parmsset     = 0;

    job_data->saddr = malloc( strlen( bcast ) + 1 );
    strcpy( job_data->saddr, bcast );
    
    job_data->daddr = malloc( strlen( bcast ) + 1 );
    strcpy( job_data->daddr, bcast );

} /* init_job_data */



/*
**
*/
void
free_job_data( JOB_DATA *job_data )
{

    FREE( job_data->templatefile );
    FREE( job_data->datafile );
    FREE( job_data->payload );
    FREE( job_data->saddr );
    FREE( job_data->daddr );

} /* free_job_data */



/*
**
*/
void
print_job_data( JOB_DATA job_data )
{

    printf( "\n" );
    printf( "%s\n\n", APPNAMELONG );
    printf( "Use source address: <not implemented yet>\n" );
    printf( "Use source port: <not implemented yet>\n" );
    printf( "Use destination address: %s\n", job_data.daddr );
    printf( "Use destination port: %u\n", job_data.dport );
    
    if ( job_data.tcp )
        printf( "Use protocol: TCP\n" );
    else 
        printf( "Use protocol: UDP\n" );

    printf( "Use broadcast flag: %s\n",
            (job_data.broadcast ? "yes" : "no" ) );
    
    printf( "Use multicast: <not implemented yet>\n" );
    printf( "Delay between packets: %ld ms\n", job_data.delay / 1000 );
    
    if ( job_data.templatefile )
        printf( "Using template from file: %s\n",
                job_data.templatefile );

    if ( job_data.datafile )
        printf( "Loading payload from file: %s\n",
                job_data.datafile );

    printf( "Size of payload: %zd bytes\n", job_data.payloadsize );

    if ( job_data.cnt == 0 )
        printf( "Sending packets forever\n" );
    else
        printf( "Sending %lld packet%s",
                job_data.cnt, ( job_data.cnt == 1 ? "\n" : "s\n" ) );
        
    printf( "\n" );
    
} /* print_job_data */
