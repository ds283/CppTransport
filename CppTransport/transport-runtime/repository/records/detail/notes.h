//
// Created by David Seery on 07/04/2016.
// --@@
// Copyright (c) 2016 University of Sussex. All rights reserved.
//
// This file is part of the CppTransport platform.
//
// CppTransport is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 2 of the License, or
// (at your option) any later version.
//
// CppTransport is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with CppTransport.  If not, see <http://www.gnu.org/licenses/>.
//
// @license: GPL-2
// @contributor: David Seery <D.Seery@sussex.ac.uk>
// --@@
//

#ifndef CPPTRANSPORT_NOTES_H
#define CPPTRANSPORT_NOTES_H


#include <string>

#include "transport-runtime/serialization/serializable.h"

#include "boost/date_time.hpp"

#include "json/json.h"


namespace transport
  {


    constexpr auto CPPTRANSPORT_NODE_NOTE_UID = "uid";
    constexpr auto CPPTRANSPORT_NODE_NOTE_NOTE = "note";
    constexpr auto CPPTRANSPORT_NODE_NOTE_TIMESTAMP = "timestamp";


    class note: public serializable
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! value constructor
        note(std::string u, std::string n);

        //! deserialization constructor
        note(Json::Value& reader);

        //! destructor is default
        ~note() = default;


        // INTERFACE

      public:

        //! get uid
        const std::string& get_uid() const { return(this->uid); }

        //! get note
        const std::string& get_note() const { return(this->text); }

        //! get timestamp
        boost::posix_time::ptime get_timestamp() const { return(this->timestamp); }


        // SERIALIZATION INTERFACE

        //! Serialize
        virtual void serialize(Json::Value& writer) const override;


        // INTERNAL DATA

      private:

        //! uid of depositer
        std::string uid;

        //! note
        std::string text;

        //! timestamp
        boost::posix_time::ptime timestamp;

      };


    note::note(std::string u, std::string n)
      : uid(std::move(u)),
        text(std::move(n)),
        timestamp(boost::posix_time::second_clock::universal_time())
      {
      }


    note::note(Json::Value& reader)
      : uid(reader[CPPTRANSPORT_NODE_NOTE_UID].asString()),
        text(reader[CPPTRANSPORT_NODE_NOTE_NOTE].asString())
      {
        std::string timestamp_str = reader[CPPTRANSPORT_NODE_NOTE_TIMESTAMP].asString();
        timestamp = boost::posix_time::from_iso_string(timestamp_str);
      }


    void note::serialize(Json::Value& writer) const
      {
        writer[CPPTRANSPORT_NODE_NOTE_UID] = this->uid;
        writer[CPPTRANSPORT_NODE_NOTE_NOTE] = this->text;
        writer[CPPTRANSPORT_NODE_NOTE_TIMESTAMP] = boost::posix_time::to_iso_string(this->timestamp);
      }


  }


#endif //CPPTRANSPORT_NOTES_H
