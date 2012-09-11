/*
** $Id: flood.h,v 1.16 2008-09-21 14:14:56 welle Exp $
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


#ifndef _FLOOD_H_
#define _FLOOD_H_ 1

#include "jobdata.h"

#define APPNAME     "Flood"
#define APPVER      "v0.2.3"
#define APPNAMELONG APPNAME " " APPVER " - Your friendly network packet generator"

#define DEFAULTPAYLOADSIZE 128

#define ERR_EXIT       0
#define ERR_SOCK_CONT  1


#define _HAVE_MSG_NOSIGNAL_


/* 
** Definitions specific to plattforms
*/
#ifdef _SOLARIS_
#undef _HAVE_MSG_NOSIGNAL_
#endif


#ifndef _HAVE_MSG_NOSIGNAL_
#define MSG_NOSIGNAL 0
#endif


int main( int argc, char **argv );
long long flood( JOB_DATA job_data );
int create_client_socket( JOB_DATA job_data );
void sig_int_handler( int s );
void print_help();
void parse_cmd_line( int argc, char **argv, JOB_DATA *job_data );
int append_to_payload( JOB_DATA *job_data, unsigned char *field, int len );
int build_payload_from_template( JOB_DATA *job_data );
int create_payload( JOB_DATA *job_data );
int load_raw_payload_from_file( JOB_DATA *job_data );
int create_byte_filled_payload( JOB_DATA *job_data, unsigned char fill );

#endif
