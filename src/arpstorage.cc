/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*- */
/*
 * arpstorage.cc
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
#include "arpstorage.h"

#include <glibmm/convert.h> //For Glib::ConvertError
#include <glibmm/stringutils.h> //For Glib::Ascii::strtod
#include <libxml++/libxml++.h>

//============================================================================
#include <fstream>
#include <iostream>


//============================================================================
arpstorage::arpstorage( arpui * pui ) 
  : xmlpp::SaxParser(), ui(pui)
{
}

arpstorage::~arpstorage()
{
}

void arpstorage::on_start_document()
{
//  std::cout << "on_start_document()" << std::endl;
}

void arpstorage::on_end_document()
{
//  std::cout << "on_end_document()" << std::endl;
}

void arpstorage::on_start_element(const Glib::ustring& name,
                                   const AttributeList& attributes)
{
   //std::cout << "on_start_element: " << name << std::endl;
   if ( name == "channel" ) {
      ui->arpchannel->set_value( get_attribute_value("setting", attributes) );
      ui->on_channel_change();
   } else if ( name == "patch" ) {
      ui->arpprogram->set_value( get_attribute_value("setting", attributes) );
      ui->on_program_change();
   } else if ( name == "velocity" ) {
      ui->arpvelocity->set_value( get_attribute_value("setting", attributes) );
      ui->on_velocity_change();
   } else if ( name == "range" ) {
      ui->arprange->set_value( get_attribute_value("setting", attributes) );
      ui->on_range_change();
   } else if ( name == "interval" ) {
      ui->arpinterval->set_value( get_attribute_value("setting", attributes) );
      ui->on_interval_change();
   } else if ( name == "transpose" ) {
      ui->arptranspose->set_value( get_attribute_value("setting", attributes) );
      ui->on_transpose_change();
   } else if ( name == "scale" ) {
      ui->arpscale->set_active( get_attribute_value("setting", attributes) );
      ui->on_scale_change();
   } else if ( name == "notepercent" ) {
      ui->arpnotepercent->set_value( get_attribute_value("setting", attributes) );
      ui->on_notepercent_change();
   } else if ( name == "active" ) {
      ui->arpactive->set_active( get_attribute_value("setting", attributes) );
      ui->on_active_change();
   } else if ( name == "channelized" ) {
      ui->arpchannelized->set_active( get_attribute_value("setting", attributes) );
      ui->on_channelized_change();
   } else if ( name == "tapout" ) {
      ui->arptapout->set_active( get_attribute_value("setting", attributes) );
      ui->on_tapout_change();
   } else if ( name == "lockedlatching" ) {
      ui->arplatchlock->set_active( get_attribute_value("setting", attributes) );
      ui->on_latchlock_change();
   } else if ( name == "tapin" ) {
      ui->arptapout->set_active( get_attribute_value("setting", attributes) );
      ui->on_tapout_change();
   } else if ( name == "noteorder" ) {
      ui->arpnoteorder->set_active( get_attribute_value("setting", attributes) );
      ui->on_noteorder_change();
   } else if ( name == "notevalue" ) {
      ui->arpnoteduration->set_active( get_attribute_value("setting", attributes) );
      ui->on_noteduration_change();
   }
   
}

int arpstorage::get_attribute_value( const std::string & name, const AttributeList& attributes ) 
{
   // Print attributes:
   int value = 0;
   for(xmlpp::SaxParser::AttributeList::const_iterator iter = attributes.begin(); iter != attributes.end(); ++iter)
   {
      if ( iter->name == name ) {
         Glib::ustring svalue = iter->value;
         value = (int)Glib::Ascii::strtod(iter->value);
         //std::cout << "attribute: '" << iter->name << "' is '" << value << "'" << std::endl;
      } else {
         std::cout << "unexpected attribute: '" << iter->name  << "'" << std::endl;
      }
   }
   return value;
}

void arpstorage::on_end_element(const Glib::ustring& name)
{
//  std::cout << "on_end_element()" << std::endl;
}


void arpstorage::on_characters(const Glib::ustring& text)
{
//  try
//  {
//    std::cout << "on_characters(): " << text << std::endl;
//  }
//  catch(const Glib::ConvertError& ex)
//  {
//    std::cerr << "arpstorage::on_characters(): Exception caught while converting text for std::cout: " << ex.what() << std::endl;
//  }
}

void arpstorage::on_comment(const Glib::ustring& text)
{
//  try
//  {
//    std::cout << "on_comment(): " << text << std::endl;
//  }
//  catch(const Glib::ConvertError& ex)
//  {
//    std::cerr << "arpstorage::on_comment(): Exception caught while converting text for std::cout: " << ex.what() << std::endl;
//  }
}

void arpstorage::on_warning(const Glib::ustring& text)
{
  try
  {
    std::cout << "on_warning(): " << text << std::endl;
  }
  catch(const Glib::ConvertError& ex)
  {
    std::cerr << "arpstorage::on_warning(): Exception caught while converting text for std::cout: " << ex.what() << std::endl;
  }
}

void arpstorage::on_error(const Glib::ustring& text)
{
  try
  {
    std::cout << "on_error(): " << text << std::endl;
  }
  catch(const Glib::ConvertError& ex)
  {
    std::cerr << "arpstorage::on_error(): Exception caught while converting text for std::cout: " << ex.what() << std::endl;
  }
}

void arpstorage::on_fatal_error(const Glib::ustring& text)
{
  try
  {
    std::cout << "on_fatal_error(): " << text << std::endl;
  }
  catch(const Glib::ConvertError& ex)
  {
    std::cerr << "arpstorage::on_characters(): Exception caught while converting value for std::cout: " << ex.what() << std::endl;
  }
}
//============================================================================

void write_setting( const std::string & name, std::ofstream & fs, int value ) {
   fs << "   <" << name << " setting=" << '"' << value << '"' << "/>"
      << std::endl;
}

//============================================================================
bool arpstorage::store_session( )
{
   bool retval = false;
   std::string fname = Glib::get_home_dir() + "/.arpage/";
   fname += ui->arpprefix + ".xml";

   std::ofstream session_file;
   session_file.open(fname.c_str() );

   if ( session_file ) {
      session_file << "<arpeggiator setting=" << '"' << ui->arpprefix 
                   << '"' << ">" << std::endl;
      write_setting( "channel", session_file,
       ui->arpchannel->get_value_as_int() );
      write_setting("patch", session_file,
       ui->arpprogram->get_value_as_int() );
      write_setting("velocity", session_file,
       ui->arpvelocity->get_value_as_int() );
      write_setting("range", session_file,
       ui->arprange->get_value_as_int() );
      write_setting("interval", session_file,
       ui->arpinterval->get_value_as_int() );
      write_setting("transpose", session_file,
       ui->arptranspose->get_value_as_int() );
      write_setting("scale", session_file,
       ui->arpscale->get_active_row_number() );
      write_setting("notepercent", session_file,
       ui->arpnotepercent->get_value() );
      write_setting("active", session_file,
       ui->arpactive->get_active() );
      write_setting("channelized", session_file,
       ui->arpchannelized->get_active() );
      write_setting("tapout", session_file,
       ui->arptapout->get_active() );
      write_setting("lockedlatching", session_file,
       ui->arplatchlock->get_active() );
      write_setting("tapin", session_file,
       ui->arptapout->get_active() );
      write_setting("noteorder", session_file,
       ui->arpnoteorder->get_active_row_number() );
      write_setting("notevalue", session_file,
       ui->arpnoteduration->get_active_row_number() );
      session_file << "</arpeggiator>" << std::endl;
      session_file.flush();
      session_file.close();
      retval = true;
   }
   return retval;
}

