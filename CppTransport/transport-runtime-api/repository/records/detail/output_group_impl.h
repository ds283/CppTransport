//
// Created by David Seery on 25/01/2016.
// Copyright (c) 2016 University of Sussex. All rights reserved.
//

#ifndef CPPTRANSPORT_OUTPUT_GROUP_IMPL_H
#define CPPTRANSPORT_OUTPUT_GROUP_IMPL_H


namespace transport
  {

    constexpr auto CPPTRANSPORT_NODE_OUTPUTGROUP_TASK_NAME = "parent-task";
    constexpr auto CPPTRANSPORT_NODE_OUTPUTGROUP_DATA_ROOT = "output-path";
    constexpr auto CPPTRANSPORT_NODE_OUTPUTGROUP_LOCKED = "locked";
    constexpr auto CPPTRANSPORT_NODE_OUTPUTGROUP_NOTES = "notes";
    constexpr auto CPPTRANSPORT_NODE_OUTPUTGROUP_TAGS = "tags";

    constexpr auto CPPTRANSPORT_NODE_PAYLOAD_INTEGRATION_DATABASE = "database-path";
    constexpr auto CPPTRANSPORT_NODE_PAYLOAD_INTEGRATION_FAILED = "failed";
    constexpr auto CPPTRANSPORT_NODE_PAYLOAD_INTEGRATION_WORKGROUP = "workgroup";
    constexpr auto CPPTRANSPORT_NODE_PAYLOAD_INTEGRATION_FAILED_SERIALS = "failed-serials";
    constexpr auto CPPTRANSPORT_NODE_PAYLOAD_INTEGRATION_SEEDED = "seeded";
    constexpr auto CPPTRANSPORT_NODE_PAYLOAD_INTEGRATION_SEED_GROUP = "seed-group";
    constexpr auto CPPTRANSPORT_NODE_PAYLOAD_INTEGRATION_STATISTICS = "has-statistics";
    constexpr auto CPPTRANSPORT_NODE_PAYLOAD_INTEGRATION_ICS = "has-ics";
    constexpr auto CPPTRANSPORT_NODE_PAYLOAD_INTEGRATION_SIZE = "size";

    constexpr auto CPPTRANSPORT_NODE_PAYLOAD_POSTINTEGRATION_DATABASE = "database-path";
    constexpr auto CPPTRANSPORT_NODE_PAYLOAD_POSTINTEGRATION_FAILED = "failed";
    constexpr auto CPPTRANSPORT_NODE_PAYLOAD_POSTINTEGRATION_PAIRED = "paired";
    constexpr auto CPPTRANSPORT_NODE_PAYLOAD_POSTINTEGRATION_PARENT_GROUP = "parent-group";
    constexpr auto CPPTRANSPORT_NODE_PAYLOAD_POSTINTEGRATION_SEEDED = "seeded";
    constexpr auto CPPTRANSPORT_NODE_PAYLOAD_POSTINTEGRATION_SEED_GROUP = "seed-group";
    constexpr auto CPPTRANSPORT_NODE_PAYLOAD_POSTINTEGRATION_FAILED_SERIALS = "failed-serials";
    constexpr auto CPPTRANSPORT_NODE_PAYLOAD_POSTINTEGRATION_SIZE = "size";

    constexpr auto CPPTRANSPORT_NODE_PRECOMPUTED_ROOT = "contains-products";
    constexpr auto CPPTRANSPORT_NODE_PRECOMPUTED_ZETA_TWOPF = "zeta-twopf";
    constexpr auto CPPTRANSPORT_NODE_PRECOMPUTED_ZETA_THREEPF = "zeta-threepf";
    constexpr auto CPPTRANSPORT_NODE_PRECOMPUTED_ZETA_REDBSP = "zeta-redbsp";
    constexpr auto CPPTRANSPORT_NODE_PRECOMPUTED_FNL_LOCAL = "fNL_local";
    constexpr auto CPPTRANSPORT_NODE_PRECOMPUTED_FNL_EQUI = "fNL_equi";
    constexpr auto CPPTRANSPORT_NODE_PRECOMPUTED_FNL_ORTHO = "fNL_ortho";
    constexpr auto CPPTRANSPORT_NODE_PRECOMPUTED_FNL_DBI = "fNL_DBI";

    constexpr auto CPPTRANSPORT_NODE_PAYLOAD_CONTENT_FAILED = "failed";
    constexpr auto CPPTRANSPORT_NODE_PAYLOAD_CONTENT_ARRAY = "generated-products";
    constexpr auto CPPTRANSPORT_NODE_PAYLOAD_CONTENT_USED_GROUPS = "used-content-groups";
    constexpr auto CPPTRANSPORT_NODE_PAYLOAD_CONTENT_PRODUCT_NAME = "parent-product";
    constexpr auto CPPTRANSPORT_NODE_PAYLOAD_CONTENT_FILENAME = "filename";
    constexpr auto CPPTRANSPORT_NODE_PAYLOAD_CONTENT_CREATED = "creation-time";
    constexpr auto CPPTRANSPORT_NODE_PAYLOAD_CONTENT_NOTES = "notes";
    constexpr auto CPPTRANSPORT_NODE_PAYLOAD_CONTENT_TAGS = "tags";
    constexpr auto CPPTRANSPORT_NODE_PAYLOAD_GROUPS_SUMMARY = "content-groups-summary";


    // OUTPUT_GROUP METHODS


    template <typename Payload>
    output_group_record<Payload>::output_group_record(const std::string& tn, const paths_group& p,
                                                      bool lock, const std::list<note>& nt, const std::list<std::string>& tg,
                                                      repository_record::handler_package& pkg)
      : repository_record(pkg),
        task(tn),
        paths(p),
        locked(lock),
        notes(nt),
        tags(tg),
        payload()
      {
        // remove any duplicated tags
        tags.sort();
        tags.unique();
      }


    template <typename Payload>
    template <typename Stream>
    void output_group_record<Payload>::write(Stream& out) const
      {
        out << CPPTRANSPORT_OUTPUT_GROUP;
        if(this->locked) out << ", " << CPPTRANSPORT_OUTPUT_GROUP_LOCKED;
        out << '\n';
        out << "  " << CPPTRANSPORT_OUTPUT_GROUP_REPO_ROOT << " = " << this->paths.root << '\n';
        out << "  " << CPPTRANSPORT_OUTPUT_GROUP_DATA_ROOT << " = " << this->paths.output << '\n';

        unsigned int count = 0;

        for(const std::string& note : this->notes)
          {
            out << "  " << CPPTRANSPORT_OUTPUT_GROUP_NOTE << " " << count << '\n';
            out << "    " << note << '\n';
            count++;
          }

        count = 0;
        out << "  " << CPPTRANSPORT_OUTPUT_GROUP_TAGS << ": ";
        for(const std::string& tag : this->tags)
          {
            if(count > 0) out << ", ";
            out << tag;
            count++;
          }
        out << '\n';

        this->payload.write(out);

        out << '\n';
      }


    template <typename Payload>
    bool output_group_record<Payload>::check_tags(std::list<std::string> match_tags) const
      {
        // remove all this group's tags from the matching set.
        // If any remain after this process, then the match set isn't a subset of the group's tags.
        for(std::list<std::string>::const_iterator t = this->tags.begin(); t != this->tags.end(); ++t)
          {
            match_tags.remove(*t);
          }

        return (!match_tags.empty());
      }


    template <typename Payload>
    output_group_record<Payload>::output_group_record(Json::Value& reader, const boost::filesystem::path& root,
                                                      repository_record::handler_package& pkg)
      : repository_record(reader, pkg),
        payload(reader)
      {
        paths.root = root;

        task         = reader[CPPTRANSPORT_NODE_OUTPUTGROUP_TASK_NAME].asString();
        paths.output = reader[CPPTRANSPORT_NODE_OUTPUTGROUP_DATA_ROOT].asString();
        locked       = reader[CPPTRANSPORT_NODE_OUTPUTGROUP_LOCKED].asBool();

        Json::Value note_list = reader[CPPTRANSPORT_NODE_OUTPUTGROUP_NOTES];
        assert(note_list.isArray());

        for(Json::Value::iterator t = note_list.begin(); t != note_list.end(); ++t)
          {
            notes.emplace_back(*t);
          }

        Json::Value tag_list = reader[CPPTRANSPORT_NODE_OUTPUTGROUP_TAGS];
        assert(tag_list.isArray());

        for(Json::Value::iterator t = tag_list.begin(); t != tag_list.end(); ++t)
          {
            tags.push_back(t->asString());
          }
      }


    template <typename Payload>
    void output_group_record<Payload>::serialize(Json::Value& writer) const
      {
        writer[CPPTRANSPORT_NODE_RECORD_TYPE] = std::string(CPPTRANSPORT_NODE_RECORD_CONTENT);

        writer[CPPTRANSPORT_NODE_OUTPUTGROUP_TASK_NAME] = this->task;
        writer[CPPTRANSPORT_NODE_OUTPUTGROUP_DATA_ROOT] = this->paths.output.string();
        writer[CPPTRANSPORT_NODE_OUTPUTGROUP_LOCKED]    = this->locked;

        Json::Value note_list(Json::arrayValue);

        for(const note& item : this->notes)
          {
            Json::Value note_element(Json::objectValue);
            item.serialize(note_element);
            note_list.append(note_element);
          }
        writer[CPPTRANSPORT_NODE_OUTPUTGROUP_NOTES] = note_list;

        Json::Value tag_list(Json::arrayValue);

        for(const std::string& tag : this->tags)
          {
            Json::Value tag_element = tag;
            tag_list.append(tag_element);
          }
        writer[CPPTRANSPORT_NODE_OUTPUTGROUP_TAGS] = tag_list;

        this->payload.serialize(writer);

        this->repository_record::serialize(writer);
      }


    template <typename Payload>
    void output_group_record<Payload>::add_note(const std::string& note)
      {
        this->notes.emplace_back(this->handlers.env.get_userid(), note);
        this->metadata.add_history_item(this->handlers.env.get_userid(), history_actions::add_note);
        this->metadata.update_last_edit_time();
      }


    template <typename Payload>
    void output_group_record<Payload>::set_lock_status(bool g)
      {
        this->locked = g;
        this->metadata.add_history_item(this->handlers.env.get_userid(), history_actions::locked);
        this->metadata.update_last_edit_time();
      }


    template <typename Payload>
    void output_group_record<Payload>::add_tag(const std::string& tag)
      {
        this->tags.push_back(tag);
        this->tags.sort();
        this->tags.unique();

        this->metadata.add_history_item(this->handlers.env.get_userid(), history_actions::add_tag, tag);
        this->metadata.update_last_edit_time();
      }


    template <typename Payload>
    void output_group_record<Payload>::remove_tag(const std::string& tag)
      {
        std::list<std::string>::const_iterator it = std::find(this->tags.cbegin(), this->tags.cend(), tag);
        if(it != this->tags.end())
          {
            this->tags.erase(it);
            this->metadata.add_history_item(this->handlers.env.get_userid(), history_actions::remove_tag, tag);
            this->metadata.update_last_edit_time();
          }
      }


    template <typename Payload>
    void output_group_record<Payload>::remove_note(unsigned int number)
      {
        std::list<note>::const_iterator it = this->notes.cbegin();
        while(number > 0 && it != this->notes.end())
          {
            --number;
            ++it;
          }

        if(number == 0 && it != this->notes.end())
          {
            this->notes.erase(it);
            this->metadata.add_history_item(this->handlers.env.get_userid(), history_actions::remove_note);
            this->metadata.update_last_edit_time();
          }
      }


    // output an output_group_record descriptor to a standard stream
    template <typename Payload, typename Char, typename Traits>
    std::basic_ostream<Char, Traits>& operator<<(std::basic_ostream<Char, Traits>& out, const output_group_record<Payload>& group)
      {
        group.write(out);
        return (out);
      }


    // output an output_group_record descriptor to a standard stream
    template <typename Payload, typename Char, typename Traits, typename Allocator>
    boost::log::basic_formatting_ostream<Char, Traits, Allocator>& operator<<(boost::log::basic_formatting_ostream<Char, Traits, Allocator>& out, const output_group_record<Payload>& group)
      {
        group.write(out);
        return (out);
      }


    precomputed_products::precomputed_products(Json::Value& reader)
      {
        zeta_twopf   = reader[CPPTRANSPORT_NODE_PRECOMPUTED_ROOT][CPPTRANSPORT_NODE_PRECOMPUTED_ZETA_TWOPF].asBool();
        zeta_threepf = reader[CPPTRANSPORT_NODE_PRECOMPUTED_ROOT][CPPTRANSPORT_NODE_PRECOMPUTED_ZETA_THREEPF].asBool();
        zeta_redbsp  = reader[CPPTRANSPORT_NODE_PRECOMPUTED_ROOT][CPPTRANSPORT_NODE_PRECOMPUTED_ZETA_REDBSP].asBool();
        fNL_local    = reader[CPPTRANSPORT_NODE_PRECOMPUTED_ROOT][CPPTRANSPORT_NODE_PRECOMPUTED_FNL_LOCAL].asBool();
        fNL_equi     = reader[CPPTRANSPORT_NODE_PRECOMPUTED_ROOT][CPPTRANSPORT_NODE_PRECOMPUTED_FNL_EQUI].asBool();
        fNL_ortho    = reader[CPPTRANSPORT_NODE_PRECOMPUTED_ROOT][CPPTRANSPORT_NODE_PRECOMPUTED_FNL_ORTHO].asBool();
        fNL_DBI      = reader[CPPTRANSPORT_NODE_PRECOMPUTED_ROOT][CPPTRANSPORT_NODE_PRECOMPUTED_FNL_DBI].asBool();
      }


    void precomputed_products::serialize(Json::Value& writer) const
      {
        writer[CPPTRANSPORT_NODE_PRECOMPUTED_ROOT][CPPTRANSPORT_NODE_PRECOMPUTED_ZETA_TWOPF]   = this->zeta_twopf;
        writer[CPPTRANSPORT_NODE_PRECOMPUTED_ROOT][CPPTRANSPORT_NODE_PRECOMPUTED_ZETA_THREEPF] = this->zeta_threepf;
        writer[CPPTRANSPORT_NODE_PRECOMPUTED_ROOT][CPPTRANSPORT_NODE_PRECOMPUTED_ZETA_REDBSP]  = this->zeta_redbsp;
        writer[CPPTRANSPORT_NODE_PRECOMPUTED_ROOT][CPPTRANSPORT_NODE_PRECOMPUTED_FNL_LOCAL]    = this->fNL_local;
        writer[CPPTRANSPORT_NODE_PRECOMPUTED_ROOT][CPPTRANSPORT_NODE_PRECOMPUTED_FNL_EQUI]     = this->fNL_equi;
        writer[CPPTRANSPORT_NODE_PRECOMPUTED_ROOT][CPPTRANSPORT_NODE_PRECOMPUTED_FNL_ORTHO]    = this->fNL_ortho;
        writer[CPPTRANSPORT_NODE_PRECOMPUTED_ROOT][CPPTRANSPORT_NODE_PRECOMPUTED_FNL_DBI]      = this->fNL_DBI;
      }


    template <typename Stream>
    void precomputed_products::write(Stream& out) const
      {
        out << CPPTRANSPORT_PAYLOAD_HAS_ZETA_TWO << ": " << (this->zeta_twopf ? CPPTRANSPORT_YES : CPPTRANSPORT_NO) << '\n';
        out << CPPTRANSPORT_PAYLOAD_HAS_ZETA_THREE << ": " << (this->zeta_threepf ? CPPTRANSPORT_YES : CPPTRANSPORT_NO) << '\n';
        out << CPPTRANSPORT_PAYLOAD_HAS_ZETA_REDBSP << ": " << (this->zeta_redbsp ? CPPTRANSPORT_YES : CPPTRANSPORT_NO) << '\n';
        out << CPPTRANSPORT_PAYLOAD_HAS_FNL_LOCAL << ": " << (this->fNL_local ? CPPTRANSPORT_YES : CPPTRANSPORT_NO) << '\n';
        out << CPPTRANSPORT_PAYLOAD_HAS_FNL_EQUI << ": " << (this->fNL_equi ? CPPTRANSPORT_YES : CPPTRANSPORT_NO) << '\n';
        out << CPPTRANSPORT_PAYLOAD_HAS_FNL_ORTHO << ": " << (this->fNL_ortho ? CPPTRANSPORT_YES : CPPTRANSPORT_NO) << '\n';
        out << CPPTRANSPORT_PAYLOAD_HAS_FNL_DBI << ": " << (this->fNL_DBI ? CPPTRANSPORT_YES : CPPTRANSPORT_NO) << '\n';
      }


    integration_payload::integration_payload(Json::Value& reader)
      : metadata(reader)
      {
        container          = reader[CPPTRANSPORT_NODE_PAYLOAD_INTEGRATION_DATABASE].asString();
        fail               = reader[CPPTRANSPORT_NODE_PAYLOAD_INTEGRATION_FAILED].asBool();
        workgroup_number   = reader[CPPTRANSPORT_NODE_PAYLOAD_INTEGRATION_WORKGROUP].asUInt();
        seeded             = reader[CPPTRANSPORT_NODE_PAYLOAD_INTEGRATION_SEEDED].asBool();
        seed_group         = reader[CPPTRANSPORT_NODE_PAYLOAD_INTEGRATION_SEED_GROUP].asString();
        statistics         = reader[CPPTRANSPORT_NODE_PAYLOAD_INTEGRATION_STATISTICS].asBool();
        initial_conditions = reader[CPPTRANSPORT_NODE_PAYLOAD_INTEGRATION_ICS].asBool();
        size               = reader[CPPTRANSPORT_NODE_PAYLOAD_INTEGRATION_SIZE].asUInt();

        Json::Value failure_array = reader[CPPTRANSPORT_NODE_PAYLOAD_INTEGRATION_FAILED_SERIALS];
        assert(failure_array.isArray());
        failed_serials.clear();
        for(Json::Value::iterator t = failure_array.begin(); t != failure_array.end(); ++t)
          {
            failed_serials.push_back(t->asUInt());
          }
      }


    void integration_payload::serialize(Json::Value& writer) const
      {
        writer[CPPTRANSPORT_NODE_PAYLOAD_INTEGRATION_DATABASE]   = this->container.string();
        writer[CPPTRANSPORT_NODE_PAYLOAD_INTEGRATION_FAILED]     = this->fail;
        writer[CPPTRANSPORT_NODE_PAYLOAD_INTEGRATION_WORKGROUP]  = this->workgroup_number;
        writer[CPPTRANSPORT_NODE_PAYLOAD_INTEGRATION_SEEDED]     = this->seeded;
        writer[CPPTRANSPORT_NODE_PAYLOAD_INTEGRATION_SEED_GROUP] = this->seed_group;
        writer[CPPTRANSPORT_NODE_PAYLOAD_INTEGRATION_STATISTICS] = this->statistics;
        writer[CPPTRANSPORT_NODE_PAYLOAD_INTEGRATION_ICS]        = this->initial_conditions;
        writer[CPPTRANSPORT_NODE_PAYLOAD_INTEGRATION_SIZE]       = this->size;

        Json::Value failure_array(Json::arrayValue);
        for(std::list<unsigned int>::const_iterator t = this->failed_serials.begin(); t != this->failed_serials.end(); ++t)
          {
            Json::Value element = *t;
            failure_array.append(element);
          }
        writer[CPPTRANSPORT_NODE_PAYLOAD_INTEGRATION_FAILED_SERIALS] = failure_array;

        this->metadata.serialize(writer);
      }


    template <typename Stream>
    void integration_payload::write(Stream& out) const
      {
        out << CPPTRANSPORT_PAYLOAD_INTEGRATION_DATA << " = " << this->container << '\n';
      }


    postintegration_payload::postintegration_payload(Json::Value& reader)
      : metadata(reader),
        precomputed(reader)
      {
        container    = reader[CPPTRANSPORT_NODE_PAYLOAD_INTEGRATION_DATABASE].asString();
        fail         = reader[CPPTRANSPORT_NODE_PAYLOAD_POSTINTEGRATION_FAILED].asBool();
        paired       = reader[CPPTRANSPORT_NODE_PAYLOAD_POSTINTEGRATION_PAIRED].asBool();
        parent_group = reader[CPPTRANSPORT_NODE_PAYLOAD_POSTINTEGRATION_PARENT_GROUP].asString();
        seeded       = reader[CPPTRANSPORT_NODE_PAYLOAD_POSTINTEGRATION_SEEDED].asBool();
        seed_group   = reader[CPPTRANSPORT_NODE_PAYLOAD_POSTINTEGRATION_SEED_GROUP].asString();
        size         = reader[CPPTRANSPORT_NODE_PAYLOAD_POSTINTEGRATION_SIZE].asUInt();

        Json::Value failure_array = reader[CPPTRANSPORT_NODE_PAYLOAD_POSTINTEGRATION_FAILED_SERIALS];
        assert(failure_array.isArray());
        failed_serials.clear();
        for(Json::Value::iterator t = failure_array.begin(); t != failure_array.end(); ++t)
          {
            failed_serials.push_back(t->asUInt());
          }
      }


    void postintegration_payload::serialize(Json::Value& writer) const
      {
        writer[CPPTRANSPORT_NODE_PAYLOAD_POSTINTEGRATION_DATABASE]     = this->container.string();
        writer[CPPTRANSPORT_NODE_PAYLOAD_POSTINTEGRATION_FAILED]       = this->fail;
        writer[CPPTRANSPORT_NODE_PAYLOAD_POSTINTEGRATION_PAIRED]       = this->paired;
        writer[CPPTRANSPORT_NODE_PAYLOAD_POSTINTEGRATION_PARENT_GROUP] = this->parent_group;
        writer[CPPTRANSPORT_NODE_PAYLOAD_POSTINTEGRATION_SEEDED]       = this->seeded;
        writer[CPPTRANSPORT_NODE_PAYLOAD_POSTINTEGRATION_SEED_GROUP]   = this->seed_group;
        writer[CPPTRANSPORT_NODE_PAYLOAD_POSTINTEGRATION_SIZE]         = this->size;

        Json::Value failure_array(Json::arrayValue);
        for(std::list<unsigned int>::const_iterator t = this->failed_serials.begin(); t != this->failed_serials.end(); ++t)
          {
            Json::Value element = *t;
            failure_array.append(element);
          }
        writer[CPPTRANSPORT_NODE_PAYLOAD_POSTINTEGRATION_FAILED_SERIALS] = failure_array;

        this->metadata.serialize(writer);
        this->precomputed.serialize(writer);
      }


    template <typename Stream>
    void postintegration_payload::write(Stream& out) const
      {
        out << CPPTRANSPORT_PAYLOAD_INTEGRATION_DATA << " = " << this->container << '\n';

        this->precomputed.write(out);
      }


    output_payload::output_payload(Json::Value& reader)
      : metadata(reader)
      {
        fail = reader[CPPTRANSPORT_NODE_PAYLOAD_CONTENT_FAILED].asBool();

        Json::Value& content_array = reader[CPPTRANSPORT_NODE_PAYLOAD_CONTENT_ARRAY];
        assert(content_array.isArray());

        for(Json::Value::iterator t = content_array.begin(); t != content_array.end(); ++t)
          {
            this->content.push_back( derived_content(*t) );
          }

        Json::Value& summary_array = reader[CPPTRANSPORT_NODE_PAYLOAD_GROUPS_SUMMARY];
        assert(summary_array.isArray());

        for(Json::Value::iterator t = summary_array.begin(); t != summary_array.end(); ++t)
          {
            this->used_groups.push_back( (*t).asString() );
          }
      }


    void output_payload::serialize(Json::Value& writer) const
      {
        writer[CPPTRANSPORT_NODE_PAYLOAD_CONTENT_FAILED] = this->fail;

        Json::Value content_array(Json::arrayValue);

        for(const derived_content& item : this->content)
          {
            Json::Value element(Json::objectValue);
            item.serialize(element);
            content_array.append(element);
          }
        writer[CPPTRANSPORT_NODE_PAYLOAD_CONTENT_ARRAY] = content_array;

        Json::Value summary_array(Json::arrayValue);

        for(const std::string& group : this->used_groups)
          {
            Json::Value element = group;
            summary_array.append(element);
          }
        writer[CPPTRANSPORT_NODE_PAYLOAD_GROUPS_SUMMARY] = summary_array;

        this->metadata.serialize(writer);
      }


    template <typename Stream>
    void output_payload::write(Stream& out) const
      {
        for(const derived_content& item : this->content)
          {
            out << CPPTRANSPORT_PAYLOAD_OUTPUT_CONTENT_PRODUCT << " = " << item.get_parent_product() << ", "
            << CPPTRANSPORT_PAYLOAD_OUTPUT_CONTENT_PATH    << " = " << item.get_filename() << '\n';
          }
      }


    derived_content::derived_content(Json::Value& reader)
      {
        parent_product = reader[CPPTRANSPORT_NODE_PAYLOAD_CONTENT_PRODUCT_NAME].asString();
        filename = reader[CPPTRANSPORT_NODE_PAYLOAD_CONTENT_FILENAME].asString();

        std::string ctime_string = reader[CPPTRANSPORT_NODE_PAYLOAD_CONTENT_CREATED].asString();
        created = boost::posix_time::from_iso_string(ctime_string);

        Json::Value& content_groups_array = reader[CPPTRANSPORT_NODE_PAYLOAD_CONTENT_USED_GROUPS];
        assert(content_groups_array.isArray());

        for(Json::Value::iterator t = content_groups_array.begin(); t != content_groups_array.end(); ++t)
          {
            this->content_groups.push_back(t->asString());
          }

        Json::Value note_list = reader[CPPTRANSPORT_NODE_PAYLOAD_CONTENT_NOTES];
        assert(note_list.isArray());

        for(Json::Value::iterator t = note_list.begin(); t != note_list.end(); ++t)
          {
            notes.emplace_back(*t);
          }

        Json::Value tag_list = reader[CPPTRANSPORT_NODE_PAYLOAD_CONTENT_TAGS];
        assert(tag_list.isArray());

        for(Json::Value::iterator t = tag_list.begin(); t != tag_list.end(); ++t)
          {
            tags.push_back(t->asString());
          }
      }


    void derived_content::serialize(Json::Value& writer) const
      {
        writer[CPPTRANSPORT_NODE_PAYLOAD_CONTENT_PRODUCT_NAME] = this->parent_product;
        writer[CPPTRANSPORT_NODE_PAYLOAD_CONTENT_FILENAME]     = this->filename.string();
        writer[CPPTRANSPORT_NODE_PAYLOAD_CONTENT_CREATED]      = boost::posix_time::to_iso_string(this->created);

        Json::Value content_groups_array(Json::arrayValue);

        for(const std::string& group : this->content_groups)
          {
            Json::Value element = group;
            content_groups_array.append(element);
          }
        writer[CPPTRANSPORT_NODE_PAYLOAD_CONTENT_USED_GROUPS] = content_groups_array;

        Json::Value note_list(Json::arrayValue);

        for(const note& item : this->notes)
          {
            Json::Value note_element(Json::objectValue);
            item.serialize(note_element);
            note_list.append(note_element);
          }
        writer[CPPTRANSPORT_NODE_PAYLOAD_CONTENT_NOTES] = note_list;

        Json::Value tag_list(Json::arrayValue);

        for(const std::string& tag : this->tags)
          {
            Json::Value tag_element = tag;
            tag_list.append(tag_element);
          }
        writer[CPPTRANSPORT_NODE_PAYLOAD_CONTENT_TAGS] = tag_list;
      }

  }   // namespace transport


#endif //CPPTRANSPORT_OUTPUT_GROUP_IMPL_H
