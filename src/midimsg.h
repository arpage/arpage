/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*- */
/*
 * midimsg.h
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
#ifndef __MIDIMSG_H__
#define __MIDIMSG_H__

#include "arpage.h"

#include <jack/jack.h>
#include <jack/midiport.h>
#include <cstring>

//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------
struct MidiMessage {

   jack_nframes_t   time;
   int		        len;   /* Length of MIDI message, in bytes. */
   unsigned char    data[3];

   MidiMessage(): len(0),time(0) {
      memset(data, -1, 3);
   }

   MidiMessage( jack_midi_event_t *event ) {
      if ( event ) {
         len = event->size;
         time = event->time;
         memcpy(data, event->buffer, sizeof(data));
	  }
   }

   MidiMessage & operator = ( jack_midi_event_t *event ) {
      if ( event ) {
         len = event->size;
         time = event->time;
         memcpy(data, event->buffer, sizeof(data));
	  }
	  return *this;
   }

   inline int channel() const {
     return data[0] & 0x0F;
   }
   inline int channel_message() const {
     return (data[0] & 0xF0);
   }
   inline int note_number() const {
      return data[1];
   }
   inline int velocity() const {
      return data[2];
   }
   inline int is_control_msg() const  {
      return (data[0] & 0xF0) == 0xB0;
   }
   inline int controller_number() const {
      return data[1];
   }
   inline int controller_value() const {
      return data[2];
   }

  inline bool is_noteon() {
      return (channel_message() == MIDI_NOTE_ON) && (velocity() > 0);
   }

  inline bool is_noteoff() {
      return (channel_message() == MIDI_NOTE_OFF) ||
            ((channel_message() == MIDI_NOTE_ON) && (velocity() == 0));
   }

   MidiMessage( const MidiMessage & rhs ): time(rhs.time), len(rhs.len) {
      memcpy(data, rhs.data, sizeof(data));
   }

   MidiMessage & operator = ( const MidiMessage & rhs ) {
      len = rhs.len;
      time = rhs.time;
      memcpy(data, rhs.data, sizeof(data));
      return *this;
   }

private:
};

//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------
void
make_note( int channel, int note, int velocity, int on, unsigned char *buffer );

//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------
void
make_msg( int msg, int channel, int value, unsigned char *buffer );

//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------
void
make_pc( int channel, int program, unsigned char *buffer );

//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------
void
make_syx( int len, unsigned char msg[], unsigned char *buffer );

#endif
