/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*- */
/*
 * pattelem.h
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

#ifndef __PATTELEM__
#define __PATTELEM__

#include "midimsg.h"

//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------
struct pattern_element {

   MidiMessage msg;
   int note_offset;
   int scale_offset;
   int note_transpose;

   inline int note_number() const {
      return msg.note_number() + note_offset;
   }

   inline int input_note_number() const {
      return msg.note_number();
   }

   pattern_element()
      : note_offset(0), scale_offset(0) {
   }

   pattern_element( const pattern_element & rhs )
      : msg(rhs.msg), note_offset(rhs.note_offset), scale_offset(rhs.scale_offset) {
   }

   pattern_element( const MidiMessage & rhs )
      : msg(rhs), note_offset(0), scale_offset(0) {
   }

   pattern_element & operator = ( const pattern_element & rhs ) {
      msg     = rhs.msg;
      note_offset = rhs.note_offset;
      scale_offset = rhs.scale_offset;
	  return *this;
   }

   pattern_element & operator = ( const MidiMessage & rhs ) {
      msg          = rhs;
      note_offset  = 0;
      scale_offset = 0;
	  return *this;
   }

};
#endif
