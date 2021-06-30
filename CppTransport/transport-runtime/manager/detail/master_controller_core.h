//
// Created by David Seery on 22/01/2016.
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

#ifndef CPPTRANSPORT_MANAGER_DETAIL_MASTER_CONTROLLER_CORE_H
#define CPPTRANSPORT_MANAGER_DETAIL_MASTER_CONTROLLER_CORE_H


#include "transport-runtime/manager/detail/master_controller_decl.h"
#include "transport-runtime/repository/repository_toolkit.h"
#include "transport-runtime/repository/repository_graphkit.h"
#include "transport-runtime/repository/repository_upgradekit.h"

#include "transport-runtime/build_data.h"
#include "transport-runtime/defaults.h"

#include "boost/timer/timer.hpp"


namespace transport
  {

    template <typename number>
    bool master_controller_impl::ContentGroupFilterPredicate<number>::match_integration_content_group
      (const std::string& name) const
      {
        auto rec = this->repo.query_integration_content(name);
        if(!rec) return false;

        // check required tags match, if present
        if(!rec->check_tags(this->tags)) return false;

        auto& payload = rec->get_payload();

        // if failed, no match
        if(payload.is_failed()) return false;

        // test whether payload contains everything required by specifier
        return payload >= this->specifier;
      }


    template <typename number>
    bool master_controller_impl::ContentGroupFilterPredicate<number>::match_postintegration_content_group
    (const std::string& name) const
      {
        auto rec = this->repo.query_postintegration_content(name);
        if(!rec) return false;

        // check required tags match, if present
        if(!rec->check_tags(this->tags)) return false;

        auto& payload = rec->get_payload();

        // if failed, no match
        if(payload.is_failed()) return false;

        // test whether payload contains everything required by specifier
        return payload >= this->specifier;
      }


    template <typename number>
    master_controller<number>::master_controller(boost::mpi::environment& e, boost::mpi::communicator& w,
                                                 local_environment& le, argument_cache& ac, model_manager<number>& f,
                                                 task_gallery<number>& g)
      : environment(e),
        world(w),
        local_env(le),
        arg_cache(ac),
        model_mgr(f),
        gallery(g),
        data_mgr(data_manager_factory<number>(le, ac)),
        journal(w.size()-1),
        err(error_handler(le, ac)),
        warn(warning_handler(le, ac)),
        msg(message_handler(le, ac)),
        cmdline_reports(le, ac),
        HTML_reports(le, ac),
        last_push_to_repo(boost::posix_time::second_clock::universal_time()),
        work_scheduler(w.size() > 0 ? static_cast<unsigned int>(w.size()-1) : 0),
        work_manager(w.size() > 0 ? static_cast<unsigned int>(w.size()-1) : 0),
        reporter(work_scheduler, work_manager, busyidle_timers, le, ac)
      {
        // create global busy/idle timer
        busyidle_timers.add_new_timer(CPPTRANSPORT_DEFAULT_TIMER);
      }


    template <typename number>
    void master_controller<number>::pre_process_tasks()
      {
        // capture busy/idle timers and switch to busy mode
        busyidle_instrument timers(this->busyidle_timers);

        // perform repository operations if requested
        if(this->arg_cache.get_recovery_mode() && this->repo)
          {
            this->repo->perform_recovery(*this->data_mgr, this->get_rank());
          }

        if(this->arg_cache.get_upgrade_mode() && this->repo)
          {
            auto ukit = upgradekit_factory<number>(*this->repo, this->arg_cache, this->err, this->warn, this->msg);
            (*ukit)();
          }
        
        if(this->arg_cache.get_model_list())
          {
            this->model_mgr.write_models(std::cout);
          }

        if(this->arg_cache.get_create_model() && this->repo)
          {
            this->gallery.commit(*this->repo);
          }

        // handle repository action switches
        // these were left unhandled during option parsing
        this->recognize_action_switches(this->option_map);
      }


    template <typename number>
    void master_controller<number>::post_process_tasks()
      {
        // capture busy/idle timers and switch to busy mode
        busyidle_instrument timers(this->busyidle_timers);

        if(this->repo)
          {
            this->cmdline_reports.report(*this->repo);
          }

        if(this->repo && this->data_mgr)
          {
            this->HTML_reports.report(*this->repo, *this->data_mgr);
          }

        if(this->aggregation_profile_root)
          {
            for(const aggregation_profiler& profiler : this->aggregation_profiles)
              {
                profiler.write_to_csv(*this->aggregation_profile_root);
              }
          }
      }


    template <typename number>
    void master_controller<number>::autocomplete_task_schedule()
      {
        // capture busy/idle timers and switch to busy mode
        busyidle_instrument timers(this->busyidle_timers);

        // check whether any work is required: if there are no jobs there is no need for autocompletion
        if(this->job_queue.empty()) return;

        // work out which tasks are needed (and what properties their content should have, if any)
        // to satisfy the dependencies of all tasks specified on the command line
        requirement_list_type requirements;
        for(const job_descriptor& job : this->job_queue)
          {
            switch(job.get_type())
              {
                case job_type::job_task:
                  {
                    this->add_task_requirements(requirements, job.get_name());
                    break;
                  }
              }
          }

        // required_tasks holds a list of all tasks needed to generate output for the set of tasks specified on
        // the command line
        // Of these, we want to check which have no current content, and therefore need tasks scheduling
        this->prune_tasks_with_content(requirements);

        // required_tasks now contains only those tasks for which output is needed;
        // we need to prune it for paired tasks, because these do not need to be handled separately
        this->prune_paired_tasks(requirements);

        // sort task list into order, and insert any required job descriptors
        repository_graphkit<number> graphkit{*this->repo, this->err, this->warn, this->msg};
        std::unique_ptr<repository_distance_matrix> dmat = graphkit.task_distance_matrix();
        auto topological_order = dmat->get_graph().compute_topological_order();
        if(topological_order)
          this->insert_job_descriptors(requirements, *topological_order);
      }


    template <typename number>
    void master_controller<number>::add_postintegration_task_requirements
      (requirement_list_type& requirements, const postintegration_task<number>& task)
      {
        const auto& parent_task = *task.get_parent_task();

        switch(task.get_task_type())
          {
            case postintegration_task_type::twopf:
              {
                // for 2pf tasks, need to check whether spectral data is expected
                const auto& z2pf = dynamic_cast< const zeta_twopf_db_task<number>& >(task);

                // parent task is of integration type
                requirement_type elt{parent_task.get_name(),
                                     {false, false, z2pf.get_collect_spectral_data()}};

                auto t = std::find(requirements.begin(), requirements.end(), elt);
                if(t == requirements.end())
                  {
                    requirements.push_back(std::move(elt));
                  }
                break;
              }

            case postintegration_task_type::threepf:
              {
                // no specific requirements, since 2pf and 3pf will always be available
                // in content generated by an integration task
                requirement_type elt{parent_task.get_name(), {false, false, false}};

                auto t = std::find(requirements.begin(), requirements.end(), elt);
                if(t == requirements.end())
                  {
                    requirements.push_back(std::move(elt));
                  }
                break;
              }

            case postintegration_task_type::fNL:
              {
                // fNL tasks expect reduced bispectrum and zeta 2pf to be present
                const auto& zfNL = dynamic_cast< const fNL_task<number>& >(task);

                // parent task is of postintegration type
                precomputed_products products{true, false, false, true,
                                              false, false, false, false};
                requirement_type elt{parent_task.get_name(), content_group_specifier{products}};

                auto t = std::find(requirements.begin(), requirements.end(), elt);
                if(t == requirements.end())
                  {
                    requirements.push_back(std::move(elt));
                  }
                break;
              }
          }

        // recursively add requirements for parent task
        const auto& ptk = *task.get_parent_task();
        this->add_task_requirements(requirements, ptk.get_name());
      }


    template <typename number>
    void master_controller<number>::add_output_task_requirements
      (requirement_list_type& requirements, const output_task<number>& task)
      {
        const auto& elements = task.get_elements();

        for(const auto& elt : elements)
          {
            auto& product = elt.get_product();

            // get list of tasks this product depends on
            auto task_list = product.get_task_dependencies();
            for(const auto& e : task_list) // TODO: change to std::views::values in C++20 and remove use of .second
              {
                const auto& t = e.second.get_task();
                const auto& s = e.second.get_specifier();

                requirement_type r{t.get_name(), s};
                auto u = std::find(requirements.begin(), requirements.end(), r);
                if(u == requirements.end())
                  {
                    requirements.push_back(std::move(r));
                  }
                this->add_task_requirements(requirements, t.get_name());
              }
          }
      }


    template <typename number>
    void master_controller<number>::add_task_requirements
      (requirement_list_type& requirements, const std::string& task_name)
      {
        auto rec = this->repo->query_task(task_name);

        if(!rec)
          {
            std::ostringstream msg;
            msg << CPPTRANSPORT_TASK_MISSING << " '" << task_name << "'";
            throw runtime_exception(exception_type::REPOSITORY_ERROR, msg.str());
          }

        switch(rec->get_type())
          {
            case task_type::integration:
              {
                // integration tasks have no dependencies
                break;
              }

            case task_type::postintegration:
              {
                // postintegration tasks depend on a parent task
                const auto& prec = dynamic_cast< postintegration_task_record<number>& >(*rec);
                const auto& tk = *prec.get_task();

                this->add_postintegration_task_requirements(requirements, tk);
                break;
              }

            case task_type::output:
              {
                // output tasks depend on derived products, each of which may itself depend on other tasks
                const auto& orec = dynamic_cast< output_task_record<number>& >(*rec);
                const auto& tk = *orec.get_task();

                this->add_output_task_requirements(requirements, tk);
                break;
              }
          }
      }


    template <typename number>
    void master_controller<number>::prune_tasks_with_content(requirement_list_type& requirements)
      {
        // capture busy/idle timers and switch to busy mode
        busyidle_instrument timers(this->busyidle_timers);

        // work through list of tasks needed as dependencies, deciding whether they have content groups
        for(auto t = requirements.begin(); t != requirements.end(); /* intentionally no update step */)
          {
            auto rec = this->repo->query_task(t->first);

            if(!rec)
              {
                std::ostringstream msg;
                msg << CPPTRANSPORT_TASK_MISSING << " '" << t->first << "'";
                throw runtime_exception(exception_type::REPOSITORY_ERROR, msg.str());
              }

            // check for presence of content group matching the required specifiers (if present)
            bool found_match = false;
            found_match = rec->get_content_groups(
              master_controller_impl::ContentGroupFilterPredicate<number>(*this->repo, t->second, this->tags)).size() > 0;

            if(found_match)
              {
                // group present; no need to schedule this task, so erase it from the list. Leaves t pointing to next item.
                t = requirements.erase(t);
              }
            else
              {
                // no groups present; move on to next item
                ++t;
              }
          }
      }


    template <typename number>
    void master_controller<number>::prune_paired_tasks(requirement_list_type& requirements)
      {
        // capture busy/idle timers and switch to busy mode
        busyidle_instrument timers(this->busyidle_timers);

        // filter out any tasks in requirements which are only there because they're paired
        for(const auto& elt : requirements)
          {
            // determine whether this elt has any paired tasks
            // if so, remove them from requirements unless they were specified on the command line
            auto rec = this->repo->query_task(elt.first);

            if(!rec)
              {
                std::ostringstream msg;
                msg << CPPTRANSPORT_TASK_MISSING << " '" << elt.first << "'";
                throw runtime_exception(exception_type::REPOSITORY_ERROR, msg.str());
              }

            std::string paired_task;

            if(rec->get_type() == task_type::postintegration)
              {
                const auto& prec = dynamic_cast< const postintegration_task_record<number>& >(*rec);
                const postintegration_task<number>& tk = *prec.get_task();

                // only zeta 2pf and zeta 3pf tasks can be paired
                if(tk.get_task_type() == postintegration_task_type::twopf)
                  {
                    const auto& ztk = dynamic_cast< const zeta_twopf_task<number>& >(tk);
                    if(ztk.is_paired()) paired_task = ztk.get_parent_task()->get_name();
                  }
                else if(tk.get_task_type() == postintegration_task_type::threepf)
                  {
                    const auto& ztk = dynamic_cast< const zeta_threepf_task<number>& >(tk);
                    if(ztk.is_paired()) paired_task = ztk.get_parent_task()->get_name();
                  }
              }

            // if there is a paired elt, check whether it should be removed from requirements
            if(!paired_task.empty())
              {
                auto u = std::find_if(requirements.begin(), requirements.end(),
                                      [&](const auto& item) { return item.first == paired_task; });

                // only remove if it wasn't explicitly specified on the command line
                if(u != requirements.end())
                  {
                    auto j = std::find_if(this->job_queue.begin(), this->job_queue.end(),
                                          FindJobDescriptorByName(paired_task));

                    if(j == this->job_queue.end()) requirements.erase(u);
                  }
              }
          }
      }


    template <typename number>
    void master_controller<number>::insert_job_descriptors
      (const requirement_list_type& requirements, const ordered_record_name_set& order)
      {
        // capture busy/idle timers and switch to busy mode
        busyidle_instrument timers(this->busyidle_timers);

        // add extra tasks to job queue (not yet in any particular order)
        for(const auto& elt : requirements)
          {
            auto j = std::find_if(this->job_queue.cbegin(), this->job_queue.cend(),
                                  FindJobDescriptorByName(elt.first));

            if(j == this->job_queue.cend())   // no corresponding job in queue, so we need to insert one
              {
                this->job_queue.emplace_back(job_type::job_task, elt.first, this->tags);
              }
          }

        // sort job queue into topological order
        this->job_queue.sort(CompareJobDescriptorByList(order));
      }


    template <typename number>
    void master_controller<number>::validate_tasks()
      {
        // capture busy/idle timers and switch to busy mode
        busyidle_instrument timers(this->busyidle_timers);

        for(auto t = this->job_queue.begin(); t != this->job_queue.end(); /* intentionally no update step */)
          {
            const job_descriptor& job = *t;

            try
              {
                std::unique_ptr< task_record<number> > record = this->repo->query_task(job.get_name());
                ++t;
              }
            catch(runtime_exception& xe)
              {
                if(xe.get_exception_code() == exception_type::RECORD_NOT_FOUND)
                  {
                    std::ostringstream msg;
                    msg << CPPTRANSPORT_TASK_MISSING << " '" << job.get_name() << "'";
                    this->warn(msg.str());
                  }
                else
                  {
                    std::ostringstream msg;
                    msg << CPPTRANSPORT_TASK_CANT_BE_READ << " '" << job.get_name() << "' (" << xe.what() << ")";
                    this->warn(msg.str());
                  }
                t = this->job_queue.erase(t);
              }
          }
      }


    template <typename number>
    void master_controller<number>::execute_tasks()
      {
        // capture busy/idle timers and switch to busy mode
        busyidle_instrument timers(this->busyidle_timers);

        if(!(this->get_rank() == 0)) throw runtime_exception(exception_type::MPI_ERROR, CPPTRANSPORT_EXEC_SLAVE);

        // set up worker scope
        // WorkerPool_Context will send WORKER_SETUP messages to all workers if the repository is not null
        // when it goes out of scope its destructor will send TERMINATE messages
        // this way, even if we break out of this function by an exception,
        // the workers should all get TERMINATE instructions
        WorkerPool_Context<number> bundle(this->environment, this->world, this->repo.get(), this->journal, this->arg_cache, this->busyidle_timers);

        if(!this->repo)
          {
            this->err(CPPTRANSPORT_REPO_NONE);
            return;
          }
        
        // validate that requested tasks can be read from the database
        // any that can't be read are removed from the job queue and a warning issued
        this->validate_tasks();
    
        // schedule extra tasks if any explicitly-required tasks depend on content from
        // a second task, but no content group is available
        this->autocomplete_task_schedule();
    
        unsigned int tasks_complete = 0;
        unsigned int tasks_processed = 0;
        for(const job_descriptor& job : this->job_queue)
          {
            // advise report manager that we are commencing a new task
            this->reporter.new_task(job.get_name(), tasks_processed+1, this->job_queue.size());
        
            switch(job.get_type())
              {
                case job_type::job_task:
                  {
                    try
                      {
                        this->process_task(job);
                        ++tasks_complete; // increment number of tasks successfully completed
                      }
                    catch(runtime_exception& xe)
                      {
#ifdef TRACE_OUTPUT
                        std::cout << "TRACE_OUTPUT J" << '\n';
#endif
                        this->err(xe.what());
                      }
                    catch(std::exception& xe)
                      {
#ifdef TRACE_OUTPUT
                        std::cout << "TRACE_OUTPUT K" << '\n';
#endif
                        std::ostringstream msg;
                        msg << CPPTRANSPORT_UNEXPECTED_UNHANDLED << " " << xe.what();
                        this->err(msg.str());
                      }
                    break;
                  }
              }
        
            // advise report manager that we have finished a task
            this->reporter.end_task();
            ++tasks_processed; // increment number of tasks processed (not necessarily successfully)
          }
    
        // produce activity reports if they have been requested
        if(this->arg_cache.get_gantt_chart())
          {
            this->reporter.announce(CPPTRANSPORT_PROCESSING_GANTT_CHART);
            this->journal.make_gantt_chart(this->arg_cache.get_gantt_filename(), this->local_env, this->arg_cache);
          }
        
        if(this->arg_cache.get_journal())
          {
            this->reporter.announce(CPPTRANSPORT_PROCESSING_ACTIVITY_JOURNAL);
            this->journal.make_journal(this->arg_cache.get_journal_filename(), this->local_env, this->arg_cache);
          }
    
        // issue advisory
        if(tasks_complete > 0) this->reporter.summary_report(tasks_complete, this->compute_worker_loads());
      }
    
    
    template <typename number>
    load_data master_controller<number>::compute_worker_loads()
      {
        // capture busy/idle timers and switch to busy mode
        busyidle_instrument timers(this->busyidle_timers);
        
        // set up instrument to journal the MPI communication if needed
        journal_instrument instrument(this->journal, master_work_event::event_type::MPI_begin, master_work_event::event_type::MPI_end);
        
        double max = 0.0;
        double min = 1.0;
        double total = 0.0;
        unsigned int respondents = 0;
        
        // send QUERY_PERFORMANCE_DATA message to all workers
        std::vector<boost::mpi::request> requests(this->world.size()-1);
        for(unsigned int i = 0; i < this->world.size()-1; ++i)
          {
            requests[i] = this->world.isend(this->worker_rank(i), MPI::QUERY_PERFORMANCE_DATA);
          }
        
        // wait for all messages to be received
        boost::mpi::wait_all(requests.begin(), requests.end());
        
        // collect REPORT_PERFORMANCE_DATA messages
        while(respondents < this->world.size()-1)
          {
            timers.idle();
            boost::mpi::status stat = this->world.probe();
            
            timers.busy();
            switch(stat.tag())
              {
                case MPI::REPORT_PERFORMANCE_DATA:
                  {
                    MPI::performance_data_payload payload;
                    this->world.recv(stat.source(), MPI::REPORT_PERFORMANCE_DATA, payload);
                    ++respondents;
                    
                    double load = payload.get_load_average();
                    total += load;
                    
                    if(load > max) max = load;
                    if(load < min) min = load;
                    break;
                  }
    
                default:
                  {
                    // ignore unexpected messages
                    this->world.recv(stat.source(), stat.tag());
                    break;
                  }
              }
          }
        
        double avg = respondents > 0 ? total / respondents : total;
        return std::make_tuple(min, max, avg);
      }


    template <typename number>
    void master_controller<number>::process_task(const job_descriptor& job)
      {
        // capture busy/idle timers and switch to busy mode
        busyidle_instrument timers(this->busyidle_timers);

        std::unique_ptr< task_record<number> > record;

        try
          {
            // query a task record with the name we're looking for from the database;
            // will throw RECORD_NOT_FOUND if the task record is not present

            // notice that this record will persist throughout the entire job, until this function exits
            // however, we have to be cautious about using it because other processes may write to the
            // repository for the same job

            // Because we don't hold any open transactions at this point the record is returned in readonly mode
            // When commits need to occur, the record is looked up again with an active transaction,
            // meaning that it is converted to read/write status
            record = this->repo->query_task(job.get_name());
          }
        catch (runtime_exception& xe)    // exceptions should not occur, since records were validated. But catch exceptions just to be sure
          {
            if(xe.get_exception_code() == exception_type::RECORD_NOT_FOUND)
              {
                std::ostringstream msg;
                msg << CPPTRANSPORT_TASK_MISSING << " '" << job.get_name() << "'";
                this->err(msg.str());
              }
            else
              {
                std::ostringstream msg;
                msg << CPPTRANSPORT_TASK_CANT_BE_READ << " '" << job.get_name() << "' (" << xe.what() << ")";
                this->err(msg.str());
              }
            return;
          }

        // introspect task type
        switch(record->get_type())
          {
            case task_type::integration:
              {
                auto* int_rec = dynamic_cast< integration_task_record<number>* >(record.get());

                assert(int_rec != nullptr);
                if(int_rec == nullptr) throw runtime_exception(exception_type::REPOSITORY_ERROR, CPPTRANSPORT_REPO_RECORD_CAST_FAILED);

                this->dispatch_integration_task(*int_rec, job.is_seeded(), job.get_seed_group(), job.get_tags());
                break;
              }

            case task_type::output:
              {
                auto* out_rec = dynamic_cast< output_task_record<number>* >(record.get());

                assert(out_rec != nullptr);
                if(out_rec == nullptr) throw runtime_exception(exception_type::REPOSITORY_ERROR, CPPTRANSPORT_REPO_RECORD_CAST_FAILED);

                this->dispatch_output_task(*out_rec, job.get_tags());
                break;
              }

            case task_type::postintegration:
              {
                auto* pint_rec = dynamic_cast< postintegration_task_record<number>* >(record.get());

                assert(pint_rec != nullptr);
                if(pint_rec == nullptr) throw runtime_exception(exception_type::REPOSITORY_ERROR, CPPTRANSPORT_REPO_RECORD_CAST_FAILED);

                this->dispatch_postintegration_task(*pint_rec, job.is_seeded(), job.get_seed_group(), job.get_tags());
                break;
              }
          }
      }

    
    // WORKER HANDLING


    template <typename number>
    template <typename WriterObject>
    void master_controller<number>::capture_worker_properties(WriterObject& writer)
      {
        // capture busy/idle timers and switch to busy mode
        busyidle_instrument timers(this->busyidle_timers);

        // set up instrument to journal the MPI communication if needed
        journal_instrument instrument(this->journal, master_work_event::event_type::MPI_begin, master_work_event::event_type::MPI_end);

        base_writer::logger& log = writer.get_log();

        // rebuild information about our workers; this information
        // it is updated whenever we start a new task, because the details can vary
        // between model instances (eg. CPU or GPU backends)
        this->work_scheduler.reset();
        this->work_manager.new_task(writer.get_name());

        while(!this->work_scheduler.is_ready())
          {
            timers.idle();
            boost::mpi::status stat = this->world.probe();

            timers.busy();
            switch(stat.tag())
              {
                case MPI::WORKER_IDENTIFICATION:
                  {
                    MPI::slave_information_payload payload;
                    this->world.recv(stat.source(), MPI::WORKER_IDENTIFICATION, payload);
                    this->work_scheduler.initialize_worker(log, this->worker_number(stat.source()), payload);
                    break;
                  }

                default:
                  {
                    BOOST_LOG_SEV(log, base_writer::log_severity_level::error)
                      << "!! Received unexpected MPI message " << stat.tag() << " from worker " << stat.source() << "; discarding";
                    this->world.recv(stat.source(), stat.tag());
                    break;
                  }
              }
          }

        // now all data is available, ask scheduler to fix maximum unit of allocation
        this->work_scheduler.complete_queue_setup();
      }


    template <typename number>
    void master_controller<number>::workers_end_of_task(base_writer::logger& log)
      {
        // capture busy/idle timers and switch to busy mode
        busyidle_instrument timers(this->busyidle_timers);

        BOOST_LOG_SEV(log, base_writer::log_severity_level::notification) << "++ Notifying workers of end-of-work";

        std::vector<boost::mpi::request> requests(this->world.size()-1);
        for(unsigned int i = 0; i < this->world.size()-1; ++i)
          {
            requests[i] = this->world.isend(this->worker_rank(i), MPI::END_OF_WORK);
          }

        // wait for all messages to be received, then return
        boost::mpi::wait_all(requests.begin(), requests.end());
      }
    
    
    template <typename number>
    void master_controller<number>::set_local_checkpoint_interval(unsigned int m)
      {
        // capture busy/idle timers and switch to busy mode
        busyidle_instrument timers(this->busyidle_timers);

        // set up instrument to journal the MPI communication if needed
        journal_instrument instrument(this->journal, master_work_event::event_type::MPI_begin, master_work_event::event_type::MPI_end);

        std::vector<boost::mpi::request> requests(world.size()-1);

        for(unsigned int i = 0; i < world.size()-1; ++i)
          {
            requests[i] = world.isend(this->worker_rank(i), MPI::SET_LOCAL_CHECKPOINT, m*60);
          }

        timers.idle();
        boost::mpi::wait_all(requests.begin(), requests.end());
        timers.busy();

        this->data_mgr->set_local_checkpoint_interval(m*60);
      }


    template <typename number>
    void master_controller<number>::unset_local_checkpoint_interval()
      {
        // capture busy/idle timers and switch to busy mode
        busyidle_instrument timers(this->busyidle_timers);

        // set up instrument to journal the MPI communication if needed
        journal_instrument instrument(this->journal, master_work_event::event_type::MPI_begin, master_work_event::event_type::MPI_end);

        std::vector<boost::mpi::request> requests(world.size()-1);

        for(unsigned int i = 0; i < world.size()-1; ++i)
          {
            requests[i] = world.isend(this->worker_rank(i), MPI::UNSET_LOCAL_CHECKPOINT);
          }

        timers.idle();
        boost::mpi::wait_all(requests.begin(), requests.end());
        timers.busy();

        this->data_mgr->unset_local_checkpoint_interval();
      }


    template <typename number>
    void master_controller<number>::assign_work_to_workers(base_writer::logger& log)
      {
        // capture busy/idle timers and switch to busy mode
        busyidle_instrument timers(this->busyidle_timers);

        // set up instrument to journal the MPI communication if needed
        journal_instrument instrument(this->journal, master_work_event::event_type::MPI_begin, master_work_event::event_type::MPI_end);

        // generate a list of work assignments
        std::list<work_assignment> work = this->work_scheduler.assign_work(log);
    
        BOOST_LOG_SEV(log, base_writer::log_severity_level::normal)
          << "++ Generating new work assignments for " << work.size() << " worker"
          << (work.size() > 1 ? std::string{"s"} : std::string{})
          << " (" << this->work_scheduler.get_items_remaining() << " work items remain in queue)";

        // push assignments to workers
        std::vector<boost::mpi::request> msg_status(work.size());

        unsigned int c = 0;
        for(const work_assignment& assgn : work)
          {
            MPI::work_assignment_payload payload(assgn.get_items());

            // send message to worker with new assignment information
            msg_status[c] = this->world.isend(this->worker_rank(assgn.get_worker()), MPI::NEW_WORK_ASSIGNMENT, payload);
            ++c;

            // mark this worker, and these work items, as assigned
            this->work_scheduler.mark_assigned(assgn);

            // issue message to log
            unsigned int items = assgn.get_items().size();
            unsigned int worker = assgn.get_worker();
    
            BOOST_LOG_SEV(log, base_writer::log_severity_level::normal)
              << "++ Assigned " << items << " work item" << (items == 1 ? std::string{} : std::string{"s"})
              << " to worker " << worker << " [MPI rank=" << this->worker_rank(worker) << "]";
          }

        // wait for all assignments to be received
        timers.idle();
        BOOST_LOG_SEV(log, base_writer::log_severity_level::normal) << "++ Waiting for all assignment notifications to be received";
        boost::mpi::wait_all(msg_status.begin(), msg_status.end());
        BOOST_LOG_SEV(log, base_writer::log_severity_level::normal) << "++ All assignment messages received by workers";
        timers.busy();
      }
    
    
    template <typename number>
    template <typename WriterObject, typename PayloadObject>
    void master_controller<number>::unassign_worker(unsigned int worker, WriterObject& writer, PayloadObject& payload)
      {
        // capture busy/idle timers and switch to busy mode
        busyidle_instrument timers(this->busyidle_timers);
        
        this->work_scheduler.mark_unassigned(worker, payload.get_wallclock_time(), payload.get_items_processed());
        this->work_manager.update_load_average(worker, payload.get_load_average());

        // push estimate completion time to repository if needed
        boost::posix_time::ptime now = boost::posix_time::second_clock::universal_time();
        boost::posix_time::time_duration time_since_last_update = now - this->last_push_to_repo;
        
        // update time-to-completion estimate only every 60 seconds to avoid producing excessive database traffic
        if(time_since_last_update.total_seconds() > 60)
          {
            this->last_push_to_repo = now;
            this->repo->advise_completion_time(writer.get_name(), this->work_scheduler.get_estimated_completion());
          }
      }


    template <typename number>
    template <typename WriterObject>
    bool master_controller<number>::poll_workers(integration_aggregator<number>& int_agg, postintegration_aggregator<number>& post_agg, derived_content_aggregator<number>& derived_agg,
                                                 integration_metadata& int_metadata, output_metadata& out_metadata, content_group_name_set& content_groups,
                                                 WriterObject& writer, slave_work_event::event_type begin_label, slave_work_event::event_type end_label)
      {
        // capture busy/idle timers and switch to busy mode
        busyidle_instrument timers(this->busyidle_timers);

        BOOST_LOG_SEV(writer.get_log(), base_writer::log_severity_level::notification) << "++ Master entering main polling loop";

        bool success = true;

        base_writer::logger& log = writer.get_log();

        // set up aggregation queues
        unsigned int aggregation_counter = 0;
        std::list< std::unique_ptr<aggregation_record> > aggregation_queue;

        // wait for workers to report their characteristics
        this->capture_worker_properties(writer);

        // CloseDown_Context object is responsible for calling this->workers_end_of_task()
        // when needed; should do so even if we exit this function via an exception
        CloseDown_Context<number> closedown_handler(*this, log);

        // record time of last-received message, so we can determine for how long we have been idle
        boost::posix_time::ptime last_msg_time = boost::posix_time::second_clock::universal_time();
        bool emit_agg_queue_msg = true;

        // poll workers, scattering work and aggregating the results until work items are exhausted
        timers.idle();
        while(!this->work_scheduler.all_inactive())
          {
            timers.busy();
            // send closedown instruction if no more work
            if(this->work_scheduler.is_finished() && !closedown_handler()) closedown_handler.send_closedown();

            // generate new work assignments if needed, and push them to the workers
            if(this->work_scheduler.assignable())
              {
                this->reporter.periodic_report(writer);
                this->assign_work_to_workers(log);
              }

            // check whether any messages are waiting in the queue
            timers.idle();
            boost::optional<boost::mpi::status> stat = this->world.iprobe();

            while(stat) // consume messages until no more are available
              {
                timers.busy();

                // update time of last received message and reset flag which enables logging
                last_msg_time = boost::posix_time::second_clock::universal_time();
                emit_agg_queue_msg = true;

                this->work_manager.update_contact_time(this->worker_number(stat->source()), last_msg_time);

                switch(stat->tag())
                  {
                    case MPI::INTEGRATION_DATA_READY:
                      {
                        if(int_agg)
                          {
                            MPI::data_ready_payload payload;
                            this->world.recv(stat->source(), MPI::INTEGRATION_DATA_READY, payload);
                            this->journal.add_entry(slave_work_event(this->worker_number(stat->source()), slave_work_event::event_type::integration_aggregation, payload.get_timestamp(), aggregation_counter));
                            aggregation_queue.push_back(std::make_unique< integration_aggregation_record<number> >(this->worker_number(stat->source()), aggregation_counter++, int_agg, int_metadata, payload));
                            BOOST_LOG_SEV(log, base_writer::log_severity_level::normal) << "++ Worker " << stat->source() << " sent aggregation notification for container '" << payload.get_container_path().string() << "'";
                          }
                        else
                          {
                            BOOST_LOG_SEV(log, base_writer::log_severity_level::normal) << "!! Received INTEGRATION_DATA_READY message from worker " << stat->source() << ", but no integration aggregator has been assigned";
                          }
                        break;
                      }

                    case MPI::DERIVED_CONTENT_READY:
                      {
                        if(derived_agg)
                          {
                            MPI::content_ready_payload payload;
                            this->world.recv(stat->source(), MPI::DERIVED_CONTENT_READY, payload);
                            this->journal.add_entry(slave_work_event(this->worker_number(stat->source()), slave_work_event::event_type::derived_content_aggregation, payload.get_timestamp(), aggregation_counter));
                            aggregation_queue.push_back(std::make_unique< derived_content_aggregation_record<number> >(this->worker_number(stat->source()), aggregation_counter++, derived_agg, out_metadata, payload));
                            BOOST_LOG_SEV(log, base_writer::log_severity_level::normal) << "++ Worker " << stat->source() << " sent content-ready notification";
                          }
                        else
                          {
                            BOOST_LOG_SEV(log, base_writer::log_severity_level::normal) << "!! Received DERIVED_CONTENT_READY message from worker " << stat->source() << ", but no derived content aggregator has been assigned";
                          }
                        break;
                      }

                    case MPI::POSTINTEGRATION_DATA_READY:
                      {
                        if(post_agg)
                          {
                            MPI::data_ready_payload payload;
                            this->world.recv(stat->source(), MPI::POSTINTEGRATION_DATA_READY, payload);
                            this->journal.add_entry(slave_work_event(this->worker_number(stat->source()), slave_work_event::event_type::postintegration_aggregation, payload.get_timestamp(), aggregation_counter));
                            aggregation_queue.push_back(std::make_unique< postintegration_aggregation_record<number> >(this->worker_number(stat->source()), aggregation_counter++, post_agg, out_metadata, payload));
                            BOOST_LOG_SEV(log, base_writer::log_severity_level::normal) << "++ Worker " << stat->source() << " sent aggregation notification for container '" << payload.get_container_path().string() << "'";
                          }
                        else
                          {
                            BOOST_LOG_SEV(log, base_writer::log_severity_level::normal) << "!! Received POSTINTEGRATION_DATA_READY message from worker " << stat->source() << ", but no postintegration aggregator has been assigned";
                          }
                        break;
                      }

                    case MPI::NEW_WORK_ACKNOWLEDGMENT:
                      {
                        MPI::work_acknowledgment_payload payload;
                        this->world.recv(stat->source(), MPI::NEW_WORK_ACKNOWLEDGMENT, payload);
                        this->journal.add_entry(slave_work_event(this->worker_number(stat->source()), begin_label, payload.get_timestamp()));
                        BOOST_LOG_SEV(log, base_writer::log_severity_level::normal) << "++ Worker " << stat->source() << " advising receipt of work assignment at time " << boost::posix_time::to_simple_string(payload.get_timestamp());

                        break;
                      }

                    case MPI::FINISHED_INTEGRATION:
                      {
                        MPI::finished_integration_payload payload;
                        this->world.recv(stat->source(), MPI::FINISHED_INTEGRATION, payload);
                        this->journal.add_entry(slave_work_event(this->worker_number(stat->source()), end_label, payload.get_timestamp()));
                        BOOST_LOG_SEV(log, base_writer::log_severity_level::normal) << "++ Worker " << stat->source() << " advising finished work assignment in wallclock time " << format_time(payload.get_wallclock_time());

                        // mark this worker as unassigned, and update its mean time per work item
                        this->unassign_worker(this->worker_number(stat->source()), writer, payload);

                        this->update_integration_metadata(payload, int_metadata);
                        if(payload.get_num_failures() > 0) writer.merge_failure_list(payload.get_failed_serials());

                        break;
                      }

                    case MPI::INTEGRATION_FAIL:
                      {
                        MPI::finished_integration_payload payload;
                        this->world.recv(stat->source(), MPI::INTEGRATION_FAIL, payload);
                        this->journal.add_entry(slave_work_event(this->worker_number(stat->source()), end_label, payload.get_timestamp()));
                        BOOST_LOG_SEV(log, base_writer::log_severity_level::normal) << "!! Worker " << stat->source() << " advising failure of work assignment (successful work items consumed wallclock time " << format_time(payload.get_wallclock_time()) << ")";
    
                        // mark this worker as unassigned, and update its mean time per work item
                        this->unassign_worker(this->worker_number(stat->source()), writer, payload);

                        this->update_integration_metadata(payload, int_metadata);
                        if(payload.get_num_failures() > 0) writer.merge_failure_list(payload.get_failed_serials());

                        success = false;
                        break;
                      }

                    case MPI::FINISHED_DERIVED_CONTENT:
                      {
                        MPI::finished_derived_payload payload;
                        this->world.recv(stat->source(), MPI::FINISHED_DERIVED_CONTENT, payload);
                        this->journal.add_entry(slave_work_event(this->worker_number(stat->source()), end_label, payload.get_timestamp()));
                        BOOST_LOG_SEV(log, base_writer::log_severity_level::normal) << "++ Worker " << stat->source() << " advising finished work assignment in CPU time " << format_time(payload.get_wallclock_time());

                        // mark this scheduler as unassigned, and update its mean time per work item
                        this->unassign_worker(this->worker_number(stat->source()), writer, payload);

                        this->update_output_metadata(payload, out_metadata);
                        this->update_content_group_list(payload, content_groups);

                        break;
                      }

                    case MPI::DERIVED_CONTENT_FAIL:
                      {
                        MPI::finished_derived_payload payload;
                        this->world.recv(stat->source(), MPI::DERIVED_CONTENT_FAIL, payload);
                        this->journal.add_entry(slave_work_event(this->worker_number(stat->source()), end_label, payload.get_timestamp()));
                        BOOST_LOG_SEV(log, base_writer::log_severity_level::normal) << "!! Worker " << stat->source() << " advising failure of work assignment (successful work items consumed wallclock time " << format_time(payload.get_wallclock_time()) << ")";
    
                        // mark this scheduler as unassigned, and update its mean time per work item
                        this->unassign_worker(this->worker_number(stat->source()), writer, payload);

                        this->update_output_metadata(payload, out_metadata);
                        this->update_content_group_list(payload, content_groups);

                        success = false;
                        break;
                      }

                    case MPI::FINISHED_POSTINTEGRATION:
                      {
                        MPI::finished_postintegration_payload payload;
                        this->world.recv(stat->source(), MPI::FINISHED_POSTINTEGRATION, payload);
                        this->journal.add_entry(slave_work_event(this->worker_number(stat->source()), end_label, payload.get_timestamp()));
                        BOOST_LOG_SEV(log, base_writer::log_severity_level::normal) << "++ Worker " << stat->source() << " advising finished work assignment in wallclock time " << format_time(payload.get_cpu_time());
    
                        // mark this scheduler as unassigned, and update its mean time per work item
                        this->unassign_worker(this->worker_number(stat->source()), writer, payload);

                        // update metadata and list of used content groups from information reported by the worker
                        this->update_output_metadata(payload, out_metadata);
                        this->update_content_group_list(payload, content_groups);

                        break;
                      }

                    case MPI::POSTINTEGRATION_FAIL:
                      {
                        MPI::finished_postintegration_payload payload;
                        this->world.recv(stat->source(), MPI::POSTINTEGRATION_FAIL, payload);
                        this->journal.add_entry(slave_work_event(this->worker_number(stat->source()), end_label, payload.get_timestamp()));
                        BOOST_LOG_SEV(log, base_writer::log_severity_level::normal) << "!! Worker " << stat->source() << " advising failure of work assignment (successful work items consumed wallclock time " << format_time(payload.get_cpu_time()) << ")";
    
                        // mark this scheduler as unassigned, and update its mean time per work item
                        this->unassign_worker(this->worker_number(stat->source()), writer, payload);

                        this->update_output_metadata(payload, out_metadata);
                        this->update_content_group_list(payload, content_groups);

                        success = false;
                        break;
                      }

                    case MPI::WORKER_CLOSE_DOWN:
                      {
                        MPI::performance_data_payload payload;
                        this->world.recv(stat->source(), MPI::WORKER_CLOSE_DOWN, payload);
                        this->work_scheduler.mark_inactive(this->worker_number(stat->source()));
                        this->work_manager.update_load_average(this->worker_number(stat->source()), payload.get_load_average());

                        unsigned int num_active = this->work_scheduler.get_number_active();
                        BOOST_LOG_SEV(log, base_writer::log_severity_level::normal)
                          << "++ Worker " << stat->source() << " advising close-down after end-of-work; "
                          << num_active << " worker" << (num_active != 1 ? std::string{"s"} : std::string{}) << " still active";
                        break;
                      }

                    case MPI::REPORT_ERROR:
                      {
                        MPI::error_report payload;
                        this->world.recv(stat->source(), MPI::REPORT_ERROR, payload);

                        std::ostringstream msg;
                        msg << payload.get_message() << " " << CPPTRANSPORT_MASTER_REPORTED_BY_WORKER << " " << stat->source();
                        this->err(msg.str());
                        this->reporter.add_alert(msg.str());
                        break;
                      }

                    default:
                      {
                        BOOST_LOG_SEV(log, base_writer::log_severity_level::warning) << "!! Received unexpected message " << stat->tag() << " waiting in the queue; discarding";
                        this->world.recv(stat->source(), stat->tag());
                        break;
                      }
                  }

                timers.idle();
                stat = this->world.iprobe();  // check for another message
              }

            // we arrive at this point only when no more messages are available to be received

            // check whether any aggregations are in the queue, and process them if we have been idle for sufficiently long
            if(!aggregation_queue.empty())
              {
                timers.busy();

                boost::posix_time::ptime now = boost::posix_time::second_clock::universal_time();
                boost::posix_time::time_duration idle_time = now - last_msg_time;
                if(idle_time.total_seconds() > 5)
                  {
                    if(emit_agg_queue_msg)
                      {
                        BOOST_LOG_SEV(log, base_writer::log_severity_level::normal)
                          << "++ Idle for more than 5 seconds: processing queued aggregations";
                        emit_agg_queue_msg = false;
                      }

                    aggregation_queue.front()->aggregate();
                    aggregation_queue.pop_front();
                  }
              }
          }

        timers.busy();
        // issue final progress report if needed
        this->reporter.periodic_report(writer);

        boost::posix_time::ptime now = boost::posix_time::second_clock::universal_time();
        BOOST_LOG_SEV(log, base_writer::log_severity_level::notification)
          << "++ All work items completed at " << boost::posix_time::to_simple_string(now);

        // process any remaining aggregations
        if(!aggregation_queue.empty())
          {
            while(!aggregation_queue.empty())
              {
                aggregation_queue.front()->aggregate();
                aggregation_queue.pop_front();
              }
            this->reporter.database_report(writer);
          }

        return(success);
      }

  }   // namespace transport


#endif //CPPTRANSPORT_MANAGER_DETAIL_MASTER_CONTROLLER_CORE_H
