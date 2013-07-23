/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*- */
/*
 * arprun.cc
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
#include "arprun.h"
#include "arpage.h"
#include <iostream>

#define PULSE_WM 0

#define SET_NOTE_ON  1
#define SET_NOTE_OFF 0

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void * port_get_buffer( jack_port_t * p, jack_nframes_t nframes ) {
   void * port_buffer = jack_port_get_buffer( p, nframes );
   if (port_buffer == NULL) {
      std::cout << "jack_port_get_buffer failed!" << std::endl;
   }
   return port_buffer;
}

//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------
arp_runner::arp_runner( int midi_channel, int program_change, double bpp ) :

   output_port(NULL),
   input_port(NULL),
   jack_client(NULL),
   out_port_buff(NULL),
   in_port_buff(NULL),

   pulse_duration(0),
   ticks_per_pulse(0),

   // these values may be modified by the UI
   user_beats_per_pulse(bpp),
   user_percent(.80),
   user_channel(midi_channel),
   user_program(program_change),
   user_velocity(0),
   user_scale(13),
   user_active(0),
   user_channelized(0),
   user_locked(1),

   // these are the counters to determine when to emit note-on
   pulse_countdown(0),
   last_pos(0),

   // state
   sound_is_off(0),
   this_program(-1),
   initial_program(-1),
   this_bar(-1),
   this_beat(-1),
   this_tick(-1),
   this_percent(0.0) ,
   this_beats_per_pulse(0.0),
   note_is_on(0),
   fresh_run(1),
   latch_tapout(0),
   last_tappedout_note(-1),
   is_rolling(0)
{

   //FIXME:  midi CC map should be a configurable parameter...
   ccmap[2].pr       = this;
   ccmap[2].lo       = 0;
   ccmap[2].hi       = 127;
   ccmap[2].modifier = &arp_runner::velocity;

   ccmap[1].pr       = this;
   ccmap[1].lo       = 5;
   ccmap[1].hi       = 90;
   ccmap[1].opt      = (double)(127.0f / (ccmap[1].hi - ccmap[1].lo));
   ccmap[1].modifier = &arp_runner::int_pulse_percent;
}



//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------
inline void pos_out( const char * msg, const jack_position_t * pos, const arp_pattern & patt ) {
   std::cout << msg << ": " << pos->bar << '/' << pos->beat << '/' << pos->tick << std::endl;
   std::cout << "   " << patt.note_at( patt.pos() ) << '/' << patt.pos() << std::endl;
}

inline void pos_out( const char * msg, const jack_position_t * pos ) {
   std::cout << msg << ": " << pos->bar << '/' << pos->beat << '/' << pos->tick << std::endl;
}


//----------------------------------------------------------------------------
//
//
//----------------------------------------------------------------------------
void
arp_runner::output_stop_sound( jack_nframes_t nframes ) {

   output_stop_pattern(nframes);
   patt.clear_pattern();
   fresh_run = 1;

   unsigned char *buffer = jack_midi_event_reserve(out_port_buff, nframes, 3);
   if (buffer != NULL)
      make_msg( MIDI_ALL_MIDI_CONTROLLERS_OFF, user_channel, SET_NOTE_OFF,
                buffer );

   buffer = jack_midi_event_reserve(out_port_buff, 0, 3);
   if (buffer != NULL)
      make_msg( MIDI_ALL_NOTES_OFF, user_channel, 0, buffer );

   sounding(0);
}


//----------------------------------------------------------------------------
// Send the note-on event for the next available MIDI note from the input
// queue to the MIDI output port.
//----------------------------------------------------------------------------
int
arp_runner::output_next_pulse( jack_nframes_t nframes ) {
   int note_sent = 0;
   unsigned char *buffer = jack_midi_event_reserve(out_port_buff, nframes,3);
   if (buffer != NULL) {
      int note = patt.note_at( patt.pos() )+patt.transpose();
      int vel  = user_velocity ? user_velocity : patt.velocity_at(patt.pos());
      make_note( user_channel, note, vel, SET_NOTE_ON, buffer );
      note_sent = note;
      sounding(1);
   } else {
      std::cout << "WTF on output note on " << std::endl;
   }
   return note_sent;
}


//----------------------------------------------------------------------------
// Send the a note-off event for the currently playing MIDI note to the MIDI
// output port.
//----------------------------------------------------------------------------
int
arp_runner::output_stop_note( jack_nframes_t nframes ) {
   int sent_noteoff = 0;
   unsigned char *buffer = jack_midi_event_reserve(out_port_buff, nframes, 3);
   if (buffer != NULL) {
      pattern_element relhead = patt.releasehead();
      int vel = user_velocity ? user_velocity : relhead.msg.velocity();
      make_note( user_channel, relhead.note_number()+patt.transpose(),
                 vel, SET_NOTE_OFF, buffer );
      sent_noteoff = 1;
      sounding(0);
   } else {
     std::cout << "WTF on output note off " << std::endl;
   }
   return sent_noteoff;
}

//----------------------------------------------------------------------------
// Send a program change message to the MIDI output port.
//----------------------------------------------------------------------------
void
arp_runner::output_pc( jack_nframes_t nframes ) {

   unsigned char *buffer = jack_midi_event_reserve(out_port_buff, nframes, 3);
   if (buffer != NULL) {
      std::cout << "Output PC: " << user_program 
                << ", channel: " << user_channel << std::endl;
      make_pc( user_channel, user_program, buffer );
   } else {
      std::cout << "WTF on output program change " << std::endl;
   }
}

//----------------------------------------------------------------------------
// Send the a note-off event for each note in our pattern
// output port.
//----------------------------------------------------------------------------
void
arp_runner::output_stop_pattern( jack_nframes_t nframes ) {

   while( patt.relsize() ) {
      output_stop_note(nframes);
      patt.release_advance();
   }
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
bool
arp_runner::change_detect( const jack_position_t * pos )  {

   bool retval = false;
   if ( user_beats_per_pulse != this_beats_per_pulse ) {
      this_beats_per_pulse = user_beats_per_pulse;
      ticks_per_pulse = pos->ticks_per_beat * user_beats_per_pulse;
      pulse_duration = ticks_per_pulse * user_percent;
      retval = true;
   }

   if ( user_percent != this_percent ) {
      this_percent = user_percent;
      pulse_duration = ticks_per_pulse * user_percent;
      retval = true;
   }

   this_bar  = pos->bar;
   this_beat = pos->beat;
   this_tick = pos->tick;

   return retval;
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void
arp_runner::program_change(jack_nframes_t nframes) {
   this_program = initial_program = user_program;
   output_pc(nframes);
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void
arp_runner::refresh_buffers( jack_nframes_t nframes ) {
   // refresh midi i/o buffers at the start of each cycle
    in_port_buffer( port_get_buffer(  in_port(), nframes ) );
   out_port_buffer( port_get_buffer( out_port(), nframes ) );

}

bool
arp_runner::program_change_needed() {
   bool retval = false;
   if ( initial_program == -1 ) {
      if ( is_rolling ) {
         std::cout << "Initial PC Not Sent - Schedulding Now! " << std::endl;
         retval = true;
      }
   } else {
      if ( this_program != user_program ) {
         std::cout << "User PC: " << user_program 
                   << ", but Last PC: " << this_program
                   << " - Schedulding Now! " << std::endl;
         retval = true;
      }
   }
   return retval;
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void
arp_runner::process_callback( jack_nframes_t nframes,
	                            const jack_position_t * pos ) {

   if ( pos->tick == last_pos )
      return;

   if ( program_change_needed() ) {
      program_change(nframes);
      return;
   } 

   int pos_delta =  (last_pos > pos->tick) ?
                   ((pos->ticks_per_beat - last_pos)+pos->tick) :
                    (pos->tick - last_pos);

   if ( patt.release_countdown( pos_delta ) ) {
      output_stop_note(nframes);
      patt.release_advance();
      if ( ! patt.size() ) {
         fresh_run = 1;
      }
   }
   
   // TODO:  the right thing if we've changed position radically
   bool change_detected = change_detect(pos);

   pulse_countdown -= pos_delta;

   if ( pulse_countdown <= PULSE_WM ) {
      if ( patt.size() ) {
         
         output_next_pulse(nframes);
         patt.addreleasenote( patt.element_at( patt.pos() ), pulse_duration + pulse_countdown );
         patt.pattern_advance();
      }
      pulse_countdown = (ticks_per_pulse + pulse_countdown);
   }

   last_pos = pos->tick;
}

//============================================================================
//
//============================================================================
