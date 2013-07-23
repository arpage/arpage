/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*- */
/*
 * arpui.cc
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
#include "arpui.h"
#include "arppatt.h"
#include <iomanip>

#include "arpstorage.h"

arpui::arpui( const std::string & prefix, Glib::RefPtr<Gtk::Builder> & builder, 
             arp_runner * pr, Gtk::Main* pm )
{
   note_active = 0;
   latch_active = 0;
   is_rolling = 0;

   this_bar = this_beat = this_tick = 0;

   arpprefix = prefix;
   runner = pr;
   main = pm;

   builder->get_widget( prefix+"channel",     arpchannel);
   builder->get_widget( prefix+"range",       arprange  );
   builder->get_widget( prefix+"interval",    arpinterval);

   builder->get_widget( prefix+"patch",       arpprogram);
   builder->get_widget( prefix+"transpose",   arptranspose);
   builder->get_widget( prefix+"velocity",    arpvelocity);
   builder->get_widget( prefix+"latch",       arplatchlock );
   builder->get_widget( prefix+"tapout",      arptapout );
   builder->get_widget( prefix+"scale",       arpscale );
   
   builder->get_widget( prefix+"percent",      arpnotepercent );   
   builder->get_widget( prefix+"noteduration", arpnoteduration );   
   
   builder->get_widget( prefix+"image1",  image1 );
   builder->get_widget( prefix+"image2",  image2 );
   builder->get_widget( prefix+"image3",  image3 );
   
   builder->get_widget( prefix+"active",      arpactive );
   builder->get_widget( prefix+"setlatch",    arpsetlatch );
   builder->get_widget( prefix+"channelized", arpchannelized );
   builder->get_widget( prefix+"noteorder",   arpnoteorder );
   
   pix_redled         = Gdk::Pixbuf::create_from_file( REDLED_PNG );
   pix_redled_off     = Gdk::Pixbuf::create_from_file( REDLED_OFF_PNG );
   pix_blueled        = Gdk::Pixbuf::create_from_file( BLUELED_PNG );
   pix_blueled_off    = Gdk::Pixbuf::create_from_file( BLUELED_OFF_PNG );
   pix_greenled       = Gdk::Pixbuf::create_from_file( GREENLED_PNG );
   pix_greenled_off   = Gdk::Pixbuf::create_from_file( GREENLED_OFF_PNG );
   pix_cyanled        = Gdk::Pixbuf::create_from_file( CYANLED_PNG );

   image1->set( pix_redled );
   image2->set( pix_greenled_off );
   image3->set( pix_greenled_off );

   arpnoteorder->set_active(arp_pattern::MODE_IN);
   arpnoteduration->set_active(NOTEDUR_16);
   
   arpnotepercent->signal_value_changed().connect( sigc::mem_fun(*this,&arpui::on_notepercent_change) );
   arpnotepercent->set_range(5,310);
   arpnotepercent->set_value(80);
   arpnotepercent->set_increments(1,20);
	
   arpchannel->signal_value_changed().connect( sigc::mem_fun(*this,&arpui::on_channel_change) );
   arpchannel->set_range(1,16);

   arprange->signal_value_changed().connect( sigc::mem_fun(*this,&arpui::on_range_change) );
   arprange->set_range(-60,60);
   
   arpinterval->signal_value_changed().connect( sigc::mem_fun(*this,&arpui::on_interval_change) );
   arpinterval->set_range(-24,24);
	
   arptranspose->signal_value_changed().connect( sigc::mem_fun(*this,&arpui::on_transpose_change) );
   arptranspose->set_range(-24,24);
	
   arpprogram->signal_value_changed().connect( sigc::mem_fun(*this,&arpui::on_program_change) );
   arpprogram->set_range(0,127);
	
   arpvelocity->signal_value_changed().connect( sigc::mem_fun(*this,&arpui::on_velocity_change) );
   arpvelocity->set_range(0,127);
	
   arpscale->signal_changed().connect( sigc::mem_fun(*this,&arpui::on_scale_change) );
	
   arpnoteorder->signal_changed().connect( sigc::mem_fun(*this,&arpui::on_noteorder_change) );

   arpactive->signal_toggled().connect( sigc::mem_fun(*this,&arpui::on_active_change) );
   
   arpsetlatch->signal_toggled().connect( sigc::mem_fun(*this,&arpui::on_setlatch_change) );
   
   arplatchlock->signal_toggled().connect( sigc::mem_fun(*this,&arpui::on_latchlock_change) );
	
   arptapout->signal_toggled().connect( sigc::mem_fun(*this,&arpui::on_tapout_change) );
	
   arpchannelized->signal_toggled().connect( sigc::mem_fun(*this,&arpui::on_channelized_change) );   
	
   arpnoteduration->signal_changed().connect( sigc::mem_fun(*this,&arpui::on_noteduration_change) );

   main->signal_quit().connect(sigc::mem_fun(this, &arpui::store_session));
}

bool
arpui::store_session() {
   //std::cout << "Calling Store Session" << std::endl;
   arpstorage storage(this);
   storage.store_session();
   return 1;
}

void
arpui::on_channel_change() {
   runner->midi_channel( arpchannel->get_value_as_int()-1 );
}

void
arpui::on_range_change() {
   runner->patt.range( arprange->get_value_as_int() );
}

void
arpui::on_interval_change() {
   runner->patt.interval( arpinterval->get_value_as_int() );
}

void
arpui::on_transpose_change() {
   runner->patt.transpose( arptranspose->get_value_as_int() );
}

void
arpui::on_tapout_change() {
   runner->tapout_enabled( arptapout->get_active() );
}

bool
arpui::pos_actions() {

   if ( runner->rolling() ) {
      if ( !is_rolling ) {
         is_rolling = 1;
      }
   } else {
      if ( is_rolling ) {
         is_rolling = 0;
         store_session();
      }
   }

   if ( runner->pc() >=0 && runner->pc() <= 10 ) {
      //Glib::ustring line = Glib::ustring::compose("* %1 *",runner->pc());
      //lblDivs->set_text(line);
   }

   if ( this_bar != runner->bar() ) {
      this_bar = runner->bar();
      //Glib::ustring line = Glib::ustring::compose("%1", this_bar);
      //lblBar->set_text(line);
   }

   if ( this_beat != runner->beat() ) {
      this_beat = runner->beat();
      //Glib::ustring line = Glib::ustring::compose("%1", this_beat);
      //lblBeat->set_text(line);
   }

   if ( this_tick != runner->tick() ) {
      this_tick = runner->tick();
      //Glib::ustring line = Glib::ustring::compose("%1", this_tick);
      //lblTick->set_text(line);
   }

   return 1;
}

void
arpui::on_active_change() {
   if ( arpactive->get_active() ) {
      image1->set( pix_greenled );
      runner->active(1);
   } else {
      image1->set( pix_redled );
      runner->active(0);
   }
}

void
arpui::on_setlatch_change() {
   runner->latched( arpsetlatch->get_active() );	
}

bool
arpui::note_sounding_actions() {
   if ( runner->sounding() ) {
      if ( !note_active ) {
         image2->set( pix_greenled );
         note_active = 1;
      }
   } else {
      if ( note_active ) {
         image2->set( pix_greenled_off );
         note_active = 0;
      }
   }

   if ( runner->latched() ) {
      if ( !latch_active ) {
         image3->set( pix_blueled );
         latch_active = 1;
      }
   } else {
      if ( latch_active ) {
         image3->set( pix_greenled_off );
         latch_active = 0;
      }
   }

   return 1;
}

void
arpui::on_channelized_change() {
   runner->channelized( arpchannelized->get_active() );
}

void
arpui::on_scale_change() {
  runner->patt.scale( arpscale->get_active_row_number() );
}

void
arpui::on_noteorder_change() {

   switch ( arpnoteorder->get_active_row_number() )
   {
     case 0: runner->patt.mode( arp_pattern::MODE_IN );       break;
     case 1: runner->patt.mode( arp_pattern::MODE_UP );       break;
     case 2: runner->patt.mode( arp_pattern::MODE_DN );       break;
     case 3: runner->patt.mode( arp_pattern::MODE_UPDN );     break;
     case 4: runner->patt.mode( arp_pattern::MODE_DNUP );     break;
     case 5: runner->patt.mode( arp_pattern::MODE_RAND );     break;
     case 6: runner->patt.mode( arp_pattern::MODE_UPDNEXC );  break;
     case 7: runner->patt.mode( arp_pattern::MODE_DNUPEXC );  break;
     case 8: runner->patt.mode( arp_pattern::MODE_RANDDYN );  break;
   }
}

void
arpui::on_latchlock_change() {
   runner->can_add_to_latched( ! arplatchlock->get_active() );
}

void
arpui::on_program_change() {
   runner->program( arpprogram->get_value_as_int() );
}

void
arpui::on_velocity_change() {
   runner->velocity( arpvelocity->get_value_as_int() );
}

void
arpui::on_noteduration_change() {
	 switch( arpnoteduration->get_active_row_number() ) 
	 {
      case NOTEDUR_4  : runner->beats_per_pulse(1.0);  break;
      case NOTEDUR_4T : runner->beats_per_pulse(.666); break;
      case NOTEDUR_8  : runner->beats_per_pulse(.5);   break;
      case NOTEDUR_8T : runner->beats_per_pulse(.333); break;
      case NOTEDUR_16 : runner->beats_per_pulse(.25);  break;
      case NOTEDUR_16T: runner->beats_per_pulse(.166); break;
      case NOTEDUR_32 : runner->beats_per_pulse(.125); break;
      case NOTEDUR_32T: runner->beats_per_pulse(.083); break;
  }
}

void
arpui::on_notepercent_change() {
   runner->pulse_percent( arpnotepercent->get_value() / 100 );
}
