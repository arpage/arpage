/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*- */
/*
 * scales.h
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
#ifndef __SCALES_H__
#define __SCALES_H__

#include <vector>

//============================================================================
//
// Each scale is just a vector of ints specifying, for scale position i, how
// many semitones to advance for scale position (i+1) % scale.size()
//
// Does not handle up vs. down motion, where # of semitones to move differs
// between up vs. down.
//
//============================================================================

typedef std::vector<int> scale_t;

struct scales {

   inline const scale_t & operator[] ( int i ) {
      scale_t * retval = (scale_t*)0;
      switch ( i ) {
         case  0: retval = &major;      break;
         case  1: retval = &minor;      break;
         case  2: retval = &diminished; break;
         case  3: retval = &augmented;  break;
         case  4: retval = &pentatonic; break;
         case  5: retval = &melodic;    break;
         case  6: retval = &harmonic;   break;
         case  7: retval = &dorian;     break;
         case  8: retval = &phrygian;   break;
         case  9: retval = &lydian;     break;
         case 10: retval = &mixolydian; break;
         case 11: retval = &aeolian;    break;
         case 12: retval = &locrian;    break;
         case 13: retval = &none;       break;
      };
      return *retval;
   }

   scale_t none;
   scale_t major;
   scale_t minor;
   scale_t diminished;
   scale_t augmented;
   scale_t pentatonic;
   scale_t melodic;
   scale_t harmonic;
   scale_t dorian;
   scale_t phrygian;
   scale_t lydian;
   scale_t mixolydian;
   scale_t aeolian;
   scale_t locrian;

   scales();
};

#endif
