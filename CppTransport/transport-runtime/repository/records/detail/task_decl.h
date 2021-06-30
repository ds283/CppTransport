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

#ifndef CPPTRANSPORT_TASK_DECL_H
#define CPPTRANSPORT_TASK_DECL_H


namespace transport
  {

    // GENERIC TASK RECORD


    namespace task_record_impl
      {

        class EmptyContentGroupFilterPredicate
          {
          public:
            EmptyContentGroupFilterPredicate() = default;
            ~EmptyContentGroupFilterPredicate() = default;

            bool operator()(const std::string& group) const { return true; }
          };

      } // namespace task_record_impl


    template <typename number>
    class task_record: public repository_record
      {

      public:


        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! construct a task record
        task_record(const std::string& nm, repository_record::handler_package& pkg);

        //! deserialization constructor
        task_record(Json::Value& f, repository_record::handler_package& pkg);

        virtual ~task_record() = default;


        // GET CONTENTS

      public:

        //! get task object
        virtual task<number>* get_abstract_task() const = 0;


        // ADMINISTRATION

      public:

        //! Get record type
        virtual task_type get_type() const = 0;

        //! Add new content group
        //! Notice that this *does not* write a content group into the main repository database table.
        //! It is added only to the internal list of content groups held in this record. The internal list
        //! therefore is at risk of getting out of sync with the main database table.
        //! Insertion into the main database table is done by the store function for content_group_record<>,
        //! which is invoked when we commit that record.
        void add_new_content_group(const std::string& name);

        //! Remove content group
        //! Notice that this *does not* remove the content group into the main repository database table.
        //! It is only removed from the internal list of content groups held in this record. The internal list
        //! therefore is at risk of getting out of sync with the main database table.
        void delete_content_group(const std::string& name);

        //! Get content groups
        template <typename FilterPredicate = task_record_impl::EmptyContentGroupFilterPredicate>
        content_group_name_set get_content_groups(FilterPredicate p=FilterPredicate()) const
          {
            content_group_name_set groups;

            for(const auto& g : this->content_groups)
              {
                if(p(g))
                  {
                    groups.insert(g);
                  }
              }

            return groups;
          }


        // SERIALIZATION -- implements a 'serializable' interface

      public:

        //! serialize this object
        virtual void serialize(Json::Value& writer) const override;


        // INTERNAL DATA

      protected:

        //! List of content groups associated with this task
        // TODO: consider how to prevent this getting out of sync with the main repository database tables, which
        //  encode the same information independently.
        //  Notice that the enumerate_*_task_content() methods in repository<> do not use this information.
        //  They use the repository database table.
        //  Only the get_content_groups() method of task_record<> uses this information.
        //  Possibly they should be synced up in a database maintenance pass.
        content_group_name_set content_groups;

      };


    // INTEGRATION TASK RECORD


    template <typename number>
    class integration_task_record: public task_record<number>
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! construct an integration task record for a twopf task
        integration_task_record(const twopf_task<number>& tk, repository_record::handler_package& pkg);

        //! construct an integration task record for a threepf task
        integration_task_record(const threepf_task<number>& tk, repository_record::handler_package& pkg);

        //! override copy constructor to perform a deep copy
        integration_task_record(const integration_task_record& obj);

        //! deserialization constructor
        integration_task_record(Json::Value& reader, const boost::filesystem::path& repo_root, bool network_mode,
                                package_finder<number>& f, repository_record::handler_package& pkg);

        //! destructor is default
        virtual ~integration_task_record() = default;


        // INTERFACE

      public:

        //! Get task
        integration_task<number>* get_task() const { return(this->tk.get()); }

        //! Get abstract task
        virtual task<number>* get_abstract_task() const override { return(this->tk.get()); }

        //! Get task type
        integration_task_type get_task_type() const { return(this->type); }


        // K-CONFIGURATION DATABASE HANDLING

      public:

        //! Get relative path to kconfiguration database
        const boost::filesystem::path& get_relative_kconfig_database_path() const { return(this->kconfig_db); }

        //! Set relative path to kconfiguration database
        void set_relative_kconfig_database_path(const boost::filesystem::path& p) { this->kconfig_db = p; }

        //! Write kconfiguration database
        void write_kconfig_database(const boost::filesystem::path& db_path, bool network_mode) const;


        // ADMINISTRATION

      public:

        //! Get type
        virtual task_type get_type() const override final { return(task_type::integration); }


        // SERIALIZATION -- implements a 'serializable' interface

      public:

        //! serialize this object
        virtual void serialize(Json::Value& writer) const override;


        // CLONE

      public:

        //! clone this object
        virtual integration_task_record<number>* clone() const override { return new integration_task_record<number>(static_cast<const integration_task_record<number>&>(*this)); };


        // INTERNAL DATA

      protected:

        //! Task associated with this record
        std::unique_ptr< integration_task<number> > tk;

        //! Type of task associated with this record
        integration_task_type type;

        //! k-configuration database associated with this record
        boost::filesystem::path kconfig_db;

      };


    // POSTINTEGRATION TASK RECORD


    template <typename number>
    class postintegration_task_record: public task_record<number>
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! construct a postintegration_task_record for a zeta twopf task
        postintegration_task_record(const zeta_twopf_task<number>& tk, repository_record::handler_package& pkg);

        //! construct a postintegration_task_record for a zeta threepf task
        postintegration_task_record(const zeta_threepf_task<number>& tk, repository_record::handler_package& pkg);

        //! construct a postintegration_task_record for an fNL task
        postintegration_task_record(const fNL_task<number>& tk, repository_record::handler_package& pkg);

        //! override copy constructor to perform a deep copy
        postintegration_task_record(const postintegration_task_record& obj);

        //! deserialization constructor
        postintegration_task_record(Json::Value& reader, task_finder<number>& f, repository_record::handler_package& pkg);

        //! destructor is default
        virtual ~postintegration_task_record() = default;


        // GET CONTENTS

      public:

        //! Get task
        // TODO: refactor this so it returns a reference rather than a pointer to a task
        postintegration_task<number>* get_task() const { return(this->tk.get()); }

        //! Get abstract task
        virtual task<number>* get_abstract_task() const override { return(this->tk.get()); }

        //! Get task type
        postintegration_task_type get_task_type() const { return(this->type); }


        // ADMINISTRATION

      public:

        //! Get type
        virtual task_type get_type() const override final { return(task_type::postintegration); }


        // SERIALIZATION -- implements a 'serializable' interface

      public:

        //! serialize this object
        virtual void serialize(Json::Value& writer) const override;


        // CLONE

      public:

        //! clone this object
        virtual postintegration_task_record<number>* clone() const override { return new postintegration_task_record<number>(static_cast<const postintegration_task_record<number>&>(*this)); }


        // INTERNAL DATA

      protected:

        //! Task associated with this record
        std::unique_ptr< postintegration_task<number> > tk;

        //! Type of task associated with this record
        postintegration_task_type type;

      };


    // OUTPUT TASK RECORD


    template <typename number>
    class output_task_record: public task_record<number>
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! construct an output task record
        output_task_record(const output_task<number>& tk, repository_record::handler_package& pkg);

        //! override copy constructor to perform a deep copy
        output_task_record(const output_task_record& obj);

        //! deserialization constructor
        output_task_record(Json::Value& reader, derived_product_finder<number>& f, repository_record::handler_package& pkg);

        //! destructor is default
        virtual ~output_task_record() = default;


        // GET CONTENTS

      public:

        //! Get task
        output_task<number>* get_task() const { return(this->tk.get()); }

        //! Get abstract task
        virtual task<number>* get_abstract_task() const override { return(this->tk.get()); }


        // ADMINISTRATION

      public:

        //! Get type
        virtual task_type get_type() const override final { return(task_type::output); }


        // SERIALIZATION -- implements a 'serializable' interface

      public:

        //! serialize this object
        virtual void serialize(Json::Value& writer) const override;


        // CLONE

      public:

        //! clone this object
        virtual output_task_record<number>* clone() const override { return new output_task_record<number>(static_cast<const output_task_record<number>&>(*this)); };


        // INTERNAL DATA

      protected:

        //! Task associated with this record
        std::unique_ptr< output_task<number> > tk;

      };

  }   // namespace transport


#endif //CPPTRANSPORT_TASK_DECL_H
