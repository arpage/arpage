/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*- */
/*
 * arpage.h
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
#ifndef __ARPAGE_H__
#define __ARPAGE_H__

#ifdef ENABLE_NLS
#  include <libintl.h>
#endif

#define MIDI_NOTE_ON                    0x90
#define MIDI_NOTE_OFF                   0x80
#define MIDI_HOLD_PEDAL                   64
#define MIDI_CONTROLLER                 0xB0
#define MIDI_ALL_MIDI_CONTROLLERS_OFF    121
#define MIDI_ALL_NOTES_OFF               123
#define MIDI_PROGRAM_CHANGE             0xC0

#define NFRAMES 0
//#define NFRAMES nframes

//----------------------------------------------------------------------------
// Needed?  A value specifying an empty pattern slot.
//----------------------------------------------------------------------------
#define NOTE_NONE  -1

#define HINYBBLE(n) (((n)&0xF0)>>4)
#define LONYBBLE(n) ((n)&0x0F)

#endif
