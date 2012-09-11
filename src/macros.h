/*
** $Id: macros.h,v 1.7 2008-09-14 14:18:03 welle Exp $
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


#ifndef _MACROS_H_
#define _MACROS_H_

#include <sys/types.h>
#include <unistd.h>

#ifndef MAX
#define MAX(A,B) ((A) < (B) ? B : A)
#endif
#ifndef MIN
#define MIN(A,B) ((A) < (B) ? A : B)
#endif

#ifdef DEBUG
#define LOG( format, ... ) fprintf( stderr, APPNAME"[%d]: "format, (int)getpid(), ##__VA_ARGS__ );
#else
#define LOG( format, ...)
#endif

#define ERR( format, ... ) fprintf( stderr, APPNAME"[%d]: "format, (int)getpid(), ##__VA_ARGS__ )

#define FREE(p) if ( p ) { free( p ); p = NULL; }

#endif

