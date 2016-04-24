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

#ifndef CPPTRANSPORT_RECORD_DECL_H
#define CPPTRANSPORT_RECORD_DECL_H


#include "transport-runtime/repository/detail/repository_mode.h"
#include "transport-runtime/transactions/transaction_manager.h"
#include "transport-runtime/manager/environment.h"

#include "boost/optional.hpp"


namespace transport
  {

    // GENERIC REPOSITORY RECORD


    //! Encapsulates common details shared by all repository records
    class repository_record: public serializable
      {

      public:

        typedef std::function<void(repository_record&, transaction_manager&)> commit_handler;

        //! handler_package encapsulates all details for further processing of this repository record,
        //! eg. to (re-)commit it to the database
        class handler_package
          {

            // CONSTRUCTOR, DESTRUCTOR

          public:

            //! constructor requires values for all arguments
            handler_package(commit_handler c, local_environment& e, boost::optional<transaction_manager&> m)
              : commit(std::move(c)),
                env(e),
                mgr(m)
              {
              }

            // DATA

          public:

            // POLICY CLASSES

            //! local environment
            local_environment& env;

            //! commit handler
            const commit_handler commit;


            // TRANSACTION SUPPORT

            //! transaction manager, if provided
            //! if no transaction manager is supplied, the record is in a readonly mode
            boost::optional<transaction_manager&> mgr;

          };

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! construct a repository record with a default name (taken from its creation time)
        repository_record(handler_package pkg);

        //! construct a repository record with a supplied name
        repository_record(const std::string& nm, repository_record::handler_package pkg);

        //! deserialization constructor
        repository_record(Json::Value& reader, repository_record::handler_package pkg);

        //! destructor is default
        virtual ~repository_record() = default;


        // NAME HANDLING

      public:

        //! Get record name
        const std::string get_name() const { return(this->name); }

        //! Set record time from creation-time metadata
        void set_name(const std::string& n) { this->name = n; }


        // GET AND SET METADATA -- delegated to the metadata container

      public:

        //! get creation time
        const boost::posix_time::ptime& get_creation_time() const { return(this->metadata.get_creation_time()); }

        //! set creation time (use with care; needed to timestamp created content group records correctly)
        void set_creation_time(const boost::posix_time::ptime& t) { this->metadata.set_creation_time(t); }

        //! get last-edit time
        //! no option to set last-edit time since that action is performed by any methods
        //! which change the record
        const boost::posix_time::ptime& get_last_edit_time() const { return(this->metadata.get_last_edit_time()); }

        //! get runtime API version
        unsigned int get_runtime_API_version() const { return(this->metadata.get_runtime_API_version()); }

        //! get history for this record
        const std::list< metadata_history >& get_history() const { return(this->metadata.get_history()); }


        // COMMIT TO DATABASE

      public:

        //! Commit this record to the database; note can't be marked const because some commit functions
        //! need to write to the repository record (eg. to set k-configuration database paths)
        void commit();


        // SERIALIZATION -- implements a 'serializable' interface

      public:

        //! serialize this object
        virtual void serialize(Json::Value& writer) const override;


        // CLONE

      public:

        //! clone this object
        virtual repository_record* clone() const = 0;


        // INTERNAL DATA

      protected:

        //! Name
        std::string name;

        //! Metadata record
        record_metadata metadata;

        //! Handler package
        handler_package handlers;

      };


    namespace record_helper
      {

        // used for sorting a list of records into lexicographic order
        template <typename RecordType>
        bool comparator(const std::unique_ptr<RecordType>& A, const std::unique_ptr<RecordType>& B)
          {
            return(A->get_name() < B->get_name());
          }

      }   // namespace content_group_helper

  }   // namespace transport


#endif //CPPTRANSPORT_RECORD_DECL_H
