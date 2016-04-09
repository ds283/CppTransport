//
// Created by David Seery on 25/01/2016.
// Copyright (c) 2016 University of Sussex. All rights reserved.
//

#ifndef CPPTRANSPORT_METADATA_DECL_H
#define CPPTRANSPORT_METADATA_DECL_H


namespace transport
  {

    enum class history_actions
      {
        created,
        add_tag,
        remove_tag,
        add_note,
        remove_note,
        locked,
        unlocked,
        add_content,
        remove_content
      };


    // REPOSITORY HISTORY RECORD
    class metadata_history: public serializable
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! constructor
        metadata_history(std::string u, history_actions a, std::string i="");

        //! deserialization constructor
        metadata_history(Json::Value& reader);

        //! destructor is default
        ~metadata_history() = default;


        // INTERFACE

      public:

        //! convert to tag
        std::string to_string() const;


        // SERIALIZATION -- implements a 'serializable' interface

      public:

        virtual void serialize(Json::Value& writer) const override;


        // INTERNAL DATA

      private:

        //! user id of person making the change
        std::string user_id;

        //! action that was performed
        history_actions action;

        //! extra information
        std::string info;

        //! timestamp
        boost::posix_time::ptime timestamp;

      };


    // REPOSITORY METADATA RECORD


    //! Encapsulates metadata for a record stored in the repository
    class record_metadata: public serializable
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! construct a metadata record
        record_metadata(std::string u);

        //! deserialization constructor
        record_metadata(Json::Value& reader);

        //! destructor is default
        virtual ~record_metadata() = default;


        // GET AND SET METADATA

      public:

        //! get creation time
        const boost::posix_time::ptime& get_creation_time() const { return(this->creation_time); }

        //! set creation time (to be used with care; needed to timestamp created content group records correctly)
        void set_creation_time(const boost::posix_time::ptime& t) { this->creation_time = t; }

        //! get last-edit time
        const boost::posix_time::ptime& get_last_edit_time() const { return(this->last_edit_time); }

        //! reset last-edit time to now
        void update_last_edit_time() { this->last_edit_time = boost::posix_time::second_clock::universal_time(); }

        //! get runtime API version
        unsigned int get_runtime_API_version() const { return(this->runtime_api); }

        //! add history item
        void add_history_item(std::string u, history_actions a, std::string i="") { this->history.emplace_back(u, a, i); }

        //! get history
        const std::list< metadata_history >& get_history() const { return(this->history); }


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

        //! history
        std::list< metadata_history > history;

      };

  }   // namespace transport


#endif //CPPTRANSPORT_METADATA_DECL_H
