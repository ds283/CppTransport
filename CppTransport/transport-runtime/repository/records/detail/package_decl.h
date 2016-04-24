//
// Created by David Seery on 25/01/2016.
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

#ifndef CPPTRANSPORT_PACKAGE_DECL_H
#define CPPTRANSPORT_PACKAGE_DECL_H


namespace transport
  {

    // PACKAGE RECORD


    //! Encapsulates metadata for a package record stored in the repository
    template <typename number>
    class package_record: public repository_record
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! construct a package record
        package_record(const initial_conditions<number>& i, repository_record::handler_package& pkg);

        //! deserialization constructor
        package_record(Json::Value& reader, model_manager <number>& f, repository_record::handler_package& pkg);

        virtual ~package_record() = default;


        // GET CONTENTS

      public:

        //! Get initial conditionss
        const initial_conditions<number>& get_ics() const { return(this->ics); }


        // SERIALIZATION -- implements a 'serializable' interface

      public:

        //! serialize this object
        virtual void serialize(Json::Value& writer) const override;


        // CLONE

      public:

        //! clone this object
        virtual package_record<number>* clone() const override { return new package_record<number>(static_cast<const package_record<number>&>(*this)); };


        // INTERNAL DATA

      protected:

        //! Initial conditions data associated with this package
        initial_conditions<number> ics;

      };

  }   // namespace transport


#endif //CPPTRANSPORT_PACKAGE_DECL_H
