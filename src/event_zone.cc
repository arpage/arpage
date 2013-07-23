
#include <jack/jack.h>
#include <jack/midiport.h>

#include <gtkmm.h>
#include <iostream>
#include <iomanip>
#include <vector>
#include <cstring>

#include "arpage.h"
#include "midimsg.h"
#include "ccrange.h"

/* For testing propose use the local (not installed) ui file */
#define UI_FILE PACKAGE_DATA_DIR"/arpage/ui/zonage.ui"
//#define UI_FILE "src/zonage.ui"

#define MAX_ZONES 4

using namespace std;


//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void * port_get_buffer( jack_port_t * p, jack_nframes_t nframes ) {
   void * port_buffer = jack_port_get_buffer( p, nframes );
   if (port_buffer == NULL) {
      cout << "jack_port_get_buffer failed!" << endl;
   }
   return port_buffer;
}

//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------
class event_zone {
public:

   jack_client_t * client;
   jack_port_t   * out_port;
   jack_port_t   * in_port;
   void          * out_port_buff;
   void          * in_port_buff;

   int active;
   int note_hi;
   int note_lo;
   int velocity;
   int this_program;
   int program;
   int channelized;
   int transpose;
   int channel_out;
   int ccmap[128];

   event_zone(): client(NULL), out_port(NULL), in_port(NULL),
                 out_port_buff(NULL), in_port_buff(NULL),
                 active(1), note_hi(127), note_lo(0), velocity(0),
                 this_program(0), program(0), channelized(0), transpose(0), channel_out(0) {
      for ( int i=0; i<(sizeof(ccmap)/sizeof(ccmap[0])); i++ ) {
         ccmap[i] = i;
      }
   }

   event_zone(const event_zone & rhs) {
      this->operator=(rhs);
   }

   event_zone & operator = (const event_zone & rhs) {
      client  = rhs.client;
      in_port = out_port = NULL;
      out_port_buff      = in_port_buff = NULL;
      note_hi      = rhs.note_hi;
      note_lo      = rhs.note_lo;
      velocity     = rhs.velocity;
	   this_program = rhs.this_program;
	   program      = rhs.program;
	   channelized  = rhs.channelized;
      transpose    = rhs.transpose;
      channel_out  = rhs.channel_out;
      for ( int i=0; i<(sizeof(ccmap)/sizeof(ccmap[0])); i++ ) {
         ccmap[i] = rhs.ccmap[i];
      }
      return *this;
   }

   void
   refresh_buffers( jack_nframes_t nframes ) {
      // refresh midi i/o buffers at the start of each cycle
      in_port_buff  = port_get_buffer(  in_port, nframes );
      out_port_buff = port_get_buffer( out_port, nframes );
   }

};

//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------
void
process_midi_input( jack_nframes_t nframes, event_zone * zone )
{
   if ( ! zone ) return;
   event_zone * pz = zone;

   int read, events, i;
   jack_midi_event_t event;
   events = jack_midi_get_event_count( pz->in_port_buff );

   for (i = 0; i < events; i++) {
      read = jack_midi_event_get(&event, pz->in_port_buff, i);
      if (read) {
         cout << "jack_midi_event_get failed, RECEIVED NOTE LOST." << endl;
         continue;
      }
      if (event.size > 3) {
         //cout << 
		   //  "Ignoring MIDI message longer than three bytes, probably a SysEx." 
		   //  << endl;
         continue;
      }

      MidiMessage msg(&event);
      unsigned char *eb;
      if ( (msg.channel_message() == MIDI_NOTE_ON) && (msg.data[2] > 0) ) {
      //( msg.channel_message() == MIDI_NOTE_ON ) {
         if ( pz->note_hi >= msg.note_number() && pz->note_lo <= msg.note_number() ) {
            //cout << "Event " << i+1 << " Is Note On" << endl;
            eb = jack_midi_event_reserve(pz->out_port_buff, NFRAMES, 3);
            make_note( pz->channel_out, msg.note_number()+pz->transpose, 
                 pz->velocity? pz->velocity : msg.data[2], 1, eb);
         }
      } else if ((msg.channel_message() == MIDI_NOTE_OFF) || ((msg.channel_message() == MIDI_NOTE_ON) && (msg.data[2] == 0)) ) {
      //} else if ( msg.channel_message() == MIDI_NOTE_OFF ) {
         if ( pz->note_hi >= msg.note_number() && pz->note_lo <= msg.note_number() ) {
            //cout << "Event " << i+1 << " Is Note Off" << endl;
            eb = jack_midi_event_reserve(pz->out_port_buff, NFRAMES, 3);
            make_note(pz->channel_out, msg.note_number()+pz->transpose, 
                      pz->velocity? pz->velocity : msg.data[2], 0, eb );
         }
      } else if ( msg.channel_message() == MIDI_CONTROLLER ) {
         //cout << "Event " << i+1 << " Is Note Ctl" << endl;
         eb = jack_midi_event_reserve(pz->out_port_buff, NFRAMES, 3);
         make_msg(pz->ccmap[msg.data[1]],pz->channel_out, msg.data[2], eb);
      } else {
         //cout << "Event " << i+1 << " Is ??? Passing Thru" << endl;
         eb = jack_midi_event_reserve( pz->out_port_buff, nframes, 3);
         memcpy(eb,event.buffer,event.size);
      }
   }
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void
shutdown_callback(void *zone) {

   event_zone ** zones = (event_zone **)zone;
   if ( ! zones ) return;

   while ( *zones ) {
      event_zone * pz = *zones++;
      pz->in_port_buff = NULL;
      pz->out_port_buff = NULL;
      delete pz;
   }

}

//-----------------------------------------------------------------------------
// This is the audio process callback function.  Here we determine if the
// JACK transport is rolling, and do the right thing accordingly.
//-----------------------------------------------------------------------------
int
process_callback(jack_nframes_t nframes, void *zone)
{
   event_zone ** zones = (event_zone **)zone;
   if ( ! zones ) return 0;

   event_zone ** head = zones;
   while ( *head ) {
      event_zone *pz = *head++;
      if ( ! pz->active )
         continue;
      pz->refresh_buffers(NFRAMES);
      jack_midi_clear_buffer( pz->out_port_buff );
      if ( pz->this_program != pz->program ) {
         unsigned char *eb = jack_midi_event_reserve(pz->out_port_buff, NFRAMES, 3);
         make_pc( pz->channel_out, pz->program, eb );
         pz->this_program = pz->program;
      }
      process_midi_input( NFRAMES, pz );
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
midi_init( event_zone ** zones )
{
   jack_client_t *jack_client=jack_client_open("zonage", JackNullOption, NULL);

   if (!jack_client) {
      cout << "Could not connect to JACK server" << endl;
      exit(1); //FIXME: exit points?
   }
 
   event_zone **head = zones;
   while ( *head ) {
      event_zone * pz = *head++;
      pz->client = jack_client;
   }

   int err = 
		jack_set_process_callback(jack_client, process_callback, (void*)zones);

   if (err) {
      cout << "Could not register JACK process callback." << endl;
      exit(1);
   }

   jack_on_shutdown(jack_client,shutdown_callback,(void*)zones);

   head = zones;
   char portname[5];
   int  portnum = 1;

   while ( *head ) {
      event_zone * pz = *head++;

      sprintf(portname,"in%d", portnum );

      jack_port_t * input_port =
         jack_port_register(jack_client, portname,
                            JACK_DEFAULT_MIDI_TYPE, JackPortIsInput, 0);

      if (input_port == NULL) {
         cout << "Could not register JACK input port." << endl;
         exit(1);
      }

      pz->in_port = input_port;

      sprintf(portname,"out%d", portnum++ );

      jack_port_t * output_port =
         jack_port_register(jack_client, portname,
                            JACK_DEFAULT_MIDI_TYPE, JackPortIsOutput, 0);

      if (output_port == NULL) {
         cout << "Could not register JACK output port." << endl;
         exit(1);
      }

      pz->out_port = output_port;
   }

   if (jack_activate(jack_client)) {
      cout << "Cannot activate JACK client." << endl;
      exit(1);
   }

   return jack_client;
}

//=============================================================================
// Menu handler for File->Quit
void appquit( Gtk::Window * pAppWin )
{
   if(pAppWin)
   {
      pAppWin->hide(); //hide() will cause main::run() to end.
   }
}

struct zoneui {

   event_zone       * zone;
   
   Gtk::SpinButton  * channel;
   Gtk::SpinButton  * transpose;
   Gtk::SpinButton  * program;
   Gtk::SpinButton  * velocity;

   Gtk::SpinButton  * notehi;
   Gtk::SpinButton  * notelo;
   
   Gtk::CheckButton * active;
   Gtk::CheckButton * channelized;
   
   Gtk::SpinButton  * cc1in;
   Gtk::SpinButton  * cc1out;
   Gtk::SpinButton  * cc2in;
   Gtk::SpinButton  * cc2out;
   Gtk::SpinButton  * cc3in;
   Gtk::SpinButton  * cc3out;
   Gtk::SpinButton  * cc4in;
   Gtk::SpinButton  * cc4out;
   
   zoneui( const string & prefix, 
          Glib::RefPtr<Gtk::Builder> & builder, event_zone * pz );
   
   void on_channel_change() {
      zone->channel_out = channel->get_value_as_int() - 1;
   }
   
   void on_transpose_change() {
      zone->transpose = transpose->get_value_as_int();
   }
   
   void on_program_change() {
      cout << "Pre prog: " << zone->program << endl;
      zone->program = program->get_value_as_int();
      cout << "Pst prog: " << zone->program << endl;
      cout << "THS prog: " << zone->this_program << endl;
   }
   
   void on_velocity_change() {
      zone->velocity = velocity->get_value_as_int();
   }
   
   void on_active_change() {
      zone->active = active->get_active();
   }
   
   void on_channelized_change() {
      zone->channelized = channelized->get_active();
   }

   void on_notehi_change() {
      zone->note_hi = notehi->get_value_as_int();
   }

   void on_notelo_change() {
      zone->note_lo = notelo->get_value_as_int();
   }

   void on_cc1in_change() {
      zone->ccmap [ cc1in->get_value_as_int() ] = cc1out->get_value_as_int();
   }
   void on_cc1out_change() {
      zone->ccmap [ cc1in->get_value_as_int() ] = cc1out->get_value_as_int();
   }
   void on_cc2in_change() {
      zone->ccmap [ cc2in->get_value_as_int() ] = cc2out->get_value_as_int();
   }
   void on_cc2out_change() {
      zone->ccmap [ cc2in->get_value_as_int() ] = cc2out->get_value_as_int();
   }
   void on_cc3in_change() {
      zone->ccmap [ cc3in->get_value_as_int() ] = cc3out->get_value_as_int();
   }
   void on_cc3out_change() {
      zone->ccmap [ cc3in->get_value_as_int() ] = cc3out->get_value_as_int();
   }
   void on_cc4in_change() {
      zone->ccmap [ cc4in->get_value_as_int() ] = cc4out->get_value_as_int();
   }
   void on_cc4out_change() {
      zone->ccmap [ cc4in->get_value_as_int() ] = cc4out->get_value_as_int();
   }

};

zoneui::zoneui( const string & prefix, 
          Glib::RefPtr<Gtk::Builder> & builder, event_zone * pz ) {

   zone = pz;
      
   builder->get_widget( prefix+"channel",     channel);
   builder->get_widget( prefix+"patch",       program); 
   builder->get_widget( prefix+"transpose",   transpose);
   builder->get_widget( prefix+"velocity",    velocity);
   builder->get_widget( prefix+"active",      active );
   builder->get_widget( prefix+"channelized", channelized );
   builder->get_widget( prefix+"notehi",      notehi  );
   builder->get_widget( prefix+"notelo",      notelo );
   builder->get_widget( prefix+"cc1in",       cc1in  );
   builder->get_widget( prefix+"cc1out",      cc1out );
   builder->get_widget( prefix+"cc2in",       cc2in  );
   builder->get_widget( prefix+"cc2out",      cc2out );
   builder->get_widget( prefix+"cc3in",       cc3in  );
   builder->get_widget( prefix+"cc3out",      cc3out );
   builder->get_widget( prefix+"cc4in",       cc4in  );
   builder->get_widget( prefix+"cc4out",      cc4out );

   channel->signal_value_changed().connect( sigc::mem_fun(*this,&zoneui::on_channel_change) );
   channel->set_range(1,16);

   transpose->signal_value_changed().connect( sigc::mem_fun(*this,&zoneui::on_transpose_change) );
   transpose->set_range(-24,24);

   program->signal_value_changed().connect( sigc::mem_fun(*this,&zoneui::on_program_change) );
   program->set_range(0,127);

   velocity->signal_value_changed().connect( sigc::mem_fun(*this,&zoneui::on_velocity_change) );
   velocity->set_range(0,127);
   
   active->signal_toggled().connect( sigc::mem_fun(*this,&zoneui::on_active_change) );
   channelized->signal_toggled().connect( sigc::mem_fun(*this,&zoneui::on_channelized_change) );

   notehi->signal_value_changed().connect( sigc::mem_fun(*this,&zoneui::on_notehi_change) );
   notehi->set_range(0,127);

   notelo->signal_value_changed().connect( sigc::mem_fun(*this,&zoneui::on_notelo_change) );
   notelo->set_range(0,127);

   cc1in->signal_value_changed().connect( sigc::mem_fun(*this,&zoneui::on_cc1in_change) );
   cc1in->set_range(0,127);
   cc1out->signal_value_changed().connect( sigc::mem_fun(*this,&zoneui::on_cc1out_change) );
   cc1out->set_range(0,127);

   cc2in->signal_value_changed().connect( sigc::mem_fun(*this,&zoneui::on_cc2in_change) );
   cc2in->set_range(0,127);
   cc2out->signal_value_changed().connect( sigc::mem_fun(*this,&zoneui::on_cc2out_change) );
   cc2out->set_range(0,127);

   cc3in->signal_value_changed().connect( sigc::mem_fun(*this,&zoneui::on_cc3in_change) );
   cc3in->set_range(0,127);
   cc3out->signal_value_changed().connect( sigc::mem_fun(*this,&zoneui::on_cc3out_change) );
   cc3out->set_range(0,127);

   cc4in->signal_value_changed().connect( sigc::mem_fun(*this,&zoneui::on_cc4in_change) );
   cc4in->set_range(0,127);
   cc4out->signal_value_changed().connect( sigc::mem_fun(*this,&zoneui::on_cc4out_change) );
   cc4out->set_range(0,127);
}

//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------
int
main (int argc, char *argv[])
{
  Gtk::Main kit(argc, argv);

  //Load the Glade file and instiate its widgets:
  Glib::RefPtr<Gtk::Builder> builder;
  try
  {
    builder = Gtk::Builder::create_from_file(UI_FILE);
  }
  catch (const Glib::FileError & ex)
  {
    cerr << ex.what() << endl;
    return 1;
  }

  Gtk::MenuItem * pQuit;

  Gtk::Window* main_win = 0;
  builder->get_widget("main_window", main_win);
  if (main_win)
  {

    // Bind Menu Items
    //builder->get_widget( "appquit", pQuit );
    //pQuit->signal_activate().connect(
    //  sigc::bind<Gtk::Window*>( sigc::ptr_fun1(appquit), main_win) );

    // Build out our arrays of zones
    event_zone *zones[MAX_ZONES+1];
    zoneui       *uis[MAX_ZONES+1];

    zones[MAX_ZONES] = NULL;
      uis[MAX_ZONES] = NULL;

    char prefix[6];

    for ( int i=0; i<MAX_ZONES; i++ ) {

       sprintf(prefix,"zone%1d",i+1);
       zones[i] = new event_zone;
       zones[i]->active = true;
         uis[i] = new zoneui( prefix, builder, zones[i] );
    }

    


    jack_client_t  * jc = midi_init( zones );
    kit.run(*main_win);
    midi_uninit(jc);
  }

  exit(0);
}

//=============================================================================
//
//=============================================================================