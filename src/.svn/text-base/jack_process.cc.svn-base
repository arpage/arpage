/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*- */
/*
 * jack_process.cc
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

#include "jack_process.h"
#include "arpage.h"

#include <jack/midiport.h>

#include <iostream>
#include <cstdio>

#define NOTE_ON  1
#define NOTE_OFF 0
#define NOTE_NOT_IN_PATTERN -1

//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------
void
process_midi_input( jack_nframes_t nframes, arp_runner * pr )
{
   if ( ! pr ) return;

   int events, i;
   jack_midi_event_t event;
   events = jack_midi_get_event_count( pr->in_port_buffer() );

   for (i = 0; i < events; i++) {

      if ( jack_midi_event_get(&event, pr->in_port_buffer(), i) ) {
         std::cout << "jack_midi_event_get failed, RECEIVED NOTE LOST." << std::endl;
         continue;
      }

      if (event.size > 3) {
         //std::cout << "Ignoring MIDI message longer than three bytes, probably a SysEx." << std::endl;
         continue;
      }

      MidiMessage msg(&event);
      unsigned char *eb;

      if ( pr->channelized() && pr->midi_channel() != msg.channel() ) {
         continue;
      }

      int nnum  = msg.note_number()+pr->patt.transpose();
      int mchan = pr->midi_channel();

      if ( pr->channelized() && pr->midi_channel() != msg.channel() ) {
         continue;
      }

      if ( msg.is_noteon() ) {

         if ( pr->patt.latched() ) {

            if ( pr->tapout_enabled() ) {

               if ( pr->patt.findinputnote( msg.note_number() ) != NOTE_NOT_IN_PATTERN ) {

                  if ( pr->last_tapout() == -1 ) {

                     pr->patt.delnote( msg, true );
                     pr->last_tapout(msg.note_number());

                  }

               } else if ( pr->can_add_to_latched() ) {

                  pr->patt.addnote( msg );

               }
               
            } else {

               if ( pr->can_add_to_latched() ) {

                  pr->patt.addnote( msg );

               }
            }

         } else {

            if ( pr->patt.findinputnote( msg.note_number() ) == NOTE_NOT_IN_PATTERN ) {

               pr->patt.addnote( msg );

            }
         }

      } else if ( msg.is_noteoff() ) {

         if ( ! pr->patt.size() )
            continue;

         // ignore any noteoff associated with a tapout noteon.
         if (  pr->tapout_enabled() &&
              (pr->last_tapout() == msg.note_number()) ) {
            pr->last_tapout(-1);
            continue;
         }

         pr->patt.delnote( msg );

      } else if ( msg.channel_message() == MIDI_CONTROLLER ) {

         if ( msg.controller_number() == MIDI_HOLD_PEDAL ) {

			std::cout << "Hold Pedal Msg" << msg.controller_value() << std::endl;
			pr->latched( msg.controller_value() );
            //if ( ! pr->patt.size() )
            //   continue;
            //
            //if ( msg.controller_value() == 0 ) {
            //   if ( pr->patt.latched() ) {
            //         pr->patt.latched(0);
            //         pr->patt.release_latch();
            //   }
            //} else {
            //   pr->patt.latched(msg.controller_value());
            //}
         } else {

            int mapped_ctl_value = pr->control_map(msg.controller_number(),
                                                   msg.controller_value());
            //FIXME
            //output mapped contol value to midi, please.
            //
         }
      }
   }
}


//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------
void
nonrolling_midi_input( jack_nframes_t nframes, arp_runner * runner )
{
      arp_runner * pr = runner;

      jack_midi_event_t event;

      if ( pr->program_change_needed() ) {
         pr->program_change(nframes);
         return;
      }

      int events = jack_midi_get_event_count( pr->in_port_buffer() );

      for (int i = 0; i < events; i++) {

         int read = jack_midi_event_get(&event, pr->in_port_buffer(), i);
         if (read) {
            std::cout << "jack_midi_event_get failed, RECEIVED NOTE LOST." << std::endl;
            continue;
         }

         if (event.size > 3) {
            std::cout << "Ignoring MIDI message longer than three bytes, probably a SysEx." << std::endl;
            continue;
         }

         unsigned char *eb = jack_midi_event_reserve( pr->out_port_buffer(), nframes, 3);
         if (eb != NULL) {
            memcpy(eb,event.buffer,event.size);
         }
      }

}


//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void
shutdown_callback(void *runner) {

   arp_runner ** runners = (arp_runner **)runner;
   if ( ! runners ) return;

   while ( *runners ) {
      arp_runner * pr = *runners++;
      pr->output_stop_pattern(0);
      pr->patt.clear_pattern();
      pr->in_port_buffer(0);
      pr->out_port_buffer(0);
      delete pr;
   }

}

//-----------------------------------------------------------------------------
// This is the audio process callback function.  Here we determine if the
// JACK transport is rolling, and do the right thing accordingly.
//-----------------------------------------------------------------------------
int
process_callback(jack_nframes_t nframes, void *runner)
{
   arp_runner ** runners = (arp_runner **)runner;
   if ( ! runners ) return 0;

   jack_position_t pos;
   jack_transport_state_t s = jack_transport_query( runners[0]->client(), &pos);

   arp_runner ** head = runners;
   while ( *head ) {

     arp_runner *pr = *head++;
      if ( ! pr->active() )
         continue;

      if ( s == JackTransportRolling ) {

         if ( pr->sound_off() ) {
            pr->sound_off(0);
            pr->rolling(1);
            //std::cout << "Begin Rolling" << std::endl;
         }

         if ( pos.valid & JackPositionBBT ) {
            pr->refresh_buffers(NFRAMES);
            jack_midi_clear_buffer( pr->out_port_buffer() );
            process_midi_input( NFRAMES, pr );
            pr->process_callback( NFRAMES, &pos );
         }

      } else {

         pr->refresh_buffers(NFRAMES);
         jack_midi_clear_buffer( pr->out_port_buffer() );

         if ( ! pr->sound_off() ) {
           pr->output_stop_sound(NFRAMES);
           pr->sound_off(1);
           pr->rolling(0);
         }

         nonrolling_midi_input( NFRAMES, pr );
      }


   }
   return 0;
}


//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
int
midi_uninit( jack_client_t * jack_client ) {
   jack_client_close( jack_client );
   sleep(1);
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
jack_client_t *
midi_init( arp_runner ** runners )
{
   jack_client_t  * jack_client = jack_client_open("arpage", JackNullOption, NULL);

   if (!jack_client) {
      std::cout << "Could not connect to JACK server" << std::endl;
      exit(1); //FIXME: exit points?
   }

   arp_runner **head = runners;
   while ( *head) {
      arp_runner * pr = *head++;
      pr->client( jack_client );
   }

   int err = jack_set_process_callback(jack_client, process_callback, (void*)runners);
   if (err) {
      std::cout << "Could not register JACK process callback." << std::endl;
      exit(1);
   }

   jack_on_shutdown(jack_client,shutdown_callback,(void*)runners);

   head = runners;
   char portname[5];
   int  portnum = 1;

   while ( *head) {
      arp_runner * pr = *head++;

      sprintf(portname,"in%d", portnum );

      jack_port_t * input_port =
         jack_port_register(jack_client, portname,
                            JACK_DEFAULT_MIDI_TYPE, JackPortIsInput, 0);

      if (input_port == NULL) {
         std::cout << "Could not register JACK input port." << std::endl;
         exit(1);
      }

      pr->in_port( input_port );

      sprintf(portname,"out%d", portnum++ );

      jack_port_t * output_port =
         jack_port_register(jack_client, portname,
                            JACK_DEFAULT_MIDI_TYPE, JackPortIsOutput, 0);

      if (output_port == NULL) {
         std::cout << "Could not register JACK output port." << std::endl;
         exit(1);
      }

      pr->out_port( output_port );

   }

   if (jack_activate(jack_client)) {
      std::cout << "Cannot activate JACK client." << std::endl;
      exit(1);
   }

   return jack_client;
}

//===========================================================================-
//
//===========================================================================-
