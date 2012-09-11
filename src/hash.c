/*
** $Id: hash.c,v 1.4 2008-09-07 19:54:05 welle Exp $
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


#include "hash.h"


typedef struct _hash {

    char *key;
    char *val;

} HASH;


HASH  hash[MAXHASHENTRIES];

static int cur = UNDEFINEDIDX;



void
init_hash()
{
    int i;


    for ( i = 0; i < MAXHASHENTRIES; i++ ) {

        hash[i].key = NULL;
        hash[i].val = NULL;

    } /* for */

} /* init_hash */



void
drop_hash()
{

    int i;


    for ( i = 0; i < MAXHASHENTRIES; i++ ) {

        if ( hash[i].key ) free( hash[i].key ), hash[i].key = NULL;
        if ( hash[i].val ) free( hash[i].val ), hash[i].val = NULL;

    } /* for */

} /* drop_hash */



void
dump_hash()
{
    int i;


    printf( "Dump known variables:\n" );

    for ( i = 0; i < MAXHASHENTRIES; i++ ) {

        if ( hash[i].key == NULL ) 
            break;

        printf( "Key: '%s' Value: '%s'\n", hash[i].key, hash[i].val );

    } /* for */
    
    printf( "Ready.\n" );

} /* dump_hash */



char*
get_type_of_value( char *key )
{
    int i;
    int len;


    for ( i = 0; i < MAXHASHENTRIES; i++ ) {

        if ( hash[i].key == NULL )
            return NULL;

        len = strlen( key );

        if ( strncmp( hash[i].key, key, len ) == 0
             && hash[i].key[len] == ':' )
            return &(hash[i].key[len+1]);

    } /* for */

    return NULL;

} /* get_type_of_value */



char*
get_value( char *key )
{
    int i;
    int len;


    for ( i = 0; i < MAXHASHENTRIES; i++ ) {

        if ( hash[i].key == NULL )
            return NULL;

        len = strlen( key );

        if ( strncmp( hash[i].key, key, len ) == 0
             && hash[i].key[len] == ':' )
            return hash[i].val;

    } /* for */

    return NULL;

} /* get_value */



int
set_value( char *key, char *val )
{

    cur++;

    if ( cur >= MAXHASHENTRIES ) {

        return -1;

    } /* if */

    hash[cur].key = strdup( key );
    hash[cur].val = strdup( val );

    return cur;

} /* set_value */



int
get_max_size()
{

    return MAXHASHENTRIES;

} /* get_max_size */



int
get_current_size()
{

    return cur;

} /* get_current_size */
