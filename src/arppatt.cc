/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*- */
/*
 * arppatt.cc
 * Copyright (C) Mark Vitek 2009 <straypacket@gmail.com>
 *
 * arpage is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * arpage is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include "arppatt.h"
#include <iostream>
#include <climits>

//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------
inline int
up_compare( const void*a, const void*b ) {
   return ((pattern_element*)a)->input_note_number() -
          ((pattern_element*)b)->input_note_number();
}

//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------
inline int
dn_compare( const void*a, const void*b ) {
   return ((pattern_element*)b)->input_note_number() -
          ((pattern_element*)a)->input_note_number();
}

//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------
inline int
rnd_compare( const void*, const void* ) {
   return (rand() % 3) - 1;
}

//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------
int
arp_pattern::release_advance(void) {
   int retval = 0;
   if ( release_size ) {
      release_size--;
      if ( release_size ) {
         memmove(release_output, release_output+1, sizeof(pattern_element)*release_size);
         memmove(ttl,            ttl+1,            sizeof(int)*release_size);
      }
      retval = relsize();
   }
   return retval;
}

//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------
void
arp_pattern::addreleasenote( const pattern_element & elem, int elem_ttl ) {
   release_output[release_size] = elem;
   ttl[release_size] = elem_ttl;
   if ( release_size < PATTMAX )
      release_size++;
}

//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------
int
arp_pattern::release_countdown( int c ) {
   int retval = 0;
   if ( release_size ) {
      for ( int i=0; i < relsize(); i++ ) {
         ttl[i] -= c;
      }
      retval = ttl[0] <= 0;
   }
   return retval;
}

//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------
int
arp_pattern::pattern_advance(void) {
   int retval = -1;
   
   if ( pattern_size ) {
      if ( patt_scales[patt_scale].size() == 0 ) {
         pattern_input[pattern_pos].note_offset += patt_interval;
      } else {
         pattern_input[pattern_pos].note_offset +=
            (patt_scales[patt_scale][ pattern_input[pattern_pos].scale_offset ]);
		 
         pattern_input[pattern_pos].scale_offset =
            (++(pattern_input[pattern_pos].scale_offset) %
            (patt_scales[patt_scale].size()));
      }
      
      if ( patt_range >= 0 ) {
         if ( (pattern_input[pattern_pos].note_number() >
            (pattern_input[pattern_pos].input_note_number() + patt_range)) ) {
            pattern_input[pattern_pos].note_offset = 0;
            pattern_input[pattern_pos].scale_offset = 0;
         }
      } else {
         if ( (pattern_input[pattern_pos].note_number() <
            (pattern_input[pattern_pos].input_note_number() + patt_range)) ) {
            pattern_input[pattern_pos].note_offset = 0;
            pattern_input[pattern_pos].scale_offset = 0;			
         }
      }
	  
	  if ( pattern_mode == MODE_RANDDYN ) {
	      int lp = pattern_pos;
		  while ( lp == pattern_pos ) {
		     lp = (rand()%pattern_size);
		  }
		  pattern_pos = lp;
          
	  } else {
         pattern_pos = (++pattern_pos % pattern_size);
	  }
	  
	  if ( pattern_pos == 0 ) {
         if ( pattern_mode == MODE_DNUP || pattern_mode == MODE_UPDN || 
              pattern_mode == MODE_DNUPEXC || pattern_mode == MODE_UPDNEXC ) {
            if ( pattern_sorter == up_compare ) {
               pattern_sorter = dn_compare;
			} else {
               pattern_sorter = up_compare;
			}
            qsort(pattern_input,pattern_size,sizeof(pattern_element),pattern_sorter);
		    if ( pattern_mode == MODE_DNUPEXC || pattern_mode == MODE_UPDNEXC ) {
              pattern_pos = (++pattern_pos % pattern_size);
			}
         }
	  }
	  
      retval = pattern_pos;
   }
   return retval;
}


//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------
void
arp_pattern::initpattern(void) {
   pattern_pos  = 0;
   release_size = 0;
   memset(ttl,INT_MAX,sizeof(ttl));
}

//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------
int
arp_pattern::findnote( int note ) const {
   int retval = -1;
   for (int i=0; i<pattern_size; i++ ) {
      if ( pattern_input[i].note_number() == note ) {
         retval=i;
         break;
      }
   }
   return retval;
}

//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------
int
arp_pattern::findinputnote( int note ) const {
   int retval = -1;
   for (int i=0; i<pattern_size; i++ ) {
      if ( pattern_input[i].input_note_number() == note ) {
		 retval=i;
         break;
      }
   }
   return retval;
}

//----------------------------------------------------------------------------
//
//
//----------------------------------------------------------------------------
// TODO:  add up/down and down/up modes.
//        perform standard action, then double size and mirror the data
//----------------------------------------------------------------------------
void
arp_pattern::addnote( const MidiMessage & msg ) {
   
   pattern_input[pattern_size] = msg;
   if ( pattern_size < PATTMAX ) pattern_size++;
   pattern_sorter = NULL;
   switch ( pattern_mode ) {
      case MODE_DN:
      case MODE_DNUP:
      case MODE_DNUPEXC:
         pattern_sorter = dn_compare;
         break;
      case MODE_UP:
      case MODE_UPDN:
      case MODE_UPDNEXC:
         pattern_sorter = up_compare;
         break;
      case MODE_RAND:
      case MODE_RANDDYN:
         pattern_sorter = rnd_compare;
         break;
   }

   if ( pattern_sorter ) {
      qsort(pattern_input,pattern_size,sizeof(pattern_element),pattern_sorter);
   }
}

//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------
void
arp_pattern::release_latch(void) {
   if ( (!latched()) && unlatch_size ) {
      while ( unlatch_size > 0 ) {
         unlatch_size--;
         delnote( pattern_pending_delete[unlatch_size].msg );
      }
   }
}

//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------
void
arp_pattern::clear_pattern(void) {
   release_latch();
   while ( pattern_size > 0 ) {
      delnote( pattern_input[pattern_size-1].msg );
   }
}

//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------
void
arp_pattern::delnote( const MidiMessage & msg, bool ignore_latch ) {

   if ( pattern_size ) {
      
      int pos = findinputnote( msg.note_number() );
      
      if ( pos != -1 ) {
         
         if ( latched() && (! ignore_latch) ) {
            
            pattern_pending_delete[unlatch_size] = msg;
            if ( unlatch_size < PATTMAX )
               unlatch_size++; // TODO:  manage growth
            
         } else {
            if ( pattern_size > 1 ) {
               pattern_element * to =  pattern_input + pos;
               pattern_element * fr =  to + 1;
               size_t            sz = (pattern_size-(pos+1)) *
                                       sizeof(pattern_element);
               memmove( to, fr, sz );
               if ( --pattern_size == pattern_pos )
                  pattern_pos--;
            } else {
               pattern_size = pattern_pos = 0;
            }
         }
      }
   }
}
//===========================================================================-
//
//===========================================================================-
