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


#ifndef _HASH_H_
#define _HASH_H_ 1


#define MAXHASHENTRIES 1024
#define UNDEFINEDIDX   -1


void init_hash();
void drop_hash();
void dump_hash();
char *get_type_of_value( char *key );
char *get_value( char *key );
int  set_value( char *key, char *val );
int  get_max_size();
int  get_current_size();

#endif
