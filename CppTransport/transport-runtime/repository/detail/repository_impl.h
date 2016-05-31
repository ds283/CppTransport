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

#ifndef CPPTRANSPORT_REPOSITORY_IMPL_H
#define CPPTRANSPORT_REPOSITORY_IMPL_H


#include "transport-runtime/repository/detail/repository_mode.h"
#include "transport-runtime/repository/detail/repository_decl.h"

#include "boost/filesystem/operations.hpp"


namespace transport
  {

    constexpr auto CPPTRANSPORT_REPO_TASKOUTPUT_LEAF = "output";
    constexpr auto CPPTRANSPORT_REPO_FAILURE_LEAF    = "failed";
    constexpr auto CPPTRANSPORT_REPO_LOGDIR_LEAF     = "logs";
    constexpr auto CPPTRANSPORT_REPO_TEMPDIR_LEAF    = "tempfiles";
    constexpr auto CPPTRANSPORT_REPO_TRASH_LEAF      = "trash";
    constexpr auto CPPTRANSPORT_REPO_DATABASE_LEAF   = "data.sqlite";
    constexpr auto CPPTRANSPORT_REPO_LOCKFILE_LEAF   = "lockfile";


    namespace repository_impl
      {

        template <typename Payload>
        class OutputGroupFinder
          {

            // CONSTRUCTOR, DESTRUCTOR

          public:

            //! constructor captures name of group to be found
            OutputGroupFinder(const std::string& n)
              : name(n)
              {
              }


            // INTERFACE

          public:

            bool operator()(const std::pair< const std::string, std::unique_ptr< content_group_record<Payload> > >& a)
              {
                return(a.first == name);
              }


            // INTERNAL DATA

          private:

            //! name of group to compare
            std::string name;

          };


        template <typename MapType, typename Comparator>
        void remove_if(MapType& map, Comparator comp)
          {
            for(typename MapType::iterator current = map.begin(); current != map.end(); /* no increment rule */)
              {
                if(comp(*current))
                  {
                    map.erase(current++);
                  }
                else
                  {
                    ++current;
                  }
              }
          };

      }


    // pull in repository_impl namespace for this scope
    using namespace repository_impl;


    // ADMINISTRATION


    template <typename number>
    repository<number>::repository(const boost::filesystem::path path, model_manager <number>& f, repository_mode mode,
                                   local_environment& ev, argument_cache& ar,
                                   package_finder<number> pf, task_finder<number> tf, derived_product_finder<number> dpf)
      : root_path(path.is_absolute() ? path : boost::filesystem::absolute(path)),
        access_mode(mode),
        env(ev),
        args(ar),
        error(error_handler(ev, ar)),
        warning(warning_handler(ev, ar)),
        message(message_handler(ev, ar)),
        m_finder(f),
        pkg_finder(pf),
        tk_finder(tf),
        dprod_finder(dpf),
        transactions(0)
      {
      }


    template <typename number>
    std::string repository<number>::get_name() const
      {
        if(this->root_path.filename().string() == ".")
          {
            return this->root_path.parent_path().filename().string();
          }
        else
          {
            return this->root_path.filename().string();
          }
      }


    // TRANSACTIONS


    template <typename number>
    transaction_manager repository<number>::generate_transaction_manager(std::unique_ptr<transaction_handler> handle)
      {
        if(this->transactions > 0) throw runtime_exception(exception_type::TRANSACTION_ERROR, CPPTRANSPORT_TRANSACTION_UNDERWAY);
        this->transactions++;

        return transaction_manager(this->root_path / CPPTRANSPORT_REPO_LOCKFILE_LEAF, std::move(handle));
      }


    template <typename number>
    void repository<number>::begin_transaction()
      {
        // flush caches at the beginning of a transaction, to avoid becoming out of sync with the database
        // in principle one could imagine a more complex implementation which keeps track of
        // those records that have become dirty ... but currently there seems no need for such complexity
        this->flush_caches();
      }


    template <typename number>
    void repository<number>::commit_transaction()
      {
        // flush caches likewise at the end of a transaction
        this->flush_caches();
      }


    template <typename number>
    void repository<number>::abort_transaction()
      {
        // flush caches when a transaction is aborted
        this->flush_caches();
      }


    template <typename number>
    void repository<number>::release_transaction()
      {
        if(this->transactions == 0) throw runtime_exception(exception_type::TRANSACTION_ERROR, CPPTRANSPORT_TRANSACTION_OVER_RELEASE);
        this->transactions--;
      }


    // RECORD CACHE, CACHE MANAGEMENT


    template <typename number>
    void repository<number>::flush_caches()
      {
        this->pkg_cache.clear();
        this->task_cache.clear();
        this->derived_cache.clear();
        this->int_cache.clear();
        this->pint_cache.clear();
        this->out_cache.clear();
      }


    // GENERATE WRITERS FOR TASKS


    template <typename number>
    std::unique_ptr< integration_writer<number> >
    repository<number>::new_integration_task_content(integration_task_record<number>& rec, const std::list<std::string>& tags,
                                                     unsigned int worker, unsigned int workgroup, unsigned int num_cores, std::string suffix)
      {
        std::unique_ptr< repository_integration_writer_commit<number> > commit = std::make_unique< repository_integration_writer_commit<number> >(*this);
        std::unique_ptr< repository_integration_writer_abort<number> > abort = std::make_unique< repository_integration_writer_abort<number> >(*this);

        return this->base_new_integration_task_content(rec, tags, worker, workgroup, num_cores, std::move(commit), std::move(abort), suffix);
      }


    template <typename number>
    std::unique_ptr< integration_writer<number> >
    repository<number>::recover_integration_task_content(const std::string& name, integration_task_record<number>& rec,
                                                         const boost::filesystem::path& output, const boost::filesystem::path& sql_path,
                                                         const boost::filesystem::path& logdir_path, const boost::filesystem::path& tempdir_path,
                                                         unsigned int worker, unsigned int workgroup)
      {
        std::unique_ptr< repository_integration_writer_commit<number> > commit = std::make_unique< repository_integration_writer_commit<number> >(*this);
        std::unique_ptr< repository_integration_writer_abort<number> > abort = std::make_unique< repository_integration_writer_abort<number> >(*this);

        return this->base_recover_integration_task_content(name, rec, output, sql_path, logdir_path, tempdir_path, worker, workgroup, std::move(commit), std::move(abort));
      }


    template <typename number>
    std::unique_ptr< derived_content_writer<number> >
    repository<number>::new_output_task_content(output_task_record<number>& rec, const std::list<std::string>& tags,
                                                unsigned int worker, unsigned int num_cores, std::string suffix)
      {
        std::unique_ptr< repository_derived_content_writer_commit<number> > commit = std::make_unique< repository_derived_content_writer_commit<number> >(*this);
        std::unique_ptr< repository_derived_content_writer_abort<number> > abort = std::make_unique< repository_derived_content_writer_abort<number> >(*this);

        return this->base_new_output_task_content(rec, tags, worker, num_cores, std::move(commit), std::move(abort), suffix);
      }


    template <typename number>
    std::unique_ptr< derived_content_writer<number> >
    repository<number>::recover_output_task_content(const std::string& name, output_task_record<number>& rec,
                                                    const boost::filesystem::path& output, const boost::filesystem::path& logdir_path,
                                                    const boost::filesystem::path& tempdir_path, unsigned int worker)
      {
        std::unique_ptr< repository_derived_content_writer_commit<number> > commit = std::make_unique< repository_derived_content_writer_commit<number> >(*this);
        std::unique_ptr< repository_derived_content_writer_abort<number> > abort = std::make_unique< repository_derived_content_writer_abort<number> >(*this);

        return this->base_recover_output_task_content(name, rec, output, logdir_path, tempdir_path, worker, std::move(commit), std::move(abort));
      }


    template <typename number>
    std::unique_ptr< postintegration_writer<number> >
    repository<number>::new_postintegration_task_content(postintegration_task_record<number>& rec, const std::list<std::string>& tags,
                                                         unsigned int worker, unsigned int num_cores, std::string suffix)
      {
        std::unique_ptr< repository_postintegration_writer_commit<number> > commit = std::make_unique< repository_postintegration_writer_commit<number> >(*this);
        std::unique_ptr< repository_postintegration_writer_abort<number> > abort = std::make_unique< repository_postintegration_writer_abort<number> >(*this);

        return this->base_new_postintegration_task_content(rec, tags, worker, num_cores, std::move(commit), std::move(abort), suffix);
      }


    template <typename number>
    std::unique_ptr< postintegration_writer<number> >
    repository<number>::recover_postintegration_task_content(const std::string& name, postintegration_task_record<number>& rec,
                                                             const boost::filesystem::path& output, const boost::filesystem::path& sql_path,
                                                             const boost::filesystem::path& logdir_path, const boost::filesystem::path& tempdir_path,
                                                             unsigned int worker)
      {
        std::unique_ptr< repository_postintegration_writer_commit<number> > commit = std::make_unique< repository_postintegration_writer_commit<number> >(*this);
        std::unique_ptr< repository_postintegration_writer_abort<number> > abort = std::make_unique< repository_postintegration_writer_abort<number> >(*this);

        return this->base_recover_postintegration_task_content(name, rec, output, sql_path, logdir_path, tempdir_path, worker, std::move(commit), std::move(abort));
      }


    template <typename number>
    std::unique_ptr< integration_writer<number> >
    repository<number>::base_new_integration_task_content(integration_task_record<number>& rec, const std::list<std::string>& tags,
                                                          unsigned int worker, unsigned int workgroup, unsigned int num_cores,
                                                          std::unique_ptr< repository_integration_writer_commit<number> > commit,
                                                          std::unique_ptr< repository_integration_writer_abort<number> > abort,
                                                          std::string suffix)
      {
        // get current time
        boost::posix_time::ptime now = boost::posix_time::second_clock::universal_time();

        // request a name for this content group
        boost::filesystem::path parent_path = static_cast<boost::filesystem::path>(CPPTRANSPORT_REPO_TASKOUTPUT_LEAF) / rec.get_name();
        std::string output_leaf = this->reserve_content_name(rec.get_name(), parent_path, now, suffix, num_cores);

        boost::filesystem::path output_path = parent_path / output_leaf;
        boost::filesystem::path sql_path    = output_path / CPPTRANSPORT_REPO_DATABASE_LEAF;

        // temporary stuff, location not recorded in the database
        boost::filesystem::path log_path  = output_path / CPPTRANSPORT_REPO_LOGDIR_LEAF;
        boost::filesystem::path temp_path = output_path / CPPTRANSPORT_REPO_TEMPDIR_LEAF;

        // create directories
        boost::filesystem::create_directories(this->get_root_path() / output_path);
        boost::filesystem::create_directories(this->get_root_path() / log_path);
        boost::filesystem::create_directories(this->get_root_path() / temp_path);

        generic_writer::metadata_group metadata;
        metadata.tags          = tags;
        metadata.creation_time = now;

        generic_writer::paths_group paths;
        paths.root   = this->get_root_path();
        paths.output = output_path;
        paths.data   = sql_path;
        paths.log    = log_path;
        paths.temp   = temp_path;

        // integration_writer constructor takes a copy of the integration_task_record
        return std::make_unique< integration_writer<number> >(output_leaf, rec, std::move(commit), std::move(abort), metadata, paths, worker, workgroup);
      }


    template <typename number>
    std::unique_ptr <integration_writer<number>>
    repository<number>::base_recover_integration_task_content(const std::string& name, integration_task_record<number>& rec,
                                                              const boost::filesystem::path& output_path, const boost::filesystem::path& sql_path,
                                                              const boost::filesystem::path& logdir_path, const boost::filesystem::path& tempdir_path,
                                                              unsigned int worker, unsigned int workgroup,
                                                              std::unique_ptr< repository_integration_writer_commit<number> > commit,
                                                              std::unique_ptr< repository_integration_writer_abort<number> > abort)
      {
        // get current time
        boost::posix_time::ptime now = boost::posix_time::second_clock::universal_time();

        generic_writer::metadata_group metadata;
        metadata.creation_time = now;

        generic_writer::paths_group paths;
        paths.root   = this->get_root_path();
        paths.output = output_path;
        paths.data   = sql_path;
        paths.log    = logdir_path;
        paths.temp   = tempdir_path;

        return std::make_unique< integration_writer<number> >(name, rec, std::move(commit), std::move(abort), metadata, paths, worker, workgroup);
      }


    template <typename number>
    std::unique_ptr< derived_content_writer<number> >
    repository<number>::base_new_output_task_content(output_task_record<number>& rec, const std::list<std::string>& tags,
                                                     unsigned int worker, unsigned int num_cores,
                                                     std::unique_ptr< repository_derived_content_writer_commit<number> > commit,
                                                     std::unique_ptr< repository_derived_content_writer_abort<number> > abort,
                                                     std::string suffix)
      {
        // get current time
        boost::posix_time::ptime now = boost::posix_time::second_clock::universal_time();

        // request a name for this content group
        boost::filesystem::path parent_path = static_cast<boost::filesystem::path>(CPPTRANSPORT_REPO_TASKOUTPUT_LEAF) / rec.get_name();
        std::string output_leaf = this->reserve_content_name(rec.get_name(), parent_path, now, suffix, num_cores);

        boost::filesystem::path output_path = parent_path / output_leaf;

        // temporary stuff, location not recorded in the database
        boost::filesystem::path log_path  = output_path / CPPTRANSPORT_REPO_LOGDIR_LEAF;
        boost::filesystem::path temp_path = output_path / CPPTRANSPORT_REPO_TEMPDIR_LEAF;

        // create directories
        boost::filesystem::create_directories(this->get_root_path() / output_path);
        boost::filesystem::create_directories(this->get_root_path() / log_path);
        boost::filesystem::create_directories(this->get_root_path() / temp_path);

        generic_writer::metadata_group metadata;
        metadata.tags          = tags;
        metadata.creation_time = now;

        generic_writer::paths_group paths;
        paths.root   = this->get_root_path();
        paths.output = output_path;
        paths.log    = log_path;
        paths.temp   = temp_path;

        return std::make_unique< derived_content_writer<number> >(output_leaf, rec, std::move(commit), std::move(abort), metadata, paths, worker);
      }


    template <typename number>
    std::unique_ptr< derived_content_writer<number> >
    repository<number>::base_recover_output_task_content(const std::string& name, output_task_record<number>& rec,
                                                         const boost::filesystem::path& output_path,
                                                         const boost::filesystem::path& logdir_path, const boost::filesystem::path& tempdir_path,
                                                         unsigned int worker,
                                                         std::unique_ptr< repository_derived_content_writer_commit<number> > commit,
                                                         std::unique_ptr< repository_derived_content_writer_abort<number> > abort)
      {
        // get current time
        boost::posix_time::ptime now = boost::posix_time::second_clock::universal_time();

        generic_writer::metadata_group metadata;
        metadata.creation_time = now;

        generic_writer::paths_group paths;
        paths.root   = this->get_root_path();
        paths.output = output_path;
        paths.log    = logdir_path;
        paths.temp   = tempdir_path;

        return std::make_unique< derived_content_writer<number> >(name, rec, std::move(commit), std::move(abort), metadata, paths, worker);
      }


    template <typename number>
    std::unique_ptr< postintegration_writer<number> >
    repository<number>::base_new_postintegration_task_content(postintegration_task_record<number>& rec, const std::list<std::string>& tags,
                                                              unsigned int worker, unsigned int num_cores,
                                                              std::unique_ptr< repository_postintegration_writer_commit<number> > commit,
                                                              std::unique_ptr< repository_postintegration_writer_abort<number> > abort,
                                                              std::string suffix)
      {
        // get current time
        boost::posix_time::ptime now = boost::posix_time::second_clock::universal_time();

        // request a name for this content group
        boost::filesystem::path parent_path = static_cast<boost::filesystem::path>(CPPTRANSPORT_REPO_TASKOUTPUT_LEAF) / rec.get_name();
        std::string output_leaf = this->reserve_content_name(rec.get_name(), parent_path, now, suffix, num_cores);

        boost::filesystem::path output_path = parent_path / output_leaf;
        boost::filesystem::path sql_path    = output_path / CPPTRANSPORT_REPO_DATABASE_LEAF;

        // temporary stuff, location not recorded in the database
        boost::filesystem::path log_path    = output_path / CPPTRANSPORT_REPO_LOGDIR_LEAF;
        boost::filesystem::path temp_path   = output_path / CPPTRANSPORT_REPO_TEMPDIR_LEAF;

        // create directories
        boost::filesystem::create_directories(this->get_root_path() / output_path);
        boost::filesystem::create_directories(this->get_root_path() / log_path);
        boost::filesystem::create_directories(this->get_root_path() / temp_path);

        generic_writer::metadata_group metadata;
        metadata.tags          = tags;
        metadata.creation_time = now;

        generic_writer::paths_group paths;
        paths.root   = this->get_root_path();
        paths.output = output_path;
        paths.data   = sql_path;
        paths.log    = log_path;
        paths.temp   = temp_path;

        return std::make_unique< postintegration_writer<number> >(output_leaf, rec, std::move(commit), std::move(abort), metadata, paths, worker);
      }


    template <typename number>
    std::unique_ptr< postintegration_writer<number> >
    repository<number>::base_recover_postintegration_task_content(const std::string& name, postintegration_task_record<number>& rec,
                                                                  const boost::filesystem::path& output_path, const boost::filesystem::path& sql_path,
                                                                  const boost::filesystem::path& logdir_path, const boost::filesystem::path& tempdir_path,
                                                                  unsigned int worker,
                                                                  std::unique_ptr< repository_postintegration_writer_commit<number> > commit,
                                                                  std::unique_ptr< repository_postintegration_writer_abort<number> > abort)
      {
        // get current time
        boost::posix_time::ptime now = boost::posix_time::second_clock::universal_time();

        generic_writer::metadata_group metadata;
        metadata.creation_time = now;

        generic_writer::paths_group paths;
        paths.root   = this->get_root_path();
        paths.output = output_path;
        paths.data   = sql_path;
        paths.log    = logdir_path;
        paths.temp   = tempdir_path;

        return std::make_unique< postintegration_writer<number> >(name, rec, std::move(commit), std::move(abort), metadata, paths, worker);
      }


    // STANDARD WRITER CALLBACKS


    template <typename number>
    template <typename Payload>
    void repository<number>::advise_commit(content_group_record<Payload>* group)
      {
        std::ostringstream msg;

        boost::posix_time::ptime now = boost::posix_time::second_clock::local_time();

        msg << CPPTRANSPORT_REPO_COMMITTING_CONTENT_GROUP_A << " '" << group->get_name() << "' "
          << CPPTRANSPORT_REPO_COMMITTING_CONTENT_GROUP_B << " '" << group->get_task_name() << "' "
          << CPPTRANSPORT_REPO_COMMITTING_CONTENT_GROUP_D << " " << boost::posix_time::to_simple_string(now);
        this->message(msg.str());

        if(group->get_payload().is_failed())
          {
            std::ostringstream warn;
            warn << CPPTRANSPORT_REPO_WARN_CONTENT_GROUP_A << " '" << group->get_name() << "' " << CPPTRANSPORT_REPO_WARN_CONTENT_GROUP_B;
            this->warning(warn.str());
          }
      }


    template <typename number>
    void repository<number>::close_integration_writer(integration_writer<number>& writer, transaction_manager& mgr)
      {
        // read record from database (so that we have a fresh and up-to-date copy of the data)
        std::unique_ptr< task_record<number> > raw_rec = this->query_task(writer.get_task_name(), mgr);
        integration_task_record<number>* rec = dynamic_cast< integration_task_record<number>* >(raw_rec.get());

        assert(rec != nullptr);
        if(rec == nullptr) throw runtime_exception(exception_type::REPOSITORY_ERROR, CPPTRANSPORT_REPO_RECORD_CAST_FAILED);

        // set up notes for the new output record, if it exists
        std::list<note> notes;
        if(!writer.is_collecting_statistics())
          {
            std::ostringstream msg;
            msg << CPPTRANSPORT_REPO_NOTE_NO_STATISTICS << " '" << rec->get_task()->get_model()->get_backend() << "'";
            notes.emplace_back(this->env.get_userid(), msg.str());
          }

        // create a new, empty content group record
        std::unique_ptr< content_group_record<integration_payload> >
          output_record(this->integration_content_group_record_factory(writer, false, notes, mgr));

        // stamp content group with the correct 'created' time stamp
        output_record->set_creation_time(writer.get_creation_time());
        output_record->set_name(writer.get_name());

        // populate payload with content from the writer
        integration_payload& payload = output_record->get_payload();
        payload.set_container_path(writer.get_relative_container_path());
        payload.set_metadata(writer.get_metadata());
        payload.set_workgroup_number(writer.get_workgroup_number());
        payload.set_data_type(writer.get_data_type());

        if(writer.is_seeded()) payload.set_seed(writer.get_seed_group());

        payload.set_fail(writer.is_failed());
        payload.set_failed_serials(writer.get_missing_serials());

        payload.set_statistics(writer.is_collecting_statistics());
        payload.set_initial_conditions(writer.is_collecting_initial_conditions());

        try
          {
            payload.set_size(boost::filesystem::file_size(this->root_path / writer.get_relative_container_path()));
          }
        catch(boost::filesystem::filesystem_error& xe)
          {
            payload.set_size(0);
          }

        // commit new output record
        output_record->commit();

        // add this content group to the integration task record
        rec->add_new_content_group(output_record->get_name());
        rec->commit();

        this->advise_commit(output_record.get());
      }


    template <typename number>
    void repository<number>::abort_integration_writer(integration_writer<number>& writer, transaction_manager& mgr)
      {
        boost::filesystem::path fail_path = this->get_root_path() / CPPTRANSPORT_REPO_FAILURE_LEAF / writer.get_task_name();

        if(!boost::filesystem::exists(fail_path)) boost::filesystem::create_directories(fail_path);
        if(boost::filesystem::is_directory(fail_path))
          {
            boost::filesystem::path abs_dest = fail_path / writer.get_relative_output_path().leaf();

            try
              {
                boost::filesystem::rename(writer.get_abs_output_path(), abs_dest);
              }
            catch(boost::filesystem::filesystem_error& xe)
              {
                throw runtime_exception(exception_type::REPOSITORY_ERROR, CPPTRANSPORT_REPO_CANT_WRITE_FAILURE_PATH);
              }

            std::ostringstream msg1;
            msg1 << CPPTRANSPORT_REPO_FAILED_TASK_CONTENT_GROUP_A << " '" << writer.get_task_name() << "': " << CPPTRANSPORT_REPO_FAILED_TASK_CONTENT_GROUP_B;
            this->error(msg1.str());

            std::ostringstream msg2;
            msg2 << CPPTRANSPORT_REPO_CONTENT_FAILED_A << " '" << writer.get_name() << "' " << CPPTRANSPORT_REPO_CONTENT_FAILED_B;
            this->warning(msg2.str());
          }
        else throw runtime_exception(exception_type::REPOSITORY_ERROR, CPPTRANSPORT_REPO_CANT_WRITE_FAILURE_PATH);
      }


    template <typename number>
    void repository<number>::close_postintegration_writer(postintegration_writer<number>& writer, transaction_manager& mgr)
      {
        // read record from database (so that we have a fresh and up-to-date copy of the data)
        std::unique_ptr< task_record<number> > raw_rec = this->query_task(writer.get_task_name(), mgr);
        postintegration_task_record<number>* rec = dynamic_cast< postintegration_task_record<number>* >(raw_rec.get());

        assert(rec != nullptr);
        if(rec == nullptr) throw runtime_exception(exception_type::REPOSITORY_ERROR, CPPTRANSPORT_REPO_RECORD_CAST_FAILED);

        // convert to task class
        postintegration_task<number>* tk = rec->get_task();
        assert(tk != nullptr);

        // get task class for parent
        derivable_task<number>* ptk = tk->get_parent_task();
        assert(ptk != nullptr);

        // create a new, empty content group record
        std::unique_ptr<content_group_record<postintegration_payload>>
          output_record(this->postintegration_content_group_record_factory(writer, false, std::list<note>(), mgr));

        // stamp content group with the correct 'created' time stamp
        output_record->set_creation_time(writer.get_creation_time());
        output_record->set_name(writer.get_name());

        // populate payload with content from the writer
        postintegration_payload& payload = output_record->get_payload();
        payload.set_container_path(writer.get_relative_container_path());
        payload.set_metadata(writer.get_metadata());

        if(writer.is_seeded()) payload.set_seed(writer.get_seed_group());

        payload.set_pair(writer.is_paired());
        payload.set_parent_group(writer.get_parent_group());

        payload.set_fail(writer.is_failed());
        payload.set_failed_serials(writer.get_missing_serials());

        try
          {
            payload.set_size(boost::filesystem::file_size(this->root_path / writer.get_relative_container_path()));
          }
        catch(boost::filesystem::filesystem_error& xe)
          {
            payload.set_size(0);
          }

        // tag this content group with its contents
        if(writer.get_products().get_zeta_twopf())   payload.get_precomputed_products().add_zeta_twopf();
        if(writer.get_products().get_zeta_threepf()) payload.get_precomputed_products().add_zeta_threepf();
        if(writer.get_products().get_zeta_redbsp())  payload.get_precomputed_products().add_zeta_redbsp();
        if(writer.get_products().get_fNL_local())    payload.get_precomputed_products().add_fNL_local();
        if(writer.get_products().get_fNL_equi())     payload.get_precomputed_products().add_fNL_equi();
        if(writer.get_products().get_fNL_ortho())    payload.get_precomputed_products().add_fNL_ortho();
        if(writer.get_products().get_fNL_DBI())      payload.get_precomputed_products().add_fNL_DBI();

        // commit new output record
        output_record->commit();

        // add this content group to the integration task record
        rec->add_new_content_group(output_record->get_name());
        rec->commit();

        this->advise_commit(output_record.get());
      }


    template <typename number>
    void repository<number>::abort_postintegration_writer(postintegration_writer<number>& writer, transaction_manager& mgr)
      {
        boost::filesystem::path fail_path = this->get_root_path() / CPPTRANSPORT_REPO_FAILURE_LEAF / writer.get_task_name();

        if(!boost::filesystem::exists(fail_path)) boost::filesystem::create_directories(fail_path);
        if(boost::filesystem::is_directory(fail_path))
          {
            boost::filesystem::path abs_dest = fail_path / writer.get_relative_output_path().leaf();

            try
              {
                boost::filesystem::rename(writer.get_abs_output_path(), abs_dest);
              }
            catch(boost::filesystem::filesystem_error& xe)
              {
                throw runtime_exception(exception_type::REPOSITORY_ERROR, CPPTRANSPORT_REPO_CANT_WRITE_FAILURE_PATH);
              }

            std::ostringstream msg1;
            msg1 << CPPTRANSPORT_REPO_FAILED_POSTINT_GROUP_A << " '" << writer.get_task_name() << "': " << CPPTRANSPORT_REPO_FAILED_POSTINT_GROUP_B;
            this->error(msg1.str());

            std::ostringstream msg2;
            msg2 << CPPTRANSPORT_REPO_CONTENT_FAILED_A << " '" << writer.get_name() << "' " << CPPTRANSPORT_REPO_CONTENT_FAILED_B;
            this->warning(msg2.str());
          }
        else throw runtime_exception(exception_type::REPOSITORY_ERROR, CPPTRANSPORT_REPO_CANT_WRITE_FAILURE_PATH);
      }


    template <typename number>
    void repository<number>::close_derived_content_writer(derived_content_writer<number>& writer, transaction_manager& mgr)
      {
        // read record from database (so that we have a fresh and up-to-date copy of the data)
        std::unique_ptr< task_record<number> > raw_rec = this->query_task(writer.get_task_name(), mgr);
        output_task_record<number>* rec = dynamic_cast< output_task_record<number>* >(raw_rec.get());

        assert(rec != nullptr);
        if(rec == nullptr) throw runtime_exception(exception_type::REPOSITORY_ERROR, CPPTRANSPORT_REPO_RECORD_CAST_FAILED);

        // create a new, empty content group record
        std::unique_ptr< content_group_record<output_payload> >
          output_record(this->output_content_group_record_factory(writer, false, std::list<note>(), mgr));

        // stamp content group with the correct 'created' time stamp
        output_record->set_creation_time(writer.get_creation_time());
        output_record->set_name(writer.get_name());

        // populate payload with content from the writer
        output_payload& payload = output_record->get_payload();
        for(const derived_content& c : writer.get_content())
          {
            payload.add_derived_content(c);
          }
        payload.set_metadata(writer.get_metadata());
        payload.set_fail(false);
        payload.set_content_groups_summary(writer.get_content_groups());

        // commit new output record
        output_record->commit();

        // add this content group to the integration task record
        rec->add_new_content_group(output_record->get_name());
        rec->commit();

        this->advise_commit(output_record.get());
      }


    template <typename number>
    void repository<number>::abort_derived_content_writer(derived_content_writer<number>& writer, transaction_manager& mgr)
      {
        boost::filesystem::path fail_path = this->get_root_path() / CPPTRANSPORT_REPO_FAILURE_LEAF / writer.get_task_name();

        if(!boost::filesystem::exists(fail_path)) boost::filesystem::create_directories(fail_path);
        if(boost::filesystem::is_directory(fail_path))
          {
            boost::filesystem::path abs_dest = fail_path / writer.get_relative_output_path().leaf();

            try
              {
                boost::filesystem::rename(writer.get_abs_output_path(), abs_dest);
              }
            catch(boost::filesystem::filesystem_error& xe)
              {
                throw runtime_exception(exception_type::REPOSITORY_ERROR, CPPTRANSPORT_REPO_CANT_WRITE_FAILURE_PATH);
              }

            std::ostringstream msg1;
            msg1 << CPPTRANSPORT_REPO_FAILED_OUTPUT_CONTENT_GROUP_A << " '" << writer.get_task_name() << "': " << CPPTRANSPORT_REPO_FAILED_OUTPUT_CONTENT_GROUP_B;
            this->error(msg1.str());

            std::ostringstream msg2;
            msg2 << CPPTRANSPORT_REPO_CONTENT_FAILED_A << " '" << writer.get_name() << "' " << CPPTRANSPORT_REPO_CONTENT_FAILED_B;
            this->warning(msg2.str());
          }
        else throw runtime_exception(exception_type::REPOSITORY_ERROR, CPPTRANSPORT_REPO_CANT_WRITE_FAILURE_PATH);
      }


    // FIND AN OUTPUT GROUP MATCHING DEFINED TAGS


    template<typename number>
    std::unique_ptr< content_group_record<integration_payload> >
    repository<number>::find_integration_task_output(const std::string& name, const std::list<std::string>& tags)
      {
        // search for content groups associated with this task
        integration_content_db db = this->enumerate_integration_task_content(name);

        // remove items which are marked as failed
        repository_impl::remove_if(db, [&] (const std::pair< const std::string, std::unique_ptr< content_group_record<integration_payload> > >& group) { return(group.second->get_payload().is_failed()); } );

        // remove items from the list which have mismatching tags
        repository_impl::remove_if(db, [&] (const std::pair< const std::string, std::unique_ptr< content_group_record<integration_payload> > >& group) { return(!group.second->check_tags(tags)); } );

        if(db.empty())
          {
            std::ostringstream msg;
            msg << CPPTRANSPORT_REPO_NO_MATCHING_CONTENT_GROUPS << " '" << name << "'";
            throw runtime_exception(exception_type::REPOSITORY_ERROR, msg.str());
          }

        std::unique_ptr< content_group_record<integration_payload> > rval;
        (*db.begin()).second.swap(rval);
        return(std::move(rval));    // std::move required by GCC 5.2 although standard implies that copy elision should occur
      }


    template<typename number>
    std::unique_ptr< content_group_record<postintegration_payload> >
    repository<number>::find_postintegration_task_output(const std::string& name, const std::list<std::string>& tags)
      {
        // search for content groups associated with this task
        postintegration_content_db db = this->enumerate_postintegration_task_content(name);

        // remove items which are marked as failed
        repository_impl::remove_if(db, [&] (const std::pair< const std::string, std::unique_ptr< content_group_record<postintegration_payload> > >& group) { return(group.second->get_payload().is_failed()); } );

        // remove items from the list which have mismatching tags
        repository_impl::remove_if(db, [&] (const std::pair< const std::string, std::unique_ptr< content_group_record<postintegration_payload> > >& group) { return(!group.second.get()->check_tags(tags)); } );

        if(db.empty())
          {
            std::ostringstream msg;
            msg << CPPTRANSPORT_REPO_NO_MATCHING_CONTENT_GROUPS << " '" << name << "'";
            throw runtime_exception(exception_type::REPOSITORY_ERROR, msg.str());
          }

        std::unique_ptr< content_group_record<postintegration_payload> > rval;
        (*db.begin()).second.swap(rval);
        return(std::move(rval));    // std::move required by GCC 5.2 although standard implies that copy elision should occur
      }

  }   // namespace transport


#endif //CPPTRANSPORT_REPOSITORY_IMPL_H
