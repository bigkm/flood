/*
** $Id: jobdata.h,v 1.2 2008-09-21 12:05:16 welle Exp $
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


#ifndef _JOBDATA_H_
#define _JOBDATA_H_ 1


/*
** Flags, indicating which attribute ist set from the
** command line.
*/
#define JD_BCAST       1<<0
#define JD_SPORT       1<<1
#define JD_DPORT       1<<2
#define JD_SADDR       1<<3
#define JD_DADDR       1<<4
#define JD_DELAY       1<<5
#define JD_TCP         1<<6
#define JD_MCAST       1<<7
#define JD_VERBOSE     1<<8
#define JD_REACTONERR  1<<9
#define JD_CNT         1<<10
#define JD_PAYLOADSIZE 1<<11


typedef struct  {
        
    char          broadcast;
    unsigned      sport;
    unsigned      dport;
    char          *saddr;
    char          *daddr;
    long          delay;
    long long     cnt;
    char          tcp;
    char          mcast;
    char          verbose;
    char          *templatefile;
    char          *datafile;
    size_t        payloadsize;
    unsigned char *payload;
    long          reactonerr;
    long          parmsset;

} JOB_DATA;


void init_job_data( JOB_DATA *job_data );
void free_job_data( JOB_DATA *job_data );
void print_job_data( JOB_DATA job_data );

#endif
