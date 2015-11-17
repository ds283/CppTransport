//
// Created by David Seery on 08/01/2014.
// Copyright (c) 2014-15 University of Sussex. All rights reserved.
//


#ifndef __data_manager_H_
#define __data_manager_H_


#include <vector>
#include <list>
#include <functional>
#include <memory>

// forward-declare model if needed
#include "transport-runtime-api/models/model_forward_declare.h"

// need repository definitions
#include "transport-runtime-api/repository/records/repository_records.h"
#include "transport-runtime-api/repository/writers/writers.h"

// need batcher definitions
#include "transport-runtime-api/data/batchers/batchers.h"

// need datapipe defintions
#include "transport-runtime-api/data/datapipe/datapipe.h"

#include "transport-runtime-api/tasks/configuration-database/twopf_config_database.h"
#include "transport-runtime-api/tasks/configuration-database/threepf_config_database.h"

#include "boost/filesystem/operations.hpp"
#include "boost/timer/timer.hpp"
#include "boost/algorithm/string.hpp"


namespace transport
  {

    template <typename number>
    class data_manager
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! Create a data_manager instance with a nominated capacity per batcher
        data_manager(unsigned int bcap, unsigned int dcap, unsigned int ckp)
          : batcher_capacity(bcap),
            pipe_capacity(dcap),
            checkpoint_interval(ckp)
          {
          }


        //! Destroy the data_manager instance. In practice this would always be delegated to an implementation class
        virtual ~data_manager() = default;


        // CAPACITY ADMIN

      public:

        //! Return the maximum memory available for batchers on this worker
        size_t get_batcher_capacity() const
          {
            return (this->batcher_capacity);
          }


        //! Set the maximum memory avilable for batchers on this worker
        void set_batcher_capacity(size_t size)
          {
            this->batcher_capacity = size;
          }


        //! Return the maximum memory available for data cache on this worker
        size_t get_pipe_capacity() const
          {
            return (this->pipe_capacity);
          }


        //! Set capacity available for data cache on this worker
        void set_pipe_capacity(size_t size)
          {
            this->pipe_capacity = size;
          }


        // CHECKPOINTING ADMIN

      public:

        //! Return checkpointing interval, measured in seconds
        unsigned int get_checkpoint_interval() const
          {
            return (this->checkpoint_interval);
          }


        //! Set checkpointing interval in seconds. Setting a value of 0 disables checkpointing
        void set_checkpoint_interval(unsigned int interval)
          {
            this->checkpoint_interval = interval;
          }


        // WRITER HANDLING

      public:

        //! Initialize an integration_writer object.
        //! Never overwrites existing data; if the container already exists, an exception is thrown
        virtual void initialize_writer(std::shared_ptr< integration_writer<number> >& writer, bool recovery_mode = false) = 0;

        //! Close an integration_writer object.
        virtual void close_writer(std::shared_ptr< integration_writer<number> >& writer) = 0;

        //! Initialize a derived_content_writer object.
        virtual void initialize_writer(std::shared_ptr< derived_content_writer<number> >& writer, bool recovery_mode = false) = 0;

        //! Close an open derived_content_writer object.
        virtual void close_writer(std::shared_ptr< derived_content_writer<number> >& writer) = 0;

        //! Initialize a postintegration_writer object.
        virtual void initialize_writer(std::shared_ptr< postintegration_writer<number> >& writer, bool recovery_mode = false) = 0;

        //! Close an open postintegration_writer object.
        virtual void close_writer(std::shared_ptr< postintegration_writer<number> >& writer) = 0;


        // WRITE TABLES FOR A DATA CONTAINER

      public:

        //! Create tables needed for a twopf container
        virtual void create_tables(std::shared_ptr< integration_writer<number> >& writer, twopf_task<number>* tk) = 0;

        //! Create tables needed for a threepf container
        virtual void create_tables(std::shared_ptr< integration_writer<number> >& writer, threepf_task<number>* tk) = 0;

        //! Create tables needed for a zeta twopf container
        virtual void create_tables(std::shared_ptr< postintegration_writer<number> >& writer, zeta_twopf_task<number>* tk) = 0;

        //! Create tables needed for a zeta threepf container
        virtual void create_tables(std::shared_ptr< postintegration_writer<number> >& writer, zeta_threepf_task<number>* tk) = 0;

        //! Create tables needed for an fNL container
        virtual void create_tables(std::shared_ptr< postintegration_writer<number> >& writer, fNL_task<number>* tk) = 0;


        // SEEDING

      public:

        //! Seed a writer for a twopf task
        virtual void seed_writer(std::shared_ptr< integration_writer<number> >& writer, twopf_task<number>* tk,
                                 const std::shared_ptr< output_group_record<integration_payload> >& seed) = 0;

        //! Seed a writer for a threepf task
        virtual void seed_writer(std::shared_ptr< integration_writer<number> >& writer, threepf_task<number>* tk,
                                 const std::shared_ptr< output_group_record<integration_payload> >& seed) = 0;

        //! Seed a writer for a zeta twopf task
        virtual void seed_writer(std::shared_ptr< postintegration_writer<number> >& writer, zeta_twopf_task<number>* tk,
                                 const std::shared_ptr< output_group_record<postintegration_payload> >& seed) = 0;

        //! Seed a writer for a zeta threepf task
        virtual void seed_writer(std::shared_ptr< postintegration_writer<number> >& writer, zeta_threepf_task<number>* tk,
                                 const std::shared_ptr< output_group_record<postintegration_payload> >& seed) = 0;

        //! Seed a writer for an fNL task
        virtual void seed_writer(std::shared_ptr< postintegration_writer<number> >& writer, fNL_task<number>* tk,
                                 const std::shared_ptr< output_group_record<postintegration_payload> >& seed) = 0;


        // CONSTRUCT BATCHERS

      public:

        //! Create a temporary container for twopf data. Returns a batcher which can be used for writing to the container.
        virtual twopf_batcher<number> create_temp_twopf_container(twopf_task<number>* tk, const boost::filesystem::path& tempdir,
                                                                  const boost::filesystem::path& logdir,
                                                                  unsigned int worker, unsigned int group, model<number>* m,
                                                                  generic_batcher::container_dispatch_function dispatcher) = 0;

        //! Create a temporary container for threepf data. Returns a batcher which can be used for writing to the container.
        virtual threepf_batcher<number> create_temp_threepf_container(threepf_task<number>* tk, const boost::filesystem::path& tempdir,
                                                                      const boost::filesystem::path& logdir,
                                                                      unsigned int worker, unsigned int group, model<number>* m,
                                                                      generic_batcher::container_dispatch_function dispatcher) = 0;

        //! Create a temporary container for zeta twopf data. Returns a batcher which can be used for writing to the container.
        virtual zeta_twopf_batcher<number> create_temp_zeta_twopf_container(const boost::filesystem::path& tempdir, const boost::filesystem::path& logdir,
                                                                            unsigned int worker, model<number>* m,
                                                                            generic_batcher::container_dispatch_function dispatcher) = 0;

        //! Create a temporary container for zeta threepf data. Returns a batcher which can be used for writing to the container.
        virtual zeta_threepf_batcher<number> create_temp_zeta_threepf_container(const boost::filesystem::path& tempdir, const boost::filesystem::path& logdir,
                                                                                unsigned int worker, model<number>* m,
                                                                                generic_batcher::container_dispatch_function dispatcher) = 0;

        //! Create a temporary container for fNL data. Returns a batcher which can be used for writing to the container.
        virtual fNL_batcher<number> create_temp_fNL_container(const boost::filesystem::path& tempdir, const boost::filesystem::path& logdir,
                                                              unsigned int worker, model<number>* m,
                                                              generic_batcher::container_dispatch_function dispatcher, derived_data::template_type type) = 0;


        // INTEGRITY CHECK

      public:   // have to be public so we can use std::bind

        //! Check integrity for a twopf container
        void check_twopf_integrity_handler(integration_writer<number>& writer, integration_task<number>* tk);

        //! Check integrity for a threepf container
        void check_threepf_integrity_handler(integration_writer<number>& writer, integration_task<number>* tk);

        //! Check integrity for a zeta twopf container
        void check_zeta_twopf_integrity_handler(postintegration_writer<number>& writer, postintegration_task<number>* tk);

        //! Check integrity for a zeta threepf container
        void check_zeta_threepf_integrity_handler(postintegration_writer<number>& writer, postintegration_task<number>* tk);

        //! Check integrity for an fNL container
        void check_fNL_integrity_handler(postintegration_writer<number>& writer, postintegration_task<number>* tk);

      public:

        //! Synchronize missing serial numbers between an integration writer and a postintegration writer
        void synchronize_missing_serials(std::shared_ptr< integration_writer<number> > i_writer, std::shared_ptr< postintegration_writer<number> > p_writer,
                                         integration_task<number>* i_tk, postintegration_task<number>* p_tk);


        // CALCULATE MISSING SERIAL NUMBERS

      protected:

        //! get missing serial numbers from a twopf-re table; should be provided by implementation
        virtual std::list<unsigned int> get_missing_twopf_re_serials(integration_writer<number>& writer) = 0;

        //! get missing serial numbers from a twopf-im table; should be provided by implementation
        virtual std::list<unsigned int> get_missing_twopf_im_serials(integration_writer<number>& writer) = 0;

        //! get missing serial numbers from a threepf table; should be provided by implementation
        virtual std::list<unsigned int> get_missing_threepf_serials(integration_writer<number>& writer) = 0;

        //! get missing serial numbers from a zeta twopf table; should be provided by implementation
        virtual std::list<unsigned int> get_missing_zeta_twopf_serials(postintegration_writer<number>& writer) = 0;

        //! get missing serial numbers from a zeta threepf table; should be provided by implementation
        virtual std::list<unsigned int> get_missing_zeta_threepf_serials(postintegration_writer<number>& writer) = 0;

        // DROP GROUPS OF SERIAL NUMBERS

      protected:

        //! drop a set of k-configurations from a twopf-re table; should be provided by implementation
        virtual void drop_twopf_re_configurations(integration_writer<number>& writer, const std::list<unsigned int>& serials, const twopf_kconfig_database& db) = 0;

        //! drop a set of k-configurations from a twopf-im table; should be provided by implementation
        virtual void drop_twopf_im_configurations(integration_writer<number>& writer, const std::list<unsigned int>& serials, const twopf_kconfig_database& db) = 0;

        //! drop a set of k-configurations from a threepf table; should be provided by implementation
        virtual void drop_threepf_configurations(integration_writer<number>& writer, const std::list<unsigned int>& serials, const threepf_kconfig_database& db) = 0;

        //! drop a set of k-configurations from a zeta twopf table; should be provided by implementation
        virtual void drop_zeta_twopf_configurations(postintegration_writer<number>& writer, const std::list<unsigned int>& serials, const twopf_kconfig_database& db) = 0;

        //! drop a set of k-configurations from a zeta threepf table; should be provided by implementation
        virtual void drop_zeta_threepf_configurations(postintegration_writer<number>& writer, const std::list<unsigned int>& serials, const threepf_kconfig_database& db) = 0;

        //! drop statistics for a set of k-configurations; should be provided by implementation
        virtual void drop_statistics_configurations(integration_writer<number>& writer, const std::list<unsigned int>& serials, const twopf_kconfig_database& db) = 0;
        virtual void drop_statistics_configurations(integration_writer<number>& writer, const std::list<unsigned int>& serials, const threepf_kconfig_database& db) = 0;

        //! drop initial conditions for a set of k-configurations; should be provided by implementation
        virtual void drop_initial_conditions_configurations(integration_writer<number>& writer, const std::list<unsigned int>& serials, const twopf_kconfig_database& db) = 0;
        virtual void drop_initial_conditions_configurations(integration_writer<number>& writer, const std::list<unsigned int>& serials, const threepf_kconfig_database& db) = 0;

        // UTILITY FUNCTIONS

      protected:

        //! log missing data from a container, checking against a list provided by the backend if one is provided.
        //! returns: serial numbers of any further configurations that should be dropped (they were in the list provided by the backend, but not already missing)
        template <typename WriterObject, typename Database>
        std::list<unsigned int> advise_missing_content(WriterObject& writer, const std::list<unsigned int>& serials, const Database& db);

        //! compute the twopf configurations which should be dropped to match a give list of threepf serials
        std::list<unsigned int> compute_twopf_drop_list(const std::list<unsigned int>& serials, const threepf_kconfig_database& configs);

        //! map a list of twopf configuration serial numbers to corresponding threepf configuration serial numbers
        std::list<unsigned int> map_twopf_to_threepf_serials(const std::list<unsigned int>& twopf_list, const threepf_kconfig_database& threepf_db);


        // DATA PIPES AND DATA ACCESS

      public:

        //! Create a datapipe
        virtual datapipe<number> create_datapipe(const boost::filesystem::path& logdir, const boost::filesystem::path& tempdir,
                                                 typename datapipe<number>::integration_content_finder integration_finder,
                                                 typename datapipe<number>::postintegration_content_finder postintegration_finder,
                                                 typename datapipe<number>::dispatch_function dispatcher,
                                                 unsigned int worker, bool no_log = false) = 0;

        //! Pull a set of time sample-points from a datapipe
        virtual void pull_time_config(datapipe<number>* pipe, const derived_data::SQL_time_config_query& tquery,
                                      std::vector<time_config>& sample) = 0;

        //! Pull a set of 2pf k-configuration serial numbers from a datapipe
        virtual void pull_kconfig_twopf(datapipe<number>* pipe, const derived_data::SQL_twopf_kconfig_query& kquery,
                                        std::vector<twopf_kconfig>& sample) = 0;

        //! Pull a set of 3pd k-configuration serial numbesr from a datapipe
        //! Simultaneously, populates three lists (k1, k2, k3) with serial numbers for the 2pf k-configurations
        //! corresponding to k1, k2, k3
        virtual void pull_kconfig_threepf(datapipe<number>* pipe, const derived_data::SQL_threepf_kconfig_query& kquery,
                                          std::vector<threepf_kconfig>& sample) = 0;

        //! Pull a time sample of a background field from a datapipe
        virtual void pull_background_time_sample(datapipe<number>* pipe, unsigned int id, const derived_data::SQL_query& query,
                                                 std::vector<number>& sample) = 0;

        //! Pull a time sample of a twopf component at fixed k-configuration from a datapipe
        virtual void pull_twopf_time_sample(datapipe<number>* pipe, unsigned int id, const derived_data::SQL_query& query,
                                            unsigned int k_serial, std::vector<number>& sample, twopf_type type) = 0;

        //! Pull a sample of a threepf at fixed k-configuration from a datapipe
        virtual void pull_threepf_time_sample(datapipe<number>* pipe, unsigned int id, const derived_data::SQL_query& query,
                                              unsigned int k_serial, std::vector<number>& sample, threepf_type type) = 0;

        //! Pull a sample of a tensor twopf at fixed k-configuration from a datapipe
        virtual void pull_tensor_twopf_time_sample(datapipe<number>* pipe, unsigned int id, const derived_data::SQL_query& query,
                                                   unsigned int k_serial, std::vector<number>& sample) = 0;

        //! Pull a sample of the zeta twopf at fixed k-configuration from a datapipe
        virtual void pull_zeta_twopf_time_sample(datapipe<number>*, const derived_data::SQL_query& query,
                                                 unsigned int k_serial, std::vector<number>& sample) = 0;

        //! Pull a sample of the zeta threepf at fixed k-configuration from a datapipe
        virtual void pull_zeta_threepf_time_sample(datapipe<number>*, const derived_data::SQL_query& query,
                                                   unsigned int k_serial, std::vector<number>& sample) = 0;

        //! Pull a sample of the zeta reduced bispectrum at fixed k-configuration from a datapipe
        virtual void pull_zeta_redbsp_time_sample(datapipe<number>*, const derived_data::SQL_query& query,
                                                  unsigned int k_serial, std::vector<number>& sample) = 0;

        //! Pull a sample of fNL from a datapipe
        virtual void pull_fNL_time_sample(datapipe<number>*, const derived_data::SQL_query& query,
                                          std::vector<number>& sample, derived_data::template_type type) = 0;

        //! Pull a sample of bispectrum.template from a datapipe
        virtual void pull_BT_time_sample(datapipe<number>*, const derived_data::SQL_query& query,
                                         std::vector<number>& sample, derived_data::template_type type) = 0;

        //! Pull a sample of template.template from a datapipe
        virtual void pull_TT_time_sample(datapipe<number>*, const derived_data::SQL_query& query,
                                         std::vector<number>& sample, derived_data::template_type type) = 0;

        //! Pull a kconfig sample of a twopf component at fixed time from a datapipe
        virtual void pull_twopf_kconfig_sample(datapipe<number>* pipe, unsigned int id, const derived_data::SQL_query& query,
                                               unsigned int t_serial, std::vector<number>& sample, twopf_type type) = 0;

        //! Pull a kconfig sample of a threepf at fixed time from a datapipe
        virtual void pull_threepf_kconfig_sample(datapipe<number>* pipe, unsigned int id, const derived_data::SQL_query& query,
                                                 unsigned int t_serial, std::vector<number>& sample, threepf_type type) = 0;

        //! Pull a kconfig sample of a tensor twopf component at fixed time from a datapipe
        virtual void pull_tensor_twopf_kconfig_sample(datapipe<number>* pipe, unsigned int id, const derived_data::SQL_query& query,
                                                      unsigned int t_serial, std::vector<number>& sample) = 0;

        //! Pull a kconfig sample of the zeta twopf at fixed time from a datapipe
        virtual void pull_zeta_twopf_kconfig_sample(datapipe<number>*, const derived_data::SQL_query& query,
                                                    unsigned int t_serial, std::vector<number>& sample) = 0;

        //! Pull a kconfig sample of the zeta threepf at fixed time from a datapipe
        virtual void pull_zeta_threepf_kconfig_sample(datapipe<number>*, const derived_data::SQL_query& query,
                                                      unsigned int t_serial, std::vector<number>& sample) = 0;

        //! Pull a kconfig sample of the zeta reduced bispectrum at fixed time from a datapipe
        virtual void pull_zeta_redbsp_kconfig_sample(datapipe<number>*, const derived_data::SQL_query& query,
                                                     unsigned int t_serial, std::vector<number>& sample) = 0;

        //! Pull a sample of k-configuration statistics from a datapipe
        virtual void pull_k_statistics_sample(datapipe<number>* pipe, const derived_data::SQL_query& query,
                                              std::vector<kconfiguration_statistics>& data) = 0;


        // INTERNAL DATA

      protected:

        //! Capacity available for batchers
        unsigned int batcher_capacity;

        //! Capacity available for data cache
        unsigned int pipe_capacity;

        //! Checkpointing interval. 0 indicates that checkpointing is disabled
        unsigned int checkpoint_interval;

      };


    // INTEGRITY CHECK


    namespace integrity_check_impl
      {

        template <typename ConfigurationType>
        class ConfigurationFinder
          {
          public:
            ConfigurationFinder(unsigned int s)
              : serial(s)
              {
              }


            bool operator()(const ConfigurationType& a)
              {
                return (a.serial == this->serial);
              }


          private:
            unsigned int serial;
          };


        template <typename RecordData>
        class RecordFinder
          {
          public:
            RecordFinder(unsigned int s)
              : serial(s)
              {
              }


            bool operator()(const RecordData& a)
              {
                return ((*a).serial == this->serial);
              }


          private:
            unsigned int serial;
          };

      }   // namespace integrity_check_impl


    template <typename number>
    template <typename WriterObject, typename Database>
    std::list<unsigned int> data_manager<number>::advise_missing_content(WriterObject& writer, const std::list<unsigned int>& serials, const Database& db)
      {
        BOOST_LOG_SEV(writer.get_log(), base_writer::log_severity_level::normal) << "** Detected missing data in container";
        writer.set_fail(true);

        std::list<unsigned int> advised_list = writer.get_failed_serials();
        if(advised_list.size() > 0) BOOST_LOG_SEV(writer.get_log(), base_writer::log_severity_level::normal) << "** Note: backend provided list of " << advised_list.size() << " missing items to cross-check";

        for(typename std::list<unsigned int>::const_iterator t = serials.begin(); t != serials.end(); ++t)
          {
            // find this configuration
            typename Database::const_config_iterator u = std::find_if(db.config_begin(), db.config_end(),
                                                                      integrity_check_impl::ConfigurationFinder<typename Database::const_config_iterator::type>(*t));

            // emit configuration information
            std::ostringstream msg;
            msg << *u;
            std::string msg_str = msg.str();
            boost::algorithm::trim_right(msg_str);
            BOOST_LOG_SEV(writer.get_log(), base_writer::log_severity_level::normal) << "** " << msg_str;

            // search for this element in the advised list
            std::list<unsigned int>::iterator ad = std::find(advised_list.begin(), advised_list.end(), *t);

            // was this an item on the list we already knew would be missing?
            if(ad != advised_list.end()) advised_list.erase(ad);
          }

        // return any remainder
        return (advised_list);
      }


    template <typename number>
    std::list<unsigned int> data_manager<number>::compute_twopf_drop_list(const std::list<unsigned int>& serials, const threepf_kconfig_database& threepf_db)
      {
        std::list<unsigned int> drop_serials;

        // TODO: this is a O(N^2) algorithm; it would be nice if it could be replaced with something better
        for(std::list<unsigned int>::const_iterator t = serials.begin(); t != serials.end(); ++t)
          {
            threepf_kconfig_database::const_record_iterator u = std::find_if(threepf_db.record_begin(), threepf_db.record_end(),
                                                                             integrity_check_impl::RecordFinder<threepf_kconfig_database::const_record_iterator::type>(*t));

            if(u != threepf_db.record_end())
              {
                if(u->is_twopf_k1_stored()) drop_serials.push_back((*u)->k1_serial);
                if(u->is_twopf_k2_stored()) drop_serials.push_back((*u)->k2_serial);
                if(u->is_twopf_k3_stored()) drop_serials.push_back((*u)->k3_serial);
              }
          }

        drop_serials.sort();
        drop_serials.unique();

        return (drop_serials);
      }


    template <typename number>
    std::list<unsigned int> data_manager<number>::map_twopf_to_threepf_serials(const std::list<unsigned int>& twopf_list, const threepf_kconfig_database& threepf_db)
      {
        std::list<unsigned int> threepf_list;

        // TODO: this is a O(N^2) algorithm; it would be nice if it could be replaced with something better
        for(std::list<unsigned int>::const_iterator t = twopf_list.begin(); t != twopf_list.end(); ++t)
          {
            for(threepf_kconfig_database::const_record_iterator u = threepf_db.record_begin(); u != threepf_db.record_end(); ++u)
              {
                if(u->is_twopf_k1_stored() && (*u)->k1_serial == *t)
                  {
                    threepf_list.push_back((*u)->serial);
                    break;
                  }
                if(u->is_twopf_k2_stored() && (*u)->k2_serial == *t)
                  {
                    threepf_list.push_back((*u)->serial);
                    break;
                  }
                if(u->is_twopf_k3_stored() && (*u)->k3_serial == *t)
                  {
                    threepf_list.push_back((*u)->serial);
                    break;
                  }
              }
          }

        threepf_list.sort();
        threepf_list.unique();

        return (threepf_list);
      }


    template <typename number>
    void data_manager<number>::check_twopf_integrity_handler(integration_writer<number>& writer, integration_task<number>* itk)
      {
        twopf_task<number>* tk = dynamic_cast< twopf_task<number>* >(itk);
        assert(tk != nullptr);

        BOOST_LOG_SEV(writer.get_log(), base_writer::log_severity_level::normal) << '\n' << "** Performing integrity check for twopf container '" << writer.get_abs_container_path().string() << "'";

        std::list<unsigned int> serials = this->get_missing_twopf_re_serials(writer);

        if(serials.size() > 0)
          {
            std::list<unsigned int> remainder = this->advise_missing_content(writer, serials, tk->get_twopf_database());  // marks set_fail() for writer

            if(remainder.size() > 0)
              {
                BOOST_LOG_SEV(writer.get_log(), base_writer::log_severity_level::normal) << "** Dropping extra configurations not missing from container, but advised by backend:";
                this->drop_twopf_re_configurations(writer, remainder, tk->get_twopf_database());
                serials.merge(remainder);
              }

            // push list of missing serial numbers to writer and mark as a fail
            writer.set_missing_serials(serials);

            if(writer.is_collecting_statistics()) this->drop_statistics_configurations(writer, serials, tk->get_twopf_database());
            if(writer.is_collecting_initial_conditions()) this->drop_initial_conditions_configurations(writer, serials, tk->get_twopf_database());
          }
      }


    template <typename number>
    void data_manager<number>::check_threepf_integrity_handler(integration_writer<number>& writer, integration_task<number>* itk)
      {
        threepf_task<number>* tk = dynamic_cast< threepf_task<number>* >(itk);
        assert(tk != nullptr);

        BOOST_LOG_SEV(writer.get_log(), base_writer::log_severity_level::normal) << '\n' << "** Performing integrity check for threepf container '" << writer.get_abs_container_path().string() << "'";

        // get lists of missing serial numbers for threepf, real twopf and imaginary twopf
        std::list<unsigned int> twopf_re_serials = this->get_missing_twopf_re_serials(writer);
        std::list<unsigned int> twopf_im_serials = this->get_missing_twopf_im_serials(writer);
        std::list<unsigned int> threepf_serials  = this->get_missing_threepf_serials(writer);

        // merge missing twopf lists into a single one
        std::list<unsigned int> twopf_total_serials = twopf_re_serials;
        std::list<unsigned int> temp = twopf_im_serials;
        twopf_total_serials.merge(temp);
        twopf_total_serials.unique();

        // map missing twopf serials into threepf serials
        std::list<unsigned int> twopf_to_threepf_map = this->map_twopf_to_threepf_serials(twopf_total_serials, tk->get_threepf_database());

        // advise missing threepf serials
        if(threepf_serials.size() > 0)
          {
            std::list<unsigned int> remainder = this->advise_missing_content(writer, threepf_serials, tk->get_threepf_database());  // marks set_fail() for writer

            if(remainder.size() > 0)
              {
                BOOST_LOG_SEV(writer.get_log(), base_writer::log_severity_level::normal) << '\n' << "** Dropping extra threepf configurations not missing from container, but advised by backend:";
                this->drop_threepf_configurations(writer, remainder, tk->get_threepf_database());
                threepf_serials.merge(remainder);   // not necessary to remove duplicates, since there should not be any; result is sorted
              }
          }

        // check if any twopf configurations require dropping even more threepfs
        std::list<unsigned int> undropped;
        std::set_difference(twopf_to_threepf_map.begin(), twopf_to_threepf_map.end(),
                            threepf_serials.begin(), threepf_serials.end(), std::back_inserter(undropped));

        if(undropped.size() > 0)
          {
            BOOST_LOG_SEV(writer.get_log(), base_writer::log_severity_level::normal) << "** Dropping extra threepf configurations not missing from container, but implied by missing twopf configurations:";
            this->drop_threepf_configurations(writer, undropped, tk->get_threepf_database());
            threepf_serials.merge(undropped);   // not necessary to remove duplicates, since there should be any; result is sorted
          }

        if(threepf_serials.size() > 0)
          {
            writer.set_missing_serials(threepf_serials);

            if(writer.is_collecting_statistics())         this->drop_statistics_configurations(writer, threepf_serials, tk->get_threepf_database());
            if(writer.is_collecting_initial_conditions()) this->drop_initial_conditions_configurations(writer, threepf_serials, tk->get_threepf_database());

            // build list of twopf configurations which should be dropped for this entire set of threepf configurations
            std::list<unsigned int> twopf_drop = this->compute_twopf_drop_list(threepf_serials, tk->get_threepf_database());

            // compute real twopf configurations which should be dropped.
            undropped.clear();
            std::set_difference(twopf_drop.begin(), twopf_drop.end(),
                                twopf_re_serials.begin(), twopf_re_serials.end(), std::back_inserter(undropped));

            if(undropped.size() > 0)
              {
                BOOST_LOG_SEV(writer.get_log(), base_writer::log_severity_level::normal) << '\n' << "** Dropping real twopf configurations entailed by these threepf configurations, but present in the container";
                this->drop_twopf_re_configurations(writer, undropped, tk->get_twopf_database());
              }

            // compute imaginary twopf configurations which should be dropped
            undropped.clear();
            std::set_difference(twopf_drop.begin(), twopf_drop.end(),
                                twopf_im_serials.begin(), twopf_im_serials.end(), std::back_inserter(undropped));

            if(undropped.size() > 0)
              {
                BOOST_LOG_SEV(writer.get_log(), base_writer::log_severity_level::normal) << '\n' << "** Dropping imaginary twopf configurations entailed by these threepf configurations, but present in the container";
                this->drop_twopf_im_configurations(writer, undropped, tk->get_twopf_database());
              }
          }
      }


    template <typename number>
    void data_manager<number>::check_zeta_twopf_integrity_handler(postintegration_writer<number>& writer, postintegration_task<number>* ptk)
      {
        zeta_twopf_task<number>* tk = dynamic_cast< zeta_twopf_task<number>* >(ptk);
        assert(tk != nullptr);

        BOOST_LOG_SEV(writer.get_log(), base_writer::log_severity_level::normal) << '\n' << "** Performing integrity check for zeta twopf container '" << writer.get_abs_container_path().string() << "'";

        std::list<unsigned int> serials = this->get_missing_zeta_twopf_serials(writer);

        if(serials.size() > 0)
          {
            std::list<unsigned int> remainder = this->advise_missing_content(writer, serials, tk->get_twopf_database());  // marks set_fail() for writer

            if(remainder.size() > 0)
              {
                BOOST_LOG_SEV(writer.get_log(), base_writer::log_severity_level::normal) << '\n' << "** Dropping extra configurations not missing from container, but advised by backend:";
                this->drop_zeta_twopf_configurations(writer, remainder, tk->get_twopf_database());
              }

            // push list of missing serial numbers to writer
            std::list<unsigned int> merged_missing = serials;
            merged_missing.merge(remainder);

            writer.set_missing_serials(merged_missing);
          }
      }


    template <typename number>
    void data_manager<number>::check_zeta_threepf_integrity_handler(postintegration_writer<number>& writer, postintegration_task<number>* ptk)
      {
        zeta_threepf_task<number>* tk = dynamic_cast< zeta_threepf_task<number>* >(ptk);
        assert(tk != nullptr);

        BOOST_LOG_SEV(writer.get_log(), base_writer::log_severity_level::normal) << '\n' << "** Performing integrity check for zeta threepf container '" << writer.get_abs_container_path().string() << "'";

        // get lists of missing serial numbers for threepf, redbsp and twopf
        std::list<unsigned int> threepf_serials = this->get_missing_zeta_threepf_serials(writer);
        std::list<unsigned int> twopf_serials   = this->get_missing_zeta_twopf_serials(writer);

        // map missing twopf serials into threepf serials
        std::list<unsigned int> twopf_to_threepf_map = this->map_twopf_to_threepf_serials(twopf_serials, tk->get_threepf_database());

        if(threepf_serials.size() > 0)
          {
            std::list<unsigned int> remainder = this->advise_missing_content(writer, threepf_serials, tk->get_threepf_database());  // marks set_fail() for writer

            if(remainder.size() > 0)
              {
                BOOST_LOG_SEV(writer.get_log(), base_writer::log_severity_level::normal) << '\n' << "** Dropping extra configurations not missing from container, but advised by backend:";
                this->drop_zeta_threepf_configurations(writer, remainder, tk->get_threepf_database());
                threepf_serials.merge(remainder);
              }
          }

        // check if any twopf configurations require dropping even more threepfs
        std::list<unsigned int> undropped;
        std::set_difference(twopf_to_threepf_map.begin(), twopf_to_threepf_map.end(),
                            threepf_serials.begin(), threepf_serials.end(), std::back_inserter(undropped));

        if(undropped.size() > 0)
          {
            BOOST_LOG_SEV(writer.get_log(), base_writer::log_severity_level::normal) << "** Dropping extra threepf configurations not missing from container, but implied by missing twopf configurations:";
            this->drop_zeta_threepf_configurations(writer, undropped, tk->get_threepf_database());
            threepf_serials.merge(undropped);   // not necessary to remove duplicates, since there should be any; result is sorted
          }

        if(threepf_serials.size() > 0)
          {
            writer.set_missing_serials(threepf_serials);

            // build list of twopf configurations which should be dropped for this entire set of threepf configurations
            std::list<unsigned int> twopf_drop = this->compute_twopf_drop_list(threepf_serials, tk->get_threepf_database());

            // compute twopf configurations which should be dropped
            undropped.clear();
            std::set_difference(twopf_drop.begin(), twopf_drop.end(),
                                twopf_serials.begin(), twopf_serials.end(), std::back_inserter(undropped));

            if(undropped.size() > 0)
              {
                BOOST_LOG_SEV(writer.get_log(), base_writer::log_severity_level::normal) << '\n' << "** Dropping twopf configurations entailed by these threepf configurations, but present in the container";
                this->drop_zeta_twopf_configurations(writer, undropped, tk->get_twopf_database());
              }
          }
      }


    template <typename number>
    void data_manager<number>::check_fNL_integrity_handler(postintegration_writer<number>& writer, postintegration_task<number>* tk)
      {
        BOOST_LOG_SEV(writer.get_log(), base_writer::log_severity_level::normal) << '\n' << "** Performing integrity check for fNL container '" << writer.get_abs_container_path().string() << "'";
      }


    template <typename number>
    void data_manager<number>::synchronize_missing_serials(std::shared_ptr< integration_writer<number> > i_writer, std::shared_ptr< postintegration_writer<number> > p_writer,
                                                           integration_task<number>* i_tk, postintegration_task<number>* p_tk)
      {
        // get serial numbers missing individually from each writer
        std::list<unsigned int> integration_missing = i_writer->get_missing_serials();
        std::list<unsigned int> postintegration_missing = p_writer->get_missing_serials();

        // merge into a single list
        std::list<unsigned int> total_missing = integration_missing;
        std::list<unsigned int> temp = postintegration_missing;
        total_missing.merge(temp);
        total_missing.unique();

        // check for discrepancies
        std::list<unsigned int> integration_discrepant;
        std::set_difference(total_missing.begin(), total_missing.end(),
                            integration_missing.begin(), integration_missing.end(), std::back_inserter(integration_discrepant));

        if(integration_discrepant.size() > 0)
          {
            BOOST_LOG_SEV(i_writer->get_log(), base_writer::log_severity_level::normal) << '\n' << "** Synchronizing " << integration_discrepant.size() << " configurations in integration container which are missing in postintegration container";
            i_writer->merge_failure_list(integration_discrepant);
            i_writer->check_integrity(i_tk);
          }

        std::list<unsigned int> postintegration_discrepant;
        std::set_difference(total_missing.begin(), total_missing.end(),
                            postintegration_missing.begin(), postintegration_missing.end(), std::back_inserter(postintegration_discrepant));

        if(postintegration_discrepant.size() > 0)
          {
            BOOST_LOG_SEV(p_writer->get_log(), base_writer::log_severity_level::normal) << '\n' << "** Synchronizing " << postintegration_discrepant.size() << " configurations in postintegration container which are missing in integration container";
            p_writer->merge_failure_list(postintegration_discrepant);
            p_writer->check_integrity(p_tk);
          }
      }


  }   // namespace transport



#endif //__data_manager_H_
