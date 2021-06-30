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

#ifndef CPPTRANSPORT_TASK_IMPL_H
#define CPPTRANSPORT_TASK_IMPL_H


namespace transport
  {

    constexpr auto CPPTRANSPORT_NODE_TASK_CONTENT_GROUPS = "content-groups";

    constexpr auto CPPTRANSPORT_NODE_RECORD_INTEGRATION_TASK_TYPE = "integration-record-type";
    constexpr auto CPPTRANSPORT_NODE_RECORD_INTEGRATION_TASK_TWOPF = "twopf";
    constexpr auto CPPTRANSPORT_NODE_RECORD_INTEGRATION_TASK_THREEPF = "threepf";
    constexpr auto CPPTRANSPORT_NODE_RECORD_INTEGRATION_TASK_KCONFIG_DATABASE = "kconfig-database";

    constexpr auto CPPTRANSPORT_NODE_RECORD_POSTINTEGRATION_TASK_TYPE = "type";
    constexpr auto CPPTRANSPORT_NODE_RECORD_POSTINTEGRATION_TASK_TWOPF = "twopf";
    constexpr auto CPPTRANSPORT_NODE_RECORD_POSTINTEGRATION_TASK_THREEPF = "threepf";
    constexpr auto CPPTRANSPORT_NODE_RECORD_POSTINTEGRATION_TASK_FNL = "fNL";


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
        Json::Value& group_list = reader[CPPTRANSPORT_NODE_TASK_CONTENT_GROUPS];
        assert(group_list.isArray());

        for(auto& t : group_list)
          {
            std::string name = t.asString();
            this->content_groups.insert(name);
          }
      }


    template <typename number>
    void task_record<number>::serialize(Json::Value& writer) const
      {
        Json::Value group_list(Json::arrayValue);

        for(const auto& string : this->content_groups)
          {
            Json::Value group_element = string;
            group_list.append(group_element);
          }
        writer[CPPTRANSPORT_NODE_TASK_CONTENT_GROUPS] = group_list;

        this->repository_record::serialize(writer);
      }


    template <typename number>
    void task_record<number>::add_new_content_group(const std::string& name)
      {
        auto result = this->content_groups.insert(name);
        if(result.second)
          {
            this->metadata.add_history_item(this->handlers.env.get_userid(), history_actions::add_content, name);
            this->metadata.update_last_edit_time();
          }
      }


    template <typename number>
    void task_record<number>::delete_content_group(const std::string& name)
      {
        auto t = std::find(this->content_groups.cbegin(), this->content_groups.cend(), name);

        if(t == this->content_groups.end()) return;

        this->content_groups.erase(t);
        this->metadata.add_history_item(this->handlers.env.get_userid(), history_actions::remove_content, name);
        this->metadata.update_last_edit_time();
      }


    // INTEGRATION TASK RECORD


    template <typename number>
    integration_task_record<number>::integration_task_record(const twopf_task<number>& t, repository_record::handler_package& pkg)
      : task_record<number>(t.get_name(), pkg),
        type(integration_task_type::twopf),
        tk(dynamic_cast<integration_task<number>*>(t.clone()))
      {
        assert(tk != nullptr);
      }


    template <typename number>
    integration_task_record<number>::integration_task_record(const threepf_task<number>& t, repository_record::handler_package& pkg)
      : task_record<number>(t.get_name(), pkg),
        type(integration_task_type::threepf),
        tk(dynamic_cast<integration_task<number>*>(t.clone()))
      {
        assert(tk != nullptr);
      }


    template <typename number>
    integration_task_record<number>::integration_task_record(const integration_task_record<number>& obj)
      : task_record<number>(obj),
        tk(dynamic_cast<integration_task<number>*>(obj.tk->clone())),
        type(obj.type),
        kconfig_db(obj.kconfig_db)
      {
        assert(tk != nullptr);
      }


    template <typename number>
    integration_task_record<number>::integration_task_record(Json::Value& reader, const boost::filesystem::path& repo_root, bool network_mode,
                                                             package_finder<number>& f, repository_record::handler_package& pkg)
      : task_record<number>(reader, pkg)
      {
        // deserialize location of database
        kconfig_db = reader[CPPTRANSPORT_NODE_RECORD_INTEGRATION_TASK_KCONFIG_DATABASE].asString();

        // deserialize task type
        std::string task_type = reader[CPPTRANSPORT_NODE_RECORD_INTEGRATION_TASK_TYPE].asString();

        if(task_type == CPPTRANSPORT_NODE_RECORD_INTEGRATION_TASK_TWOPF)        type = integration_task_type::twopf;
        else if(task_type == CPPTRANSPORT_NODE_RECORD_INTEGRATION_TASK_THREEPF) type = integration_task_type::threepf;
        else
          {
            std::ostringstream msg;
            msg << CPPTRANSPORT_REPO_FAIL_INTEGRATION_TASK_TYPE << " '" << task_type << "'";
            throw runtime_exception(exception_type::SERIALIZATION_ERROR, msg.str());
          }

        // ingest k-configuration database: first find absolute path to container
        boost::filesystem::path abs_database = repo_root / kconfig_db;

        // open container and obtain sqlite3 handle
        sqlite3* handle;
        if(sqlite3_open_v2(abs_database.c_str(), &handle, SQLITE_OPEN_READONLY, nullptr) != SQLITE_OK)
          {
            std::ostringstream msg;
            msg << CPPTRANSPORT_REPO_FAIL_KCONFIG_DATABASE_OPEN << " '" << this->get_name() << "'";
            throw runtime_exception(exception_type::REPOSITORY_BACKEND_ERROR, msg.str());
          }

        sqlite3_extended_result_codes(handle, 1);
        sqlite3_operations::consistency_pragmas(handle);

        // ingest task data
        tk = integration_task_helper::deserialize<number>(this->name, reader, handle, f);

        // close handle
        sqlite3_close(handle);

        assert(tk != nullptr);
        if(tk == nullptr) throw runtime_exception(exception_type::SERIALIZATION_ERROR, CPPTRANSPORT_REPO_TASK_DESERIALIZE_FAIL);
      }


    template <typename number>
    void integration_task_record<number>::serialize(Json::Value& writer) const
      {
        writer[CPPTRANSPORT_NODE_RECORD_TYPE] = std::string(CPPTRANSPORT_NODE_RECORD_INTEGRATION_TASK);

        switch(this->type)
          {
            case integration_task_type::twopf:
              {
                writer[CPPTRANSPORT_NODE_RECORD_INTEGRATION_TASK_TYPE] = std::string(CPPTRANSPORT_NODE_RECORD_INTEGRATION_TASK_TWOPF);
                break;
              }

            case integration_task_type::threepf:
              {
                writer[CPPTRANSPORT_NODE_RECORD_INTEGRATION_TASK_TYPE] = std::string(CPPTRANSPORT_NODE_RECORD_INTEGRATION_TASK_THREEPF);
                break;
              }
          }

        writer[CPPTRANSPORT_NODE_RECORD_INTEGRATION_TASK_KCONFIG_DATABASE] = this->kconfig_db.string();

        this->tk->serialize(writer);
        this->task_record<number>::serialize(writer);
      }


    template <typename number>
    void integration_task_record<number>::write_kconfig_database(const boost::filesystem::path& db_path, bool network_mode) const
      {
        // create database
        sqlite3* handle;
        if(sqlite3_open_v2(db_path.c_str(), &handle, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, nullptr) != SQLITE_OK)
          {
            std::ostringstream msg;
            msg << CPPTRANSPORT_REPO_FAIL_KCONFIG_DATABASE_OPEN << " '" << this->get_name() << "'";
            throw runtime_exception(exception_type::REPOSITORY_BACKEND_ERROR, msg.str());
          }

        sqlite3_extended_result_codes(handle, 1);
        sqlite3_operations::consistency_pragmas(handle);

        this->tk->write_kconfig_database(handle);

        // perform routine maintenance on kconfig database
        // should be quick, since the kconfig database isn't expected to be very large
        sqlite3_operations::exec(handle, "VACUUM;");
        sqlite3_close(handle);
      }


    // POSTINTEGRATION TASK RECORD


    template <typename number>
    postintegration_task_record<number>::postintegration_task_record(const zeta_twopf_task<number>& t, repository_record::handler_package& pkg)
      : task_record<number>(t.get_name(), pkg),
        tk(dynamic_cast<postintegration_task<number>*>(t.clone())),
        type(postintegration_task_type::twopf)
      {
        assert(tk);
      }


    template <typename number>
    postintegration_task_record<number>::postintegration_task_record(const zeta_threepf_task<number>& t, repository_record::handler_package& pkg)
      : task_record<number>(t.get_name(), pkg),
        tk(dynamic_cast<postintegration_task<number>*>(t.clone())),
        type(postintegration_task_type::threepf)
      {
        assert(tk);
      }


    template <typename number>
    postintegration_task_record<number>::postintegration_task_record(const fNL_task<number>& t, repository_record::handler_package& pkg)
      : task_record<number>(t.get_name(), pkg),
        tk(dynamic_cast<postintegration_task<number>*>(t.clone())),
        type(postintegration_task_type::fNL)
      {
        assert(tk);
      }


    template <typename number>
    postintegration_task_record<number>::postintegration_task_record(const postintegration_task_record<number>& obj)
      : task_record<number>(obj),
        tk(dynamic_cast<postintegration_task<number>*>(obj.tk->clone())),
        type(obj.type)
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

        std::string task_type = reader[CPPTRANSPORT_NODE_RECORD_POSTINTEGRATION_TASK_TYPE].asString();

        if(task_type == CPPTRANSPORT_NODE_RECORD_POSTINTEGRATION_TASK_TWOPF)        this->type = postintegration_task_type::twopf;
        else if(task_type == CPPTRANSPORT_NODE_RECORD_POSTINTEGRATION_TASK_THREEPF) this->type = postintegration_task_type::threepf;
        else if(task_type == CPPTRANSPORT_NODE_RECORD_POSTINTEGRATION_TASK_FNL)     this->type = postintegration_task_type::fNL;
        else
          {
            std::ostringstream msg;
            msg << CPPTRANSPORT_REPO_FAIL_INTEGRATION_TASK_TYPE << " '" << task_type << "'";
            throw runtime_exception(exception_type::SERIALIZATION_ERROR, msg.str());
          }
      }


    template <typename number>
    void postintegration_task_record<number>::serialize(Json::Value& writer) const
      {
        writer[CPPTRANSPORT_NODE_RECORD_TYPE] = std::string(CPPTRANSPORT_NODE_RECORD_POSTINTEGRATION_TASK);

        switch(this->type)
          {
            case postintegration_task_type::twopf:
              {
                writer[CPPTRANSPORT_NODE_RECORD_POSTINTEGRATION_TASK_TYPE] = std::string(CPPTRANSPORT_NODE_RECORD_POSTINTEGRATION_TASK_TWOPF);
                break;
              }

            case postintegration_task_type::threepf:
              {
                writer[CPPTRANSPORT_NODE_RECORD_POSTINTEGRATION_TASK_TYPE] = std::string(CPPTRANSPORT_NODE_RECORD_POSTINTEGRATION_TASK_THREEPF);
                break;
              }

            case postintegration_task_type::fNL:
              {
                writer[CPPTRANSPORT_NODE_RECORD_POSTINTEGRATION_TASK_TYPE] = std::string(CPPTRANSPORT_NODE_RECORD_POSTINTEGRATION_TASK_FNL);
                break;
              }
          }

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
