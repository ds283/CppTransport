//
// Created by David Seery on 25/01/2016.
// Copyright (c) 2016 University of Sussex. All rights reserved.
//

#ifndef CPPTRANSPORT_DATAPIPE_IMPL_H
#define CPPTRANSPORT_DATAPIPE_IMPL_H


#include "transport-runtime/data/datapipe/datapipe_decl.h"


namespace transport
  {

    // DATAPIPE METHODS


    template <typename number>
    datapipe<number>::datapipe(unsigned int cap, const boost::filesystem::path& lp, const boost::filesystem::path& tp, unsigned int w,
                               data_manager<number>& dm, utility_callbacks& u, bool no_log)
      : logdir_path(lp),
        temporary_path(tp),
        worker_number(w),
        utilities(u),
        data_mgr(dm),
        time_config_cache_table(nullptr),
        twopf_kconfig_cache_table(nullptr),
        threepf_kconfig_cache_table(nullptr),
        statistics_cache_table(nullptr),
        data_cache_table(nullptr),
        time_config_cache(CPPTRANSPORT_DEFAULT_CONFIGURATION_CACHE_SIZE),
        twopf_kconfig_cache(CPPTRANSPORT_DEFAULT_CONFIGURATION_CACHE_SIZE),
        threepf_kconfig_cache(CPPTRANSPORT_DEFAULT_CONFIGURATION_CACHE_SIZE),
        statistics_cache(CPPTRANSPORT_DEFAULT_CONFIGURATION_CACHE_SIZE),
        data_cache(cap),
        type(attachment_type::none_attached),
        N_fields(0)
      {
        this->database_timer.stop();

        std::ostringstream log_file;
        log_file << CPPTRANSPORT_LOG_FILENAME_A << worker_number << CPPTRANSPORT_LOG_FILENAME_B;

        boost::filesystem::path log_path = logdir_path / log_file.str();

        if(!no_log)
          {
            boost::shared_ptr<boost::log::core> core = boost::log::core::get();

            boost::shared_ptr<boost::log::sinks::text_file_backend> backend =
              boost::make_shared<boost::log::sinks::text_file_backend>(boost::log::keywords::file_name = log_path.string());

            // enable auto-flushing of log entries
            // this degrades performance, but we are not writing many entries and they
            // will not be lost in the event of a crash
            backend->auto_flush(true);

            // Wrap it into the frontend and register in the core.
            // The backend requires synchronization in the frontend.
            this->log_sink = boost::shared_ptr<sink_t>(new sink_t(backend));

            core->add_sink(this->log_sink);

            boost::log::add_common_attributes();
          }
        else
          {
            this->log_sink.reset();
          }

        BOOST_LOG_SEV(this->log_source, log_severity_level::normal) << "** Instantiated datapipe (cache capacity " << format_memory(cap) << ")"
        << " on MPI host " << host_info.get_host_name()
        << ", OS = " << host_info.get_os_name()
        << ", Version = " << host_info.get_os_version()
        << " (Release = " << host_info.get_os_release()
        << ") | " << host_info.get_architecture()
        << " | CPU vendor = " << host_info.get_cpu_vendor_id() << '\n';
      }


    template <typename number>
    datapipe<number>::~datapipe()
      {
//		    assert(this->database_timer.is_stopped());

        if(!(this->database_timer.is_stopped()))
          {
            BOOST_LOG_SEV(this->log_source, log_severity_level::error) << ":: Error: datapipe being destroyed, but database timer is still running";
          }

        BOOST_LOG_SEV(this->log_source, log_severity_level::normal) << "";
        BOOST_LOG_SEV(this->log_source, log_severity_level::normal) << "-- Closing datapipe: final usage statistics:";
        BOOST_LOG_SEV(this->log_source, log_severity_level::normal) << "--   time spent querying database       = " << format_time(this->database_timer.elapsed().wall);
        BOOST_LOG_SEV(this->log_source, log_severity_level::normal) << "--   time-configuration cache hits      = " << this->time_config_cache.get_hits() << " | unloads = " << this->time_config_cache.get_unloads();
        BOOST_LOG_SEV(this->log_source, log_severity_level::normal) << "--   twopf k-configuration cache hits   = " << this->twopf_kconfig_cache.get_hits() << " | unloads = " << this->twopf_kconfig_cache.get_unloads();
        BOOST_LOG_SEV(this->log_source, log_severity_level::normal) << "--   threepf k-configuration cache hits = " << this->threepf_kconfig_cache.get_hits() << " | unloads = " << this->threepf_kconfig_cache.get_unloads();
        BOOST_LOG_SEV(this->log_source, log_severity_level::normal) << "--   statistics cache hits              = " << this->statistics_cache.get_hits() << " | unloads = " << this->statistics_cache.get_unloads();
        BOOST_LOG_SEV(this->log_source, log_severity_level::normal) << "--   data cache hits:                   = " << this->data_cache.get_hits() << " | unloads = " << this->data_cache.get_unloads();
        BOOST_LOG_SEV(this->log_source, log_severity_level::normal) << "";
        BOOST_LOG_SEV(this->log_source, log_severity_level::normal) << "--   time-configuration evictions       = " << format_time(this->time_config_cache.get_eviction_timer());
        BOOST_LOG_SEV(this->log_source, log_severity_level::normal) << "--   twopf k-configuration evictions    = " << format_time(this->twopf_kconfig_cache.get_eviction_timer());
        BOOST_LOG_SEV(this->log_source, log_severity_level::normal) << "--   threepf k-configuration evictions  = " << format_time(this->threepf_kconfig_cache.get_eviction_timer());
        BOOST_LOG_SEV(this->log_source, log_severity_level::normal) << "--   statistics cache evictions         = " << format_time(this->statistics_cache.get_eviction_timer());
        BOOST_LOG_SEV(this->log_source, log_severity_level::normal) << "--   data evictions                     = " << format_time(this->data_cache.get_eviction_timer());

        // detach any attached content group, if necessary
        switch(this->type)
          {
            case attachment_type::integration_attached:
              {
                if(this->attached_integration_group) this->detach();
                break;
              }

            case attachment_type::postintegration_attached:
              {
                if(this->attached_postintegration_group) this->detach();
                break;
              }

            case attachment_type::none_attached:
              {
                break;
              }
          }

        if(this->log_sink)    // implicitly converts to bool, value true if not null
          {
            boost::shared_ptr<boost::log::core> core = boost::log::core::get();
            core->remove_sink(this->log_sink);
          }
      }


    template <typename number>
    bool datapipe<number>::validate_unattached(void) const
      {
        if(this->type != attachment_type::none_attached) return(false);

        if(this->attached_integration_group.get() != nullptr
           || this->attached_postintegration_group.get() != nullptr) return(false);

        if(this->time_config_cache_table != nullptr
           || this->twopf_kconfig_cache_table != nullptr
           || this->threepf_kconfig_cache_table != nullptr
           || this->statistics_cache_table != nullptr
           || this->data_cache_table != nullptr) return(false);

        return(true);
      }


    template <typename number>
    bool datapipe<number>::validate_attached(void) const
      {
        switch(this->type)
          {
            case attachment_type::none_attached:
              {
                return(false);
              }

            case attachment_type::integration_attached:
              {
                if(this->attached_integration_group.get() == nullptr) return(false);
                break;
              }

            case attachment_type::postintegration_attached:
              {
                if(this->attached_postintegration_group.get() == nullptr) return(false);
                break;
              }
          }

        if(this->time_config_cache_table == nullptr
           || this->twopf_kconfig_cache_table == nullptr
           || this->threepf_kconfig_cache_table == nullptr
           || this->statistics_cache_table == nullptr
           || this->data_cache_table == nullptr) return(false);

        return(true);
      }


    template <typename number>
    bool datapipe<number>::validate_attached(attachment_type t) const
      {
        if(this->type != t) throw runtime_exception(exception_type::DATAPIPE_ERROR, CPPTRANSPORT_DATAMGR_PIPE_WRONG_CONTENT);
        return(this->validate_attached());
      }


    template <typename number>
    std::string datapipe<number>::attach(derivable_task<number>* tk, const std::list<std::string>& tags)
      {
        assert(this->validate_unattached());
        if(!this->validate_unattached()) throw runtime_exception(exception_type::DATAPIPE_ERROR, CPPTRANSPORT_DATAMGR_ATTACH_PIPE_ALREADY_ATTACHED);

        assert(tk != nullptr);
        if(tk == nullptr) throw runtime_exception(exception_type::DATAPIPE_ERROR, CPPTRANSPORT_DATAMGR_PIPE_NULL_TASK);

        // work out what sort of content group we are trying to attach
        integration_task<number>* itk     = nullptr;
        postintegration_task<number>* ptk = nullptr;

        if((itk = dynamic_cast< integration_task<number>* >(tk)) != nullptr)    // trying to attach to an integration content group
          {
            this->type                       = attachment_type::integration_attached;
            this->attached_integration_group = this->data_mgr.datapipe_attach_integration_content(this, this->utilities.integration_finder, tk->get_name(), tags);

            // remember number of fields associated with this container
            this->N_fields = itk->get_model()->get_N_fields();

            integration_payload& payload = this->attached_integration_group->get_payload();
            this->attach_cache_tables(payload);

            BOOST_LOG_SEV(this->get_log(), log_severity_level::normal) << "** ATTACH integration content group " << boost::posix_time::to_simple_string(this->attached_integration_group->get_creation_time())
            << " (from integration task '" << tk->get_name() << "')";

            return(this->attached_integration_group->get_name());
          }
        else if((ptk = dynamic_cast< postintegration_task<number>* >(tk)) != nullptr)      // trying to attach to a postintegration content group
          {
            this->type                           = attachment_type::postintegration_attached;
            this->attached_postintegration_group = this->data_mgr.datapipe_attach_postintegration_content(this, this->utilities.postintegration_finder, tk->get_name(), tags);

            this->N_fields = 0;

            postintegration_payload& payload = this->attached_postintegration_group->get_payload();
            this->attach_cache_tables(payload);

            BOOST_LOG_SEV(this->get_log(), log_severity_level::normal) << "** ATTACH postintegration content group " << boost::posix_time::to_simple_string(this->attached_postintegration_group->get_creation_time())
            << " (from postintegration task '" << tk->get_name() << "')";

            return(this->attached_postintegration_group->get_name());
          }

        std::stringstream msg;
        msg << CPPTRANSPORT_DATAMGR_UNKNOWN_DERIVABLE_TASK << " '" << tk->get_name() << "'";
        throw runtime_exception(exception_type::DATAPIPE_ERROR, msg.str());
      }


    template <typename number>
    template <typename Payload>
    void datapipe<number>::attach_cache_tables(Payload& payload)
      {
        // attach new cache tables for this container

        this->time_config_cache_table     = &(this->time_config_cache.get_table_handle(payload.get_container_path().string()));
        this->twopf_kconfig_cache_table   = &(this->twopf_kconfig_cache.get_table_handle(payload.get_container_path().string()));
        this->threepf_kconfig_cache_table = &(this->threepf_kconfig_cache.get_table_handle(payload.get_container_path().string()));
        this->statistics_cache_table      = &(this->statistics_cache.get_table_handle(payload.get_container_path().string()));
        this->data_cache_table            = &(this->data_cache.get_table_handle(payload.get_container_path().string()));
      }


    template <typename number>
    void datapipe<number>::detach(void)
      {
        assert(this->validate_attached());
        if(!this->validate_attached()) throw runtime_exception(exception_type::DATAPIPE_ERROR, CPPTRANSPORT_DATAMGR_DETACH_PIPE_NOT_ATTACHED);

        this->data_mgr.datapipe_detach(this);

        switch(this->type)
          {
            case attachment_type::integration_attached:
              {
                BOOST_LOG_SEV(this->get_log(), datapipe<number>::log_severity_level::normal) << "** DETACH content group " << boost::posix_time::to_simple_string(this->attached_integration_group->get_creation_time());
                break;
              }

            case attachment_type::postintegration_attached:
              {
                BOOST_LOG_SEV(this->get_log(), datapipe<number>::log_severity_level::normal) << "** DETACH content group " << boost::posix_time::to_simple_string(this->attached_postintegration_group->get_creation_time());
                break;
              }

            case attachment_type::none_attached:
              {
                break;
              }
          }

        this->attached_integration_group.reset();
        this->attached_postintegration_group.reset();

        this->type = attachment_type::none_attached;

        this->time_config_cache_table     = nullptr;
        this->twopf_kconfig_cache_table   = nullptr;
        this->threepf_kconfig_cache_table = nullptr;
        this->statistics_cache_table      = nullptr;
        this->data_cache_table            = nullptr;
      }


    template <typename number>
    content_group_record<integration_payload>* datapipe<number>::get_attached_integration_record()
      {
        return(this->attached_integration_group.get());   // is null if nothing attached
      }


    template <typename number>
    content_group_record<postintegration_payload>* datapipe<number>::get_attached_postintegration_record()
      {
        return(this->attached_postintegration_group.get());
      }



    template <typename number>
    typename datapipe<number>::time_config_handle& datapipe<number>::new_time_config_handle(const derived_data::SQL_time_config_query& query) const
      {
        assert(this->validate_attached());
        if(!this->validate_attached()) throw runtime_exception(exception_type::DATAPIPE_ERROR, CPPTRANSPORT_DATAMGR_PIPE_NOT_ATTACHED);

        return(this->time_config_cache_table->get_serial_handle(query));
      }


    template <typename number>
    typename datapipe<number>::twopf_kconfig_handle& datapipe<number>::new_twopf_kconfig_handle(const derived_data::SQL_twopf_kconfig_query& query) const
      {
        assert(this->validate_attached());
        if(!this->validate_attached()) throw runtime_exception(exception_type::DATAPIPE_ERROR, CPPTRANSPORT_DATAMGR_PIPE_NOT_ATTACHED);

        return(this->twopf_kconfig_cache_table->get_serial_handle(query));
      }


    template <typename number>
    typename datapipe<number>::threepf_kconfig_handle& datapipe<number>::new_threepf_kconfig_handle(const derived_data::SQL_threepf_kconfig_query& query) const
      {
        assert(this->validate_attached());
        if(!this->validate_attached()) throw runtime_exception(exception_type::DATAPIPE_ERROR, CPPTRANSPORT_DATAMGR_PIPE_NOT_ATTACHED);

        return(this->threepf_kconfig_cache_table->get_serial_handle(query));
      }


    template <typename number>
    typename datapipe<number>::k_statistics_handle& datapipe<number>::new_k_statistics_handle(const derived_data::SQL_query& query) const
      {
        assert(this->validate_attached());
        if(!this->validate_attached()) throw runtime_exception(exception_type::DATAPIPE_ERROR, CPPTRANSPORT_DATAMGR_PIPE_NOT_ATTACHED);

        return(this->statistics_cache_table->get_serial_handle(query));
      }


    template <typename number>
    typename datapipe<number>::time_data_handle& datapipe<number>::new_time_data_handle(const derived_data::SQL_time_config_query& query) const
      {
        assert(this->validate_attached());
        if(!this->validate_attached()) throw runtime_exception(exception_type::DATAPIPE_ERROR, CPPTRANSPORT_DATAMGR_PIPE_NOT_ATTACHED);

        return(this->data_cache_table->get_serial_handle(query));
      }


    template <typename number>
    typename datapipe<number>::kconfig_data_handle& datapipe<number>::new_kconfig_data_handle(const derived_data::SQL_twopf_kconfig_query& query) const
      {
        assert(this->validate_attached());
        if(!this->validate_attached()) throw runtime_exception(exception_type::DATAPIPE_ERROR, CPPTRANSPORT_DATAMGR_PIPE_NOT_ATTACHED);

        return(this->data_cache_table->get_serial_handle(query));
      }


    template <typename number>
    typename datapipe<number>::kconfig_data_handle& datapipe<number>::new_kconfig_data_handle(const derived_data::SQL_threepf_kconfig_query& query) const
      {
        assert(this->validate_attached());
        if(!this->validate_attached()) throw runtime_exception(exception_type::DATAPIPE_ERROR, CPPTRANSPORT_DATAMGR_PIPE_NOT_ATTACHED);

        return(this->data_cache_table->get_serial_handle(query));
      }


    template <typename number>
    typename datapipe<number>::time_zeta_handle& datapipe<number>::new_time_zeta_handle(const derived_data::SQL_time_config_query& query) const
      {
        assert(this->validate_attached());
        if(!this->validate_attached()) throw runtime_exception(exception_type::DATAPIPE_ERROR, CPPTRANSPORT_DATAMGR_PIPE_NOT_ATTACHED);

        return(this->data_cache_table->get_serial_handle(query));
      }


    template <typename number>
    typename datapipe<number>::kconfig_zeta_handle& datapipe<number>::new_kconfig_zeta_handle(const derived_data::SQL_twopf_kconfig_query& query) const
      {
        assert(this->validate_attached());
        if(!this->validate_attached()) throw runtime_exception(exception_type::DATAPIPE_ERROR, CPPTRANSPORT_DATAMGR_PIPE_NOT_ATTACHED);

        return(this->data_cache_table->get_serial_handle(query));
      }


    template <typename number>
    typename datapipe<number>::kconfig_zeta_handle& datapipe<number>::new_kconfig_zeta_handle(const derived_data::SQL_threepf_kconfig_query& query) const
      {
        assert(this->validate_attached());
        if(!this->validate_attached()) throw runtime_exception(exception_type::DATAPIPE_ERROR, CPPTRANSPORT_DATAMGR_PIPE_NOT_ATTACHED);

        return(this->data_cache_table->get_serial_handle(query));
      }


    template <typename number>
    time_config_tag<number> datapipe<number>::new_time_config_tag()
      {
        return time_config_tag<number>(this);
      }


    template <typename number>
    twopf_kconfig_tag<number> datapipe<number>::new_twopf_kconfig_tag()
      {
        return twopf_kconfig_tag<number>(this);
      }


    template <typename number>
    threepf_kconfig_tag<number> datapipe<number>::new_threepf_kconfig_tag()
      {
        return threepf_kconfig_tag<number>(this);
      }


    template <typename number>
    k_statistics_tag<number> datapipe<number>::new_k_statistics_tag()
      {
        return k_statistics_tag<number>(this);
      }


    template <typename number>
    background_time_data_tag<number> datapipe<number>::new_background_time_data_tag(unsigned int id)
      {
        return background_time_data_tag<number>(this, id);
      }


    template <typename number>
    cf_time_data_tag<number> datapipe<number>::new_cf_time_data_tag(cf_data_type type, unsigned int id, unsigned int kserial)
      {
        return cf_time_data_tag<number>(this, type, id, kserial);
      }


    template <typename number>
    cf_kconfig_data_tag<number> datapipe<number>::new_cf_kconfig_data_tag(cf_data_type type, unsigned int id, unsigned int tserial)
      {
        return cf_kconfig_data_tag<number>(this, type, id, tserial);
      }


    template <typename number>
    zeta_twopf_time_data_tag<number> datapipe<number>::new_zeta_twopf_time_data_tag(const twopf_kconfig& kdata)
      {
        return zeta_twopf_time_data_tag<number>(this, kdata);
      }


    template <typename number>
    zeta_threepf_time_data_tag<number> datapipe<number>::new_zeta_threepf_time_data_tag(const threepf_kconfig& kdata)
      {
        return zeta_threepf_time_data_tag<number>(this, kdata);
      }


    template <typename number>
    zeta_reduced_bispectrum_time_data_tag<number> datapipe<number>::new_zeta_reduced_bispectrum_time_data_tag(const threepf_kconfig& kdata)
      {
        return zeta_reduced_bispectrum_time_data_tag<number>(this, kdata);
      }


    template <typename number>
    zeta_twopf_kconfig_data_tag<number> datapipe<number>::new_zeta_twopf_kconfig_data_tag(unsigned int tserial)
      {
        return zeta_twopf_kconfig_data_tag<number>(this, tserial);
      }


    template <typename number>
    zeta_threepf_kconfig_data_tag<number> datapipe<number>::new_zeta_threepf_kconfig_data_tag(unsigned int tserial)
      {
        return zeta_threepf_kconfig_data_tag<number>(this, tserial);
      }


    template <typename number>
    zeta_reduced_bispectrum_kconfig_data_tag<number> datapipe<number>::new_zeta_reduced_bispectrum_kconfig_data_tag(unsigned int tserial)
      {
        return zeta_reduced_bispectrum_kconfig_data_tag<number>(this, tserial);
      }


    template <typename number>
    fNL_time_data_tag<number> datapipe<number>::new_fNL_time_data_tag(derived_data::template_type type)
      {
        return fNL_time_data_tag<number>(this, type);
      }


    template <typename number>
    BT_time_data_tag<number> datapipe<number>::new_BT_time_data_tag(derived_data::template_type type)
      {
        return BT_time_data_tag<number>(this, type);
      }


    template <typename number>
    TT_time_data_tag<number> datapipe<number>::new_TT_time_data_tag(derived_data::template_type type)
      {
        return TT_time_data_tag<number>(this, type);
      }

  };


#endif //CPPTRANSPORT_DATAPIPE_IMPL_H
