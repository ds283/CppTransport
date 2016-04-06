//
// Created by David Seery on 25/01/2016.
// Copyright (c) 2016 University of Sussex. All rights reserved.
//

#ifndef CPPTRANSPORT_METADATA_IMPL_H
#define CPPTRANSPORT_METADATA_IMPL_H


namespace transport
  {

    constexpr auto CPPTRANSPORT_NODE_HISTORY_USERID = "uid";
    constexpr auto CPPTRANSPORT_NODE_HISTORY_ACTION = "action";
    constexpr auto CPPTRANSPORT_NODE_HISTORY_INFO = "info";
    constexpr auto CPPTRANSPORT_NODE_HISTORY_TIMESTAMP = "timestamp";

    constexpr auto CPPTRANSPORT_NODE_HISTORY_CREATED = "created";
    constexpr auto CPPTRANSPORT_NODE_HISTORY_ADD_TAG = "add-tag";
    constexpr auto CPPTRANSPORT_NODE_HISTORY_REMOVE_TAG = "remove-tag";
    constexpr auto CPPTRANSPORT_NODE_HISTORY_ADD_NOTE = "add-note";
    constexpr auto CPPTRANSPORT_NODE_HISTORY_REMOVE_NOTE = "remove-note";
    constexpr auto CPPTRANSPORT_NODE_HISTORY_LOCK = "lock";
    constexpr auto CPPTRANSPORT_NODE_HISTORY_UNLOCK = "unlock";
    constexpr auto CPPTRANSPORT_NODE_HISTORY_ADD_CONTENT = "add-content";
    constexpr auto CPPTRANSPORT_NODE_HISTORY_REMOVE_CONTENT = "remove-content";

    constexpr auto CPPTRANSPORT_NODE_METADATA_GROUP = "metadata";
    constexpr auto CPPTRANSPORT_NODE_METADATA_CREATED = "created";
    constexpr auto CPPTRANSPORT_NODE_METADATA_EDITED = "edited";
    constexpr auto CPPTRANSPORT_NODE_METADATA_RUNTIME_API = "api";
    constexpr auto CPPTRANSPORT_NODE_METADATA_HISTORY = "history";


    // METADATA HISTORY METHODS


    metadata_history::metadata_history(std::string u, history_actions a, std::string i)
      : user_id(std::move(u)),
        action(a),
        info(std::move(i)),
        timestamp(boost::posix_time::second_clock::universal_time())
      {
      }


    metadata_history::metadata_history(Json::Value& reader)
      {
        this->user_id = reader[CPPTRANSPORT_NODE_HISTORY_USERID].asString();
        this->info = reader[CPPTRANSPORT_NODE_HISTORY_INFO].asString();

        std::string timestamp_str = reader[CPPTRANSPORT_NODE_HISTORY_TIMESTAMP].asString();
        this->timestamp = boost::posix_time::from_iso_string(timestamp_str);

        std::string action_str = reader[CPPTRANSPORT_NODE_HISTORY_ACTION].asString();
        this->action = history_actions::created;
        if(action_str == std::string(CPPTRANSPORT_NODE_HISTORY_CREATED))              this->action = history_actions::created;
        else if(action_str == std::string(CPPTRANSPORT_NODE_HISTORY_ADD_TAG))         this->action = history_actions::add_tag;
        else if(action_str == std::string(CPPTRANSPORT_NODE_HISTORY_REMOVE_TAG))      this->action = history_actions::remove_tag;
        else if(action_str == std::string(CPPTRANSPORT_NODE_HISTORY_ADD_NOTE))        this->action = history_actions::add_note;
        else if(action_str == std::string(CPPTRANSPORT_NODE_HISTORY_REMOVE_NOTE))     this->action = history_actions::remove_note;
        else if(action_str == std::string(CPPTRANSPORT_NODE_HISTORY_LOCK))            this->action = history_actions::locked;
        else if(action_str == std::string(CPPTRANSPORT_NODE_HISTORY_UNLOCK))          this->action = history_actions::unlocked;
        else if(action_str == std::string(CPPTRANSPORT_NODE_HISTORY_ADD_CONTENT))     this->action = history_actions::add_content;
        else if(action_str == std::string(CPPTRANSPORT_NODE_HISTORY_REMOVE_CONTENT))  this->action = history_actions::remove_content;
        else
          {
            std::ostringstream msg;
            msg << CPPTRANSPORT_UNKNOWN_METADATA_HISTORY_ACTION << " '" << action_str << "'";
            throw runtime_exception(exception_type::SERIALIZATION_ERROR, msg.str());
          }
      }


    void metadata_history::serialize(Json::Value& writer) const
      {
        writer[CPPTRANSPORT_NODE_HISTORY_USERID] = this->user_id;
        writer[CPPTRANSPORT_NODE_HISTORY_INFO] = this->info;
        writer[CPPTRANSPORT_NODE_HISTORY_TIMESTAMP] = boost::posix_time::to_iso_string(this->timestamp);

        switch(this->action)
          {
            case history_actions::created:
              {
                writer[CPPTRANSPORT_NODE_HISTORY_ACTION] = std::string(CPPTRANSPORT_NODE_HISTORY_CREATED);
                break;
              }

            case history_actions::add_tag:
              {
                writer[CPPTRANSPORT_NODE_HISTORY_ACTION] = std::string(CPPTRANSPORT_NODE_HISTORY_ADD_TAG);
                break;
              }

            case history_actions::remove_tag:
              {
                writer[CPPTRANSPORT_NODE_HISTORY_ACTION] = std::string(CPPTRANSPORT_NODE_HISTORY_REMOVE_TAG);
                break;
              }

            case history_actions::add_note:
              {
                writer[CPPTRANSPORT_NODE_HISTORY_ACTION] = std::string(CPPTRANSPORT_NODE_HISTORY_ADD_NOTE);
                break;
              }

            case history_actions::remove_note:
              {
                writer[CPPTRANSPORT_NODE_HISTORY_ACTION] = std::string(CPPTRANSPORT_NODE_HISTORY_REMOVE_NOTE);
                break;
              }

            case history_actions::locked:
              {
                writer[CPPTRANSPORT_NODE_HISTORY_ACTION] = std::string(CPPTRANSPORT_NODE_HISTORY_LOCK);
                break;
              }

            case history_actions::unlocked:
              {
                writer[CPPTRANSPORT_NODE_HISTORY_ACTION] = std::string(CPPTRANSPORT_NODE_HISTORY_UNLOCK);
                break;
              }

            case history_actions::add_content:
              {
                writer[CPPTRANSPORT_NODE_HISTORY_ACTION] = std::string(CPPTRANSPORT_NODE_HISTORY_ADD_CONTENT);
                break;
              }

            case history_actions::remove_content:
              {
                writer[CPPTRANSPORT_NODE_HISTORY_ACTION] = std::string(CPPTRANSPORT_NODE_HISTORY_REMOVE_CONTENT);
                break;
              }
          }
      }


    std::string metadata_history::to_string() const
      {
        std::string time = boost::posix_time::to_simple_string(this->timestamp);
        std::ostringstream msg;

        switch(this->action)
          {
            case history_actions::created:
              {
                msg << CPPTRANSPORT_HISTORY_CREATED << " " << this->user_id << " " << CPPTRANSPORT_HISTORY_AT << " " << time;
                break;
              }

            case history_actions::add_tag:
              {
                msg << CPPTRANSPORT_HISTORY_TAG << " '" << this->info << "' " << CPPTRANSPORT_HISTORY_ADDED << " " << this->user_id << " " << CPPTRANSPORT_HISTORY_AT << " " << time;
                break;
              }

            case history_actions::remove_tag:
              {
                msg << CPPTRANSPORT_HISTORY_TAG << " '" << this->info << "' " << CPPTRANSPORT_HISTORY_REMOVED << " " << this->user_id << " " << CPPTRANSPORT_HISTORY_AT << " " << time;
                break;
              }

            case history_actions::add_note:
              {
                msg << CPPTRANSPORT_HISTORY_NOTE << " '" << this->info << "' " << CPPTRANSPORT_HISTORY_ADDED << " " << this->user_id << " " << CPPTRANSPORT_HISTORY_AT << " " << time;
                break;
              }

            case history_actions::remove_note:
              {
                msg << CPPTRANSPORT_HISTORY_NOTE << " '" << this->info << "' " << CPPTRANSPORT_HISTORY_REMOVED << " " << this->user_id << " " << CPPTRANSPORT_HISTORY_AT << " " << time;
                break;
              }

            case history_actions::locked:
              {
                msg << CPPTRANSPORT_HISTORY_LOCKED << " " << this->user_id << " " << CPPTRANSPORT_HISTORY_AT << " " << time;
                break;
              }

            case history_actions::unlocked:
              {
                msg << CPPTRANSPORT_HISTORY_UNLOCKED << " " << this->user_id << " " << CPPTRANSPORT_HISTORY_AT << " " << time;
                break;
              }

            case history_actions::add_content:
              {
                msg << CPPTRANSPORT_HISTORY_CONTENT << " " << this->info << " " << CPPTRANSPORT_HISTORY_ADDED << " " << this->user_id << " " << CPPTRANSPORT_HISTORY_AT << " " << time;
                break;
              }

            case history_actions::remove_content:
              {
                msg << CPPTRANSPORT_HISTORY_CONTENT << " " << this->info << " " << CPPTRANSPORT_HISTORY_REMOVED << " " << this->user_id << " " << CPPTRANSPORT_HISTORY_AT << " " << time;
                break;
              }
          }

        return(msg.str());
      }


    // REPOSITORY METADATA METHODS


    record_metadata::record_metadata(std::string u)
      : creation_time(boost::posix_time::second_clock::universal_time()),
        last_edit_time(boost::posix_time::second_clock::universal_time()),    // don't initialize from creation_time; order of initialization depends on order of *declaration* in class, and that might change
        runtime_api(CPPTRANSPORT_RUNTIME_API_VERSION)
      {
        history.emplace_back(std::move(u), history_actions::created);
      }


    record_metadata::record_metadata(Json::Value& reader)
      {
        std::string ctime_str = reader[CPPTRANSPORT_NODE_METADATA_GROUP][CPPTRANSPORT_NODE_METADATA_CREATED].asString();
        this->creation_time = boost::posix_time::from_iso_string(ctime_str);

        std::string etime_str = reader[CPPTRANSPORT_NODE_METADATA_GROUP][CPPTRANSPORT_NODE_METADATA_EDITED].asString();
        this->last_edit_time = boost::posix_time::from_iso_string(etime_str);

        this->runtime_api = reader[CPPTRANSPORT_NODE_METADATA_GROUP][CPPTRANSPORT_NODE_METADATA_RUNTIME_API].asUInt();

        Json::Value history = reader[CPPTRANSPORT_NODE_METADATA_GROUP][CPPTRANSPORT_NODE_METADATA_HISTORY];
        assert(history.isArray());

        for(Json::Value::iterator t = history.begin(); t != history.end(); ++t)
          {
            this->history.emplace_back(*t);
          }
      }


    void record_metadata::serialize(Json::Value& writer) const
      {
        Json::Value metadata(Json::objectValue);

        writer[CPPTRANSPORT_NODE_METADATA_GROUP][CPPTRANSPORT_NODE_METADATA_CREATED] = boost::posix_time::to_iso_string(this->creation_time);
        writer[CPPTRANSPORT_NODE_METADATA_GROUP][CPPTRANSPORT_NODE_METADATA_EDITED] = boost::posix_time::to_iso_string(this->last_edit_time);
        writer[CPPTRANSPORT_NODE_METADATA_GROUP][CPPTRANSPORT_NODE_METADATA_RUNTIME_API] = this->runtime_api;

        Json::Value history(Json::arrayValue);
        for(const metadata_history& history_item : this->history)
          {
            Json::Value value(Json::objectValue);
            history_item.serialize(value);
            history.append(value);
          }

        writer[CPPTRANSPORT_NODE_METADATA_GROUP][CPPTRANSPORT_NODE_METADATA_HISTORY] = history;
      }

  }


#endif //CPPTRANSPORT_METADATA_IMPL_H
