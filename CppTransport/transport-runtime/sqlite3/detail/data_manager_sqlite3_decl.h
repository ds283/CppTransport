//
// Created by David Seery on 26/01/2016.
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
        data_manager_sqlite3(local_environment& e, argument_cache& a)
          : data_manager<number>(e, a),
            temporary_container_serial(0)
          {
          }

        //! Destroy a data_manager_sqlite3 instance
        ~data_manager_sqlite3();


        // TRANSACTIONS

      public:

        //! Generate transaction management object
        transaction_manager transaction_factory(integration_writer<number>& writer) final;

        //! Generate transaction management object
        transaction_manager transaction_factory(postintegration_writer<number>& writer) final;

      protected:

        //! Internal implementation of transaction factory
        template <typename WriterObject>
        transaction_manager internal_transaction_factory(WriterObject& writer);

        //! Make a transaction on a raw SQLite handle
        transaction_manager transaction_factory(sqlite3* db, boost::filesystem::path lockfile);

        //! Begin a transaction on the database.
        void begin_transaction(sqlite3* db);

        //! Commit a transaction to the database.
        void commit_transaction(sqlite3* db);

        //! Rollback database to beginning of last transaction
        void abort_transaction(sqlite3* db);

        //! Release a transaction on the database
        void release_transaction();


        friend class data_manager_sqlite3_transaction_handler<number>;


        // WRITER HANDLING -- implements a 'data_manager' interface

      public:

        //! Initialize a new integration_writer object, including the data container.
        //! Never overwrites existing data; if the container already exists, an exception is thrown
        void initialize_writer(integration_writer<number>& writer, bool recover_mode=false) override;

        //! Close an open integration_writer object.

        //! Any open sqlite3 handles are closed, meaning that any integration_writer objects will be invalidated.
        //! After closing, attempting to use an integration_writer will lead to unsubtle errors.
        void close_writer(integration_writer<number>& writer) override;

        //! Initialize a new derived_content_writer object.
        void initialize_writer(derived_content_writer<number>& writer, bool recover_mode=false) override;

        //! Close an open derived_content_writer object.

        //! Any open sqlite3 handles are closed. Attempting to use the writer after closing
        //! will lead to unsubtle errors.
        void close_writer(derived_content_writer<number>& writer) override;

        //! Initialize a new postintegration_writer object.
        //! Note: does not set flags for spectral data content, which may or may not be present depending
        //! on the integration payload that is sourcing this writer
        void initialize_writer(postintegration_writer<number>& writer, bool recover_mode=false) override;

        //! Close an open postintegration_writer object
        void close_writer(postintegration_writer<number>& writer) override;

        //! Close a paired integration_writer and postintegration_writer set
        void close_writer(integration_writer<number>& i_writer, postintegration_writer<number>& p_writer) override;

      protected:

        //! Internal method to close the SQLite container associated with a handle
        template <typename WriterObject>
        void close_writer_handle(WriterObject& writer);


        // WRITE TABLES -- implements a 'data_manager' interface

      public:

        //! Create tables needed for a twopf container
        void create_tables(integration_writer<number>& writer, twopf_task<number>* tk) override;

        //! Create tables needed for a threepf container
        void create_tables(integration_writer<number>& writer, threepf_task<number>* tk) override;

        //! Create tables needed for a zeta twopf container
        void create_tables(postintegration_writer<number>& writer, zeta_twopf_task<number>* tk) override;

        //! Create tables needed for a zeta threepf container
        void create_tables(postintegration_writer<number>& writer, zeta_threepf_task<number>* tk) override;

        //! Create tables needed for an fNL container
        void create_tables(postintegration_writer<number>& writer, fNL_task<number>* tk) override;


        // SEEDING -- implements a 'data manager' interface

      public:

        //! Seed a writer for a twopf task
        void seed_writer(integration_writer <number>& writer, twopf_task <number>* tk,
                         const content_group_record <integration_payload>& seed) override;

        //! Seed a writer for a threepf task
        void seed_writer(integration_writer <number>& writer, threepf_task <number>* tk,
                         const content_group_record <integration_payload>& seed) override;

        //! Seed a writer for a zeta twopf task
        void seed_writer(postintegration_writer <number>& writer, zeta_twopf_task <number>* tk,
                         const content_group_record <postintegration_payload>& seed) override;

        //! Seed a writer for a zeta threepf task
        void seed_writer(postintegration_writer <number>& writer, zeta_threepf_task <number>* tk,
                         const content_group_record <postintegration_payload>& seed) override;

        //! Seed a writer for an fNL task
        void seed_writer(postintegration_writer <number>& writer, fNL_task <number>* tk,
                         const content_group_record <postintegration_payload>& seed) override;


        // CONSTRUCT BATCHERS  -- implements a 'data_manager' interface

      public:

        //! Create a temporary container for twopf data. Returns a batcher which can be used for writing to the container.
        twopf_batcher<number>
        create_temp_twopf_container(twopf_task<number>* tk, const boost::filesystem::path& tempdir,
                                    const boost::filesystem::path& logdir,
                                    unsigned int worker, unsigned int group, model<number>* m,
                                    std::unique_ptr<container_dispatch_function> dispatcher) override;

        //! Create a temporary container for threepf data. Returns a batcher which can be used for writing to the container.
        threepf_batcher<number>
        create_temp_threepf_container(threepf_task<number>* tk, const boost::filesystem::path& tempdir,
                                      const boost::filesystem::path& logdir,
                                      unsigned int worker, unsigned int group, model<number>* m,
                                      std::unique_ptr<container_dispatch_function> dispatcher) override;

        //! Create a temporary container for zeta twopf data. Returns a batcher which can be used for writing to the container.
        zeta_twopf_batcher<number>
        create_temp_zeta_twopf_container(zeta_twopf_task<number>* tk, const boost::filesystem::path& tempdir,
                                         const boost::filesystem::path& logdir, unsigned int worker, model<number>* m,
                                         std::unique_ptr<container_dispatch_function> dispatcher) override;

        //! Create a temporary container for zeta threepf data. Returns a batcher which can be used for writing to the container.
        zeta_threepf_batcher<number>
        create_temp_zeta_threepf_container(zeta_threepf_task<number>* tk, const boost::filesystem::path& tempdir,
                                           const boost::filesystem::path& logdir, unsigned int worker, model<number>* m,
                                           std::unique_ptr<container_dispatch_function> dispatcher) override;

        //! Create a temporary container for fNL data. Returns a batcher which can be used for writing to the container.
        fNL_batcher<number>
        create_temp_fNL_container(fNL_task<number>* tk, const boost::filesystem::path& tempdir,
                                  const boost::filesystem::path& logdir, unsigned int worker, model<number>* m,
                                  std::unique_ptr<container_dispatch_function> dispatcher,
                                  derived_data::bispectrum_template type) override;

      protected:

        //! Create a SQLite container
        sqlite3* make_temp_container(const boost::filesystem::path& container);

        //! make tables for a temporary 2pf container
        void make_temp_twopf_tables(transaction_manager& mgr, sqlite3* db, unsigned int Nfields, bool statistics, bool ics);

        //! make tables for a temporary 3pf container
        void make_temp_threepf_tables(transaction_manager& mgr, sqlite3* db, unsigned int Nfields, bool statistics, bool ics);

        //! make tables for a temporary zeta 2pf container
        void make_temp_zeta_twopf_tables(transaction_manager& mgr, sqlite3* db, unsigned int Nfields);

        //! make tables for a temporary zeta 3pf container
        void make_temp_zeta_threepf_tables(transaction_manager& mgr, sqlite3* db, unsigned int Nfields);

        //! make tables for a temporary fNL container
        void make_temp_fNL_tables(transaction_manager& mgr, sqlite3* db, derived_data::bispectrum_template type);


        // AGGREGATION HANDLERS

      protected:

        //! Aggregate a temporary twopf container into a principal container
        bool aggregate_twopf_batch(integration_writer<number>& writer, const boost::filesystem::path& temp_ctr);

        //! Aggregate a temporary threepf container into a principal container
        bool aggregate_threepf_batch(integration_writer<number>& writer, const boost::filesystem::path& temp_ctr);

        //! Aggregate a derived product
        bool aggregate_derived_product(derived_content_writer<number>& writer, const std::string& temp_name, const content_group_name_set& used_groups);

        //! Aggregate a temporary zeta_twopf container
        bool aggregate_zeta_twopf_batch(postintegration_writer<number>& writer, const boost::filesystem::path& temp_ctr);

        //! Aggregate a temporary zeta_threepf container
        bool aggregate_zeta_threepf_batch(postintegration_writer<number>& writer, const boost::filesystem::path& temp_ctr);

        //! Aggregate a temporary fNL container
        bool aggregate_fNL_batch(postintegration_writer<number>& writer, const boost::filesystem::path& temp_ctr, derived_data::bispectrum_template type);


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
        serial_number_list get_missing_twopf_re_serials(integration_writer<number>& writer) override;

        //! get missing serial numbers from a twopf-im table; should be provided by implementation
        serial_number_list get_missing_twopf_im_serials(integration_writer<number>& writer) override;

        //! get missing serial numbers from a twopf-re spectral index table; should be provided by implementation
        serial_number_list get_missing_twopf_si_re_serials(integration_writer<number>& writer) override;

        //! get missing serial numbers from a tensor twopf table; should be provided by implementation
        serial_number_list get_missing_tensor_twopf_serials(integration_writer<number>& writer) override;

        //! get missing serial numbers from a tensor twopf spectral index table; should be provided by implementation
        serial_number_list get_missing_tensor_twopf_si_serials(integration_writer<number>& writer) override;

        //! get missing serial numbers from a threepf-momentum table; should be provided by implementation
        serial_number_list get_missing_threepf_momentum_serials(integration_writer <number>& writer) override;

        //! get missing serial numbers from a threepf-deriv table; should be provided by implementation
        serial_number_list get_missing_threepf_deriv_serials(integration_writer <number>& writer) override;

        //! get missing serial numbers from a zeta twopf table; should be provided by implementation
        serial_number_list get_missing_zeta_twopf_serials(postintegration_writer<number>& writer) override;

        //! get missing serial numbers from a zeta threepf table; should be provided by implementation
        serial_number_list get_missing_zeta_threepf_serials(postintegration_writer<number>& writer) override;

        //! get missing numbers from a 1st-order gauge xfm table; should be provided by implementation
        serial_number_list get_missing_gauge_xfm1_serials(postintegration_writer<number>& writer) override;

        //! get missing numbers from a 2nd-order gauge xfm 123-order table; should be provided by implementation
        serial_number_list get_missing_gauge_xfm2_123_serials(postintegration_writer<number>& writer) override;

        //! get missing numbers from a 2nd-order gauge xfm 213-order table; should be provided by implementation
        serial_number_list get_missing_gauge_xfm2_213_serials(postintegration_writer<number>& writer) override;

        //! get missing numbers from a 2nd-order gauge xfm 312-order table; should be provided by implementation
        serial_number_list get_missing_gauge_xfm2_312_serials(postintegration_writer<number>& writer) override;


        // DROP GROUPS OF SERIAL NUMBERS

        //! drop a set of k-configurations from a twopf-re table; should be provided by implementation
        void drop_twopf_re_configurations(transaction_manager& mgr, integration_writer <number>& writer,
                                          const serial_number_list& serials, const serial_number_list& missing,
                                          const twopf_kconfig_database& dbase) override;

        //! drop a set of k-configurations from a twopf-im table; should be provided by implementation
        void drop_twopf_im_configurations(transaction_manager& mgr, integration_writer <number>& writer,
                                          const serial_number_list& serials, const serial_number_list& missing,
                                          const twopf_kconfig_database& dbase) override;

        //! drop a set of k-configurations from a twopf-re spectral index table; should be provided by implementation
        void drop_twopf_si_re_configurations(transaction_manager& mgr, integration_writer <number>& writer,
                                             const serial_number_list& serials, const serial_number_list& missing,
                                             const twopf_kconfig_database& dbase) override;

        //! drop a set of k-configurations from a tensor twopf table; should be provided by implementation
        void drop_tensor_twopf_configurations(transaction_manager& mgr, integration_writer <number>& writer,
                                              const serial_number_list& serials,
                                              const serial_number_list& missing,
                                              const twopf_kconfig_database& dbase) override;

        //! drop a set of k-configurations from a tensor twopf spectral index table; should be provided by implementation
        void drop_tensor_twopf_si_configurations(transaction_manager& mgr, integration_writer <number>& writer,
                                                 const serial_number_list& serials,
                                                 const serial_number_list& missing,
                                                 const twopf_kconfig_database& dbase) override;

        //! drop a set of k-configurations from a threepf-momentum table; should be provided by implementation
        void drop_threepf_momentum_configurations(transaction_manager& mgr, integration_writer <number>& writer,
                                                  const serial_number_list& serials,
                                                  const serial_number_list& missing,
                                                  const threepf_kconfig_database& dbase) override;

        //! drop a set of k-configurations from a threepf-deriv table; should be provided by implementation
        void drop_threepf_deriv_configurations(transaction_manager& mgr, integration_writer <number>& writer,
                                               const serial_number_list& serials,
                                               const serial_number_list& missing,
                                               const threepf_kconfig_database& dbase) override;

        //! drop a set of k-configurations from a zeta twopf table; should be provided by implementation
        void drop_zeta_twopf_configurations(transaction_manager& mgr, postintegration_writer <number>& writer,
                                            const serial_number_list& serials,
                                            const serial_number_list& missing,
                                            const twopf_kconfig_database& dbase) override;

        //! drop a set of k-configurations from a zeta threepf table; should be provided by implementation
        void drop_zeta_threepf_configurations(transaction_manager& mgr, postintegration_writer <number>& writer,
                                              const serial_number_list& serials,
                                              const serial_number_list& missing,
                                              const threepf_kconfig_database& dbase) override;

        //! drop a set of k-configurations from a 1st-order gauge xfm table; should be provided by implementation
        void drop_gauge_xfm1_configurations(transaction_manager& mgr, postintegration_writer <number>& writer,
                                            const serial_number_list& serials,
                                            const serial_number_list& missing,
                                            const twopf_kconfig_database& dbase) override;

        //! drop a set of k-configurations from a 2nd-order gauge xfm 123-order table; should be provided by implementation
        void drop_gauge_xfm2_123_configurations(transaction_manager& mgr, postintegration_writer <number>& writer,
                                                const serial_number_list& serials,
                                                const serial_number_list& missing,
                                                const threepf_kconfig_database& dbase) override;

        //! drop a set of k-configurations from a 2nd-order gauge xfm 213-order table; should be provided by implementation
        void drop_gauge_xfm2_213_configurations(transaction_manager& mgr, postintegration_writer <number>& writer,
                                                const serial_number_list& serials,
                                                const serial_number_list& missing,
                                                const threepf_kconfig_database& dbase) override;

        //! drop a set of k-configurations from a 2nd-order gauge xfm 312-order table; should be provided by implementation
        void drop_gauge_xfm2_312_configurations(transaction_manager& mgr, postintegration_writer <number>& writer,
                                                const serial_number_list& serials,
                                                const serial_number_list& missing,
                                                const threepf_kconfig_database& dbase) override;

        //! drop statistics for a set of k-configurations
        void drop_statistics_configurations(transaction_manager& mgr, integration_writer <number>& writer,
                                            const serial_number_list& serials,
                                            const twopf_kconfig_database& dbase) override;

        void drop_statistics_configurations(transaction_manager& mgr, integration_writer <number>& writer,
                                            const serial_number_list& serials,
                                            const threepf_kconfig_database& dbase) override;

        //! drop initial conditions for a set of k-configurations
        void drop_initial_conditions_configurations(transaction_manager& mgr, integration_writer <number>& writer,
                                                    const serial_number_list& serials,
                                                    const twopf_kconfig_database& dbase) override;

        void drop_initial_conditions_configurations(transaction_manager& mgr, integration_writer <number>& writer,
                                                    const serial_number_list& serials,
                                                    const threepf_kconfig_database& dbase) override;


        friend class sqlite3_twopf_writer_integrity<number>;
        friend class sqlite3_threepf_writer_integrity<number>;
        friend class sqlite3_zeta_twopf_writer_integrity<number>;
        friend class sqlite3_zeta_threepf_writer_integrity<number>;
        friend class sqlite3_fNL_writer_integrity<number>;


        // FINALIZATION HANDLERS

      protected:

        //! finalize twopf writer
        void finalize_twopf_writer(integration_writer<number>& writer);

        //! finalize threepf writer
        void finalize_threepf_writer(integration_writer<number>& writer);

        //! finalize zeta twopf writer
        void finalize_zeta_twopf_writer(postintegration_writer<number>& writer);

        //! finalize zeta threepf writer
        void finalize_zeta_threepf_writer(postintegration_writer<number>& writer);

        //! finalize fNL writer
        void finalize_fNL_writer(postintegration_writer<number>& writer);


        friend class sqlite3_twopf_writer_finalize<number>;
        friend class sqlite3_threepf_writer_finalize<number>;
        friend class sqlite3_zeta_twopf_writer_finalize<number>;
        friend class sqlite3_zeta_threepf_writer_finalize<number>;
        friend class sqlite3_fNL_writer_finalize<number>;


        // DATA PIPES -- implements a 'data_manager' interface

        // CREATE

      public:

        //! Create a new datapipe
        std::unique_ptr< datapipe<number> >
          create_datapipe(const boost::filesystem::path& logdir, const boost::filesystem::path& tempdir,
                          integration_content_finder<number>& integration_finder,
                          postintegration_content_finder<number>& postintegration_finder,
                          datapipe_dispatch_function<number>& dispatcher,
                          unsigned int worker, bool no_log = false) override;

        // ATTACH

      public:

        //! Attach an integration content group to a datapipe
        std::unique_ptr< content_group_record<integration_payload> >
        datapipe_attach_integration_content(datapipe<number>& pipe, integration_content_finder<number>& finder,
                                            const std::string& name, const tag_list& tags) override;

        //! Attach an postintegration content group to a datapipe
        std::unique_ptr< content_group_record<postintegration_payload> >
        datapipe_attach_postintegration_content(datapipe<number>& pipe, postintegration_content_finder<number>& finder,
                                                const std::string& name, const tag_list& tags) override;

        //! Detach a content_group_record from a pipe
        void datapipe_detach(datapipe<number>* pipe) override;

        // PULL

      public:

        //! Pull a set of time sample-points from a datapipe
        void pull_time_config(datapipe <number>* pipe, const derived_data::SQL_time_query& tquery,
                              std::vector<time_config>& sample) override;

        //! Pull a set of 2pf k-configuration serial numbers from a datapipe
        void pull_kconfig_twopf(datapipe <number>* pipe, const derived_data::SQL_twopf_query& kquery,
                                std::vector<twopf_kconfig>& sample) override;

        //! Pull a set of 3pd k-configuration serial numbesr from a datapipe
        //! Simultaneously, populates three lists (k1, k2, k3) with serial numbers for the 2pf k-configurations
        //! corresponding to k1, k2, k3
        void pull_kconfig_threepf(datapipe <number>* pipe, const derived_data::SQL_threepf_query& query,
                                  std::vector<threepf_kconfig>& sample) override;

        //! Pull a time sample of a background field from a datapipe
        void pull_background_time_sample(datapipe <number>* pipe, unsigned int id, const derived_data::SQL_query& query,
                                         std::vector<number>& sample) override;

        //! Pull a time sample of a twopf component at fixed k-configuration from a datapipe
        void pull_twopf_time_sample(datapipe <number>* pipe, unsigned int id, const derived_data::SQL_query& query,
                                    unsigned int k_serial, std::vector<number>& sample, twopf_type type) override;

        //! Pull a time sample of a twopf 'spectral index' component at fixed k-configuration from a datapipe
        void pull_twopf_si_time_sample(datapipe <number>* pipe, unsigned int id, const derived_data::SQL_query& query,
                                       unsigned int k_serial, std::vector<number>& sample) override;

        //! Pull a sample of a threepf component at fixed k-configuration from a datapipe
        void pull_threepf_time_sample(datapipe <number>* pipe, unsigned int id, const derived_data::SQL_query& query,
                                      unsigned int k_serial, std::vector<number>& sample, threepf_type type) override;

        //! Pull a sample of a tensor twopf component at fixed k-configuration from a datapipe
        void pull_tensor_twopf_time_sample(datapipe <number>* pipe, unsigned int id, const derived_data::SQL_query& query,
                                           unsigned int k_serial, std::vector<number>& sample) override;

        //! Pull a sample of a tensor twopf 'spectral index' component at fixed k-configuration from a datapipe
        void pull_tensor_twopf_si_time_sample(datapipe <number>* pipe, unsigned int id, const derived_data::SQL_query& query,
                                              unsigned int k_serial, std::vector<number>& sample) override;

        //! Pull a sample of the zeta twopf at fixed k-configuration from a datapipe
        void pull_zeta_twopf_time_sample(datapipe <number>* pipe, const derived_data::SQL_query& query,
                                         unsigned int k_serial, std::vector<number>& sample) override;

        //! Pull a sample of the zeta threepf at fixed k-configuration from a datapipe
        void pull_zeta_threepf_time_sample(datapipe <number>* pipe, const derived_data::SQL_query& query,
                                           unsigned int k_serial, std::vector<number>& sample) override;

        //! Pull a sample of the zeta reduced bispectrum at fixed k-configuration from a datapipe
        void pull_zeta_redbsp_time_sample(datapipe <number>* pipe, const derived_data::SQL_query& query,
                                          unsigned int k_serial, std::vector<number>& sample) override;

        //! Pull a sample of fNL from a datapipe
        void pull_fNL_time_sample(datapipe <number>* pipe, const derived_data::SQL_query& query,
                                  std::vector<number>& sample, derived_data::bispectrum_template type) override;

        //! Pull a sample of bispectrum.template from a datapipe
        void pull_BT_time_sample(datapipe <number>* pipe, const derived_data::SQL_query& query,
                                 std::vector<number>& sample, derived_data::bispectrum_template type) override;

        //! Pull a sample of template.template from a datapipe
        void pull_TT_time_sample(datapipe <number>* pipe, const derived_data::SQL_query& query,
                                 std::vector<number>& sample, derived_data::bispectrum_template type) override;

        //! Pull a kconfig sample of a twopf component at fixed time from a datapipe
        void pull_twopf_kconfig_sample(datapipe <number>* pipe, unsigned int id, const derived_data::SQL_query& query,
                                       unsigned int t_serial, std::vector<number>& sample, twopf_type type) override;

        //! Pull a kconfig sample of a twopf 'spectral index' component at fixed time from a datapipe
        void pull_twopf_si_kconfig_sample(datapipe <number>* pipe, unsigned int id, const derived_data::SQL_query& query,
                                          unsigned int t_serial, std::vector<number>& sample) override;

        //! Pull a kconfig sample of a threepf at fixed time from a datapipe
        void pull_threepf_kconfig_sample(datapipe <number>* pipe, unsigned int id,
                                         const derived_data::SQL_query& query,
                                         unsigned int t_serial, std::vector<number>& sample,
                                         threepf_type type) override;

        //! Pull a kconfig sample of a tensor twopf component at fixed time from a datapipe
        void pull_tensor_twopf_kconfig_sample(datapipe <number>* pipe, unsigned int id,
                                              const derived_data::SQL_query& query,
                                              unsigned int t_serial, std::vector<number>& sample) override;

        //! Pull a kconfig sample of a tensor twopf 'spectral index' component at fixed time from a datapipe
        void pull_tensor_twopf_si_kconfig_sample(datapipe <number>* pipe, unsigned int id, const derived_data::SQL_query& query,
                                                 unsigned int t_serial, std::vector<number>& sample) override;

        //! Pull a kconfig sample of the zeta twopf at fixed time from a datapipe
        void pull_zeta_twopf_kconfig_sample(datapipe <number>* pipe, const derived_data::SQL_query& query,
                                            unsigned int t_serial, std::vector<number>& sample) override;

        //! Pull a kconfig sample of the zeta threepf at fixed time from a datapipe
        void pull_zeta_threepf_kconfig_sample(datapipe <number>* pipe, const derived_data::SQL_query& query,
                                              unsigned int t_serial, std::vector<number>& sample) override;

        //! Pull a kconfig sample of the zeta reduced bispectrum at fixed time from a datapipe
        void pull_zeta_redbsp_kconfig_sample(datapipe <number>* pipe, const derived_data::SQL_query& query,
                                             unsigned int t_serial, std::vector<number>& sample) override;

        //! Pull a sample of k-configuration statistics from a datapipe
        void pull_k_statistics_sample(datapipe <number>* pipe, const derived_data::SQL_query& query,
                                      std::vector<kconfiguration_statistics>& data) override;

      protected:

        //! Attach a SQLite database to a datapipe
        void datapipe_attach_container(datapipe <number>& pipe, const boost::filesystem::path& ctr_path);


        // RAW DATA ACCESS -- DOESN'T REQUIRE USE OF DATAPIPE

      public:

        //! Read the worker information table for a container
        worker_information_db read_worker_information(const boost::filesystem::path& ctr_path) override;

        //! Read the timing table for a container (supposing one to be present)
        timing_db read_timing_information(const boost::filesystem::path& ctr_path) override;

      protected:

        //! utility function to open a SQLite3 container
        sqlite3* open_container(const boost::filesystem::path& ctr_path);

        //! utility function to close a SQLite3 container
        void close_container(sqlite3* db);


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
        void replace_temp_fNL_container(const boost::filesystem::path& tempdir, unsigned int worker, derived_data::bispectrum_template type,
                                        generic_batcher& batcher, replacement_action action);

        //! Generate the name for a temporary container
        boost::filesystem::path generate_temporary_container_path(const boost::filesystem::path& tempdir, unsigned int worker);

        //! Generate the name for a lockfile
        boost::filesystem::path generate_lockfile_path(const boost::filesystem::path& tempdir, unsigned int worker);


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
