/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*- */
/*
 * jack_process.h
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
#ifndef __JACK_PROCESS_H__
#define __JACK_PROCESS_H__

#include <jack/jack.h>
#include "arprun.h"

//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------
void
process_midi_input( jack_nframes_t nframes, arp_runner * runner );

//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------
void
nonrolling_midi_input( jack_nframes_t nframes, arp_runner * runner );

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void * port_get_buffer( jack_port_t * p, jack_nframes_t nframes );


//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void
shutdown_callback(void *runner);


//-----------------------------------------------------------------------------
// This is the audio process callback function.  Here we determine if the
// JACK transport is rolling, and do the right thing accordingly.
//-----------------------------------------------------------------------------
int
process_callback( jack_nframes_t nframes, void *runner );


//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
int
midi_uninit( jack_client_t * jack_client );


//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
jack_client_t *
midi_init( arp_runner ** runners );

#endif
