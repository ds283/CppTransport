//
// Created by David Seery on 19/03/15.
// Copyright (c) 2015 University of Sussex. All rights reserved.
//


#ifndef __task_manager_master_H_
#define __task_manager_master_H_


#include <list>
#include <set>
#include <vector>

#include "transport-runtime-api/models/model.h"
#include "transport-runtime-api/manager/instance_manager.h"
#include "transport-runtime-api/tasks/task.h"
#include "transport-runtime-api/tasks/integration_tasks.h"
#include "transport-runtime-api/tasks/output_tasks.h"

#include "transport-runtime-api/manager/mpi_operations.h"

#include "transport-runtime-api/manager/repository.h"
#include "transport-runtime-api/manager/json_repository_interface.h"
#include "transport-runtime-api/manager/data_manager.h"

#include "transport-runtime-api/scheduler/context.h"
#include "transport-runtime-api/scheduler/scheduler.h"
#include "transport-runtime-api/scheduler/work_queue.h"

#include "transport-runtime-api/utilities/formatter.h"

#include "transport-runtime-api/defaults.h"
#include "transport-runtime-api/messages.h"
#include "transport-runtime-api/exceptions.h"

#include "boost/mpi.hpp"
#include "boost/serialization/string.hpp"
#include "boost/timer/timer.hpp"
#include "boost/lexical_cast.hpp"


namespace transport
	{

    // MASTER FUNCTIONS


    template <typename number>
    void task_manager<number>::execute_tasks(void)
	    {
        if(!this->is_master()) throw runtime_exception(runtime_exception::MPI_ERROR, __CPP_TRANSPORT_EXEC_SLAVE);

        if(this->repo == nullptr) this->error(__CPP_TRANSPORT_REPO_NONE);
        else
	        {
            // push repository information to all workers
            this->master_push_repository();

            for(typename std::list<job_descriptor>::const_iterator t = this->job_queue.begin(); t != this->job_queue.end(); t++)
	            {
                switch((*t).type)
	                {
                    case job_task:
	                    {
                        this->master_process_task(*t);
                        break;
	                    }

                    case job_get_product:
                    case job_get_task:
                    case job_get_package:
                    case job_get_content:
	                    {
                        this->master_process_get(*t);
                        break;
	                    }

                    default:
	                    throw runtime_exception(runtime_exception::RUNTIME_ERROR, __CPP_TRANSPORT_UNKNOWN_JOB_TYPE);
	                }
	            }
	        }

        // there is no more work, so ask all workers to shut down
        // and then exit ourselves
        this->master_terminate_workers();
	    }


    template <typename number>
    void task_manager<number>::master_process_get(const job_descriptor& job)
	    {
        try
	        {
            std::string document;

            switch(job.type)
	            {
                case job_get_package:
	                {
                    document = this->repo->export_JSON_package_record(job.name);
                    break;
	                }

                case job_get_task:
	                {
                    document = this->repo->export_JSON_task_record(job.name);
                    break;
	                }

                case job_get_product:
	                {
                    document = this->repo->export_JSON_product_record(job.name);
                    break;
	                }

                case job_get_content:
	                {
                    document = this->repo->export_JSON_content_record(job.name);
                    break;
	                }

                default:
	                assert(false);
	            }

            std::ofstream out;
            out.open(job.output);
            if(out.is_open() && !out.fail())
	            {
                out << document;
	            }
            else
	            {
                std::ostringstream msg;
                msg << __CPP_TRANSPORT_OPEN_OUTPUT_FAIL << " '" << job.output << "'";
                throw runtime_exception(runtime_exception::RUNTIME_ERROR, msg.str());
	            }
            out.close();
	        }
        catch(runtime_exception& xe)
	        {
            this->error(xe.what());
	        }
	    }


    template <typename number>
    void task_manager<number>::master_process_task(const job_descriptor& job)
	    {
        try
	        {
            // query a task record with the name we're looking for from the database
            std::unique_ptr<typename repository<number>::task_record> record(this->repo->query_task(job.name));

            switch(record->get_type())
	            {
                case repository<number>::task_record::integration:
	                {
                    typename repository<number>::integration_task_record* int_rec = dynamic_cast<typename repository<number>::integration_task_record*>(record.get());

                    assert(int_rec != nullptr);
                    if(int_rec == nullptr) throw runtime_exception(runtime_exception::REPOSITORY_ERROR, __CPP_TRANSPORT_REPO_RECORD_CAST_FAILED);

                    this->master_dispatch_integration_task(int_rec, job.tags);
                    break;
	                }

                case repository<number>::task_record::output:
	                {
                    typename repository<number>::output_task_record* out_rec = dynamic_cast<typename repository<number>::output_task_record*>(record.get());

                    assert(out_rec != nullptr);
                    if(out_rec == nullptr) throw runtime_exception(runtime_exception::REPOSITORY_ERROR, __CPP_TRANSPORT_REPO_RECORD_CAST_FAILED);

                    this->master_dispatch_output_task(out_rec, job.tags);
                    break;
	                }

                case repository<number>::task_record::postintegration:
	                {
                    typename repository<number>::postintegration_task_record* pint_rec = dynamic_cast<typename repository<number>::postintegration_task_record*>(record.get());

                    assert(pint_rec != nullptr);
                    if(pint_rec == nullptr) throw runtime_exception(runtime_exception::REPOSITORY_ERROR, __CPP_TRANSPORT_REPO_RECORD_CAST_FAILED);

                    this->master_dispatch_postintegration_task(pint_rec, job.tags);
                    break;
	                }

                default:
	                {
                    assert(false);

                    std::ostringstream msg;
                    msg << __CPP_TRANSPORT_REPO_UNKNOWN_RECORD_TYPE << " '" << job.name << "'";
                    throw runtime_exception(runtime_exception::RUNTIME_ERROR, msg.str());
	                }
	            }
	        }
        catch (runtime_exception xe)
	        {
            if(xe.get_exception_code() == runtime_exception::RECORD_NOT_FOUND)
	            {
                std::ostringstream msg;
                msg << __CPP_TRANSPORT_REPO_MISSING_RECORD << " '" << xe.what() << "'" << __CPP_TRANSPORT_REPO_SKIPPING_TASK;
                this->error(msg.str());
	            }
            else if(xe.get_exception_code() == runtime_exception::MISSING_MODEL_INSTANCE
	            || xe.get_exception_code() == runtime_exception::REPOSITORY_BACKEND_ERROR)
	            {
                std::ostringstream msg;
                msg << xe.what() << " " << __CPP_TRANSPORT_REPO_FOR_TASK << " '" << job.name << "'" << __CPP_TRANSPORT_REPO_SKIPPING_TASK;
                this->error(msg.str());
	            }
            else throw xe;
	        }
	    }


    template <typename number>
    void task_manager<number>::master_dispatch_integration_task(typename repository<number>::integration_task_record* rec, const std::list<std::string>& tags)
	    {
        assert(rec != nullptr);

        // can't process a task if there are no workers
        if(this->world.size() == 1) throw runtime_exception(runtime_exception::MPI_ERROR, __CPP_TRANSPORT_TOO_FEW_WORKERS);

        // set up a work queue representing our workers
        context ctx = this->make_workers_context();
        scheduler sch = scheduler(ctx);

        integration_task<number>* tk = rec->get_task();
        model<number>* m = rec->get_task()->get_model();

        twopf_task<number>* tka = nullptr;
        threepf_task<number>* tkb = nullptr;

        if((tka = dynamic_cast< twopf_task<number>* >(tk)) != nullptr)
	        {
            work_queue<twopf_kconfig> queue = sch.make_queue(m->backend_twopf_state_size(), *tka);
            this->master_dispatch_integration_queue(rec, tka, queue, tags);
	        }
        else if((tkb = dynamic_cast< threepf_task<number>* >(tk)) != nullptr)
	        {
            work_queue<threepf_kconfig> queue = sch.make_queue(m->backend_threepf_state_size(), *tkb);
            this->master_dispatch_integration_queue(rec, tkb, queue, tags);
	        }
        else
	        {
            std::ostringstream msg;
            msg << __CPP_TRANSPORT_UNKNOWN_DERIVED_TASK << " '" << rec->get_name() << "'";
            throw runtime_exception(runtime_exception::REPOSITORY_ERROR, msg.str());
	        }
	    }


    template <typename number>
    template <typename TaskObject, typename QueueObject>
    void task_manager<number>::master_dispatch_integration_queue(typename repository<number>::integration_task_record* rec,
                                                                 TaskObject* tk, QueueObject& queue, const std::list<std::string>& tags)
	    {
        assert(rec != nullptr);

        // create an output writer to commit the result of this integration to the repository.
        // like all writers, it aborts (ie. executes a rollback if needed) when it goes out of scope unless
        // it is explicitly committed
        std::shared_ptr<typename repository<number>::integration_writer> writer = this->repo->new_integration_task_content(rec, tags, this->get_rank());

        // initialize the writer
        this->data_mgr->initialize_writer(writer);

        // write the task distribution list -- this is subsequently read by the worker processes,
        // to find out which work items they should process
        this->data_mgr->create_taskfile(writer, queue);

        // write the various tables needed in the database
        this->data_mgr->create_tables(writer, tk);

        // instruct workers to carry out the calculation
        // this call returns when all workers have signalled that their work is done
        bool success = this->master_integration_task_to_workers(writer);

        // close the writer
        this->data_mgr->close_writer(writer);

        // commit output if successful
        if(success) writer->commit();
	    }


    template <typename number>
    bool task_manager<number>::master_integration_task_to_workers(std::shared_ptr<typename repository<number>::integration_writer>& writer)
	    {
        assert(this->repo != nullptr);
        if(!this->is_master()) throw runtime_exception(runtime_exception::MPI_ERROR, __CPP_TRANSPORT_EXEC_SLAVE);

        bool success = true;

        std::vector<boost::mpi::request> requests(this->world.size()-1);

        // write log header
        boost::posix_time::ptime now = boost::posix_time::second_clock::universal_time();
        BOOST_LOG_SEV(writer->get_log(), repository<number>::normal) << "++ NEW INTEGRATION TASK '" << writer->get_record()->get_name() << "' | initiated at " << boost::posix_time::to_simple_string(now) << std::endl;
        BOOST_LOG_SEV(writer->get_log(), repository<number>::normal) << *(writer->get_record()->get_task());

        // set up a timer to keep track of the total wallclock time used in this integration
        boost::timer::cpu_timer wallclock_timer;

        // aggregate integration times reported by worker processes
        typename repository<number>::integration_metadata metadata;

        // get paths the workers will need
        boost::filesystem::path taskfile_path = writer->get_abs_taskfile_path();
        boost::filesystem::path tempdir_path  = writer->get_abs_tempdir_path();
        boost::filesystem::path logdir_path   = writer->get_abs_logdir_path();

        MPI::new_integration_payload payload(writer->get_record()->get_name(), taskfile_path, tempdir_path, logdir_path);

        for(unsigned int i = 0; i < this->world.size()-1; i++)
	        {
            requests[i] = this->world.isend(this->worker_rank(i), MPI::NEW_INTEGRATION, payload);
	        }

        // wait for all messages to be received
        boost::mpi::wait_all(requests.begin(), requests.end());

        BOOST_LOG_SEV(writer->get_log(), repository<number>::normal) << "++ All workers received NEW_INTEGRATION instruction";

        // poll workers, receiving data until workers are exhausted
        std::set<unsigned int> workers;
        for(unsigned int i = 0; i < this->world.size()-1; i++) workers.insert(i);
        while(workers.size() > 0)
	        {
            BOOST_LOG_SEV(writer->get_log(), repository<number>::normal) <<  "++ Master polling for INTEGRATION_DATA_READY messages";
            // wait until a message is available from a worker
            boost::mpi::status stat = this->world.probe();

            switch(stat.tag())
	            {
                case MPI::INTEGRATION_DATA_READY:
	                {
                    this->aggregate_batch(writer, stat.source(), metadata);
                    break;
	                }

                case MPI::FINISHED_INTEGRATION:
	                {
                    MPI::finished_integration_payload payload;
                    this->world.recv(stat.source(), MPI::FINISHED_INTEGRATION, payload);
                    BOOST_LOG_SEV(writer->get_log(), repository<number>::normal) << "++ Worker " << stat.source() << " advising finished task in wallclock time " << format_time(payload.get_wallclock_time());

                    this->update_integration_metadata(payload, metadata);

                    workers.erase(this->worker_number(stat.source()));
                    break;
	                }

                case MPI::INTEGRATION_FAIL:
	                {
                    MPI::finished_integration_payload payload;
                    this->world.recv(stat.source(), MPI::INTEGRATION_FAIL, payload);
                    BOOST_LOG_SEV(writer->get_log(), repository<number>::normal) << "++ Worker " << stat.source() << " advising integration failure (successful tasks consumed wallclock time " << format_time(payload.get_wallclock_time()) << ")";

                    this->update_integration_metadata(payload, metadata);

                    workers.erase(this->worker_number(stat.source()));
                    success = false;
                    break;
	                }

                default:
	                {
                    BOOST_LOG_SEV(writer->get_log(), repository<number>::warning) << "++ Master received unexpected message " << stat.tag() << " waiting in the queue";
                    break;
	                }
	            }
	        }

        writer->set_metadata(metadata);

        wallclock_timer.stop();
        now = boost::posix_time::second_clock::universal_time();
        BOOST_LOG_SEV(writer->get_log(), repository<number>::normal) << "";
        BOOST_LOG_SEV(writer->get_log(), repository<number>::normal) << "++ TASK COMPLETE (at " << boost::posix_time::to_simple_string(now) << "): FINAL USAGE STATISTICS";
        BOOST_LOG_SEV(writer->get_log(), repository<number>::normal) << "++   Total wallclock time for task '" << writer->get_record()->get_name() << "' " << format_time(wallclock_timer.elapsed().wall);
        BOOST_LOG_SEV(writer->get_log(), repository<number>::normal) << "++   Total wallclock time required by worker processes = " << format_time(metadata.total_wallclock_time);
        BOOST_LOG_SEV(writer->get_log(), repository<number>::normal) << "++   Total aggregation time required by master process = " << format_time(metadata.total_aggregation_time);
        BOOST_LOG_SEV(writer->get_log(), repository<number>::normal) << "";
        BOOST_LOG_SEV(writer->get_log(), repository<number>::normal) << "++ AGGREGATE CACHE STATISTICS";
        BOOST_LOG_SEV(writer->get_log(), repository<number>::normal) << "++   Workers processed " << metadata.total_configurations << " individual integrations";
        BOOST_LOG_SEV(writer->get_log(), repository<number>::normal) << "++   Total integration time    = " << format_time(metadata.total_integration_time) << " | global mean integration time = " << format_time(metadata.total_integration_time/metadata.total_configurations);
        BOOST_LOG_SEV(writer->get_log(), repository<number>::normal) << "++   Min mean integration time = " << format_time(metadata.min_mean_integration_time) << " | global min integration time = " << format_time(metadata.global_min_integration_time);
        BOOST_LOG_SEV(writer->get_log(), repository<number>::normal) << "++   Max mean integration time = " << format_time(metadata.max_mean_integration_time) << " | global max integration time = " << format_time(metadata.global_max_integration_time);
        BOOST_LOG_SEV(writer->get_log(), repository<number>::normal) << "++   Total batching time       = " << format_time(metadata.total_batching_time) << " | global mean batching time = " << format_time(metadata.total_batching_time/metadata.total_configurations);
        BOOST_LOG_SEV(writer->get_log(), repository<number>::normal) << "++   Min mean batching time    = " << format_time(metadata.min_mean_batching_time) << " | global min batching time = " << format_time(metadata.global_min_batching_time);
        BOOST_LOG_SEV(writer->get_log(), repository<number>::normal) << "++   Max mean batching time    = " << format_time(metadata.max_mean_batching_time) << " | global max batching time = " << format_time(metadata.global_max_batching_time);

        return(success);
	    }


    template <typename number>
    void task_manager<number>::aggregate_batch(std::shared_ptr<typename repository<number>::integration_writer>& writer, int source,
                                               typename repository<number>::integration_metadata& metadata)
	    {
        MPI::data_ready_payload payload;
        this->world.recv(source, MPI::INTEGRATION_DATA_READY, payload);
        BOOST_LOG_SEV(writer->get_log(), repository<number>::normal) << "++ Worker " << source << " sent aggregation notification for container '" << payload.get_container_path() << "'";

        // set up a timer to measure how long we spend batching
        boost::timer::cpu_timer batching_timer;

        writer->aggregate(payload.get_container_path());

        batching_timer.stop();
        BOOST_LOG_SEV(writer->get_log(), repository<number>::normal) << "++ Aggregated temporary container '" << payload.get_container_path() << "' in time " << format_time(batching_timer.elapsed().wall);
        metadata.total_aggregation_time += batching_timer.elapsed().wall;

        // remove temporary container
        BOOST_LOG_SEV(writer->get_log(), repository<number>::normal) << "++ Deleting temporary container '" << payload.get_container_path() << "'";
        if(!boost::filesystem::remove(payload.get_container_path()))
	        {
            std::ostringstream msg;
            msg << __CPP_TRANSPORT_DATACTR_REMOVE_TEMP << " '" << payload.get_container_path() << "'";
            this->error(msg.str());
	        }
	    }


    template <typename number>
    void task_manager<number>::update_integration_metadata(MPI::finished_integration_payload& payload, typename repository<number>::integration_metadata& metadata)
	    {
        metadata.total_wallclock_time += payload.get_wallclock_time();

        metadata.total_integration_time += payload.get_integration_time();
        boost::timer::nanosecond_type mean_integration_time = payload.get_integration_time() / payload.get_num_integrations();
        if(metadata.max_mean_integration_time == 0 || mean_integration_time > metadata.max_mean_integration_time) metadata.max_mean_integration_time = mean_integration_time;
        if(metadata.min_mean_integration_time == 0 || mean_integration_time < metadata.min_mean_integration_time) metadata.min_mean_integration_time = mean_integration_time;

        if(metadata.global_max_integration_time == 0 || payload.get_max_integration_time() > metadata.global_max_integration_time) metadata.global_max_integration_time = payload.get_max_integration_time();
        if(metadata.global_min_integration_time == 0 || payload.get_min_integration_time() < metadata.global_min_integration_time) metadata.global_min_integration_time = payload.get_min_integration_time();

        metadata.total_batching_time += payload.get_batching_time();
        boost::timer::nanosecond_type mean_batching_time = payload.get_batching_time() / payload.get_num_integrations();
        if(metadata.max_mean_batching_time == 0 || mean_batching_time > metadata.max_mean_batching_time) metadata.max_mean_batching_time = mean_batching_time;
        if(metadata.min_mean_batching_time == 0 || mean_batching_time < metadata.min_mean_batching_time) metadata.min_mean_batching_time = mean_batching_time;

        if(metadata.global_max_batching_time == 0 || payload.get_max_batching_time() > metadata.global_max_batching_time) metadata.global_max_batching_time = payload.get_max_batching_time();
        if(metadata.global_min_batching_time == 0 || payload.get_min_batching_time() < metadata.global_min_batching_time) metadata.global_min_batching_time = payload.get_min_batching_time();

        metadata.total_configurations += payload.get_num_integrations();
	    }


    template <typename number>
    context task_manager<number>::make_workers_context(void)
	    {
        context ctx;

        // add devices to this context
        // in this implementation, we assume that all workers are symmetric and therefore we would like
        // the work queues to be balanced
        // In principle, this can be changed if we have more information about the workers
        for(unsigned int i = 0; i < this->world.size()-1; i++)
	        {
            std::ostringstream name;
            name << __CPP_TRANSPORT_WORKER_NAME << i;
            ctx.add_device(name.str());
	        }

        return(ctx);
	    }


    template <typename number>
    void task_manager<number>::master_dispatch_output_task(typename repository<number>::output_task_record* rec, const std::list<std::string>& tags)
	    {
        // can't process a task if there are no workers
        if(this->world.size() <= 1) throw runtime_exception(runtime_exception::MPI_ERROR, __CPP_TRANSPORT_TOO_FEW_WORKERS);

        // set up a work queue representing our workers
        context ctx = this->make_workers_context();
        scheduler sch = scheduler(ctx);

        output_task<number>* tk = rec->get_task();
        work_queue< output_task_element<number> > queue = sch.make_queue(*tk);

        // set up an derived_content_writer object to coordinate logging, output destination and commits into the repository.
        // like all writers, it aborts (ie. executes a rollback if needed) when it goes out of scope unless
        // it is explicitly committed
        std::shared_ptr<typename repository<number>::derived_content_writer> writer = this->repo->new_output_task_content(rec, tags, this->get_rank());

        // set up the writer for us
        this->data_mgr->initialize_writer(writer);

        // write a task distribution list -- subsequently read by the worker processes
        // to find out which work items they should process
        this->data_mgr->create_taskfile(writer, queue);

        // instruct workers to carry out their tasks
        bool success = this->master_output_task_to_workers(writer, tags);

        // close the writer
        this->data_mgr->close_writer(writer);

        // commit output to the database if successful
        if(success) writer->commit();
	    }


    template <typename number>
    bool task_manager<number>::master_output_task_to_workers(std::shared_ptr<typename repository<number>::derived_content_writer>& writer, const std::list<std::string>& tags)
	    {
        assert(this->repo != nullptr);
        if(!this->is_master()) throw runtime_exception(runtime_exception::MPI_ERROR, __CPP_TRANSPORT_EXEC_SLAVE);

        bool success = true;

        std::vector<boost::mpi::request> requests(this->world.size()-1);

        // write log header
        boost::posix_time::ptime now = boost::posix_time::second_clock::universal_time();
        BOOST_LOG_SEV(writer->get_log(), repository<number>::normal) << "++ NEW OUTPUT TASK '" << writer->get_record()->get_name() << "' | initiated at " << boost::posix_time::to_simple_string(now) << std::endl;
        BOOST_LOG_SEV(writer->get_log(), repository<number>::normal) << *(writer->get_record()->get_task());

        // set up a timer to keep track of the total wallclock time used in this task
        boost::timer::cpu_timer wallclock_timer;

        // aggregate cache information
        typename repository<number>::output_metadata metadata;

        // get paths the workers will need
        boost::filesystem::path taskfile_path = writer->get_abs_taskfile_path();
        boost::filesystem::path tempdir_path  = writer->get_abs_tempdir_path();
        boost::filesystem::path logdir_path   = writer->get_abs_logdir_path();

        MPI::new_derived_content_payload payload(writer->get_record()->get_name(), taskfile_path, tempdir_path, logdir_path, tags);

        for(unsigned int i = 0; i < this->world.size()-1; i++)
	        {
            requests[i] = this->world.isend(this->worker_rank(i), MPI::NEW_DERIVED_CONTENT, payload);
	        }

        // wait for all messages to be received
        boost::mpi::wait_all(requests.begin(), requests.end());

        BOOST_LOG_SEV(writer->get_log(), repository<number>::normal) << "++ All workers received NEW_DERIVED_CONTENT instruction";

        // poll workers, receiving data until workers are exhausted
        std::set<unsigned int> workers;
        for(unsigned int i = 0; i < this->world.size()-1; i++) workers.insert(i);
        while(workers.size() > 0)
	        {
            BOOST_LOG_SEV(writer->get_log(), repository<number>::normal) << "++ Master polling for DERIVED_CONTENT_READY messages";
            // wait until a message is available from a worker
            boost::mpi::status stat = this->world.probe();

            switch(stat.tag())
	            {
                case MPI::DERIVED_CONTENT_READY:
	                {
                    if(!this->aggregate_content(writer, stat.source(), metadata)) success = false;
                    break;
	                }

                case MPI::FINISHED_DERIVED_CONTENT:
	                {
                    MPI::finished_derived_payload payload;
                    this->world.recv(stat.source(), MPI::FINISHED_DERIVED_CONTENT, payload);
                    BOOST_LOG_SEV(writer->get_log(), repository<number>::normal) << "++ Worker " << stat.source() << " advising finished producing derived content in CPU time " << format_time(payload.get_cpu_time());

                    this->update_output_metadata(payload, metadata);

                    workers.erase(this->worker_number(stat.source()));
                    break;
	                }

                case MPI::DERIVED_CONTENT_FAIL:
	                {
                    MPI::finished_derived_payload payload;
                    this->world.recv(stat.source(), MPI::DERIVED_CONTENT_FAIL, payload);
                    BOOST_LOG_SEV(writer->get_log(), repository<number>::normal) << "++ Worker " << stat.source() << " advising failure to produce derived content (successful tasks consumed wallclock time " << format_time(payload.get_cpu_time()) << ")";

                    this->update_output_metadata(payload, metadata);

                    workers.erase(this->worker_number(stat.source()));
                    success = false;
                    break;
	                }

                default:
	                {
                    BOOST_LOG_SEV(writer->get_log(), repository<number>::warning) << "++ Master received unexpected message " << stat.tag() << " waiting in the queue";
                    break;
	                }
	            }
	        }

        writer->set_metadata(metadata);

        wallclock_timer.stop();
        now = boost::posix_time::second_clock::universal_time();
        BOOST_LOG_SEV(writer->get_log(), repository<number>::normal) << "";
        BOOST_LOG_SEV(writer->get_log(), repository<number>::normal) << "++ TASK COMPLETE (at " << boost::posix_time::to_simple_string(now) << "): FINAL USAGE STATISTICS";
        BOOST_LOG_SEV(writer->get_log(), repository<number>::normal) << "++   Total wallclock time for task '" << writer->get_record()->get_name() << "' " << format_time(wallclock_timer.elapsed().wall);
        BOOST_LOG_SEV(writer->get_log(), repository<number>::normal) << "";
        BOOST_LOG_SEV(writer->get_log(), repository<number>::normal) << "++ AGGREGATE CACHE STATISTICS";
        BOOST_LOG_SEV(writer->get_log(), repository<number>::normal) << "++   Total work time required by worker processes      = " << format_time(metadata.work_time);
        BOOST_LOG_SEV(writer->get_log(), repository<number>::normal) << "++   Total aggregation time required by master process = " << format_time(metadata.aggregation_time);
        BOOST_LOG_SEV(writer->get_log(), repository<number>::normal) << "++   Total time spent reading database                 = " << format_time(metadata.db_time);
        BOOST_LOG_SEV(writer->get_log(), repository<number>::normal) << "";
        BOOST_LOG_SEV(writer->get_log(), repository<number>::normal) << "++   Total time-configuration cache hits               = " << metadata.time_config_hits << ", unloads = " << metadata.time_config_unloads;
        BOOST_LOG_SEV(writer->get_log(), repository<number>::normal) << "++   Total twopf k-config cache hits                   = " << metadata.twopf_kconfig_hits << ", unloads = " << metadata.twopf_kconfig_unloads;
        BOOST_LOG_SEV(writer->get_log(), repository<number>::normal) << "++   Total threepf k-config cache hits                 = " << metadata.threepf_kconfig_hits << ", unloads = " << metadata.threepf_kconfig_unloads;
        BOOST_LOG_SEV(writer->get_log(), repository<number>::normal) << "++   Total data cache hits                             = " << metadata.data_hits << ", unloads = " << metadata.data_unloads;
        BOOST_LOG_SEV(writer->get_log(), repository<number>::normal) << "++   Total zeta cache hits                             = " << metadata.zeta_hits << ", unloads = " << metadata.zeta_unloads;
        BOOST_LOG_SEV(writer->get_log(), repository<number>::normal) << "";
        BOOST_LOG_SEV(writer->get_log(), repository<number>::normal) << "++   Total time config cache evictions                 = " << format_time(metadata.time_config_evictions);
        BOOST_LOG_SEV(writer->get_log(), repository<number>::normal) << "++   Total twopf k-config cache evictions              = " << format_time(metadata.twopf_kconfig_evictions);
        BOOST_LOG_SEV(writer->get_log(), repository<number>::normal) << "++   Total threepf k-config cache evictions            = " << format_time(metadata.threepf_kconfig_evictions);
        BOOST_LOG_SEV(writer->get_log(), repository<number>::normal) << "++   Total data cache evictions                        = " << format_time(metadata.data_evictions);
        BOOST_LOG_SEV(writer->get_log(), repository<number>::normal) << "++   Total zeta cache evictions                        = " << format_time(metadata.zeta_evictions);

        return(success);
	    }


    template <typename number>
    bool task_manager<number>::aggregate_content(std::shared_ptr<typename repository<number>::derived_content_writer>& writer, int source,
                                                 typename repository<number>::output_metadata& metadata)
	    {
        MPI::content_ready_payload payload;
        this->world.recv(source, MPI::DERIVED_CONTENT_READY, payload);
        BOOST_LOG_SEV(writer->get_log(), repository<number>::normal) << "++ Worker " << source << " sent content-ready notification";

        // set up a timer to measure how long we spend aggregating
        boost::timer::cpu_timer aggregate_timer;

        bool success = writer->aggregate(payload.get_product_name());

        aggregate_timer.stop();
        metadata.aggregation_time += aggregate_timer.elapsed().wall;

        return(success);
	    }


    template <typename number>
    template <typename PayloadObject>
    void task_manager<number>::update_output_metadata(PayloadObject& payload, typename repository<number>::output_metadata& metadata)
	    {
        metadata.work_time                 += payload.get_cpu_time();
        metadata.db_time                   += payload.get_database_time();
        metadata.time_config_hits          += payload.get_time_config_hits();
        metadata.time_config_unloads       += payload.get_time_config_unloads();
        metadata.time_config_evictions     += payload.get_time_config_evictions();
        metadata.twopf_kconfig_hits        += payload.get_twopf_kconfig_hits();
        metadata.twopf_kconfig_unloads     += payload.get_twopf_kconfig_unloads();
        metadata.twopf_kconfig_evictions   += payload.get_twopf_kconfig_evictions();
        metadata.threepf_kconfig_hits      += payload.get_threepf_kconfig_hits();
        metadata.threepf_kconfig_unloads   += payload.get_threepf_kconfig_unloads();
        metadata.threepf_kconfig_evictions += payload.get_threepf_kconfig_evictions();
        metadata.data_hits                 += payload.get_data_hits();
        metadata.data_unloads              += payload.get_data_unloads();
        metadata.data_evictions            += payload.get_data_evictions();
        metadata.zeta_hits                 += payload.get_zeta_hits();
        metadata.zeta_unloads              += payload.get_zeta_unloads();
        metadata.zeta_evictions            += payload.get_zeta_evictions();
	    }


    template <typename number>
    void task_manager<number>::master_dispatch_postintegration_task(typename repository<number>::postintegration_task_record* rec, const std::list<std::string>& tags)
	    {
        assert(rec != nullptr);

        // can't process a task if there are no workers
        if(this->world.size() <= 1) throw runtime_exception(runtime_exception::MPI_ERROR, __CPP_TRANSPORT_TOO_FEW_WORKERS);

        // set up a work queue representing our workers
        context ctx = this->make_workers_context();
        scheduler sch = scheduler(ctx);

        postintegration_task<number>* tk = rec->get_task();

        zeta_twopf_task<number>*   z2pf = nullptr;
        zeta_threepf_task<number>* z3pf = nullptr;
        fNL_task<number>*          zfNL = nullptr;

        if((z2pf = dynamic_cast< zeta_twopf_task<number>* >(tk)) != nullptr)
	        {
            twopf_task<number>* ptk = dynamic_cast<twopf_task<number>*>(z2pf->get_parent_task());

            assert(ptk != nullptr);
            if(ptk == nullptr)
	            {
                std::ostringstream msg;
                msg << __CPP_TRANSPORT_EXPECTED_TWOPF_TASK << " '" << z2pf->get_parent_task()->get_name() << "'";
                throw runtime_exception(runtime_exception::REPOSITORY_ERROR, msg.str());
	            }

            work_queue<twopf_kconfig> queue = sch.make_queue(sizeof(number), *ptk);
            this->master_dispatch_postintegration_queue(rec, z2pf, ptk, queue, tags);
	        }
        else if((z3pf = dynamic_cast< zeta_threepf_task<number>* >(tk)) != nullptr)
	        {
            threepf_task<number>* ptk = dynamic_cast<threepf_task<number>*>(z3pf->get_parent_task());

            assert(ptk != nullptr);
            if(ptk == nullptr)
	            {
                std::ostringstream msg;
                msg << __CPP_TRANSPORT_EXPECTED_THREEPF_TASK << " '" << z3pf->get_parent_task()->get_name() << "'";
                throw runtime_exception(runtime_exception::REPOSITORY_ERROR, msg.str());
	            }

            work_queue<threepf_kconfig> queue = sch.make_queue(sizeof(number), *ptk);
            this->master_dispatch_postintegration_queue(rec, z3pf, ptk, queue, tags);
	        }
        else if((zfNL = dynamic_cast< fNL_task<number>* >(tk)) != nullptr)
	        {
            threepf_task<number>* ptk = dynamic_cast<threepf_task<number>*>(zfNL->get_parent_task());

            assert(ptk != nullptr);
            if(ptk == nullptr)
	            {
                std::ostringstream msg;
                msg << __CPP_TRANSPORT_EXPECTED_THREEPF_TASK << " '" << zfNL->get_parent_task()->get_name() << "'";
                throw runtime_exception(runtime_exception::REPOSITORY_ERROR, msg.str());
	            }

            work_queue<threepf_kconfig> queue = sch.make_queue(sizeof(number), *ptk);
            this->master_dispatch_postintegration_queue(rec, zfNL, ptk, queue, tags);
	        }
        else
	        {
            std::ostringstream msg;
            msg << __CPP_TRANSPORT_UNKNOWN_DERIVED_TASK << " '" << rec->get_name() << "'";
            throw runtime_exception(runtime_exception::REPOSITORY_ERROR, msg.str());
	        }
	    }


    template <typename number>
    template <typename TaskObject, typename ParentTaskObject, typename QueueObject>
    void task_manager<number>::master_dispatch_postintegration_queue(typename repository<number>::postintegration_task_record* rec,
                                                                     TaskObject* tk, ParentTaskObject* ptk, QueueObject& queue, const std::list<std::string>& tags)
	    {
        assert(rec != nullptr);

        // create an output writer to commit our results into the repository
        // like all writers, it aborts (ie. executes a rollback if needed) when it goes out of scope unless
        // it is explicitly committed
        std::shared_ptr<typename repository<number>::postintegration_writer> writer = this->repo->new_postintegration_task_content(rec, tags, this->get_rank());

        // initialize the writer
        this->data_mgr->initialize_writer(writer);

        // write the task distribution list
        this->data_mgr->create_taskfile(writer, queue);

        // create new tables needed in the database
        this->data_mgr->create_tables(writer, tk);

        // instruct workers to carry out the calculation
        bool success = this->master_postintegration_task_to_workers(writer, tags);

        // close the writer
        this->data_mgr->close_writer(writer);

        // commit output if successful
        if(success) writer->commit();
	    }


    template <typename number>
    bool task_manager<number>::master_postintegration_task_to_workers(std::shared_ptr<typename repository<number>::postintegration_writer>& writer, const std::list<std::string>& tags)
	    {
        assert(this->repo != nullptr);
        if(!this->is_master()) throw runtime_exception(runtime_exception::MPI_ERROR, __CPP_TRANSPORT_EXEC_SLAVE);

        bool success = true;

        std::vector<boost::mpi::request> requests(this->world.size()-1);

        // write log header
        boost::posix_time::ptime now = boost::posix_time::second_clock::universal_time();
        BOOST_LOG_SEV(writer->get_log(), repository<number>::normal) << "++ NEW POSTINTEGRATION TASK '" << writer->get_record()->get_name() << "' | initiated at " << boost::posix_time::to_simple_string(now) << std::endl;
        BOOST_LOG_SEV(writer->get_log(), repository<number>::normal) << *(writer->get_record()->get_task());

        // set up a timer to keep track of the total wallclock time used
        boost::timer::cpu_timer wallclock_timer;

        // aggregate cache information
        typename repository<number>::output_metadata metadata;

        // get paths the workers will need
        boost::filesystem::path taskfile_path = writer->get_abs_taskfile_path();
        boost::filesystem::path tempdir_path  = writer->get_abs_tempdir_path();
        boost::filesystem::path logdir_path   = writer->get_abs_logdir_path();

        MPI::new_postintegration_payload payload(writer->get_record()->get_name(), taskfile_path, tempdir_path, logdir_path, tags);

        for(unsigned int i = 0; i < this->world.size()-1; i++)
	        {
            requests[i] = this->world.isend(this->worker_rank(i), MPI::NEW_POSTINTEGRATION, payload);
	        }

        // wait for all messages to be received
        boost::mpi::wait_all(requests.begin(), requests.end());

        BOOST_LOG_SEV(writer->get_log(), repository<number>::normal) << "++ All workers received NEW_POSTINTEGRATION instruction";

        // poll workers, receiving data until workers are exhausted
        std::set<unsigned int> workers;
        for(unsigned int i = 0; i < this->world.size()-1; i++) workers.insert(i);
        while(workers.size() > 0)
	        {
            BOOST_LOG_SEV(writer->get_log(), repository<number>::normal) <<  "++ Master polling for POSTINTEGRATION_DATA_READY messages";
            // wait until a message is available from a worker
            boost::mpi::status stat = this->world.probe();

            switch(stat.tag())
	            {
                case MPI::POSTINTEGRATION_DATA_READY:
	                {
                    this->aggregate_postprocess(writer, stat.source(), metadata);
                    break;
	                }

                case MPI::FINISHED_POSTINTEGRATION:
	                {
                    MPI::finished_postintegration_payload payload;
                    this->world.recv(stat.source(), MPI::FINISHED_POSTINTEGRATION, payload);
                    BOOST_LOG_SEV(writer->get_log(), repository<number>::normal) << "++ Worker " << stat.source() << " advising finished postintegration processing in wallclock time " << format_time(payload.get_cpu_time());

                    this->update_output_metadata(payload, metadata);

                    workers.erase(this->worker_number(stat.source()));
                    break;
	                }

                case MPI::POSTINTEGRATION_FAIL:
	                {
                    MPI::finished_postintegration_payload payload;
                    this->world.recv(stat.source(), MPI::POSTINTEGRATION_FAIL, payload);
                    BOOST_LOG_SEV(writer->get_log(), repository<number>::normal) << "++ Worker " << stat.source() << " advising failure to complete postintegration processing (successful tasks consumed wallclock time " << format_time(payload.get_cpu_time()) << ")";

                    this->update_output_metadata(payload, metadata);

                    workers.erase(this->worker_number(stat.source()));
                    success = false;
                    break;
	                }

                default:
	                {
                    BOOST_LOG_SEV(writer->get_log(), repository<number>::warning) << "++ Master received unexpected message " << stat.tag() << " waiting in the queue";
                    break;
	                }
	            }
	        }

        writer->set_metadata(metadata);

        wallclock_timer.stop();
        now = boost::posix_time::second_clock::universal_time();
        BOOST_LOG_SEV(writer->get_log(), repository<number>::normal) << "";
        BOOST_LOG_SEV(writer->get_log(), repository<number>::normal) << "++ TASK COMPLETE (at " << boost::posix_time::to_simple_string(now) << "): FINAL USAGE STATISTICS";
        BOOST_LOG_SEV(writer->get_log(), repository<number>::normal) << "++   Total wallclock time for task '" << writer->get_record()->get_name() << "' " << format_time(wallclock_timer.elapsed().wall);
        BOOST_LOG_SEV(writer->get_log(), repository<number>::normal) << "";
        BOOST_LOG_SEV(writer->get_log(), repository<number>::normal) << "++ AGGREGATE CACHE STATISTICS";
        BOOST_LOG_SEV(writer->get_log(), repository<number>::normal) << "++   Total work time required by worker processes      = " << format_time(metadata.work_time);
        BOOST_LOG_SEV(writer->get_log(), repository<number>::normal) << "++   Total aggregation time required by master process = " << format_time(metadata.aggregation_time);
        BOOST_LOG_SEV(writer->get_log(), repository<number>::normal) << "++   Total time spent reading database                 = " << format_time(metadata.db_time);
        BOOST_LOG_SEV(writer->get_log(), repository<number>::normal) << "";
        BOOST_LOG_SEV(writer->get_log(), repository<number>::normal) << "++   Total time-configuration cache hits               = " << metadata.time_config_hits << ", unloads = " << metadata.time_config_unloads;
        BOOST_LOG_SEV(writer->get_log(), repository<number>::normal) << "++   Total twopf k-config cache hits                   = " << metadata.twopf_kconfig_hits << ", unloads = " << metadata.twopf_kconfig_unloads;
        BOOST_LOG_SEV(writer->get_log(), repository<number>::normal) << "++   Total threepf k-config cache hits                 = " << metadata.threepf_kconfig_hits << ", unloads = " << metadata.threepf_kconfig_unloads;
        BOOST_LOG_SEV(writer->get_log(), repository<number>::normal) << "++   Total data cache hits                             = " << metadata.data_hits << ", unloads = " << metadata.data_unloads;
        BOOST_LOG_SEV(writer->get_log(), repository<number>::normal) << "++   Total zeta cache hits                             = " << metadata.zeta_hits << ", unloads = " << metadata.zeta_unloads;
        BOOST_LOG_SEV(writer->get_log(), repository<number>::normal) << "";
        BOOST_LOG_SEV(writer->get_log(), repository<number>::normal) << "++   Total time config cache evictions                 = " << format_time(metadata.time_config_evictions);
        BOOST_LOG_SEV(writer->get_log(), repository<number>::normal) << "++   Total twopf k-config cache evictions              = " << format_time(metadata.twopf_kconfig_evictions);
        BOOST_LOG_SEV(writer->get_log(), repository<number>::normal) << "++   Total threepf k-config cache evictions            = " << format_time(metadata.threepf_kconfig_evictions);
        BOOST_LOG_SEV(writer->get_log(), repository<number>::normal) << "++   Total data cache evictions                        = " << format_time(metadata.data_evictions);
        BOOST_LOG_SEV(writer->get_log(), repository<number>::normal) << "++   Total zeta cache evictions                        = " << format_time(metadata.zeta_evictions);

        return(success);
	    }


    template <typename number>
    void task_manager<number>::aggregate_postprocess(std::shared_ptr<typename repository<number>::postintegration_writer>& writer, int source,
                                                     typename repository<number>::output_metadata& metadata)
	    {
        MPI::data_ready_payload payload;
        this->world.recv(source, MPI::POSTINTEGRATION_DATA_READY, payload);
        BOOST_LOG_SEV(writer->get_log(), repository<number>::normal) << "++ Worker " << source << " sent aggregation notification for container '" << payload.get_container_path() << "'";

        // set up a timer to measure how long we spend batching
        boost::timer::cpu_timer aggregation_timer;

        writer->aggregate(payload.get_container_path());

        aggregation_timer.stop();
        BOOST_LOG_SEV(writer->get_log(), repository<number>::normal) << "++ Aggregated temporary container '" << payload.get_container_path() << "' in time " << format_time(aggregation_timer.elapsed().wall);
        metadata.aggregation_time += aggregation_timer.elapsed().wall;

        // remove temporary container
        BOOST_LOG_SEV(writer->get_log(), repository<number>::normal) << "++ Deleting temporary container '" << payload.get_container_path() << "'";
        if(!boost::filesystem::remove(payload.get_container_path()))
	        {
            std::ostringstream msg;
            msg << __CPP_TRANSPORT_DATACTR_REMOVE_TEMP << " '" << payload.get_container_path() << "'";
            this->error(msg.str());
	        }
	    }

	}   // namespace transport


#endif //__task_manager_master_H_
