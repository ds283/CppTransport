//
// Created by David Seery on 25/01/2016.
// Copyright (c) 2016 University of Sussex. All rights reserved.
//

#ifndef CPPTRANSPORT_DATA_MANAGER_DECL_H
#define CPPTRANSPORT_DATA_MANAGER_DECL_H


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

        //! Explicitly delete copy constructor, preventing duplication (data_manager instances should be unique)
        data_manager(const data_manager<number>& obj) = delete;

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
        virtual void initialize_writer(integration_writer<number>& writer, bool recovery_mode = false) = 0;

        //! Close an integration_writer object.
        virtual void close_writer(integration_writer<number>& writer) = 0;

        //! Initialize a derived_content_writer object.
        virtual void initialize_writer(derived_content_writer<number>& writer, bool recovery_mode = false) = 0;

        //! Close an open derived_content_writer object.
        virtual void close_writer(derived_content_writer<number>& writer) = 0;

        //! Initialize a postintegration_writer object.
        virtual void initialize_writer(postintegration_writer<number>& writer, bool recovery_mode = false) = 0;

        //! Close an open postintegration_writer object.
        virtual void close_writer(postintegration_writer<number>& writer) = 0;


        // WRITE TABLES FOR A DATA CONTAINER

      public:

        //! Create tables needed for a twopf container
        virtual void create_tables(integration_writer<number>& writer, twopf_task<number>* tk) = 0;

        //! Create tables needed for a threepf container
        virtual void create_tables(integration_writer<number>& writer, threepf_task<number>* tk) = 0;

        //! Create tables needed for a zeta twopf container
        virtual void create_tables(postintegration_writer<number>& writer, zeta_twopf_task<number>* tk) = 0;

        //! Create tables needed for a zeta threepf container
        virtual void create_tables(postintegration_writer<number>& writer, zeta_threepf_task<number>* tk) = 0;

        //! Create tables needed for an fNL container
        virtual void create_tables(postintegration_writer<number>& writer, fNL_task<number>* tk) = 0;


        // SEEDING

      public:

        //! Seed a writer for a twopf task
        virtual void seed_writer(integration_writer<number>& writer, twopf_task<number>* tk,
                                 const output_group_record<integration_payload>& seed) = 0;

        //! Seed a writer for a threepf task
        virtual void seed_writer(integration_writer<number>& writer, threepf_task<number>* tk,
                                 const output_group_record<integration_payload>& seed) = 0;

        //! Seed a writer for a zeta twopf task
        virtual void seed_writer(postintegration_writer<number>& writer, zeta_twopf_task<number>* tk,
                                 const output_group_record<postintegration_payload>& seed) = 0;

        //! Seed a writer for a zeta threepf task
        virtual void seed_writer(postintegration_writer<number>& writer, zeta_threepf_task<number>* tk,
                                 const output_group_record<postintegration_payload>& seed) = 0;

        //! Seed a writer for an fNL task
        virtual void seed_writer(postintegration_writer<number>& writer, fNL_task<number>* tk,
                                 const output_group_record<postintegration_payload>& seed) = 0;


        // CONSTRUCT BATCHERS

      public:

        //! Create a temporary container for twopf data. Returns a batcher which can be used for writing to the container.
        virtual twopf_batcher<number> create_temp_twopf_container(twopf_task<number>* tk, const boost::filesystem::path& tempdir,
                                                                  const boost::filesystem::path& logdir,
                                                                  unsigned int worker, unsigned int group, model<number>* m,
                                                                  std::unique_ptr<container_dispatch_function> dispatcher) = 0;

        //! Create a temporary container for threepf data. Returns a batcher which can be used for writing to the container.
        virtual threepf_batcher<number> create_temp_threepf_container(threepf_task<number>* tk, const boost::filesystem::path& tempdir,
                                                                      const boost::filesystem::path& logdir,
                                                                      unsigned int worker, unsigned int group, model<number>* m,
                                                                      std::unique_ptr<container_dispatch_function> dispatcher) = 0;

        //! Create a temporary container for zeta twopf data. Returns a batcher which can be used for writing to the container.
        virtual zeta_twopf_batcher<number> create_temp_zeta_twopf_container(const boost::filesystem::path& tempdir, const boost::filesystem::path& logdir,
                                                                            unsigned int worker, model<number>* m,
                                                                            std::unique_ptr<container_dispatch_function> dispatcher) = 0;

        //! Create a temporary container for zeta threepf data. Returns a batcher which can be used for writing to the container.
        virtual zeta_threepf_batcher<number> create_temp_zeta_threepf_container(const boost::filesystem::path& tempdir, const boost::filesystem::path& logdir,
                                                                                unsigned int worker, model<number>* m,
                                                                                std::unique_ptr<container_dispatch_function> dispatcher) = 0;

        //! Create a temporary container for fNL data. Returns a batcher which can be used for writing to the container.
        virtual fNL_batcher<number> create_temp_fNL_container(const boost::filesystem::path& tempdir, const boost::filesystem::path& logdir,
                                                              unsigned int worker, model<number>* m,
                                                              std::unique_ptr<container_dispatch_function> dispatcher, derived_data::template_type type) = 0;


        // INTEGRITY CHECK

      public:

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

        //! Synchronize missing serial numbers between an integration writer and a postintegration writer
        void synchronize_missing_serials(integration_writer<number>& i_writer, postintegration_writer<number>& p_writer,
                                         integration_task<number>* i_tk, postintegration_task<number>* p_tk);


        // -- CALCULATE MISSING SERIAL NUMBERS

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


        // -- DROP GROUPS OF SERIAL NUMBERS

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


        // -- INTEGRITY-CHECK UTILITY FUNCTIONS

      protected:

        //! log missing data from a container, checking against a list provided by the backend if one is provided.
        //! returns: serial numbers of any further configurations that should be dropped (they were in the list provided by the backend, but not already missing)
        template <typename WriterObject, typename Database>
        std::list<unsigned int> advise_missing_content(WriterObject& writer, const std::list<unsigned int>& serials, const Database& db);

        //! compute the twopf configurations which should be dropped to match a give list of threepf serials
        std::list<unsigned int> compute_twopf_drop_list(const std::list<unsigned int>& serials, const threepf_kconfig_database& configs);

        //! map a list of twopf configuration serial numbers to corresponding threepf configuration serial numbers
        std::list<unsigned int> map_twopf_to_threepf_serials(const std::list<unsigned int>& twopf_list, const threepf_kconfig_database& threepf_db);


        // DATA ACCESS VIA DATA PIPES


        // -- CREATE

      public:

        //! Create a datapipe
        virtual std::unique_ptr< datapipe<number> > create_datapipe(const boost::filesystem::path& logdir,
                                                                    const boost::filesystem::path& tempdir,
                                                                    integration_content_finder<number>& integration_finder,
                                                                    postintegration_content_finder<number>& postintegration_finder,
                                                                    datapipe_dispatch_function <number>& dispatcher,
                                                                    unsigned int worker, bool no_log = false) = 0;


        // -- ATTACH

      public:

        //! Attach an integration content group to a datapipe
        virtual std::unique_ptr< output_group_record<integration_payload> >
          datapipe_attach_integration_content(datapipe<number>* pipe, integration_content_finder<number>& finder,
                                              const std::string& name, const std::list<std::string>& tags) = 0;

        //! Attach an postintegration content group to a datapipe
        virtual std::unique_ptr< output_group_record<postintegration_payload> >
          datapipe_attach_postintegration_content(datapipe<number>* pipe, postintegration_content_finder<number>& finder,
                                                  const std::string& name, const std::list<std::string>& tags) = 0;

        //! Detach an output_group_record from a pipe
        virtual void datapipe_detach(datapipe<number>* pipe) = 0;


        // -- PULL

      public:

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


        // RAW DATA ACCESS -- NO USE OF DATAPIPE

      public:

        //! Read the worker information table


        // INTERNAL DATA

      protected:

        //! Capacity available for batchers
        unsigned int batcher_capacity;

        //! Capacity available for data cache
        unsigned int pipe_capacity;

        //! Checkpointing interval. 0 indicates that checkpointing is disabled
        unsigned int checkpoint_interval;

      };

  }   // namespace transport


#endif //CPPTRANSPORT_DATA_MANAGER_DECL_H
