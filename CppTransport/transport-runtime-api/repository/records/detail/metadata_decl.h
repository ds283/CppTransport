//
// Created by David Seery on 25/01/2016.
// Copyright (c) 2016 University of Sussex. All rights reserved.
//

#ifndef CPPTRANSPORT_METADATA_DECL_H
#define CPPTRANSPORT_METADATA_DECL_H


namespace transport
  {

    // REPOSITORY METADATA RECORD


    //! Encapsulates metadata for a record stored in the repository
    class record_metadata: public serializable
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! construct a metadata record
        record_metadata();

        //! deserialization constructor
        record_metadata(Json::Value& reader);

        virtual ~record_metadata() = default;


        // GET AND SET METADATA

      public:

        //! get creation time
        const boost::posix_time::ptime& get_creation_time() const { return(this->creation_time); }

        //! set creation time (to be used with care)
        void set_creation_time(const boost::posix_time::ptime& t) { this->creation_time = t; }

        //! get last-edit time
        const boost::posix_time::ptime& get_last_edit_time() const { return(this->last_edit_time); }

        //! reset last-edit time to now
        void update_last_edit_time() { this->last_edit_time = boost::posix_time::second_clock::universal_time(); }

        //! get runtime API version
        unsigned int get_runtime_API_version() const { return(this->runtime_api); }


        // SERIALIZATION -- implements a 'serializable' interface

      public:

        virtual void serialize(Json::Value& writer) const override;


        // INTERNAL DATA

      protected:

        //! creation time
        boost::posix_time::ptime creation_time;

        //! last-edited time
        boost::posix_time::ptime last_edit_time;

        //! version of runtime API used to create this record
        unsigned int runtime_api;

      };

  }   // namespace transport


#endif //CPPTRANSPORT_METADATA_DECL_H
