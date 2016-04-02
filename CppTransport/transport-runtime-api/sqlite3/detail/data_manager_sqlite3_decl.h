//
// Created by David Seery on 26/01/2016.
// Copyright (c) 2016 University of Sussex. All rights reserved.
//

#ifndef CPPTRANSPORT_DATA_MANAGER_SQLITE3_DECL_H
#define CPPTRANSPORT_DATA_MANAGER_SQLITE3_DECL_H


namespace transport
  {

    constexpr auto default_excpt_a = CPPTRANSPORT_DATACTR_OPEN_A;
    constexpr auto default_excpt_b = CPPTRANSPORT_DATACTR_OPEN_B;

    constexpr auto CPPTRANSPORT_TEMPORARY_CONTAINER_STEM = "worker";
    constexpr auto CPPTRANSPORT_TEMPORARY_CONTAINER_XTN = ".sqlite";


    // forward declare container replacement functions
    template <typename number> class sqlite3_container_replace_twopf;
    template <typename number> class sqlite3_container_replace_threepf;
    template <typename number> class sqlite3_container_replace_zeta_twopf;
    template <typename number> class sqlite3_container_replace_zeta_threepf;
    template <typename number> class sqlite3_container_replace_fNL;


    // implements the data_manager interface using sqlite3
    template <typename number>
    class data_manager_sqlite3: public data_manager<number>
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! Create a data_manager_sqlite3 instance
        data_manager_sqlite3(unsigned int bcap, unsigned int dcap, unsigned int ckp)
          : data_manager<number>(bcap, dcap, ckp),
            temporary_container_serial(0)
          {
          }

        //! Destroy a data_manager_sqlite3 instance
        ~data_manager_sqlite3();


        // WRITER HANDLONG -- implements a 'data_manager' interface

      public:

        //! Initialize a new integration_writer object, including the data container.
        //! Never overwrites existing data; if the container already exists, an exception is thrown
        virtual void initialize_writer(integration_writer<number>& writer, bool recover_mode = false) override;

        //! Close an open integration_writer object.

        //! Any open sqlite3 handles are closed, meaning that any integration_writer objects will be invalidated.
        //! After closing, attempting to use an integration_writer will lead to unsubtle errors.
        virtual void close_writer(integration_writer<number>& writer) override;

        //! Initialize a new derived_content_writer object.
        virtual void initialize_writer(derived_content_writer<number>& writer, bool recover_mode = false) override;

        //! Close an open derived_content_writer object.

        //! Any open sqlite3 handles are closed. Attempting to use the writer after closing
        //! will lead to unsubtle errors.
        virtual void close_writer(derived_content_writer<number>& writer) override;

        //! Initialize a new postintegration_writer object.
        virtual void initialize_writer(postintegration_writer<number>& writer, bool recover_mode = false) override;

        //! Close an open postintegration_writer object
        virtual void close_writer(postintegration_writer<number>& writer) override;


        // WRITE TABLES -- implements a 'data_manager' interface

      public:

        //! Create tables needed for a twopf container
        virtual void create_tables(integration_writer<number>& writer, twopf_task<number>* tk) override;

        //! Create tables needed for a threepf container
        virtual void create_tables(integration_writer<number>& writer, threepf_task<number>* tk) override;

        //! Create tables needed for a zeta twopf container
        virtual void create_tables(postintegration_writer<number>& writer, zeta_twopf_task<number>* tk) override;

        //! Create tables needed for a zeta threepf container
        virtual void create_tables(postintegration_writer<number>& writer, zeta_threepf_task<number>* tk) override;

        //! Create tables needed for an fNL container
        virtual void create_tables(postintegration_writer<number>& writer, fNL_task<number>* tk) override;


        // SEEDING -- implements a 'data manager' interface

      public:

        //! Seed a writer for a twopf task
        virtual void seed_writer(integration_writer<number>& writer, twopf_task<number>* tk,
                                 const output_group_record<integration_payload>& seed) override;

        //! Seed a writer for a threepf task
        virtual void seed_writer(integration_writer<number>& writer, threepf_task<number>* tk,
                                 const output_group_record<integration_payload>& seed) override;

        //! Seed a writer for a zeta twopf task
        virtual void seed_writer(postintegration_writer<number>& writer, zeta_twopf_task<number>* tk,
                                 const output_group_record<postintegration_payload>& seed) override;

        //! Seed a writer for a zeta threepf task
        virtual void seed_writer(postintegration_writer<number>& writer, zeta_threepf_task<number>* tk,
                                 const output_group_record<postintegration_payload>& seed) override;

        //! Seed a writer for an fNL task
        virtual void seed_writer(postintegration_writer<number>& writer, fNL_task<number>* tk,
                                 const output_group_record<postintegration_payload>& seed) override;


        // CONSTRUCT BATCHERS  -- implements a 'data_manager' interface

      public:

        //! Create a temporary container for twopf data. Returns a batcher which can be used for writing to the container.
        virtual twopf_batcher<number> create_temp_twopf_container(twopf_task<number>* tk, const boost::filesystem::path& tempdir,
                                                                  const boost::filesystem::path& logdir,
                                                                  unsigned int worker, unsigned int group, model<number>* m,
                                                                  std::unique_ptr<container_dispatch_function> dispatcher) override;

        //! Create a temporary container for threepf data. Returns a batcher which can be used for writing to the container.
        virtual threepf_batcher<number> create_temp_threepf_container(threepf_task<number>* tk, const boost::filesystem::path& tempdir,
                                                                      const boost::filesystem::path& logdir,
                                                                      unsigned int worker, unsigned int group, model<number>* m,
                                                                      std::unique_ptr<container_dispatch_function> dispatcher) override;

        //! Create a temporary container for zeta twopf data. Returns a batcher which can be used for writing to the container.
        virtual zeta_twopf_batcher<number> create_temp_zeta_twopf_container(const boost::filesystem::path& tempdir,
                                                                            const boost::filesystem::path& logdir,
                                                                            unsigned int worker, model<number>* m,
                                                                            std::unique_ptr<container_dispatch_function> dispatcher) override;

        //! Create a temporary container for zeta threepf data. Returns a batcher which can be used for writing to the container.
        virtual zeta_threepf_batcher<number> create_temp_zeta_threepf_container(const boost::filesystem::path& tempdir,
                                                                                const boost::filesystem::path& logdir,
                                                                                unsigned int worker, model<number>* m,
                                                                                std::unique_ptr<container_dispatch_function> dispatcher) override;

        //! Create a temporary container for fNL data. Returns a batcher which can be used for writing to the container.
        virtual fNL_batcher<number> create_temp_fNL_container(const boost::filesystem::path& tempdir,
                                                              const boost::filesystem::path& logdir,
                                                              unsigned int worker, model<number>* m,
                                                              std::unique_ptr<container_dispatch_function> dispatcher,
                                                              derived_data::template_type type) override;


        // AGGREGATION HANDLERS

      protected:

        //! Aggregate a temporary twopf container into a principal container
        bool aggregate_twopf_batch(integration_writer<number>& writer, const std::string& temp_ctr);

        //! Aggregate a temporary threepf container into a principal container
        bool aggregate_threepf_batch(integration_writer<number>& writer, const std::string& temp_ctr);

        //! Aggregate a derived product
        bool aggregate_derived_product(derived_content_writer<number>& writer, const std::string& temp_name, const std::list<std::string>& used_groups);

        //! Aggregate a temporary zeta_twopf container
        bool aggregate_zeta_twopf_batch(postintegration_writer<number>& writer, const std::string& temp_ctr);

        //! Aggregate a temporary zeta_threepf container
        bool aggregate_zeta_threepf_batch(postintegration_writer<number>& writer, const std::string& temp_ctr);

        //! Aggregate a temporary fNL container
        bool aggregate_fNL_batch(postintegration_writer<number>& writer, const std::string& temp_ctr, derived_data::template_type type);


        friend class sqlite3_twopf_writer_aggregate<number>;
        friend class sqlite3_threepf_writer_aggregate<number>;
        friend class sqlite3_zeta_twopf_writer_aggregate<number>;
        friend class sqlite3_zeta_threepf_writer_aggregate<number>;
        friend class sqlite3_fNL_writer_aggregate<number>;
        friend class sqlite3_derived_content_writer_aggregate<number>;


        // INTEGRITY CHECK HANDLERS

      protected:

        // CALCULATE MISSING SERIAL NUMBERS

        //! get missing serial numbers from a twopf-re table; should be provided by implementation
        virtual std::list<unsigned int> get_missing_twopf_re_serials(integration_writer<number>& writer) override;

        //! get missing serial numbers from a twopf-im table; should be provided by implementation
        virtual std::list<unsigned int> get_missing_twopf_im_serials(integration_writer<number>& writer) override;

        //! get missing serial numbers from a threepf table; should be provided by implementation
        virtual std::list<unsigned int> get_missing_threepf_serials(integration_writer<number>& writer) override;

        //! get missing serial numbers from a zeta twopf table; should be provided by implementation
        virtual std::list<unsigned int> get_missing_zeta_twopf_serials(postintegration_writer<number>& writer) override;

        //! get missing serial numbers from a zeta threepf table; should be provided by implementation
        virtual std::list<unsigned int> get_missing_zeta_threepf_serials(postintegration_writer<number>& writer) override;

        // DROP GROUPS OF SERIAL NUMBERS

        //! drop a set of k-configurations from a twopf-re table; should be provided by implementation
        virtual void drop_twopf_re_configurations(integration_writer<number>& writer, const std::list<unsigned int>& serials, const twopf_kconfig_database& db) override;

        //! drop a set of k-configurations from a twopf-im table; should be provided by implementation
        virtual void drop_twopf_im_configurations(integration_writer<number>& writer, const std::list<unsigned int>& serials, const twopf_kconfig_database& db) override;

        //! drop a set of k-configurations from a threepf table; should be provided by implementation
        virtual void drop_threepf_configurations(integration_writer<number>& writer, const std::list<unsigned int>& serials, const threepf_kconfig_database& db) override;

        //! drop a set of k-configurations from a zeta twopf table; should be provided by implementation
        virtual void drop_zeta_twopf_configurations(postintegration_writer<number>& writer, const std::list<unsigned int>& serials, const twopf_kconfig_database& db) override;

        //! drop a set of k-configurations from a zeta threepf table; should be provided by implementation
        virtual void drop_zeta_threepf_configurations(postintegration_writer<number>& writer, const std::list<unsigned int>& serials, const threepf_kconfig_database& db) override;

        //! drop statistics for a set of k-configurations
        virtual void drop_statistics_configurations(integration_writer<number>& writer, const std::list<unsigned int>& serials, const twopf_kconfig_database& db) override;
        virtual void drop_statistics_configurations(integration_writer<number>& writer, const std::list<unsigned int>& serials, const threepf_kconfig_database& db) override;

        //! drop initial conditions for a sset of k-configurations
        virtual void drop_initial_conditions_configurations(integration_writer<number>& writer, const std::list<unsigned int>& serials, const twopf_kconfig_database& db) override;
        virtual void drop_initial_conditions_configurations(integration_writer<number>& writer, const std::list<unsigned int>& serials, const threepf_kconfig_database& db) override;


        friend class sqlite3_twopf_writer_integrity<number>;
        friend class sqlite3_threepf_writer_integrity<number>;
        friend class sqlite3_zeta_twopf_writer_integrity<number>;
        friend class sqlite3_zeta_threepf_writer_integrity<number>;
        friend class sqlite3_fNL_writer_integrity<number>;


        // DATA PIPES -- implements a 'data_manager' interface

        // CREATE

      public:

        //! Create a new datapipe
        virtual std::unique_ptr< datapipe<number> > create_datapipe(const boost::filesystem::path& logdir,
                                                                    const boost::filesystem::path& tempdir,
                                                                    integration_content_finder<number>& integration_finder,
                                                                    postintegration_content_finder<number>& postintegration_finder,
                                                                    datapipe_dispatch_function<number>& dispatcher,
                                                                    unsigned int worker, bool no_log = false) override;

        // ATTACH

      public:

        //! Attach an integration content group to a datapipe
        std::unique_ptr< output_group_record<integration_payload> >
          datapipe_attach_integration_content(datapipe<number>* pipe, integration_content_finder<number>& finder,
                                              const std::string& name, const std::list<std::string>& tags) override;

        //! Attach an postintegration content group to a datapipe
        std::unique_ptr< output_group_record<postintegration_payload> >
          datapipe_attach_postintegration_content(datapipe<number>* pipe, postintegration_content_finder<number>& finder,
                                                  const std::string& name, const std::list<std::string>& tags) override;

        //! Detach an output_group_record from a pipe
        void datapipe_detach(datapipe<number>* pipe) override;

        // PULL

      public:

        //! Pull a set of time sample-points from a datapipe
        virtual void pull_time_config(datapipe<number>* pipe, const derived_data::SQL_time_config_query& tquery, std::vector<time_config>& sample) override;

        //! Pull a set of 2pf k-configuration serial numbers from a datapipe
        void pull_kconfig_twopf(datapipe<number>* pipe, const derived_data::SQL_twopf_kconfig_query& kquery, std::vector<twopf_kconfig>& sample) override;

        //! Pull a set of 3pd k-configuration serial numbesr from a datapipe
        //! Simultaneously, populates three lists (k1, k2, k3) with serial numbers for the 2pf k-configurations
        //! corresponding to k1, k2, k3
        void pull_kconfig_threepf(datapipe<number>* pipe, const derived_data::SQL_threepf_kconfig_query& query, std::vector<threepf_kconfig>& sample) override;

        //! Pull a time sample of a background field from a datapipe
        virtual void pull_background_time_sample(datapipe<number>* pipe, unsigned int id, const derived_data::SQL_query& query, std::vector<number>& sample) override;

        //! Pull a time sample of a twopf component at fixed k-configuration from a datapipe
        virtual void pull_twopf_time_sample(datapipe<number>* pipe, unsigned int id, const derived_data::SQL_query& query,
                                            unsigned int k_serial, std::vector<number>& sample, twopf_type type) override;

        //! Pull a sample of a threepf at fixed k-configuration from a datapipe
        virtual void pull_threepf_time_sample(datapipe<number>* pipe, unsigned int id, const derived_data::SQL_query& query,
                                              unsigned int k_serial, std::vector<number>& sample, threepf_type type) override;

        //! Pull a sample of a tensor twopf component at fixed k-configuration from a datapipe
        virtual void pull_tensor_twopf_time_sample(datapipe<number>* pipe, unsigned int id, const derived_data::SQL_query& query,
                                                   unsigned int k_serial, std::vector<number>& sample) override;

        //! Pull a sample of the zeta twopf at fixed k-configuration from a datapipe
        virtual void pull_zeta_twopf_time_sample(datapipe<number>* pipe, const derived_data::SQL_query& query,
                                                 unsigned int k_serial, std::vector<number>& sample) override;

        //! Pull a sample of the zeta threepf at fixed k-configuration from a datapipe
        virtual void pull_zeta_threepf_time_sample(datapipe<number>* pipe, const derived_data::SQL_query& query,
                                                   unsigned int k_serial, std::vector<number>& sample) override;

        //! Pull a sample of the zeta reduced bispectrum at fixed k-configuration from a datapipe
        virtual void pull_zeta_redbsp_time_sample(datapipe<number>* pipe, const derived_data::SQL_query& query,
                                                  unsigned int k_serial, std::vector<number>& sample) override;

        //! Pull a sample of fNL from a datapipe
        virtual void pull_fNL_time_sample(datapipe<number>* pipe, const derived_data::SQL_query& query,
                                          std::vector<number>& sample, derived_data::template_type type) override;

        //! Pull a sample of bispectrum.template from a datapipe
        virtual void pull_BT_time_sample(datapipe<number>* pipe, const derived_data::SQL_query& query,
                                         std::vector<number>& sample, derived_data::template_type type) override;

        //! Pull a sample of template.template from a datapipe
        virtual void pull_TT_time_sample(datapipe<number>* pipe, const derived_data::SQL_query& query,
                                         std::vector<number>& sample, derived_data::template_type type) override;

        //! Pull a kconfig sample of a twopf component at fixed time from a datapipe
        virtual void pull_twopf_kconfig_sample(datapipe<number>* pipe, unsigned int id, const derived_data::SQL_query& query,
                                               unsigned int t_serial, std::vector<number>& sample, twopf_type type) override;

        //! Pull a kconfig sample of a threepf at fixed time from a datapipe
        virtual void pull_threepf_kconfig_sample(datapipe<number>* pipe, unsigned int id,
                                                 const derived_data::SQL_query& query,
                                                 unsigned int t_serial, std::vector<number>& sample, threepf_type type) override;

        //! Pull a kconfig sample of a tensor twopf component at fixed time from a datapipe
        virtual void pull_tensor_twopf_kconfig_sample(datapipe<number>* pipe, unsigned int id, const derived_data::SQL_query& query,
                                                      unsigned int t_serial, std::vector<number>& sample) override;

        //! Pull a kconfig sample of the zeta twopf at fixed time from a datapipe
        virtual void pull_zeta_twopf_kconfig_sample(datapipe<number>* pipe, const derived_data::SQL_query& query,
                                                    unsigned int t_serial, std::vector<number>& sample) override;

        //! Pull a kconfig sample of the zeta threepf at fixed time from a datapipe
        virtual void pull_zeta_threepf_kconfig_sample(datapipe<number>* pipe, const derived_data::SQL_query& query,
                                                      unsigned int t_serial, std::vector<number>& sample) override;

        //! Pull a kconfig sample of the zeta reduced bispectrum at fixed time from a datapipe
        virtual void pull_zeta_redbsp_kconfig_sample(datapipe<number>* pipe, const derived_data::SQL_query& query,
                                                     unsigned int t_serial, std::vector<number>& sample) override;

        //! Pull a sample of k-configuration statistics from a datapipe
        virtual void pull_k_statistics_sample(datapipe<number>* pipe, const derived_data::SQL_query& query,
                                              std::vector<kconfiguration_statistics>& data) override;

      protected:

        //! Attach a SQLite database to a datapipe
        void datapipe_attach_container(datapipe<number>* pipe, const boost::filesystem::path& ctr_path);


        // RAW DATA ACCESS -- DOESN'T REQUIRE USE OF DATAPIPE

      public:

        //! Read the worker information table for a container
        virtual worker_information_db read_worker_information(const boost::filesystem::path& ctr_path) override;


        // INTERNAL UTILITY FUNCTIONS

      protected:

        //! Replace a temporary twopf container with a new one
        void replace_temp_twopf_container(const boost::filesystem::path& tempdir, unsigned int worker, model<number>* m,
                                          bool ics, generic_batcher& batcher, replacement_action action);

        //! Replace a temporary threepf container with a new one
        void replace_temp_threepf_container(const boost::filesystem::path& tempdir, unsigned int worker, model<number>* m,
                                            bool ics, generic_batcher& batcher, replacement_action action);

        //! Replace a temporary zeta twopf container with a new one
        void replace_temp_zeta_twopf_container(const boost::filesystem::path& tempdir, unsigned int worker, model<number>* m,
                                               generic_batcher& batcher, replacement_action action);

        //! Replace a temporary zeta threepf container with a new one
        void replace_temp_zeta_threepf_container(const boost::filesystem::path& tempdir, unsigned int worker, model<number>* m,
                                                 generic_batcher& batcher, replacement_action action);

        //! Replace a temporary fNL container with a new one
        void replace_temp_fNL_container(const boost::filesystem::path& tempdir, unsigned int worker, derived_data::template_type type,
                                        generic_batcher& batcher, replacement_action action);

        //! Generate the name for a temporary container
        boost::filesystem::path generate_temporary_container_path(const boost::filesystem::path& tempdir, unsigned int worker);


        friend class sqlite3_container_replace_twopf<number>;
        friend class sqlite3_container_replace_threepf<number>;
        friend class sqlite3_container_replace_zeta_twopf<number>;
        friend class sqlite3_container_replace_zeta_threepf<number>;
        friend class sqlite3_container_replace_fNL<number>;


        // INTERNAL DATA

      private:

        //! List of open sqlite3 connexions
        std::list< sqlite3* > open_containers;

        //! Serial number used to distinguish
        //! the set of temporary containers associated with this worker.
        //! Begins at zero and is incremented as temporary containers are generated.
        unsigned int          temporary_container_serial;

      };

  }   // namespace transport


#endif //CPPTRANSPORT_DATA_MANAGER_SQLITE3_DECL_H
