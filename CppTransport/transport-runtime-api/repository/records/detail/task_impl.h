//
// Created by David Seery on 25/01/2016.
// Copyright (c) 2016 University of Sussex. All rights reserved.
//

#ifndef CPPTRANSPORT_TASK_IMPL_H
#define CPPTRANSPORT_TASK_IMPL_H


namespace transport
  {

    constexpr auto CPPTRANSPORT_NODE_TASK_OUTPUT_GROUPS = "output-groups";

    constexpr auto CPPTRANSPORT_NODE_RECORD_INTEGRATION_TASK_KCONFIG_DATABASE = "kconfig-database";

    // GENERIC TASK RECORD


    template <typename number>
    task_record<number>::task_record(const std::string& name, repository_record::handler_package& pkg)
      : repository_record(name, pkg)
      {
      }


    template <typename number>
    task_record<number>::task_record(Json::Value& reader, repository_record::handler_package& pkg)
      : repository_record(reader, pkg)
      {
        Json::Value& group_list = reader[CPPTRANSPORT_NODE_TASK_OUTPUT_GROUPS];
        assert(group_list.isArray());

        for(Json::Value::iterator t = group_list.begin(); t != group_list.end(); ++t)
          {
            std::string name = t->asString();
            this->content_groups.push_back(name);
          }
      }


    template <typename number>
    void task_record<number>::serialize(Json::Value& writer) const
      {
        Json::Value group_list(Json::arrayValue);

        for(const std::string& string : this->content_groups)
          {
            Json::Value group_element = string;
            group_list.append(group_element);
          }
        writer[CPPTRANSPORT_NODE_TASK_OUTPUT_GROUPS] = group_list;

        this->repository_record::serialize(writer);
      }


    // INTEGRATION TASK RECORD


    template <typename number>
    integration_task_record<number>::integration_task_record(const integration_task<number>& t, repository_record::handler_package& pkg)
      : task_record<number>(t.get_name(), pkg),
        tk(dynamic_cast<integration_task<number>*>(t.clone()))
      {
        assert(tk != nullptr);
      }


    template <typename number>
    integration_task_record<number>::integration_task_record(const integration_task_record<number>& obj)
      : task_record<number>(obj),
        tk(dynamic_cast<integration_task<number>*>(obj.tk->clone())),
        kconfig_db(obj.kconfig_db)
      {
        assert(tk != nullptr);
      }


    template <typename number>
    integration_task_record<number>::integration_task_record(Json::Value& reader, const boost::filesystem::path& repo_root,
                                                             package_finder<number>& f, repository_record::handler_package& pkg)
      : task_record<number>(reader, pkg)
      {
        // deserialize location of database
        kconfig_db = reader[CPPTRANSPORT_NODE_RECORD_INTEGRATION_TASK_KCONFIG_DATABASE].asString();

        boost::filesystem::path abs_database = repo_root / kconfig_db;

        // open database
        sqlite3* handle;
        if(sqlite3_open_v2(abs_database.c_str(), &handle, SQLITE_OPEN_READONLY, nullptr) != SQLITE_OK)
          {
            std::ostringstream msg;
            msg << CPPTRANSPORT_REPO_FAIL_KCONFIG_DATABASE_OPEN << " '" << this->get_name() << "'";
            throw runtime_exception(exception_type::REPOSITORY_BACKEND_ERROR, msg.str());
          }

        tk = integration_task_helper::deserialize<number>(this->name, reader, handle, f);

        assert(tk != nullptr);
        if(tk == nullptr) throw runtime_exception(exception_type::SERIALIZATION_ERROR, CPPTRANSPORT_REPO_TASK_DESERIALIZE_FAIL);
      }


    template <typename number>
    void integration_task_record<number>::serialize(Json::Value& writer) const
      {
        writer[CPPTRANSPORT_NODE_RECORD_TYPE] = std::string(CPPTRANSPORT_NODE_RECORD_INTEGRATION_TASK);

        writer[CPPTRANSPORT_NODE_RECORD_INTEGRATION_TASK_KCONFIG_DATABASE] = this->kconfig_db.string();

        this->tk->serialize(writer);
        this->task_record<number>::serialize(writer);
      }


    template <typename number>
    void integration_task_record<number>::write_kconfig_database(const boost::filesystem::path& db_path) const
      {
        // create database
        sqlite3* handle;
        if(sqlite3_open_v2(db_path.c_str(), &handle, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, nullptr) != SQLITE_OK)
          {
            std::ostringstream msg;
            msg << CPPTRANSPORT_REPO_FAIL_KCONFIG_DATABASE_OPEN << " '" << this->get_name() << "'";
            throw runtime_exception(exception_type::REPOSITORY_BACKEND_ERROR, msg.str());
          }

        this->tk->write_kconfig_database(handle);

        sqlite3_operations::exec(handle, "VACUUM;");
        sqlite3_close(handle);
      }


    // POSTINTEGRATION TASK RECORD


    template <typename number>
    postintegration_task_record<number>::postintegration_task_record(const postintegration_task<number>& t, repository_record::handler_package& pkg)
      : task_record<number>(t.get_name(), pkg),
        tk(dynamic_cast<postintegration_task<number>*>(t.clone()))
      {
        assert(tk);
      }


    template <typename number>
    postintegration_task_record<number>::postintegration_task_record(const postintegration_task_record<number>& obj)
      : task_record<number>(obj),
        tk(dynamic_cast<postintegration_task<number>*>(obj.tk->clone()))
      {
        assert(tk);
      }


    template <typename number>
    postintegration_task_record<number>::postintegration_task_record(Json::Value& reader, task_finder<number>& f, repository_record::handler_package& pkg)
      : task_record<number>(reader, pkg),
        tk(postintegration_task_helper::deserialize<number>(this->name, reader, f))
      {
        assert(tk);
        if(!tk) throw runtime_exception(exception_type::SERIALIZATION_ERROR, CPPTRANSPORT_REPO_TASK_DESERIALIZE_FAIL);
      }


    template <typename number>
    void postintegration_task_record<number>::serialize(Json::Value& writer) const
      {
        writer[CPPTRANSPORT_NODE_RECORD_TYPE] = std::string(CPPTRANSPORT_NODE_RECORD_POSTINTEGRATION_TASK);
        this->tk->serialize(writer);
        this->task_record<number>::serialize(writer);
      }


    // OUTPUT TASK RECORD


    template <typename number>
    output_task_record<number>::output_task_record(const output_task<number>& t, repository_record::handler_package& pkg)
      : task_record<number>(t.get_name(), pkg),
        tk(dynamic_cast<output_task<number>*>(t.clone()))
      {
        assert(tk);
      }


    template <typename number>
    output_task_record<number>::output_task_record(const output_task_record<number>& obj)
      : task_record<number>(obj),
        tk(dynamic_cast<output_task<number>*>(obj.tk->clone()))
      {
        assert(tk);
      }


    template <typename number>
    output_task_record<number>::output_task_record(Json::Value& reader, derived_product_finder<number>& f, repository_record::handler_package& pkg)
      : task_record<number>(reader, pkg),
        tk(output_task_helper::deserialize<number>(this->name, reader, f))
      {
        assert(tk);

        if(!tk) throw runtime_exception(exception_type::SERIALIZATION_ERROR, CPPTRANSPORT_REPO_TASK_DESERIALIZE_FAIL);
      }


    template <typename number>
    void output_task_record<number>::serialize(Json::Value& writer) const
      {
        writer[CPPTRANSPORT_NODE_RECORD_TYPE] = std::string(CPPTRANSPORT_NODE_RECORD_OUTPUT_TASK);
        this->tk->serialize(writer);
        this->task_record<number>::serialize(writer);
      }

  }   // namespace transport


#endif //CPPTRANSPORT_TASK_IMPL_H
