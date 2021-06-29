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

#ifndef CPPTRANSPORT_DATA_MANAGER_DECL_H
#define CPPTRANSPORT_DATA_MANAGER_DECL_H


namespace transport
  {

    //! leafname for repository lockfile
    constexpr auto CPPTRANSPORT_DATAMGR_LOCKFILE_LEAF = "lockfile";

    template <typename number>
    class data_manager
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! Create a data_manager instance with a nominated capacity per batcher
        data_manager(local_environment& e, argument_cache& a)
          : env(e),
            args(a),
            transactions(0)
          {
          }

        //! Explicitly delete copy constructor, preventing duplication (data_manager instances should be unique,
        //! in order to prevent corruption due to overlapping writes)
        data_manager(const data_manager<number>& obj) = delete;

        //! Destroy the data_manager instance. In practice this would always be delegated to an implementation class
        virtual ~data_manager() = default;


        // CAPACITY ADMIN

      public:

        //! Return the maximum memory available for batchers on this worker
        //! note, argument_cache stores the capacity in bytes so no conversion is needed
        size_t get_batcher_capacity() const { return this->args.get_batcher_capacity(); }

        //! Return the maximum memory available for data cache on this worker
        //! note, argument_cache stores the capacity in bytes so no conversion is needed
        size_t get_pipe_capacity() const { return this->args.get_datapipe_capacity(); }


        // CHECKPOINTING ADMIN

      public:

        //! Return checkpointing interval, measured in seconds
        //! note, argument_cache stores the checkpoint interval in seconds so no conversion is needed
        unsigned int get_checkpoint_interval() const { if(this->local_checkpoint) return *this->local_checkpoint; else return this->args.get_checkpoint_interval(); }

        //! Set local checkpoint interval: overrides value from argument cache
        void set_local_checkpoint_interval(unsigned int m) { this->local_checkpoint = m; }

        //! Unset local checkpoint interval
        void unset_local_checkpoint_interval() { this->local_checkpoint.reset(); }


        // TRANSACTIONS

      public:

        //! Generate a transaction management object -- supplied by implementation class
        virtual transaction_manager transaction_factory(integration_writer<number>& writer) = 0;

        //! Generate a transaction management object -- supplied by implementation class
        virtual transaction_manager transaction_factory(postintegration_writer<number>& writer) = 0;

      protected:

        //! Generate a transaction management object
        transaction_manager generate_transaction_manager(boost::filesystem::path lockfile, std::unique_ptr<transaction_handler> handle);

        //! Release resources after end of transaction
        void release_transaction();


        // WRITER HANDLING

      public:

        //! Initialize an integration_writer object.
        //! Never overwrites existing data; if the container already exists, an exception is thrown
        virtual void initialize_writer(integration_writer<number>& writer, bool recovery_mode = false) = 0;

        //! Close an integration_writer object.
        virtual void close_writer(integration_writer<number>& writer) = 0;

        //! Initialize a derived_content_writer object.
        virtual void initialize_writer(derived_content_writer<number>& writer, bool recovery_mode=false) = 0;

        //! Close an open derived_content_writer object.
        virtual void close_writer(derived_content_writer<number>& writer) = 0;

        //! Initialize a postintegration_writer object.
        //! Note: does not set flags for spectral data content, which may or may not be present depending
        //! on the integration payload that is sourcing this writer
        virtual void initialize_writer(postintegration_writer<number>& writer, bool recovery_mode=false) = 0;

        //! Close an open postintegration_writer object.
        virtual void close_writer(postintegration_writer<number>& writer) = 0;

        //! Close a paired integration_writer and postintegration_writer set
        virtual void close_writer(integration_writer<number>& i_writer, postintegration_writer<number>& p_writer) = 0;


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
                                 const content_group_record<integration_payload>& seed) = 0;

        //! Seed a writer for a threepf task
        virtual void seed_writer(integration_writer<number>& writer, threepf_task<number>* tk,
                                 const content_group_record<integration_payload>& seed) = 0;

        //! Seed a writer for a zeta twopf task
        virtual void seed_writer(postintegration_writer<number>& writer, zeta_twopf_task<number>* tk,
                                 const content_group_record<postintegration_payload>& seed) = 0;

        //! Seed a writer for a zeta threepf task
        virtual void seed_writer(postintegration_writer<number>& writer, zeta_threepf_task<number>* tk,
                                 const content_group_record<postintegration_payload>& seed) = 0;

        //! Seed a writer for an fNL task
        virtual void seed_writer(postintegration_writer<number>& writer, fNL_task<number>* tk,
                                 const content_group_record<postintegration_payload>& seed) = 0;


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
        virtual zeta_twopf_batcher<number> create_temp_zeta_twopf_container(zeta_twopf_task<number>* tk, const boost::filesystem::path& tempdir,
                                                                            const boost::filesystem::path& logdir,
                                                                            unsigned int worker, model<number>* m,
                                                                            std::unique_ptr<container_dispatch_function> dispatcher) = 0;

        //! Create a temporary container for zeta threepf data. Returns a batcher which can be used for writing to the container.
        virtual zeta_threepf_batcher<number> create_temp_zeta_threepf_container(zeta_threepf_task<number>* tk, const boost::filesystem::path& tempdir,
                                                                                const boost::filesystem::path& logdir,
                                                                                unsigned int worker, model<number>* m,
                                                                                std::unique_ptr<container_dispatch_function> dispatcher) = 0;

        //! Create a temporary container for fNL data. Returns a batcher which can be used for writing to the container.
        virtual fNL_batcher<number> create_temp_fNL_container(fNL_task<number>* tk, const boost::filesystem::path& tempdir,
                                                              const boost::filesystem::path& logdir, unsigned int worker, model<number>* m,
                                                              std::unique_ptr<container_dispatch_function> dispatcher,
                                                              derived_data::bispectrum_template type) = 0;


        // INTEGRITY CHECK

      public:

        //! Check integrity for a twopf container
        void check_twopf_integrity_handler(integration_writer<number>& writer, integration_task<number>& tk);

        //! Check integrity for a threepf container
        void check_threepf_integrity_handler(integration_writer<number>& writer, integration_task<number>& tk);

        //! Check integrity for a zeta twopf container
        void check_zeta_twopf_integrity_handler(postintegration_writer<number>& writer, postintegration_task<number>& tk);

        //! Check integrity for a zeta threepf container
        void check_zeta_threepf_integrity_handler(postintegration_writer<number>& writer, postintegration_task<number>& tk);

        //! Check integrity for an fNL container
        void check_fNL_integrity_handler(postintegration_writer<number>& writer, postintegration_task<number>& tk);

      protected:

        //! Synchronize missing serial numbers between an integration writer and a postintegration writer
        void synchronize_missing_serials(integration_writer<number>& i_writer, postintegration_writer<number>& p_writer);


        // -- CALCULATE MISSING SERIAL NUMBERS

      protected:

        //! get missing serial numbers from a twopf-re table; should be provided by implementation
        virtual serial_number_list get_missing_twopf_re_serials(integration_writer<number>& writer) = 0;

        //! get missing serial numbers from a twopf-im table; should be provided by implementation
        virtual serial_number_list get_missing_twopf_im_serials(integration_writer<number>& writer) = 0;

        //! get missing serial numbers from a twopf-re spectral index table; should be provided by implementation
        virtual serial_number_list get_missing_twopf_si_re_serials(integration_writer<number>& writer) = 0;

        //! get missing serial numbers from a tensor twopf table; should be provided by implementation
        virtual serial_number_list get_missing_tensor_twopf_serials(integration_writer<number>& writer) = 0;

        //! get missing serial numbers from a tensor twopf spectral index table; should be provided by implementation
        virtual serial_number_list get_missing_tensor_twopf_si_serials(integration_writer<number>& writer) = 0;

        //! get missing serial numbers from a threepf-momentum table; should be provided by implementation
        virtual serial_number_list get_missing_threepf_momentum_serials(integration_writer <number>& writer) = 0;

        //! get missing serial numbers from a threepf-deriv table; should be provided by implementation
        virtual serial_number_list get_missing_threepf_deriv_serials(integration_writer <number>& writer) = 0;

        //! get missing serial numbers from a zeta twopf table; should be provided by implementation
        virtual serial_number_list get_missing_zeta_twopf_serials(postintegration_writer<number>& writer) = 0;

        //! get missing serial numbers from a zeta threepf table; should be provided by implementation
        virtual serial_number_list get_missing_zeta_threepf_serials(postintegration_writer<number>& writer) = 0;

        //! get missing numbers from a zeta 1st-order gauge xfm table; should be provided by implementation
        virtual serial_number_list get_missing_gauge_xfm1_serials(postintegration_writer<number>& writer) = 0;

        //! get missing numbers from a zeta 2nd-order gauge xfm 123-order table; should be provided by implementation
        virtual serial_number_list get_missing_gauge_xfm2_123_serials(postintegration_writer<number>& writer) = 0;

        //! get missing numbers from a zeta 2nd-order gauge xfm 213-order table; should be provided by implementation
        virtual serial_number_list get_missing_gauge_xfm2_213_serials(postintegration_writer<number>& writer) = 0;

        //! get missing numbers from a zeta 2nd-order gauge xfm 312-order table; should be provided by implementation
        virtual serial_number_list get_missing_gauge_xfm2_312_serials(postintegration_writer<number>& writer) = 0;


        // -- DROP GROUPS OF SERIAL NUMBERS

      protected:

        //! drop a set of k-configurations from a twopf-re table; should be provided by implementation
        virtual void drop_twopf_re_configurations(transaction_manager& mgr, integration_writer<number>& writer,
                                                  const serial_number_list& serials, const serial_number_list& missing,
                                                  const twopf_kconfig_database& dbase) = 0;

        //! drop a set of k-configurations from a twopf-im table; should be provided by implementation
        virtual void drop_twopf_im_configurations(transaction_manager& mgr, integration_writer<number>& writer,
                                                  const serial_number_list& serials, const serial_number_list& missing,
                                                  const twopf_kconfig_database& dbase) = 0;

        //! drop a set of k-configurations from a twopf-re spectral index table; should be provided by implementation
        virtual void drop_twopf_si_re_configurations(transaction_manager& mgr, integration_writer <number>& writer,
                                                     const serial_number_list& serials, const serial_number_list& missing,
                                                     const twopf_kconfig_database& dbase) = 0;

        //! drop a set of k-configurations from a tensor twopf table; should be provided by implementation
        virtual void drop_tensor_twopf_configurations(transaction_manager& mgr, integration_writer<number>& writer,
                                                      const serial_number_list& serials, const serial_number_list& missing,
                                                      const twopf_kconfig_database& dbase) = 0;

        //! drop a set of k-configurations from a tensor twopf spectral index table; should be provided by implementation
        virtual void drop_tensor_twopf_si_configurations(transaction_manager& mgr, integration_writer<number>& writer,
                                                         const serial_number_list& serials, const serial_number_list& missing,
                                                         const twopf_kconfig_database& dbase) = 0;

        //! drop a set of k-configurations from a threepf-momentum table; should be provided by implementation
        virtual void drop_threepf_momentum_configurations(transaction_manager& mgr, integration_writer <number>& writer,
                                                          const serial_number_list& serials, const serial_number_list& missing,
                                                          const threepf_kconfig_database& dbase) = 0;

        //! drop a set of k-configurations from a threepf-deriv table; should be provided by implementation
        virtual void drop_threepf_deriv_configurations(transaction_manager& mgr, integration_writer <number>& writer,
                                                       const serial_number_list& serials, const serial_number_list& missing,
                                                       const threepf_kconfig_database& dbase) = 0;

        //! drop a set of k-configurations from a zeta twopf table; should be provided by implementation
        virtual void drop_zeta_twopf_configurations(transaction_manager& mgr, postintegration_writer<number>& writer,
                                                    const serial_number_list& serials, const serial_number_list& missing,
                                                    const twopf_kconfig_database& dbase) = 0;

        //! drop a set of k-configurations from a zeta threepf table; should be provided by implementation
        virtual void drop_zeta_threepf_configurations(transaction_manager& mgr, postintegration_writer<number>& writer,
                                                      const serial_number_list& serials, const serial_number_list& missing,
                                                      const threepf_kconfig_database& dbase) = 0;

        //! drop a set of k-configurations from a 1st-order gauge xfm table; should be provided by implementation
        virtual void drop_gauge_xfm1_configurations(transaction_manager& mgr, postintegration_writer<number>& writer,
                                                    const serial_number_list& serials, const serial_number_list& missing,
                                                    const twopf_kconfig_database& dbase) = 0;

        //! drop a set of k-configurations from a 2nd-order gauge xfm 123-order table; should be provided by implementation
        virtual void drop_gauge_xfm2_123_configurations(transaction_manager& mgr, postintegration_writer<number>& writer,
                                                        const serial_number_list& serials, const serial_number_list& missing,
                                                        const threepf_kconfig_database& dbase) = 0;

        //! drop a set of k-configurations from a 2nd-order gauge xfm 213-order table; should be provided by implementation
        virtual void drop_gauge_xfm2_213_configurations(transaction_manager& mgr, postintegration_writer<number>& writer,
                                                        const serial_number_list& serials, const serial_number_list& missing,
                                                        const threepf_kconfig_database& dbase) = 0;

        //! drop a set of k-configurations from a 2nd-order gauge xfm 312-order table; should be provided by implementation
        virtual void drop_gauge_xfm2_312_configurations(transaction_manager& mgr, postintegration_writer<number>& writer,
                                                        const serial_number_list& serials, const serial_number_list& missing,
                                                        const threepf_kconfig_database& dbase) = 0;

        //! drop statistics for a set of k-configurations; should be provided by implementation
        virtual void drop_statistics_configurations(transaction_manager& mgr, integration_writer<number>& writer,
                                                    const serial_number_list& serials, const twopf_kconfig_database& dbase) = 0;
        virtual void drop_statistics_configurations(transaction_manager& mgr, integration_writer<number>& writer,
                                                    const serial_number_list& serials, const threepf_kconfig_database& dbase) = 0;

        //! drop initial conditions for a set of k-configurations; should be provided by implementation
        virtual void drop_initial_conditions_configurations(transaction_manager& mgr, integration_writer<number>& writer,
                                                            const serial_number_list& serials, const twopf_kconfig_database& dbase) = 0;
        virtual void drop_initial_conditions_configurations(transaction_manager& mgr, integration_writer<number>& writer,
                                                            const serial_number_list& serials, const threepf_kconfig_database& dbase) = 0;


        // -- INTEGRITY-CHECK UTILITY FUNCTIONS

      protected:

        //!
        template <typename WriterObject, typename Database>
        void advise_missing_content(WriterObject& writer, const serial_number_list& serials, const Database& db);

        //! compare list of missing serials numbers against a list of failed serial numbers, provided by the backend
        //! (not all backends may make this information available)
        //! returns: serial numbers of any further configurations that should be dropped (they were in the list provided by the backend, but not already missing)
        template <typename WriterObject, typename Database>
        serial_number_list find_failed_but_undropped_serials(WriterObject& writer, const serial_number_list& serials, const Database& db);

        //! compute the twopf configurations which should be dropped to match a give list of threepf serials
        serial_number_list compute_twopf_drop_list(const serial_number_list& serials, const threepf_kconfig_database& configs);

        //! map a list of twopf configuration serial numbers to corresponding threepf configuration serial numbers
        serial_number_list map_twopf_to_threepf_serials(const serial_number_list& twopf_list, const threepf_kconfig_database& threepf_db);


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
        virtual std::unique_ptr< content_group_record<integration_payload> >
        datapipe_attach_integration_content(datapipe<number>& pipe, integration_content_finder<number>& finder,
                                            const std::string& name, const tag_list& tags) = 0;

        //! Attach an postintegration content group to a datapipe
        virtual std::unique_ptr< content_group_record<postintegration_payload> >
        datapipe_attach_postintegration_content(datapipe<number>& pipe, postintegration_content_finder<number>& finder,
                                                const std::string& name, const tag_list& tags) = 0;

        //! Detach a content_group_record from a pipe
        virtual void datapipe_detach(datapipe<number>* pipe) = 0;


        // -- PULL

      public:

        //! Pull a set of time sample-points from a datapipe
        virtual void pull_time_config(datapipe<number>* pipe, const derived_data::SQL_time_query& tquery,
                                      std::vector<time_config>& sample) = 0;

        //! Pull a set of 2pf k-configuration serial numbers from a datapipe
        virtual void pull_kconfig_twopf(datapipe<number>* pipe, const derived_data::SQL_twopf_query& kquery,
                                        std::vector<twopf_kconfig>& sample) = 0;

        //! Pull a set of 3pd k-configuration serial numbesr from a datapipe
        //! Simultaneously, populates three lists (k1, k2, k3) with serial numbers for the 2pf k-configurations
        //! corresponding to k1, k2, k3
        virtual void pull_kconfig_threepf(datapipe<number>* pipe, const derived_data::SQL_threepf_query& kquery,
                                          std::vector<threepf_kconfig>& sample) = 0;

        //! Pull a time sample of a background field from a datapipe
        virtual void pull_background_time_sample(datapipe<number>* pipe, unsigned int id, const derived_data::SQL_query& query,
                                                 std::vector<number>& sample) = 0;

        //! Pull a time sample of a twopf component at fixed k-configuration from a datapipe
        virtual void pull_twopf_time_sample(datapipe<number>* pipe, unsigned int id, const derived_data::SQL_query& query,
                                            unsigned int k_serial, std::vector<number>& sample, twopf_type type) = 0;

        //! Pull a time sample of a twopf 'spectral index' component at fixed k-configuration from a datapipe
        virtual void pull_twopf_si_time_sample(datapipe<number>* pipe, unsigned int id, const derived_data::SQL_query& query,
                                               unsigned int k_serial, std::vector<number>& sample) = 0;

        //! Pull a sample of a threepf component at fixed k-configuration from a datapipe
        virtual void pull_threepf_time_sample(datapipe<number>* pipe, unsigned int id, const derived_data::SQL_query& query,
                                              unsigned int k_serial, std::vector<number>& sample, threepf_type type) = 0;

        //! Pull a sample of a tensor twopf component at fixed k-configuration from a datapipe
        virtual void pull_tensor_twopf_time_sample(datapipe<number>* pipe, unsigned int id, const derived_data::SQL_query& query,
                                                   unsigned int k_serial, std::vector<number>& sample) = 0;

        //! Pull a sample of a tensor twopf 'spectral index' component at fixed k-configuration from a datapipe
        virtual void pull_tensor_twopf_si_time_sample(datapipe<number>* pipe, unsigned int id, const derived_data::SQL_query& query,
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
                                          std::vector<number>& sample, derived_data::bispectrum_template type) = 0;

        //! Pull a sample of bispectrum.template from a datapipe
        virtual void pull_BT_time_sample(datapipe<number>*, const derived_data::SQL_query& query,
                                         std::vector<number>& sample, derived_data::bispectrum_template type) = 0;

        //! Pull a sample of template.template from a datapipe
        virtual void pull_TT_time_sample(datapipe<number>*, const derived_data::SQL_query& query,
                                         std::vector<number>& sample, derived_data::bispectrum_template type) = 0;

        //! Pull a kconfig sample of a twopf component at fixed time from a datapipe
        virtual void pull_twopf_kconfig_sample(datapipe<number>* pipe, unsigned int id, const derived_data::SQL_query& query,
                                               unsigned int t_serial, std::vector<number>& sample, twopf_type type) = 0;

        //! Pull a kconfig sample of a twopf 'spectral index' component at fixed time from a datapipe
        virtual void pull_twopf_si_kconfig_sample(datapipe<number>* pipe, unsigned int id, const derived_data::SQL_query& query,
                                                  unsigned int t_serial, std::vector<number>& sample) = 0;

        //! Pull a kconfig sample of a threepf component at fixed time from a datapipe
        virtual void pull_threepf_kconfig_sample(datapipe<number>* pipe, unsigned int id, const derived_data::SQL_query& query,
                                                 unsigned int t_serial, std::vector<number>& sample, threepf_type type) = 0;

        //! Pull a kconfig sample of a tensor twopf component at fixed time from a datapipe
        virtual void pull_tensor_twopf_kconfig_sample(datapipe<number>* pipe, unsigned int id, const derived_data::SQL_query& query,
                                                      unsigned int t_serial, std::vector<number>& sample) = 0;

        //! Pull a kconfig sample of a tensor twopf 'spectral index' component at fixed time from a datapipe
        virtual void pull_tensor_twopf_si_kconfig_sample(datapipe<number>* pipe, unsigned int id, const derived_data::SQL_query& query,
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


        // RAW DATA ACCESS -- DOESN'T REQUIRE USE OF DATAPIPE

      public:

        //! Read the worker information table for a container
        virtual worker_information_db read_worker_information(const boost::filesystem::path& ctr_path) = 0;

        //! Read the timing table for a container (supposing one to be present)
        virtual timing_db read_timing_information(const boost::filesystem::path& ctr_path) = 0;


        // INTERNAL DATA

      protected:

        // RUNTIME AGENTS

        //! local environment policy class
        local_environment& env;

        //! argument policy class
        argument_cache& args;


        // CHECKPOINTING

        //! optional local checkpoint value, overrides value taken from argument cache
        boost::optional< unsigned int > local_checkpoint;


        // TRANSACTIONS

        //! number of active transactions
        unsigned int transactions;

      };

  }   // namespace transport


#endif //CPPTRANSPORT_DATA_MANAGER_DECL_H
