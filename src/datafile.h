/*
** $Id: datafile.h,v 1.3 2008-09-21 12:05:16 welle Exp $
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


#ifndef _DATAFILE_H_
#define _DATAFILE_H_ 1

#define DF_ERROR   -1
#define DF_FLOODDF  0
#define DF_RAWDF    1

#define MAXLINELENGTH  1024

#define FLOODDATAFILEMARK "# FlOoD dAtA fIlE"

#define PRF_TPL "tpl."
#define PRF_CFG "cfg."
#define PRF_CMT "#"


void strip_crlf( char *line );
int parse_cfg_parm( JOB_DATA *job_data, char *k, char *v );
int parse_tpl_parm( JOB_DATA *job_data, char *k, char *v );
int load_data_file( JOB_DATA *job_data );

#endif
