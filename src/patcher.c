/*
** $Id: patcher.c,v 1.7 2008-09-21 12:05:16 welle Exp $
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


#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "jobdata.h"
#include "flood.h"
#include "patcher.h"
#include "macros.h"
#include "hash.h"




Patcher patcher[] = {

    { "BYTE",      patch_byte },
    { "BYTEFIELD", patch_byte_field },
    { "HEX",       patch_hex },
    { "HEXFIELD",  patch_hex_field },
    { "STRING",    patch_string },
    { NULL,       NULL },

};
 


int
patch_byte( char *v, unsigned char *patched )
{

    LOG( "Patch byte\n" );

    return patch_byte_field( v, patched );

} /* patch_byte */



int
patch_byte_field( char *v, unsigned char *patched )
{
    char *tmp;
    int len;


    LOG( "Patch byte field\n" );

    len = 0;
    tmp = v;

    while ( tmp && isdigit( (int)(tmp[0]) ) ) {
        unsigned int p;


        p = atoi( tmp );

        if ( p > 255 ) p = 255;

        patched[len] = p;
        len++;
        tmp = strchr( tmp, ',' );
        
        if ( tmp ) tmp++;

    } /* while */

    return len;

} /* patch_byte_field */



int
patch_hex( char *v, unsigned char *patched )
{

    LOG( "Patch hex\n" );

    return 0;

} /* patch_hex */



int
patch_hex_field( char *v, unsigned char *patched )
{

    LOG( "Patch hex field\n" );

    return 0;

} /* patch_hex_field */



int
patch_string( char *v, unsigned char *patched )
{
    size_t len;
    int i;


    LOG( "Patch string\n" );


    len = strlen( v );

    for ( i = 0; i < len; ++i ) {
        unsigned int p;

        p = v[i];

        if ( p > 255 ) p = 255;

        patched[i] = p;

    } /* for */

    return (int)len;

} /* patch_string */



char *
get_type_of( char *v )
{
    char *tmp;


    tmp = strchr( v, ':' );
    if ( tmp ) tmp++;

    return tmp;

} /* get_type_of */



/*
** TODO: Should not write more than buflen bytes to patched.
*/
int
patch_value( char *key, char *val,
             unsigned char *patched, unsigned int buflen )
{
    char *value;
    char *type;
    size_t  len;
    int  i;
    int  ret;


    len = strlen( val );


    /*
    ** If we really need something to patch (exchange
    ** a template variable with its current value) we 
    ** we must grab the value from our value container 
    ** first.
    */
    if ( val[0] == '@' && val[len - 1] == '@' ) {
        
        val[len - 1] = '\0';
        val++;

        value = get_value( val );
        type = get_type_of_value( val );

    } else {

        value = val;
        type = get_type_of( key );

    } /* if */


    /*
    ** Now that we have a value and its type we can
    ** convert it to an unsigned int representation.
    */
    if ( value && type ) {

        LOG( "Param: '%s' value to patch in: '%s' as type: '%s'\n",
             val, value, type );


        for ( i = 0; patcher[i].cmd != NULL; ++i ) {
            
            if ( strcasecmp( patcher[i].cmd, type ) == 0 ) {

                ret = patcher[i].f( value, patched );
                
                LOG( "Patched %d bytes\n", ret );
    
                return ret;
                    
            } /* if */
            
        } /* for */
        
    } /* if */

    return 0;

} /* patch_value */



int
patch_line( char *key, char *val,
            unsigned char *patched, unsigned int buflen )
{

    unsigned int len;
    char *start, *first, *end;
    char _parmname[buflen];
    char *parmname = _parmname;


    memset( patched, '\0', buflen );

    len = 0;
    end = val;


    while ( *end != '\0' ) {

        start = end;
        first = strchr( end, '@' );


        if  ( first ) {

            /*
            ** Copy everything between last position and
            ** the detected @ to the output.
            */
            memset( parmname, '\0', buflen );
            strncpy( parmname, start, ( first - start ) );
            len += patch_value( key, parmname, &patched[len], buflen - len );


            /*
            ** Skip the just found @
            */
            first++;

            if ( *first == '@' ) {

                /*
                ** We have found a quoted @, no template
                ** parameter. Copy it to the output.
                */
                len += patch_value( key, "@", &patched[len], buflen - len );

                /*
                ** Set the pointer after the quoted @.
                */
                end = first++;

            } else {

                /*
                ** Seems we have a template parameter so far, Try
                ** to find closing @.
                */
                end = strchr( first, '@' );
                
                if ( end ) {

                    /* 
                    ** We grab the template parameter name 
                    ** inclusive its enclosing @. 
                    */
                    memset( parmname, '\0', buflen );      
                    strncpy( parmname,
                             first - 1, ( ( end + 1 ) - ( first - 1 ) ) );
                    len += patch_value( key, parmname,
                                        &patched[len], buflen - len );

                } else {

                    ERR( "Could not parse config parameter\n" );
                    return -1;

                } /* if: found closing @ */

            } /* if: found quoted @ */

        } else { 

            /*
            ** No more @ in this line. Copy the rest
            ** of the line to the output and we are done.
            */
            len += patch_value( key, end, &patched[len], buflen - len );
            break;

        } /* if: found opening @ */
        
        end++;

    } /* while */

    return len;

} /* patch_line */
