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


#ifndef _PATCHER_H_
#define _PATCHER_H_ 1



typedef enum  {

    UNDEF     = 0,
    BYTE      = 1,
    BYTEFIELD = 2,
    HEX       = 3,
    HEXFIELD  = 4,

} PATCH_TYPE;


typedef int (*Patch)( char*, unsigned char* );

typedef struct {

    char  *cmd;
    Patch f;
    
} Patcher;


int patch_line( char *key, char *val,
                unsigned char *patched, unsigned int buflen );
int patch_value( char *key, char *val,
                 unsigned char *patched, unsigned int buflen );
int patch_byte( char *v, unsigned char *patched );
int patch_byte_field( char *v, unsigned char *patched );
int patch_hex( char *v, unsigned char *patched );
int patch_hex_field( char *v, unsigned char *patched );
int patch_string( char *v, unsigned char *patched );

#endif
