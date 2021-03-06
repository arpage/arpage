/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*- */
/*
 * main.cc
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

#include <glibmm.h>
#include <giomm/file.h>
#include <gtkmm.h>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <vector>
#include <ctime>
#include <jack/jack.h>

#define UI_FILE     PACKAGE_DATA_DIR"/arpage/ui/arpage.ui"
/* For testing propose use the local (not installed) ui file */
//#define UI_FILE "./arpage.ui"

#include "arprun.h"
#include "arpui.h"
#include "arpstorage.h"
#include "jack_process.h"

#define MAX_ARPS 4

//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------
int
main (int argc, char *argv[])
{
   Glib::thread_init();
   if ( ! Glib::thread_supported() )  {
      std::cerr << "Cannot initialize Glib threads" << std::endl;
      return 1;
   }
   srand( time(NULL) );

   Gtk::Main kit(argc, argv);

   //Load the Glade file and instiate its widgets:
   Glib::RefPtr<Gtk::Builder> builder;
   try
   {
     builder = Gtk::Builder::create_from_file(UI_FILE);
   }
   catch (const Glib::FileError & ex)
   {
     std::cerr << ex.what() << std::endl;
     return 1;
   }
   catch (const Gtk::BuilderError & ex)
   {
     std::cerr << ex.what() << std::endl;
     return 1;
   }

   Gtk::Window* main_win = 0;
   builder->get_widget("main_window", main_win);

   if (main_win)
   {
     // Build our arrays of arpeggiators and UIs
     arp_runner *runners[MAX_ARPS+1];
     arpui          *uis[MAX_ARPS+1];

     // null-terminate the arrays so we can safely loop thru them...
     // we don't need no stinkin' vectors
     runners[MAX_ARPS] = NULL;
         uis[MAX_ARPS] = NULL;

     char prefix[5];

     for ( int i=0; i<MAX_ARPS; i++ ) {

        // each arp-specific UI widget is named with the prefix arpN, so the
        // arpui constructor gets this prefix, and pulls the appropriate
        // widget from the UI config file.
        sprintf(prefix,"arp%1d",i+1);

        runners[i] = new arp_runner(i,0,0.25);
        uis[i]     = new arpui(prefix, builder, runners[i], &kit);

        // 1st arp is active by default.  others are inactive.
        runners[i]->active( !i );
        uis[i]->arpactive->set_active( !i );

        // set defaults for all arps
        runners[i]->channelized( false );
        uis[i]->on_active_change();
        uis[i]->arpchannel->set_value( i+1 );
        uis[i]->arpprogram->set_value( 0 );
        uis[i]->arpchannelized->set_active( false );
        uis[i]->arplatchlock->set_active( true );
        uis[i]->arpnoteduration->set_active(arpui::NOTEDUR_16);

        uis[i]->arprange->set_value( 0 );
        uis[i]->arpinterval->set_value( 0 );
        uis[i]->arptranspose->set_value( 0 );
        uis[i]->arpscale->set_active( 13 );

        // every 3ms update the blinky lights on the UI :)
        Glib::signal_timeout().connect( sigc::mem_fun(uis[i],&arpui::note_sounding_actions),3);

        //every 3ms update the bar/beat/tick counters on the UI
        //
        // The first arp UI handles the "global" UI updates (i.e. there is only
        // one bar/beat/tick display widget - let UI #1 handle it.
        //
        if ( !i ) {
          Glib::signal_timeout().connect( sigc::mem_fun(uis[i],&arpui::pos_actions),3);
        }

        try
        {
           std::string dirname = Glib::get_home_dir() + "/.arpage/";
           mkdir(dirname.c_str(), 0770);
           
           std::string filename = dirname + uis[i]->arpprefix + ".xml";
           std::ifstream fs;
           fs.open(filename.c_str(), std::ios::in);
           if ( ! fs.fail() ) {
              fs.close();
              arpstorage parser(uis[i]);
              parser.set_substitute_entities(true); //
              parser.parse_file( dirname + uis[i]->arpprefix + ".xml" );
           }
        }
        catch(const xmlpp::exception& ex)
        {
           std::cout << "libxml++ exception: " << ex.what() << std::endl;
        }
     }

     // initialize the JACK connections
     jack_client_t  * jc = midi_init( runners );

     //  ! Go Go Go !
     kit.run(*main_win);

     // un-initialize the JACK connections
     midi_uninit(jc);
   }

   exit(0);
}
