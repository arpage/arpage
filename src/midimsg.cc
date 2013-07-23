/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*- */
/*
 * midimsg.cc
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
#include "midimsg.h"

//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------
void
make_note( int channel, int note, int velocity, int on, unsigned char *buffer )  {
   if ( ! buffer ) return;
   unsigned char onoff = on ? 0x09 : 0x08;
   buffer[0] = (onoff << 4) | channel;  // noteon/off+channel
   buffer[1] = note;
   buffer[2] = velocity;
}

//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------
void
make_msg( int msg, int channel, int value, unsigned char *buffer )  {
   if ( ! buffer ) return;
   buffer[0] = 0xB0 | channel;
   buffer[1] = msg;
   buffer[2] = value;
}

//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------
void
make_pc( int channel, int program, unsigned char *buffer )  {
   if ( ! buffer ) return;
   buffer[0] = 0xC0 | channel;
   buffer[1] = program;
}


//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------
void
make_syx( int len, unsigned char msg[], unsigned char *buffer )  {
  for ( int i=0; i<len; i++ ) {
     buffer[i] = msg[i];
  }
}
