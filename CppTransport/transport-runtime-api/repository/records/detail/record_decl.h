//
// Created by David Seery on 25/01/2016.
// Copyright (c) 2016 University of Sussex. All rights reserved.
//

#ifndef CPPTRANSPORT_RECORD_DECL_H
#define CPPTRANSPORT_RECORD_DECL_H


#include "transport-runtime-api/repository/detail/repository_mode.h"
#include "transport-runtime-api/repository/transaction_manager.h"


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
            handler_package(commit_handler c, record_mode m)
              : commit(std::move(c)),
                mode(m)
              {
              }

            // DATA

          public:

            //! access mode
            const record_mode mode;

            //! commit handler
            const commit_handler commit;

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

        //! set creation time (use with care)
        void set_creation_time(const boost::posix_time::ptime& t) { this->metadata.set_creation_time(t); }

        //! get last-edit time
        const boost::posix_time::ptime& get_last_edit_time() const { return(this->metadata.get_last_edit_time()); }

        //! reset last-edit time to now
        void update_last_edit_time() { this->metadata.update_last_edit_time(); }

        //! get runtime API version
        unsigned int get_runtime_API_version() const { return(this->metadata.get_runtime_API_version()); }


        // COMMIT TO DATABASE

      public:

        //! Commit this record to the database; note can't be marked const because some commit functions
        //! need to write to the repository record (eg. to set k-configuration database paths)
        void commit(transaction_manager& mgr);


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

      }   // namespace output_group_helper

  }   // namespace transport


#endif //CPPTRANSPORT_RECORD_DECL_H
