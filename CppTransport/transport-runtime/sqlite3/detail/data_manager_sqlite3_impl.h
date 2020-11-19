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

#ifndef CPPTRANSPORT_DATA_MANAGER_SQLITE3_IMPL_H
#define CPPTRANSPORT_DATA_MANAGER_SQLITE3_IMPL_H


#include <transport-runtime/exceptions.h>
#include "transport-runtime/sqlite3/detail/data_manager_sqlite3_decl.h"


namespace transport
  {

    // CONSTRUCTOR, DESTRUCTOR

    template <typename number>
    data_manager_sqlite3<number>::~data_manager_sqlite3()
      {
        // close any remaining open containers, reporting any failures
        bool fail = false;

        error_handler msg(this->env, this->args);
        
        for(sqlite3* h : this->open_containers)
          {
            int status = sqlite3_close(h);

            if(status != SQLITE_OK)
              {
                if(status == SQLITE_BUSY)
                  {
                    if(!fail)
                      {
                        fail = true;
                        msg(CPPTRANSPORT_DATACTR_NOT_CLOSED);
                      }
                  }
                else
                  {
                    std::ostringstream report_msg;
                    report_msg << CPPTRANSPORT_DATACTR_CLOSE << status << ")";
                    msg(report_msg.str());
                  }
              }
          }
      }


    // TRANSACTION MANAGEMENT


    // Begin transaction on SQLite3 container
    template <typename number>
    void data_manager_sqlite3<number>::begin_transaction(sqlite3* db)
      {
        assert(db != nullptr);

        sqlite3_operations::exec(db, "BEGIN TRANSACTION;");
      }


    // End transaction on SQLite3 container
    template <typename number>
    void data_manager_sqlite3<number>::commit_transaction(sqlite3* db)
      {
        assert(db != nullptr);

        sqlite3_operations::exec(db, "COMMIT;");
      }


    // Abort transaction on SQLite3 container
    template <typename number>
    void data_manager_sqlite3<number>::abort_transaction(sqlite3* db)
      {
        assert(db != nullptr);

        sqlite3_operations::exec(db, "ROLLBACK;");
      }


    // Release transaction; nothing specific for us to do here except pass control
    // to the underlying data_manager<> interface
    template <typename number>
    void data_manager_sqlite3<number>::release_transaction()
      {
        this->data_manager<number>::release_transaction();
      }


    template <typename number>
    transaction_manager data_manager_sqlite3<number>::transaction_factory(integration_writer<number>& writer)
      {
        return this->internal_transaction_factory(writer);
      }


    template <typename number>
    transaction_manager data_manager_sqlite3<number>::transaction_factory(postintegration_writer<number>& writer)
      {
        return this->internal_transaction_factory(writer);
      }


    template <typename number>
    template <typename WriterObject>
    transaction_manager data_manager_sqlite3<number>::internal_transaction_factory(WriterObject& writer)
      {
        sqlite3* db = nullptr;
        writer.get_data_manager_handle(&db); // throws an exception if handle is unset, so the return value is guaranteed not to be nullptr

        // extract location of container and convert to location of lockfile
        boost::filesystem::path ctr_path = writer.get_abs_container_path();

        return this->transaction_factory(db, ctr_path.parent_path() / CPPTRANSPORT_DATAMGR_LOCKFILE_LEAF);
      }


    template <typename number>
    transaction_manager data_manager_sqlite3<number>::transaction_factory(sqlite3* db, boost::filesystem::path lockfile)
      {
        // generate a transaction handler bundle
        auto handle = std::make_unique< data_manager_sqlite3_transaction_handler<number> >(*this, db);

        // construct transaction manager
        return this->data_manager<number>::generate_transaction_manager(std::move(lockfile), std::move(handle));
      }


    // WRITER MANAGEMENT


    // Create data files for a new integration_writer object
    template <typename number>
    void data_manager_sqlite3<number>::initialize_writer(integration_writer<number>& writer, bool recovery_mode)
      {
        sqlite3* db = nullptr;

        // get paths of the data container
        boost::filesystem::path ctr_path = writer.get_abs_container_path();

        // open the main container
        int mode = recovery_mode ? SQLITE_OPEN_READWRITE : (SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE);
        int status = sqlite3_open_v2(ctr_path.string().c_str(), &db, mode, nullptr);

        if(status != SQLITE_OK)
          {
            std::ostringstream msg;
            if(db != nullptr)
              {
                msg << CPPTRANSPORT_DATACTR_CREATE_A << " '" << ctr_path.string() << "' " << CPPTRANSPORT_DATACTR_CREATE_B << status << ": " << sqlite3_errmsg(db) << ")";
                sqlite3_close(db);
              }
            else
              {
                msg << CPPTRANSPORT_DATACTR_CREATE_A << " '" << ctr_path.string() << "' " << CPPTRANSPORT_DATACTR_CREATE_B << status << ")";
              }
            throw runtime_exception(exception_type::DATA_CONTAINER_ERROR, msg.str());
          }

        sqlite3_extended_result_codes(db, 1);

#ifdef CPPTRANSPORT_STRICT_CONSISTENCY
        sqlite3_operations::consistency_pragmas(db, this->args.get_network_mode());
#else
        // change setting to optimize SQLite performance
        sqlite3_operations::container_write_pragmas(db, this->args.get_network_mode());
#endif

        // remember this connexion
        this->open_containers.push_back(db);
        writer.set_data_manager_handle(db);

        // set up aggregation handlers
        switch(writer.get_type())
          {
            case integration_task_type::twopf:
              {
                std::unique_ptr<sqlite3_twopf_writer_aggregate<number> > aggregate = std::make_unique<sqlite3_twopf_writer_aggregate<number> >(*this);
                std::unique_ptr<sqlite3_twopf_writer_integrity<number> > integrity = std::make_unique<sqlite3_twopf_writer_integrity<number> >(*this);
                std::unique_ptr<sqlite3_twopf_writer_finalize<number> >  finalize  = std::make_unique<sqlite3_twopf_writer_finalize<number> >(*this);

                writer.set_aggregation_handler(std::move(aggregate));
                writer.set_integrity_check_handler(std::move(integrity));
                writer.set_finalize_handler(std::move(finalize));
                break;
              }

            case integration_task_type::threepf:
              {
                std::unique_ptr<sqlite3_threepf_writer_aggregate<number> > aggregate = std::make_unique<sqlite3_threepf_writer_aggregate<number> >(*this);
                std::unique_ptr<sqlite3_threepf_writer_integrity<number> > integrity = std::make_unique<sqlite3_threepf_writer_integrity<number> >(*this);
                std::unique_ptr<sqlite3_threepf_writer_finalize<number> >  finalize  = std::make_unique<sqlite3_threepf_writer_finalize<number> >(*this);

                writer.set_aggregation_handler(std::move(aggregate));
                writer.set_integrity_check_handler(std::move(integrity));
                writer.set_finalize_handler(std::move(finalize));
                break;
              }
          }
      }


    // Close data files associated with an integration_writer object
    template <typename number>
    void data_manager_sqlite3<number>::close_writer(integration_writer<number>& writer)
      {
        // perform integrity check; updates writer with a list of missing serial numbers, if needed
        writer.check_integrity();

        // perform any finalization step
        writer.finalize();

        this->close_writer_handle(writer);
      }


    // Create data files for a new derived_content_writer object
    template <typename number>
    void data_manager_sqlite3<number>::initialize_writer(derived_content_writer<number>& writer, bool recovery_mode)
      {
        // set up aggregation handler
        std::unique_ptr< sqlite3_derived_content_writer_aggregate<number> > aggregate = std::make_unique< sqlite3_derived_content_writer_aggregate<number> >(*this);
        writer.set_aggregation_handler(std::move(aggregate));
      }


    // Close data files for a derived_content_writer object
    template <typename number>
    void data_manager_sqlite3<number>::close_writer(derived_content_writer<number>& writer)
      {
        // physically remove the tempfiles directory
        boost::filesystem::remove_all(writer.get_abs_tempdir_path());
      }


    // Initialize a new postintegration_writer object
    template <typename number>
    void data_manager_sqlite3<number>::initialize_writer(postintegration_writer<number>& writer, bool recovery_mode)
      {
        sqlite3* db = nullptr;

        // get paths to the data container
        boost::filesystem::path ctr_path = writer.get_abs_container_path();

        // open the main container
        int mode = recovery_mode ? SQLITE_OPEN_READWRITE : SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE;
        int status = sqlite3_open_v2(ctr_path.string().c_str(), &db, mode, nullptr);

        if(status != SQLITE_OK)
          {
            std::ostringstream msg;
            if(db != nullptr)
              {
                msg << CPPTRANSPORT_DATACTR_CREATE_A << " '" << ctr_path.string() << "' " << CPPTRANSPORT_DATACTR_CREATE_B << status << ": " << sqlite3_errmsg(db) << ")";
                sqlite3_close(db);
              }
            else
              {
                msg << CPPTRANSPORT_DATACTR_CREATE_A << " '" << ctr_path.string() << "' " << CPPTRANSPORT_DATACTR_CREATE_B << status << ")";
              }
            throw runtime_exception(exception_type::DATA_CONTAINER_ERROR, msg.str());
          }

        sqlite3_extended_result_codes(db, 1);

#ifdef CPPTRANSPORT_STRICT_CONSISTENCY
        sqlite3_operations::consistency_pragmas(db, this->args.get_network_mode());
#else
        // change setting to optimize SQLite performance
        sqlite3_operations::container_write_pragmas(db, this->args.get_network_mode());
#endif

        // remember this connexion
        this->open_containers.push_back(db);
        writer.set_data_manager_handle(db);

        // set aggregation handlers
        switch(writer.get_type())
          {
            case postintegration_task_type::twopf:
              {
                std::unique_ptr< sqlite3_zeta_twopf_writer_aggregate<number> > aggregate = std::make_unique< sqlite3_zeta_twopf_writer_aggregate<number> >(*this);
                std::unique_ptr< sqlite3_zeta_twopf_writer_integrity<number> > integrity = std::make_unique< sqlite3_zeta_twopf_writer_integrity<number> >(*this);
                std::unique_ptr< sqlite3_zeta_twopf_writer_finalize<number> >  finalize  = std::make_unique< sqlite3_zeta_twopf_writer_finalize<number> >(*this);

                writer.set_aggregation_handler(std::move(aggregate));
                writer.set_integrity_check_handler(std::move(integrity));
                writer.set_finalize_handler(std::move(finalize));

                writer.get_products().add_zeta_twopf();
                break;
              }

            case postintegration_task_type::threepf:
              {
                std::unique_ptr< sqlite3_zeta_threepf_writer_aggregate<number> > aggregate = std::make_unique< sqlite3_zeta_threepf_writer_aggregate<number> >(*this);
                std::unique_ptr< sqlite3_zeta_threepf_writer_integrity<number> > integrity = std::make_unique< sqlite3_zeta_threepf_writer_integrity<number> >(*this);
                std::unique_ptr< sqlite3_zeta_threepf_writer_finalize<number> >  finalize  = std::make_unique< sqlite3_zeta_threepf_writer_finalize<number> >(*this);

                writer.set_aggregation_handler(std::move(aggregate));
                writer.set_integrity_check_handler(std::move(integrity));
                writer.set_finalize_handler(std::move(finalize));

                writer.get_products().add_zeta_twopf();
                writer.get_products().add_zeta_threepf();
                writer.get_products().add_zeta_redbsp();
                break;
              }

            case postintegration_task_type::fNL:
              {
                const fNL_task<number>& ztk = writer.template get_task< fNL_task<number> >();

                std::unique_ptr< sqlite3_fNL_writer_aggregate<number> > aggregate = std::make_unique< sqlite3_fNL_writer_aggregate<number> >(*this, ztk.get_template());
                std::unique_ptr< sqlite3_fNL_writer_integrity<number> > integrity = std::make_unique< sqlite3_fNL_writer_integrity<number> >(*this);
                std::unique_ptr< sqlite3_fNL_writer_finalize<number> >  finalize  = std::make_unique< sqlite3_fNL_writer_finalize<number> >(*this);

                writer.set_aggregation_handler(std::move(aggregate));
                writer.set_integrity_check_handler(std::move(integrity));
                writer.set_finalize_handler(std::move(finalize));

                switch(ztk.get_template())
                  {
                    case derived_data::bispectrum_template::local:
                      {
                        writer.get_products().add_fNL_local();
                        break;
                      }

                    case derived_data::bispectrum_template::equilateral:
                      {
                        writer.get_products().add_fNL_equi();
                        break;
                      }

                    case derived_data::bispectrum_template::orthogonal:
                      {
                        writer.get_products().add_fNL_ortho();
                        break;
                      }

                    case derived_data::bispectrum_template::DBI:
                      {
                        writer.get_products().add_fNL_DBI();
                        break;
                      }
                  }

                break;
              }
          }
      }


    // Close a postintegration_writer object
    template <typename number>
    void data_manager_sqlite3<number>::close_writer(postintegration_writer<number>& writer)
      {
        // perform integrity check; updates writer with a list of missing serial numbers, if needed
        writer.check_integrity();

        // perform any finalization step
        writer.finalize();

        this->close_writer_handle(writer);
      }


    // Close a pair of integration_writer, postintegration_writer objects
    template <typename number>
    void data_manager_sqlite3<number>::close_writer(integration_writer<number>& i_writer, postintegration_writer<number>& p_writer)
      {
        // perform integrity check; updates writer with a list of missing serial numbers, if needed
        i_writer.check_integrity();
        p_writer.check_integrity();

        // synchronize any missing serial numbers between containers
        this->synchronize_missing_serials(i_writer, p_writer);

        // perform finalization step
        i_writer.finalize();
        p_writer.finalize();

        this->close_writer_handle(i_writer);
        this->close_writer_handle(p_writer);
      }


    template <typename number>
    template <typename WriterObject>
    void data_manager_sqlite3<number>::close_writer_handle(WriterObject& writer)
      {
        // close sqlite3 handle to principal database
        sqlite3* db = nullptr;
        writer.get_data_manager_handle(&db); // throws an exception if handle is unset, so the return value is guaranteed not to be nullptr

        // vacuum the database if it is sufficiently small
        boost::uintmax_t container_size = boost::filesystem::file_size(writer.get_abs_container_path());
        if(container_size < CPPTRANSPORT_MAX_VACUUMABLE_SIZE)
          {
            BOOST_LOG_SEV(writer.get_log(), base_writer::log_severity_level::notification)
              << '\n' << "** Performing routine maintenance on SQLite3 container '" << writer.get_abs_container_path().string() << "'";
            boost::timer::cpu_timer timer;
            char* errmsg;
            sqlite3_exec(db, "VACUUM;", nullptr, nullptr, &errmsg);
            timer.stop();
            BOOST_LOG_SEV(writer.get_log(), base_writer::log_severity_level::notification)
              << "** Database vacuum complete in wallclock time " << format_time(timer.elapsed().wall);
          }
        else
          {
            BOOST_LOG_SEV(writer.get_log(), base_writer::log_severity_level::notification)
              << "** SQLite3 container '"
              << writer.get_abs_container_path().string() << "' of size "
              << format_memory(container_size)
              << " is very large; automatic maintenance disabled";
          }

        this->open_containers.remove(db);
        sqlite3_close(db);

        // physically remove the tempfiles directory
#ifndef CPPPTRANSPORT_NO_REMOVE_INTERMEDIATE_CONTAINERS
        boost::filesystem::remove_all(writer.get_abs_tempdir_path());
#endif
      }


    // INDEX TABLE MANAGEMENT


    template <typename number>
    void data_manager_sqlite3<number>::create_tables(integration_writer<number>& writer, twopf_task<number>* tk)
      {
        sqlite3* db = nullptr;
        writer.get_data_manager_handle(&db); // throws an exception if handle is unset, so the return value is guaranteed not to be nullptr

        unsigned int Nfields = tk->get_model()->get_N_fields();

        auto mgr = this->transaction_factory(writer);

        sqlite3_operations::create_time_sample_table(mgr, db, tk);
        sqlite3_operations::create_twopf_sample_table(mgr, db, tk);
        sqlite3_operations::create_backg_table<number, typename integration_items<number>::backg_item>
          (mgr, db, Nfields, sqlite3_operations::foreign_keys_type::foreign_keys);
        sqlite3_operations::create_paged_table<number, typename integration_items<number>::twopf_re_item>
          (mgr, db, Nfields, sqlite3_operations::foreign_keys_type::foreign_keys, sqlite3_operations::kconfiguration_type::twopf_configs);
        sqlite3_operations::create_paged_table<number, typename integration_items<number>::twopf_si_re_item>
          (mgr, db, Nfields, sqlite3_operations::foreign_keys_type::foreign_keys, sqlite3_operations::kconfiguration_type::twopf_configs);
        sqlite3_operations::create_paged_table<number, typename integration_items<number>::tensor_twopf_item>
          (mgr, db, Nfields, sqlite3_operations::foreign_keys_type::foreign_keys, sqlite3_operations::kconfiguration_type::twopf_configs);
        sqlite3_operations::create_paged_table<number, typename integration_items<number>::tensor_twopf_si_item>
          (mgr, db, Nfields, sqlite3_operations::foreign_keys_type::foreign_keys, sqlite3_operations::kconfiguration_type::twopf_configs);

        sqlite3_operations::create_worker_info_table(mgr, db, sqlite3_operations::foreign_keys_type::foreign_keys);
        if(writer.is_collecting_statistics()) sqlite3_operations::create_stats_table(mgr, db, sqlite3_operations::foreign_keys_type::foreign_keys, sqlite3_operations::kconfiguration_type::twopf_configs);

        if(writer.is_collecting_initial_conditions()) sqlite3_operations::create_ics_table<number, typename integration_items<number>::ics_item>(mgr, db, Nfields, sqlite3_operations::foreign_keys_type::foreign_keys, sqlite3_operations::kconfiguration_type::twopf_configs);

        mgr.commit();
      }


    template <typename number>
    void data_manager_sqlite3<number>::create_tables(integration_writer<number>& writer, threepf_task<number>* tk)
      {
        sqlite3* db = nullptr;
        writer.get_data_manager_handle(&db); // throws an exception if handle is unset, so the return value is guaranteed not to be nullptr

        unsigned int Nfields = tk->get_model()->get_N_fields();

        auto mgr = this->transaction_factory(writer);

        sqlite3_operations::create_time_sample_table(mgr, db, tk);
        sqlite3_operations::create_twopf_sample_table(mgr, db, tk);
        sqlite3_operations::create_threepf_sample_table(mgr, db, tk);
        sqlite3_operations::create_backg_table<number, typename integration_items<number>::backg_item>
          (mgr, db, Nfields, sqlite3_operations::foreign_keys_type::foreign_keys);
        sqlite3_operations::create_paged_table<number, typename integration_items<number>::twopf_re_item>
          (mgr, db, Nfields, sqlite3_operations::foreign_keys_type::foreign_keys, sqlite3_operations::kconfiguration_type::twopf_configs);
        sqlite3_operations::create_paged_table<number, typename integration_items<number>::twopf_im_item>
          (mgr, db, Nfields, sqlite3_operations::foreign_keys_type::foreign_keys, sqlite3_operations::kconfiguration_type::twopf_configs);
        sqlite3_operations::create_paged_table<number, typename integration_items<number>::twopf_si_re_item>
          (mgr, db, Nfields, sqlite3_operations::foreign_keys_type::foreign_keys, sqlite3_operations::kconfiguration_type::twopf_configs);
        sqlite3_operations::create_paged_table<number, typename integration_items<number>::tensor_twopf_item>
          (mgr, db, Nfields, sqlite3_operations::foreign_keys_type::foreign_keys, sqlite3_operations::kconfiguration_type::twopf_configs);
        sqlite3_operations::create_paged_table<number, typename integration_items<number>::tensor_twopf_si_item>
          (mgr, db, Nfields, sqlite3_operations::foreign_keys_type::foreign_keys, sqlite3_operations::kconfiguration_type::twopf_configs);
        sqlite3_operations::create_paged_table<number, typename integration_items<number>::threepf_momentum_item>
          (mgr, db, Nfields, sqlite3_operations::foreign_keys_type::foreign_keys, sqlite3_operations::kconfiguration_type::threepf_configs);
        sqlite3_operations::create_paged_table<number, typename integration_items<number>::threepf_Nderiv_item>
          (mgr, db, Nfields, sqlite3_operations::foreign_keys_type::foreign_keys, sqlite3_operations::kconfiguration_type::threepf_configs);

        sqlite3_operations::create_worker_info_table(mgr, db, sqlite3_operations::foreign_keys_type::foreign_keys);
        if(writer.is_collecting_statistics()) sqlite3_operations::create_stats_table(mgr, db, sqlite3_operations::foreign_keys_type::foreign_keys, sqlite3_operations::kconfiguration_type::threepf_configs);

        if(writer.is_collecting_initial_conditions())
          {
            sqlite3_operations::create_ics_table<number, typename integration_items<number>::ics_item>(mgr, db, Nfields, sqlite3_operations::foreign_keys_type::foreign_keys, sqlite3_operations::kconfiguration_type::threepf_configs);
            sqlite3_operations::create_ics_table<number, typename integration_items<number>::ics_kt_item>(mgr, db, Nfields, sqlite3_operations::foreign_keys_type::foreign_keys, sqlite3_operations::kconfiguration_type::threepf_configs);
          }

        mgr.commit();
      }


    template <typename number>
    void data_manager_sqlite3<number>::create_tables(postintegration_writer<number>& writer, zeta_twopf_task<number>* tk)
      {
        sqlite3* db = nullptr;
        writer.get_data_manager_handle(&db); // throws an exception if handle is unset, so the return value is guaranteed not to be nullptr

        derivable_task<number>* d_ptk = tk->get_parent_task();
        auto* i_ptk = dynamic_cast< integration_task<number>* >(d_ptk);
        assert(i_ptk != nullptr);
        if(i_ptk == nullptr) throw runtime_exception(exception_type::RUNTIME_ERROR, CPPTRANSPORT_ZETA_INTEGRATION_CAST_FAIL);

        unsigned int Nfields = i_ptk->get_model()->get_N_fields();

        auto mgr = this->transaction_factory(writer);

        sqlite3_operations::create_time_sample_table(mgr, db, tk);
        sqlite3_operations::create_twopf_sample_table(mgr, db, tk);
        sqlite3_operations::create_zeta_twopf_table(mgr, db, sqlite3_operations::foreign_keys_type::foreign_keys);
        sqlite3_operations::create_paged_table<number, typename postintegration_items<number>::gauge_xfm1_item>
          (mgr, db, Nfields, sqlite3_operations::foreign_keys_type::foreign_keys, sqlite3_operations::kconfiguration_type::twopf_configs);

        mgr.commit();
      }


    template <typename number>
    void data_manager_sqlite3<number>::create_tables(postintegration_writer<number>& writer, zeta_threepf_task<number>* tk)
      {
        sqlite3* db = nullptr;
        writer.get_data_manager_handle(&db); // throws an exception if handle is unset, so the return value is guaranteed not to be nullptr

        derivable_task<number>* d_ptk = tk->get_parent_task();
        auto* i_ptk = dynamic_cast< integration_task<number>* >(d_ptk);
        assert(i_ptk != nullptr);
        if(i_ptk == nullptr) throw runtime_exception(exception_type::RUNTIME_ERROR, CPPTRANSPORT_ZETA_INTEGRATION_CAST_FAIL);

        unsigned int Nfields = i_ptk->get_model()->get_N_fields();

        auto mgr = this->transaction_factory(writer);

        sqlite3_operations::create_time_sample_table(mgr, db, tk);
        sqlite3_operations::create_twopf_sample_table(mgr, db, tk);
        sqlite3_operations::create_threepf_sample_table(mgr, db, tk);
        sqlite3_operations::create_zeta_twopf_table(mgr, db, sqlite3_operations::foreign_keys_type::foreign_keys);
        sqlite3_operations::create_zeta_threepf_table(mgr, db, sqlite3_operations::foreign_keys_type::foreign_keys);
        sqlite3_operations::create_paged_table<number, typename postintegration_items<number>::gauge_xfm1_item>
          (mgr, db, Nfields, sqlite3_operations::foreign_keys_type::foreign_keys, sqlite3_operations::kconfiguration_type::twopf_configs);
        sqlite3_operations::create_paged_table<number, typename postintegration_items<number>::gauge_xfm2_123_item>
          (mgr, db, Nfields, sqlite3_operations::foreign_keys_type::foreign_keys, sqlite3_operations::kconfiguration_type::threepf_configs);
        sqlite3_operations::create_paged_table<number, typename postintegration_items<number>::gauge_xfm2_213_item>
          (mgr, db, Nfields, sqlite3_operations::foreign_keys_type::foreign_keys, sqlite3_operations::kconfiguration_type::threepf_configs);
        sqlite3_operations::create_paged_table<number, typename postintegration_items<number>::gauge_xfm2_312_item>
          (mgr, db, Nfields, sqlite3_operations::foreign_keys_type::foreign_keys, sqlite3_operations::kconfiguration_type::threepf_configs);

        mgr.commit();
      }


    template <typename number>
    void data_manager_sqlite3<number>::create_tables(postintegration_writer<number>& writer, fNL_task<number>* tk)
      {
        sqlite3* db = nullptr;
        writer.get_data_manager_handle(&db); // throws an exception if handle is unset, so the return value is guaranteed not to be nullptr

        auto mgr = this->transaction_factory(writer);

        sqlite3_operations::create_time_sample_table(mgr, db, tk);
        sqlite3_operations::create_fNL_table(mgr, db, tk->get_template(), sqlite3_operations::foreign_keys_type::foreign_keys);

        mgr.commit();
      }


    // SEEDING


    template <typename number>
    void data_manager_sqlite3<number>::seed_writer(integration_writer<number>& writer, twopf_task<number>* tk,
                                                   const content_group_record<integration_payload>& seed)
      {
        sqlite3* db = nullptr;
        writer.get_data_manager_handle(&db); // throws an exception if handle is unset, so the return value is guaranteed not to be nullptr

        BOOST_LOG_SEV(writer.get_log(), base_writer::log_severity_level::notification)
          << "** Seeding SQLite3 container '" << writer.get_abs_container_path().string() << "' "
          << "from previous content group '" << seed.get_name() << "', container '" << seed.get_abs_output_path().string() << "'";

        boost::timer::cpu_timer timer;
        auto seed_container_path = seed.get_abs_repo_path() / seed.get_payload().get_container_path();

        sqlite3_operations::attach_manager mgr(db, seed_container_path);

        sqlite3_operations::aggregate_backg<number>(mgr, writer);
        sqlite3_operations::aggregate_table<number, integration_writer<number>, typename integration_items<number>::twopf_re_item>
          (mgr, writer);
        sqlite3_operations::aggregate_table<number, integration_writer<number>, typename integration_items<number>::twopf_si_re_item>
          (mgr, writer);
        sqlite3_operations::aggregate_table<number, integration_writer<number>, typename integration_items<number>::tensor_twopf_item>
          (mgr, writer);
        sqlite3_operations::aggregate_table<number, integration_writer<number>, typename integration_items<number>::tensor_twopf_si_item>
          (mgr, writer);

        sqlite3_operations::aggregate_workers<number>(mgr, writer);
        if(writer.is_collecting_statistics() && seed.get_payload().has_statistics())
          sqlite3_operations::aggregate_statistics<number>(mgr, writer);

        if(writer.is_collecting_initial_conditions() && seed.get_payload().has_initial_conditions())
          sqlite3_operations::aggregate_ics<number, typename integration_items<number>::ics_item>(mgr, writer);

        mgr.commit();

        timer.stop();
        BOOST_LOG_SEV(writer.get_log(), base_writer::log_severity_level::notification)
          << "** Seeding complete in time " << format_time(timer.elapsed().wall);
      }


    template <typename number>
    void data_manager_sqlite3<number>::seed_writer(integration_writer<number>& writer, threepf_task<number>* tk,
                                                   const content_group_record<integration_payload>& seed)
      {
        sqlite3* db = nullptr;
        writer.get_data_manager_handle(&db); // throws an exception if handle is unset, so the return value is guaranteed not to be nullptr

        BOOST_LOG_SEV(writer.get_log(), base_writer::log_severity_level::notification)
          << "** Seeding SQLite3 container '" << writer.get_abs_container_path().string() << "' "
          << "from previous content group '" << seed.get_name() << "', container '" << seed.get_abs_output_path().string() << "'";

        boost::timer::cpu_timer timer;
        auto seed_container_path = seed.get_abs_repo_path() / seed.get_payload().get_container_path();

        sqlite3_operations::attach_manager mgr(db, seed_container_path);

        sqlite3_operations::aggregate_backg<number>(mgr, writer);
        sqlite3_operations::aggregate_table<number, integration_writer<number>, typename integration_items<number>::twopf_re_item>
          (mgr, writer);
        sqlite3_operations::aggregate_table<number, integration_writer<number>, typename integration_items<number>::twopf_im_item>
          (mgr, writer);
        sqlite3_operations::aggregate_table<number, integration_writer<number>, typename integration_items<number>::twopf_si_re_item>
          (mgr, writer);
        sqlite3_operations::aggregate_table<number, integration_writer<number>, typename integration_items<number>::tensor_twopf_item>
          (mgr, writer);
        sqlite3_operations::aggregate_table<number, integration_writer<number>, typename integration_items<number>::tensor_twopf_si_item>
          (mgr, writer);

        sqlite3_operations::aggregate_table<number, integration_writer<number>, typename integration_items<number>::threepf_momentum_item>
          (mgr, writer);
        sqlite3_operations::aggregate_table<number, integration_writer<number>, typename integration_items<number>::threepf_Nderiv_item>
          (mgr, writer);

        sqlite3_operations::aggregate_workers<number>(mgr, writer);
        if(writer.is_collecting_statistics() && seed.get_payload().has_statistics()) sqlite3_operations::aggregate_statistics<number>(mgr, writer);

        if(writer.is_collecting_initial_conditions() && seed.get_payload().has_initial_conditions())
          {
            sqlite3_operations::aggregate_ics<number, typename integration_items<number>::ics_item>(mgr, writer);
            sqlite3_operations::aggregate_ics<number, typename integration_items<number>::ics_kt_item>(mgr, writer);
          }

        mgr.commit();

        timer.stop();
        BOOST_LOG_SEV(writer.get_log(), base_writer::log_severity_level::notification)
          << "** Seeding complete in time " << format_time(timer.elapsed().wall);
      }


    template <typename number>
    void data_manager_sqlite3<number>::seed_writer(postintegration_writer<number>& writer, zeta_twopf_task<number>* tk,
                                                   const content_group_record<postintegration_payload>& seed)
      {
        sqlite3* db = nullptr;
        writer.get_data_manager_handle(&db); // throws an exception if handle is unset, so the return value is guaranteed not to be nullptr

        BOOST_LOG_SEV(writer.get_log(), base_writer::log_severity_level::notification)
          << "** Seeding SQLite3 container '" << writer.get_abs_container_path().string() << "' "
          << "from previous content group '" << seed.get_name() << "', container '" << seed.get_abs_output_path().string() << "'";

        boost::timer::cpu_timer timer;
        auto seed_container_path = seed.get_abs_repo_path() / seed.get_payload().get_container_path();

        sqlite3_operations::attach_manager mgr{db, seed_container_path};

        sqlite3_operations::aggregate_table<number, postintegration_writer<number>, typename postintegration_items<number>::zeta_twopf_item>
          (mgr, writer);
        sqlite3_operations::aggregate_table<number, postintegration_writer<number>, typename postintegration_items<number>::gauge_xfm1_item>
          (mgr, writer);

        mgr.commit();

        timer.stop();
        BOOST_LOG_SEV(writer.get_log(), base_writer::log_severity_level::notification)
          << "** Seeding complete in time " << format_time(timer.elapsed().wall);
      }


    template <typename number>
    void data_manager_sqlite3<number>::seed_writer(postintegration_writer<number>& writer, zeta_threepf_task<number>* tk,
                                                   const content_group_record<postintegration_payload>& seed)
      {
        sqlite3* db = nullptr;
        writer.get_data_manager_handle(&db); // throws an exception if handle is unset, so the return value is guaranteed not to be nullptr

        BOOST_LOG_SEV(writer.get_log(), base_writer::log_severity_level::notification)
          << "** Seeding SQLite3 container '" << writer.get_abs_container_path().string() << "' "
          << "from previous content group '" << seed.get_name() << "', container '" << seed.get_abs_output_path().string() << "'";

        boost::timer::cpu_timer timer;
        auto seed_container_path = seed.get_abs_repo_path() / seed.get_payload().get_container_path();

        sqlite3_operations::attach_manager mgr{db, seed_container_path};

        sqlite3_operations::aggregate_table<number, postintegration_writer<number>, typename postintegration_items<number>::zeta_twopf_item>
          (mgr, writer);
        sqlite3_operations::aggregate_table<number, postintegration_writer<number>, typename postintegration_items<number>::gauge_xfm1_item>
          (mgr, writer);

        sqlite3_operations::aggregate_table<number, postintegration_writer<number>, typename postintegration_items<number>::zeta_threepf_item>
          (mgr, writer);
        sqlite3_operations::aggregate_table<number, postintegration_writer<number>, typename postintegration_items<number>::gauge_xfm2_123_item>
          (mgr, writer);
        sqlite3_operations::aggregate_table<number, postintegration_writer<number>, typename postintegration_items<number>::gauge_xfm2_213_item>
          (mgr, writer);
        sqlite3_operations::aggregate_table<number, postintegration_writer<number>, typename postintegration_items<number>::gauge_xfm2_312_item>
          (mgr, writer);

        mgr.commit();

        timer.stop();
        BOOST_LOG_SEV(writer.get_log(), base_writer::log_severity_level::notification)
          << "** Seeding complete in time " << format_time(timer.elapsed().wall);
      }


    template <typename number>
    void data_manager_sqlite3<number>::seed_writer(postintegration_writer<number>& writer, fNL_task<number>* tk,
                                                   const content_group_record<postintegration_payload>& seed)
      {
        assert(false);
      }


    // BATCHERS


    template <typename number, typename Batcher, typename Item>
    class MakePagedWriter
      {
      public:
        auto operator()()
          {
            return std::bind(
              &sqlite3_operations::write_paged_output<number, Batcher, Item>,
              std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
          }
      };


    template <typename number, typename Batcher, typename Item>
    class MakeUnpagedWriter
      {
      public:
        auto operator()()
          {
            return std::bind(
              &sqlite3_operations::write_unpaged<number, Batcher, Item>,
              std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
          }
      };


    template <typename number, typename Item>
    class MakeCoordinateWriter
      {
      public:
        auto operator()()
          {
            return std::bind(
              &sqlite3_operations::write_coordinate_output<number, Item>,
              std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
          }
      };


    template <typename number>
    class MakeStatsWriter
      {
      public:
        auto operator()()
          {
            return std::bind(
              &sqlite3_operations::write_stats<number>,
              std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
          }
      };


    template <typename number>
    class MakeHostWriter
      {
      public:
        auto operator()()
          {
            return std::bind(
              &sqlite3_operations::write_host_info<number>, std::placeholders::_1, std::placeholders::_2);
          }
      };


    template <typename number>
    sqlite3* data_manager_sqlite3<number>::make_temp_container(const boost::filesystem::path& container)
      {
        sqlite3* db = nullptr;
        int status = sqlite3_open_v2(container.string().c_str(), &db, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, nullptr);

        if(status != SQLITE_OK)
          {
            std::ostringstream msg;
            if(db != nullptr)
              {
                msg << CPPTRANSPORT_DATACTR_TEMPCTR_FAIL_A << " '" << container.string() << "' "
                    << CPPTRANSPORT_DATACTR_TEMPCTR_FAIL_B << status << ": " << sqlite3_errmsg(db) << ")";
                sqlite3_close(db);
              }
            else
              {
                msg << CPPTRANSPORT_DATACTR_TEMPCTR_FAIL_A << " '" << container.string() << "' "
                    << CPPTRANSPORT_DATACTR_TEMPCTR_FAIL_B << status << ")";
              }
            throw runtime_exception(exception_type::DATA_CONTAINER_ERROR, msg.str());
          }

        sqlite3_extended_result_codes(db, 1);

#ifdef CPPTRANSPORT_STRICT_CONSISTENCY
        sqlite3_operations::consistency_pragmas(db, this->args.get_network_mode());
#else
        // change setting to optimize SQLite performance
        sqlite3_operations::container_write_pragmas(db, this->args.get_network_mode());
#endif

        return(db);
      }


    template <typename number>
    twopf_batcher<number>
    data_manager_sqlite3<number>::create_temp_twopf_container(twopf_task<number>* tk, const boost::filesystem::path& tempdir, const boost::filesystem::path& logdir,
                                                              unsigned int worker, unsigned int group,
                                                              model<number>* m, std::unique_ptr<container_dispatch_function> dispatcher)
      {
        auto container = this->generate_temporary_container_path(tempdir, worker);
        auto lockfile = this->generate_lockfile_path(tempdir, worker);

        sqlite3* db = this->make_temp_container(container);

        // create the necessary tables
        auto mgr = this->transaction_factory(db, lockfile);
        this->make_temp_twopf_tables(mgr, db, m->get_N_fields(), m->supports_per_configuration_statistics(), tk->get_collect_initial_conditions());
        mgr.commit();

        // set up writers
        using items = integration_items<number>;
        
        typename twopf_batcher<number>::writer_group writers{
          [this, lockfile](integration_batcher<number>* b) -> transaction_manager
            { sqlite3* h = nullptr; b->get_manager_handle(&h); return this->transaction_factory(h, lockfile); },
          MakeCoordinateWriter<number, typename items::backg_item>()(),
          MakePagedWriter<number, integration_batcher<number>, typename items::twopf_re_item>()(),
          MakePagedWriter<number, integration_batcher<number>, typename items::twopf_si_re_item>()(),
          MakePagedWriter<number, integration_batcher<number>, typename items::tensor_twopf_item>()(),
          MakePagedWriter<number, integration_batcher<number>, typename items::tensor_twopf_si_item>()(),
          MakeStatsWriter<number>()(),
          MakeCoordinateWriter<number, typename items::ics_item>()(),
          MakeHostWriter<number>()()
        };

        // set up a replacement function
        auto replacer = std::make_unique< sqlite3_container_replace_twopf<number> >(*this, tempdir, worker, m, tk->get_collect_initial_conditions());

        // set up batcher
        twopf_batcher<number> batcher{this->get_batcher_capacity(), this->get_checkpoint_interval(), m, tk, container, logdir, writers, std::move(dispatcher), std::move(replacer), db, worker, group};

        BOOST_LOG_SEV(batcher.get_log(), generic_batcher::log_severity_level::normal) << "** Created new temporary twopf container " << container;

        // add this database to our list of open connections
        this->open_containers.push_back(db);

        return(batcher);
      }


    template <typename number>
    void data_manager_sqlite3<number>::make_temp_twopf_tables(transaction_manager& mgr, sqlite3* db, unsigned int Nfields, bool statistics, bool ics)
      {
        sqlite3_operations::create_worker_info_table(mgr, db, sqlite3_operations::foreign_keys_type::no_foreign_keys);
        if(statistics) sqlite3_operations::create_stats_table
          (mgr, db, sqlite3_operations::foreign_keys_type::no_foreign_keys,sqlite3_operations::kconfiguration_type::twopf_configs);
        if(ics) sqlite3_operations::create_ics_table<number, typename integration_items<number>::ics_item>
          (mgr, db, Nfields, sqlite3_operations::foreign_keys_type::no_foreign_keys, sqlite3_operations::kconfiguration_type::twopf_configs);
        sqlite3_operations::create_backg_table<number, typename integration_items<number>::backg_item>
          (mgr, db, Nfields, sqlite3_operations::foreign_keys_type::no_foreign_keys);
        sqlite3_operations::create_paged_table<number, typename integration_items<number>::twopf_re_item>
          (mgr, db, Nfields, sqlite3_operations::foreign_keys_type::no_foreign_keys);
        sqlite3_operations::create_paged_table<number, typename integration_items<number>::twopf_si_re_item>
          (mgr, db, Nfields, sqlite3_operations::foreign_keys_type::no_foreign_keys);
        sqlite3_operations::create_paged_table<number, typename integration_items<number>::tensor_twopf_item>
          (mgr, db, Nfields, sqlite3_operations::foreign_keys_type::no_foreign_keys);
        sqlite3_operations::create_paged_table<number, typename integration_items<number>::tensor_twopf_si_item>
          (mgr, db, Nfields, sqlite3_operations::foreign_keys_type::no_foreign_keys);
      }

    
    template <typename number>
    threepf_batcher<number>
    data_manager_sqlite3<number>::create_temp_threepf_container(threepf_task<number>* tk, const boost::filesystem::path& tempdir, const boost::filesystem::path& logdir,
                                                                unsigned int worker, unsigned int group,
                                                                model<number>* m, std::unique_ptr<container_dispatch_function> dispatcher)
      {
        auto container = this->generate_temporary_container_path(tempdir, worker);
        auto lockfile = this->generate_lockfile_path(tempdir, worker);

        sqlite3* db = this->make_temp_container(container);

        // create the necessary tables
        auto mgr = this->transaction_factory(db, lockfile);
        this->make_temp_threepf_tables(mgr, db, m->get_N_fields(), m->supports_per_configuration_statistics(), tk->get_collect_initial_conditions());
        mgr.commit();

        // set up writers
        using items = integration_items<number>;

        typename threepf_batcher<number>::writer_group writers{
          [this, lockfile](integration_batcher<number>* b) -> transaction_manager
            { sqlite3* h = nullptr; b->get_manager_handle(&h); return this->transaction_factory(h, lockfile); },
          MakeCoordinateWriter<number, typename items::backg_item>()(),
          MakePagedWriter<number, integration_batcher<number>, typename items::twopf_re_item>()(),
          MakePagedWriter<number, integration_batcher<number>, typename items::twopf_im_item>()(),
          MakePagedWriter<number, integration_batcher<number>, typename items::twopf_si_re_item>()(),
          MakePagedWriter<number, integration_batcher<number>, typename items::tensor_twopf_item>()(),
          MakePagedWriter<number, integration_batcher<number>, typename items::tensor_twopf_si_item>()(),
          MakePagedWriter<number, integration_batcher<number>, typename items::threepf_momentum_item>()(),
          MakePagedWriter<number, integration_batcher<number>, typename items::threepf_Nderiv_item>()(),
          MakeStatsWriter<number>()(),
          MakeCoordinateWriter<number, typename items::ics_item>()(),
          MakeCoordinateWriter<number, typename items::ics_kt_item>()(),
          MakeHostWriter<number>()()
        };

        // set up a replacement function
        auto replacer = std::make_unique< sqlite3_container_replace_threepf<number> >(*this, tempdir, worker, m, tk->get_collect_initial_conditions());

        // set up batcher
        threepf_batcher<number> batcher{this->get_batcher_capacity(), this->get_checkpoint_interval(), m, tk, container, logdir, writers, std::move(dispatcher), std::move(replacer), db, worker, group};
        BOOST_LOG_SEV(batcher.get_log(), generic_batcher::log_severity_level::normal) << "** Created new temporary threepf container " << container;

        // add this database to our list of open connections
        this->open_containers.push_back(db);

        return(batcher);
      }


    template <typename number>
    void data_manager_sqlite3<number>::make_temp_threepf_tables(transaction_manager& mgr, sqlite3* db, unsigned int Nfields, bool statistics, bool ics)
      {
        sqlite3_operations::create_worker_info_table(mgr, db, sqlite3_operations::foreign_keys_type::no_foreign_keys);
        if(statistics) sqlite3_operations::create_stats_table
          (mgr, db, sqlite3_operations::foreign_keys_type::no_foreign_keys,sqlite3_operations::kconfiguration_type::threepf_configs);
        if(ics)
          {
            sqlite3_operations::create_ics_table<number, typename integration_items<number>::ics_item>
              (mgr, db, Nfields, sqlite3_operations::foreign_keys_type::no_foreign_keys, sqlite3_operations::kconfiguration_type::threepf_configs);
            sqlite3_operations::create_ics_table<number, typename integration_items<number>::ics_kt_item>
              (mgr, db, Nfields, sqlite3_operations::foreign_keys_type::no_foreign_keys, sqlite3_operations::kconfiguration_type::threepf_configs);
          }
        sqlite3_operations::create_backg_table<number, typename integration_items<number>::backg_item>
          (mgr, db, Nfields, sqlite3_operations::foreign_keys_type::no_foreign_keys);
        sqlite3_operations::create_paged_table<number, typename integration_items<number>::twopf_re_item>
          (mgr, db, Nfields, sqlite3_operations::foreign_keys_type::no_foreign_keys);
        sqlite3_operations::create_paged_table<number, typename integration_items<number>::twopf_im_item>
          (mgr, db, Nfields, sqlite3_operations::foreign_keys_type::no_foreign_keys);
        sqlite3_operations::create_paged_table<number, typename integration_items<number>::twopf_si_re_item>
          (mgr, db, Nfields, sqlite3_operations::foreign_keys_type::no_foreign_keys);
        sqlite3_operations::create_paged_table<number, typename integration_items<number>::tensor_twopf_item>
          (mgr, db, Nfields, sqlite3_operations::foreign_keys_type::no_foreign_keys);
        sqlite3_operations::create_paged_table<number, typename integration_items<number>::tensor_twopf_si_item>
          (mgr, db, Nfields, sqlite3_operations::foreign_keys_type::no_foreign_keys);
        sqlite3_operations::create_paged_table<number, typename integration_items<number>::threepf_momentum_item>
          (mgr, db, Nfields, sqlite3_operations::foreign_keys_type::no_foreign_keys, sqlite3_operations::kconfiguration_type::threepf_configs);
        sqlite3_operations::create_paged_table<number, typename integration_items<number>::threepf_Nderiv_item>
          (mgr, db, Nfields, sqlite3_operations::foreign_keys_type::no_foreign_keys, sqlite3_operations::kconfiguration_type::threepf_configs);
      }


    template <typename number>
    zeta_twopf_batcher<number>
    data_manager_sqlite3<number>::create_temp_zeta_twopf_container(zeta_twopf_task<number>* tk, const boost::filesystem::path& tempdir,
                                                                   const boost::filesystem::path& logdir, unsigned int worker,
                                                                   model<number>* m, std::unique_ptr<container_dispatch_function> dispatcher)
      {
        auto container = this->generate_temporary_container_path(tempdir, worker);
        auto lockfile = this->generate_lockfile_path(tempdir, worker);

        sqlite3* db = this->make_temp_container(container);

        auto mgr = this->transaction_factory(db, lockfile);
        this->make_temp_zeta_twopf_tables(mgr, db, m->get_N_fields());
        mgr.commit();

        // set up writers
        using items = postintegration_items<number>;
        
        typename zeta_twopf_batcher<number>::writer_group writers{
          [this, lockfile](postintegration_batcher<number>* b) -> transaction_manager
            { sqlite3* h = nullptr; b->get_manager_handle(&h); return this->transaction_factory(h, lockfile); },
          MakeUnpagedWriter<number, postintegration_batcher<number>, typename items::zeta_twopf_item>()(),
          MakePagedWriter<number, postintegration_batcher<number>, typename items::gauge_xfm1_item>()()
        };

        // set up replacement function
        auto replacer = std::make_unique< sqlite3_container_replace_zeta_twopf<number> >(*this, tempdir, worker, m);

        // set up batcher
        zeta_twopf_batcher<number> batcher{this->get_batcher_capacity(), this->get_checkpoint_interval(), m, tk, container, logdir, writers, std::move(dispatcher), std::move(replacer), db, worker};

        BOOST_LOG_SEV(batcher.get_log(), generic_batcher::log_severity_level::normal) << "** Created new temporary zeta twopf container " << container;

        // add this database to our list of open connections
        this->open_containers.push_back(db);

        return(batcher);
      }


    template <typename number>
    void data_manager_sqlite3<number>::make_temp_zeta_twopf_tables(transaction_manager& mgr, sqlite3* db, unsigned int Nfields)
      {
        sqlite3_operations::create_zeta_twopf_table(mgr, db, sqlite3_operations::foreign_keys_type::no_foreign_keys);
        sqlite3_operations::create_paged_table<number, typename postintegration_items<number>::gauge_xfm1_item>(mgr, db, Nfields,
                                                                                                                sqlite3_operations::foreign_keys_type::no_foreign_keys);
      }


    template <typename number>
    zeta_threepf_batcher<number>
    data_manager_sqlite3<number>::create_temp_zeta_threepf_container(zeta_threepf_task<number>* tk, const boost::filesystem::path& tempdir, const boost::filesystem::path& logdir, unsigned int worker,
                                                                     model<number>* m, std::unique_ptr<container_dispatch_function> dispatcher)
      {
        auto container = this->generate_temporary_container_path(tempdir, worker);
        auto lockfile = this->generate_lockfile_path(tempdir, worker);

        sqlite3* db = this->make_temp_container(container);

        auto mgr = this->transaction_factory(db, lockfile);
        this->make_temp_zeta_threepf_tables(mgr, db, m->get_N_fields());
        mgr.commit();

        // set up writers
        using items = postintegration_items<number>;

        typename zeta_threepf_batcher<number>::writer_group writers{
          [this, lockfile](postintegration_batcher<number>* b) -> transaction_manager
            { sqlite3* h = nullptr; b->get_manager_handle(&h); return this->transaction_factory(h, lockfile); },
          MakeUnpagedWriter<number, postintegration_batcher<number>, typename items::zeta_twopf_item>()(),
          MakeUnpagedWriter<number, postintegration_batcher<number>, typename items::zeta_threepf_item>()(),
          MakePagedWriter<number, postintegration_batcher<number>, typename items::gauge_xfm1_item>()(),
          MakePagedWriter<number, postintegration_batcher<number>, typename items::gauge_xfm2_123_item>()(),
          MakePagedWriter<number, postintegration_batcher<number>, typename items::gauge_xfm2_213_item>()(),
          MakePagedWriter<number, postintegration_batcher<number>, typename items::gauge_xfm2_312_item>()(),
        };

        // set up replacement function
        auto replacer = std::make_unique< sqlite3_container_replace_zeta_threepf<number> >(*this, tempdir, worker, m);

        // set up batcher
        zeta_threepf_batcher<number> batcher{this->get_batcher_capacity(), this->get_checkpoint_interval(), m, tk, container, logdir, writers, std::move(dispatcher), std::move(replacer), db, worker};

        BOOST_LOG_SEV(batcher.get_log(), generic_batcher::log_severity_level::normal) << "** Created new temporary zeta threepf container " << container;

        // add this database to our list of open connections
        this->open_containers.push_back(db);

        return(batcher);
      }


    template <typename number>
    void data_manager_sqlite3<number>::make_temp_zeta_threepf_tables(transaction_manager& mgr, sqlite3* db, unsigned int Nfields)
      {
        sqlite3_operations::create_zeta_twopf_table(mgr, db, sqlite3_operations::foreign_keys_type::no_foreign_keys);
        sqlite3_operations::create_zeta_threepf_table(mgr, db, sqlite3_operations::foreign_keys_type::no_foreign_keys);
        sqlite3_operations::create_paged_table<number, typename postintegration_items<number>::gauge_xfm1_item>(mgr, db, Nfields, sqlite3_operations::foreign_keys_type::no_foreign_keys);
        sqlite3_operations::create_paged_table<number, typename postintegration_items<number>::gauge_xfm2_123_item>(mgr, db, Nfields, sqlite3_operations::foreign_keys_type::no_foreign_keys);
        sqlite3_operations::create_paged_table<number, typename postintegration_items<number>::gauge_xfm2_213_item>(mgr, db, Nfields, sqlite3_operations::foreign_keys_type::no_foreign_keys);
        sqlite3_operations::create_paged_table<number, typename postintegration_items<number>::gauge_xfm2_312_item>(mgr, db, Nfields, sqlite3_operations::foreign_keys_type::no_foreign_keys);
      }


    template <typename number>
    class MakeFNLWriter
      {
      public:
        template <typename T>
        auto operator()(T type)
          {
            return(
              std::bind(
                &sqlite3_operations::write_fNL<number>,
                  std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, type)
            );
          }
      };


    template <typename number>
    fNL_batcher<number>
    data_manager_sqlite3<number>::create_temp_fNL_container(fNL_task<number>* tk, const boost::filesystem::path& tempdir, const boost::filesystem::path& logdir, unsigned int worker,
                                                            model<number>* m, std::unique_ptr<container_dispatch_function> dispatcher, derived_data::bispectrum_template type)
      {
        auto container = this->generate_temporary_container_path(tempdir, worker);
        auto lockfile = this->generate_lockfile_path(tempdir, worker);

        sqlite3* db = this->make_temp_container(container);

        auto mgr = this->transaction_factory(db, lockfile);
        this->make_temp_fNL_tables(mgr, db, type);
        mgr.commit();

        // set up writers
        typename fNL_batcher<number>::writer_group writers{
          [this, lockfile](postintegration_batcher<number>* b) -> transaction_manager
            { sqlite3* h = nullptr; b->get_manager_handle(&h); return this->transaction_factory(h, lockfile); },
          MakeFNLWriter<number>()(type)
        };

        // set up replacement function
        auto replacer = std::make_unique< sqlite3_container_replace_fNL<number> >(*this, tempdir, worker, type);

        // set up batcher
        fNL_batcher<number> batcher{this->get_batcher_capacity(), this->get_checkpoint_interval(), m, tk, container, logdir, writers, std::move(dispatcher), std::move(replacer), db, worker, type};

        BOOST_LOG_SEV(batcher.get_log(), generic_batcher::log_severity_level::normal) << "** Created new temporary " <<
            derived_data::template_type_to_string(type) << " container " << container;

        // add this database to our list of open connections
        this->open_containers.push_back(db);

        return(batcher);
      }


    template <typename number>
    void data_manager_sqlite3<number>::make_temp_fNL_tables(transaction_manager& mgr, sqlite3* db, derived_data::bispectrum_template type)
      {
        sqlite3_operations::create_fNL_table(mgr, db, type, sqlite3_operations::foreign_keys_type::no_foreign_keys);
      }


    template <typename number>
    void data_manager_sqlite3<number>::replace_temp_twopf_container(const boost::filesystem::path& tempdir, unsigned int worker, model<number>* m,
                                                                    bool ics, generic_batcher& batcher, replacement_action action)
      {
        sqlite3* db = nullptr;
    
        BOOST_LOG_SEV(batcher.get_log(), generic_batcher::log_severity_level::normal)
          << "** " << (action == replacement_action::action_replace ? std::string{"Replacing"} : std::string{"Closing"})
          << " temporary twopf container " << batcher.get_container_path();

        batcher.get_manager_handle(&db);
        this->open_containers.remove(db);
        sqlite3_close(db);

        BOOST_LOG_SEV(batcher.get_log(), generic_batcher::log_severity_level::normal) << "** Closed SQLite3 handle for " << batcher.get_container_path();

        if(action == replacement_action::action_replace)
          {
            auto container = this->generate_temporary_container_path(tempdir, worker);
            auto lockfile = this->generate_lockfile_path(tempdir, worker);

            BOOST_LOG_SEV(batcher.get_log(), generic_batcher::log_severity_level::normal) << "** Opening new twopf container " << container;

            sqlite3* new_db = this->make_temp_container(container);
            auto mgr = this->transaction_factory(new_db, lockfile);
            this->make_temp_twopf_tables(mgr, new_db, m->get_N_fields(), m->supports_per_configuration_statistics(), ics);
            mgr.commit();

            batcher.set_container_path(container);
            batcher.set_manager_handle(new_db);

            this->open_containers.push_back(new_db);
          }
      }


    template <typename number>
    void data_manager_sqlite3<number>::replace_temp_threepf_container(const boost::filesystem::path& tempdir, unsigned int worker, model<number>* m,
                                                                      bool ics, generic_batcher& batcher, replacement_action action)
      {
        sqlite3* db = nullptr;

        BOOST_LOG_SEV(batcher.get_log(), generic_batcher::log_severity_level::normal)
          << "** " << (action == replacement_action::action_replace ? std::string{"Replacing"} : std::string{"Closing"})
          << " temporary threepf container " << batcher.get_container_path();

        batcher.get_manager_handle(&db);
        this->open_containers.remove(db);
        sqlite3_close(db);

        BOOST_LOG_SEV(batcher.get_log(), generic_batcher::log_severity_level::normal) << "** Closed SQLite3 handle for " << batcher.get_container_path();

        if(action == replacement_action::action_replace)
          {
            auto container = this->generate_temporary_container_path(tempdir, worker);
            auto lockfile = this->generate_lockfile_path(tempdir, worker);

            BOOST_LOG_SEV(batcher.get_log(), generic_batcher::log_severity_level::normal) << "** Opening new threepf container " << container;

            sqlite3* new_db = this->make_temp_container(container);
            auto mgr = this->transaction_factory(new_db, lockfile);
            this->make_temp_threepf_tables(mgr, new_db, m->get_N_fields(), m->supports_per_configuration_statistics(), ics);
            mgr.commit();

            batcher.set_container_path(container);
            batcher.set_manager_handle(new_db);

            this->open_containers.push_back(new_db);
          }
      }


    template <typename number>
    void data_manager_sqlite3<number>::replace_temp_zeta_twopf_container(const boost::filesystem::path& tempdir, unsigned int worker, model<number>* m,
                                                                         generic_batcher& batcher, replacement_action action)
      {
        sqlite3* db = nullptr;

        BOOST_LOG_SEV(batcher.get_log(), generic_batcher::log_severity_level::normal)
          << "** " << (action == replacement_action::action_replace ? std::string{"Replacing"} : std::string{"Closing"})
          << " temporary zeta twopf container " << batcher.get_container_path();

        batcher.get_manager_handle(&db);
        this->open_containers.remove(db);
        sqlite3_close(db);

        BOOST_LOG_SEV(batcher.get_log(), generic_batcher::log_severity_level::normal) << "** Closed SQLite3 handle for " << batcher.get_container_path();

        if(action == replacement_action::action_replace)
          {
            auto container = this->generate_temporary_container_path(tempdir, worker);
            auto lockfile = this->generate_lockfile_path(tempdir, worker);

            BOOST_LOG_SEV(batcher.get_log(), generic_batcher::log_severity_level::normal) << "** Opening new zeta twopf container " << container;

            sqlite3* new_db = this->make_temp_container(container);
            auto mgr = this->transaction_factory(new_db, lockfile);
            this->make_temp_zeta_twopf_tables(mgr, new_db, m->get_N_fields());
            mgr.commit();

            batcher.set_container_path(container);
            batcher.set_manager_handle(new_db);

            this->open_containers.push_back(new_db);
          }
      }


    template <typename number>
    void data_manager_sqlite3<number>::replace_temp_zeta_threepf_container(const boost::filesystem::path& tempdir, unsigned int worker, model<number>* m,
                                                                           generic_batcher& batcher, replacement_action action)
      {
        sqlite3* db = nullptr;

        BOOST_LOG_SEV(batcher.get_log(), generic_batcher::log_severity_level::normal)
          << "** " << (action == replacement_action::action_replace ? std::string{"Replacing"} : std::string{"Closing"})
          << " temporary zeta threepf container " << batcher.get_container_path();

        batcher.get_manager_handle(&db);
        this->open_containers.remove(db);
        sqlite3_close(db);

        BOOST_LOG_SEV(batcher.get_log(), generic_batcher::log_severity_level::normal) << "** Closed SQLite3 handle for " << batcher.get_container_path();

        if(action == replacement_action::action_replace)
          {
            auto container = this->generate_temporary_container_path(tempdir, worker);
            auto lockfile = this->generate_lockfile_path(tempdir, worker);

            BOOST_LOG_SEV(batcher.get_log(), generic_batcher::log_severity_level::normal) << "** Opening new zeta threepf container " << container;

            sqlite3* new_db = this->make_temp_container(container);
            auto mgr = this->transaction_factory(new_db, lockfile);
            this->make_temp_zeta_threepf_tables(mgr, new_db, m->get_N_fields());
            mgr.commit();

            batcher.set_container_path(container);
            batcher.set_manager_handle(new_db);

            this->open_containers.push_back(new_db);
          }
      }


    template <typename number>
    void data_manager_sqlite3<number>::replace_temp_fNL_container(const boost::filesystem::path& tempdir, unsigned int worker, derived_data::bispectrum_template type,
                                                                  generic_batcher& batcher, replacement_action action)
      {
        sqlite3* db = nullptr;

        BOOST_LOG_SEV(batcher.get_log(), generic_batcher::log_severity_level::normal)
          << "** " << (action == replacement_action::action_replace ? std::string{"Replacing"} : std::string{"Closing"})
          << " temporary " << derived_data::template_type_to_string(type) << " container " << batcher.get_container_path();

        batcher.get_manager_handle(&db);
        this->open_containers.remove(db);
        sqlite3_close(db);

        BOOST_LOG_SEV(batcher.get_log(), generic_batcher::log_severity_level::normal) << "** Closed SQLite3 handle for " << batcher.get_container_path();

        if(action == replacement_action::action_replace)
          {
            auto container = this->generate_temporary_container_path(tempdir, worker);
            auto lockfile = this->generate_lockfile_path(tempdir, worker);

            BOOST_LOG_SEV(batcher.get_log(), generic_batcher::log_severity_level::normal) << "** Opening new fNL container " << container;

            sqlite3* new_db = this->make_temp_container(container);
            auto mgr = this->transaction_factory(new_db, lockfile);
            this->make_temp_fNL_tables(mgr, new_db, type);
            mgr.commit();

            batcher.set_container_path(container);
            batcher.set_manager_handle(new_db);

            this->open_containers.push_back(new_db);
          }
      }


    template <typename number>
    boost::filesystem::path data_manager_sqlite3<number>::generate_temporary_container_path(const boost::filesystem::path& tempdir, unsigned int worker)
      {
        std::ostringstream container_name;
        container_name << CPPTRANSPORT_TEMPORARY_CONTAINER_STEM << worker << "_" << this->temporary_container_serial++ << CPPTRANSPORT_TEMPORARY_CONTAINER_XTN;

        return(tempdir / container_name.str());
      }


    template <typename number>
    boost::filesystem::path data_manager_sqlite3<number>::generate_lockfile_path(const boost::filesystem::path& tempdir, unsigned int worker)
      {
        std::ostringstream lockfile_name;
        lockfile_name << CPPTRANSPORT_DATAMGR_LOCKFILE_LEAF << "_" << worker;

        return(tempdir / lockfile_name.str());
      }


    template <typename number>
    bool data_manager_sqlite3<number>::aggregate_twopf_batch(integration_writer<number>& writer, const boost::filesystem::path& temp_ctr)
      {
        sqlite3* db = nullptr;
        writer.get_data_manager_handle(&db); // throws an exception if handle is unset, so the return value is guaranteed not to be nullptr

        auto record = std::make_unique< twopf_aggregation_profile_record >(writer.get_abs_container_path(), temp_ctr);
        sqlite3_operations::attach_manager mgr{db, temp_ctr, *record};

        record->backg           = sqlite3_operations::aggregate_backg<number>(mgr, writer);
        record->twopf_re        = sqlite3_operations::aggregate_table<number, integration_writer<number>, typename integration_items<number>::twopf_re_item>(mgr, writer);
        record->twopf_si_re     = sqlite3_operations::aggregate_table<number, integration_writer<number>, typename integration_items<number>::twopf_si_re_item>(mgr, writer);
        record->tensor_twopf    = sqlite3_operations::aggregate_table<number, integration_writer<number>, typename integration_items<number>::tensor_twopf_item>(mgr, writer);
        record->tensor_twopf_si = sqlite3_operations::aggregate_table<number, integration_writer<number>, typename integration_items<number>::tensor_twopf_si_item>(mgr, writer);

        record->workers = sqlite3_operations::aggregate_workers<number>(mgr, writer);
        if(writer.is_collecting_statistics()) record->statistics = sqlite3_operations::aggregate_statistics<number>(mgr, writer);

        if(writer.is_collecting_initial_conditions())
          record->ics = sqlite3_operations::aggregate_ics<number, typename integration_items<number>::ics_item>(mgr, writer);

        // commit aggregation and report profiling data
        mgr.commit();
        record->stop();
        writer.get_aggregation_profiler().add_record(std::move(record));

        return(true);
      }


    template <typename number>
    bool data_manager_sqlite3<number>::aggregate_threepf_batch(integration_writer<number>& writer, const boost::filesystem::path& temp_ctr)
      {
        sqlite3* db = nullptr;
        writer.get_data_manager_handle(&db); // throws an exception if handle is unset, so the return value is guaranteed not to be nullptr

        auto record = std::make_unique< threepf_aggregation_profile_record >(writer.get_abs_container_path(), temp_ctr);
        sqlite3_operations::attach_manager mgr{db, temp_ctr, *record};

        record->backg            = sqlite3_operations::aggregate_backg<number>(mgr, writer);
        record->twopf_re         = sqlite3_operations::aggregate_table<number, integration_writer<number>, typename integration_items<number>::twopf_re_item>(mgr, writer);
        record->twopf_im         = sqlite3_operations::aggregate_table<number, integration_writer<number>, typename integration_items<number>::twopf_im_item>(mgr, writer);
        record->twopf_si_re      = sqlite3_operations::aggregate_table<number, integration_writer<number>, typename integration_items<number>::twopf_si_re_item>(mgr, writer);
        record->tensor_twopf     = sqlite3_operations::aggregate_table<number, integration_writer<number>, typename integration_items<number>::tensor_twopf_item>(mgr, writer);
        record->tensor_twopf_si  = sqlite3_operations::aggregate_table<number, integration_writer<number>, typename integration_items<number>::tensor_twopf_si_item>(mgr, writer);
        record->threepf_momentum = sqlite3_operations::aggregate_table<number, integration_writer<number>, typename integration_items<number>::threepf_momentum_item>(mgr, writer);
        record->threepf_Nderiv   = sqlite3_operations::aggregate_table<number, integration_writer<number>, typename integration_items<number>::threepf_Nderiv_item>(mgr, writer);

        record->workers = sqlite3_operations::aggregate_workers<number>(mgr, writer);
        if(writer.is_collecting_statistics()) record->statistics = sqlite3_operations::aggregate_statistics<number>(mgr, writer);

        if(writer.is_collecting_initial_conditions())
          {
            record->ics    = sqlite3_operations::aggregate_ics<number, typename integration_items<number>::ics_item>(mgr, writer);
            record->ics_kt = sqlite3_operations::aggregate_ics<number, typename integration_items<number>::ics_kt_item>(mgr, writer);
          }

        // commit aggregation and report profiling data
        mgr.commit();
        record->stop();
        writer.get_aggregation_profiler().add_record(std::move(record));

        return(true);
      }


    template <typename number>
    bool data_manager_sqlite3<number>::aggregate_zeta_twopf_batch(postintegration_writer<number>& writer, const boost::filesystem::path& temp_ctr)
      {
        sqlite3* db = nullptr;
        writer.get_data_manager_handle(&db); // throws an exception if handle is unset, so the return value is guaranteed not to be nullptr

        auto record = std::make_unique< zeta_twopf_aggregation_profile_record >(writer.get_abs_container_path(), temp_ctr);
        sqlite3_operations::attach_manager mgr{db, temp_ctr, *record};

        record->twopf      = sqlite3_operations::aggregate_table<number, postintegration_writer<number>, typename postintegration_items<number>::zeta_twopf_item>(mgr, writer);
        record->gauge_xfm1 = sqlite3_operations::aggregate_table<number, postintegration_writer<number>, typename postintegration_items<number>::gauge_xfm1_item>(mgr, writer);

        // commit aggregation and report profiling data
        mgr.commit();
        record->stop();
        writer.get_aggregation_profiler().add_record(std::move(record));

        return(true);
      }


    template <typename number>
    bool data_manager_sqlite3<number>::aggregate_zeta_threepf_batch(postintegration_writer<number>& writer, const boost::filesystem::path& temp_ctr)
      {
        sqlite3* db = nullptr;
        writer.get_data_manager_handle(&db); // throws an exception if handle is unset, so the return value is guaranteed not to be nullptr

        auto record = std::make_unique< zeta_threepf_aggregation_profile_record >(writer.get_abs_container_path(), temp_ctr);
        sqlite3_operations::attach_manager mgr{db, temp_ctr, *record};

        record->twopf          = sqlite3_operations::aggregate_table<number, postintegration_writer<number>, typename postintegration_items<number>::zeta_twopf_item>(mgr, writer);
        record->threepf        = sqlite3_operations::aggregate_table<number, postintegration_writer<number>, typename postintegration_items<number>::zeta_threepf_item>(mgr, writer);
        record->gauge_xfm1     = sqlite3_operations::aggregate_table<number, postintegration_writer<number>, typename postintegration_items<number>::gauge_xfm1_item>(mgr, writer);
        record->gauge_xfm2_123 = sqlite3_operations::aggregate_table<number, postintegration_writer<number>, typename postintegration_items<number>::gauge_xfm2_123_item>(mgr, writer);
        record->gauge_xfm2_213 = sqlite3_operations::aggregate_table<number, postintegration_writer<number>, typename postintegration_items<number>::gauge_xfm2_213_item>(mgr, writer);
        record->gauge_xfm2_312 = sqlite3_operations::aggregate_table<number, postintegration_writer<number>, typename postintegration_items<number>::gauge_xfm2_312_item>(mgr, writer);

        // commit aggregation and report profiling data
        mgr.commit();
        record->stop();
        writer.get_aggregation_profiler().add_record(std::move(record));

        return(true);
      }


    template <typename number>
    bool data_manager_sqlite3<number>::aggregate_fNL_batch(postintegration_writer<number>& writer, const boost::filesystem::path& temp_ctr, derived_data::bispectrum_template type)
      {
        sqlite3* db = nullptr;
        writer.get_data_manager_handle(&db); // throws an exception if handle is unset, so the return value is guaranteed not to be nullptr

        auto record = std::make_unique< fNL_aggregation_profile_record >(writer.get_abs_container_path(), temp_ctr);
        sqlite3_operations::attach_manager mgr{db, temp_ctr, *record};

        record->fNL = sqlite3_operations::aggregate_fNL<number>(mgr, writer, type);

        // commit aggregation and report profiling data
        mgr.commit();
        record->stop();
        writer.get_aggregation_profiler().add_record(std::move(record));

        return(true);
      }


    template <typename number>
    bool data_manager_sqlite3<number>::aggregate_derived_product(derived_content_writer<number>& writer,
                                                                 const std::string& product_name, const std::list<std::string>& used_groups)
      {
        bool success = true;

        auto product = writer.lookup_derived_product(product_name);

        if(!product)
          {
            BOOST_LOG_SEV(writer.get_log(), base_writer::log_severity_level::error) << "!! Failed to lookup derived product '" << product_name << "'; skipping this product";
            return(false);
          }

        // find output product in the temporary folder
        auto temp_location = writer.get_abs_tempdir_path() / product->get_filename();
        auto dest_location = writer.get_abs_output_path() / product->get_filename();

        if(!boost::filesystem::exists(temp_location))
          {
            BOOST_LOG_SEV(writer.get_log(), base_writer::log_severity_level::error) << "!! Derived product " << temp_location << " missing; skipping this product";
            return(false);
          }

        if(boost::filesystem::exists(dest_location))
          {
            BOOST_LOG_SEV(writer.get_log(), base_writer::log_severity_level::error) << "!! Destination " << dest_location << " for derived product " << temp_location << " already exists; skipping this product";
            return(false);
          }

        boost::filesystem::rename(temp_location, dest_location);

        BOOST_LOG_SEV(writer.get_log(), base_writer::log_severity_level::normal) << "** Emplaced derived product " << dest_location;

        // commit this product to the current content group
        writer.push_content(*product, used_groups);

        return(success);
      }


    // INTEGRITY CHECK


    template <typename number>
    std::set<unsigned int>
    data_manager_sqlite3<number>::get_missing_twopf_re_serials(integration_writer <number>& writer)
      {
        // get sqlite3 handle to principal database
        sqlite3* db = nullptr;
        writer.get_data_manager_handle(&db); // throws an exception if handle is unset, so the return value is guaranteed not to be nullptr

        return sqlite3_operations::get_missing_serials<number, typename integration_items<number>::twopf_re_item>(db);
      }


    template <typename number>
    std::set<unsigned int>
    data_manager_sqlite3<number>::get_missing_twopf_im_serials(integration_writer<number>& writer)
      {
        // get sqlite3 handle to principal database
        sqlite3* db = nullptr;
        writer.get_data_manager_handle(&db); // throws an exception if handle is unset, so the return value is guaranteed not to be nullptr

        return sqlite3_operations::get_missing_serials<number, typename integration_items<number>::twopf_im_item>(db);
      }


    template <typename number>
    std::set<unsigned int>
    data_manager_sqlite3<number>::get_missing_twopf_si_re_serials(integration_writer<number>& writer)
      {
        // get sqlite3 handle to principal database
        sqlite3* db = nullptr;
        writer.get_data_manager_handle(&db); // throws an exception if handle is unset, so the return value is guaranteed not to be nullptr

        return sqlite3_operations::get_missing_serials<number, typename integration_items<number>::twopf_si_re_item>(db);
      }


    template <typename number>
    std::set<unsigned int>
    data_manager_sqlite3<number>::get_missing_tensor_twopf_serials(integration_writer<number>& writer)
      {
        // get sqlite3 handle to principal database
        sqlite3* db = nullptr;
        writer.get_data_manager_handle(&db); // throws an exception if handle is unset, so the return value is guaranteed not to be nullptr

        return sqlite3_operations::get_missing_serials<number, typename integration_items<number>::tensor_twopf_item>(db);
      }


    template <typename number>
    std::set<unsigned int>
    data_manager_sqlite3<number>::get_missing_tensor_twopf_si_serials(integration_writer<number>& writer)
      {
        // get sqlite3 handle to principal database
        sqlite3* db = nullptr;
        writer.get_data_manager_handle(&db); // throws an exception if handle is unset, so the return value is guaranteed not to be nullptr

        return sqlite3_operations::get_missing_serials<number, typename integration_items<number>::tensor_twopf_si_item>(db);
      }


    template <typename number>
    std::set<unsigned int>
    data_manager_sqlite3<number>::get_missing_threepf_momentum_serials(integration_writer <number>& writer)
      {
        // get sqlite3 handle to principal database
        sqlite3* db = nullptr;
        writer.get_data_manager_handle(&db); // throws an exception if handle is unset, so the return value is guaranteed not to be nullptr

        return sqlite3_operations::get_missing_serials<number, typename integration_items<number>::threepf_momentum_item>(db);
      }


    template <typename number>
    std::set<unsigned int>
    data_manager_sqlite3<number>::get_missing_threepf_deriv_serials(integration_writer <number>& writer)
      {
        // get sqlite3 handle to principal database
        sqlite3* db = nullptr;
        writer.get_data_manager_handle(&db); // throws an exception if handle is unset, so the return value is guaranteed not to be nullptr

        return sqlite3_operations::get_missing_serials<number, typename integration_items<number>::threepf_Nderiv_item>(db);
      }


    template <typename number>
    std::set<unsigned int>
    data_manager_sqlite3<number>::get_missing_zeta_twopf_serials(postintegration_writer<number>& writer)
      {
        // get sqlite3 handle to principal database
        sqlite3* db = nullptr;
        writer.get_data_manager_handle(&db); // throws an exception if handle is unset, so the return value is guaranteed not to be nullptr

        return sqlite3_operations::get_missing_serials<number, typename postintegration_items<number>::zeta_twopf_item>(db);
      }


    template <typename number>
    std::set<unsigned int>
    data_manager_sqlite3<number>::get_missing_zeta_threepf_serials(postintegration_writer<number>& writer)
      {
        // get sqlite3 handle to principal database
        sqlite3* db = nullptr;
        writer.get_data_manager_handle(&db); // throws an exception if handle is unset, so the return value is guaranteed not to be nullptr

        return sqlite3_operations::get_missing_serials<number, typename postintegration_items<number>::zeta_threepf_item>(db);
      }


    template <typename number>
    std::set<unsigned int>
    data_manager_sqlite3<number>::get_missing_gauge_xfm1_serials(postintegration_writer<number>& writer)
      {
        // get sqlite3 handle to principal database
        sqlite3* db = nullptr;
        writer.get_data_manager_handle(&db); // throws an exception if handle is unset, so the return value is guaranteed not to be nullptr

        return sqlite3_operations::get_missing_serials<number, typename postintegration_items<number>::gauge_xfm1_item>(db);
      }


    template <typename number>
    std::set<unsigned int>
    data_manager_sqlite3<number>::get_missing_gauge_xfm2_123_serials(postintegration_writer<number>& writer)
      {
        // get sqlite3 handle to principal database
        sqlite3* db = nullptr;
        writer.get_data_manager_handle(&db); // throws an exception if handle is unset, so the return value is guaranteed not to be nullptr

        return sqlite3_operations::get_missing_serials<number, typename postintegration_items<number>::gauge_xfm2_123_item>(db);
      }


    template <typename number>
    std::set<unsigned int>
    data_manager_sqlite3<number>::get_missing_gauge_xfm2_213_serials(postintegration_writer<number>& writer)
      {
        // get sqlite3 handle to principal database
        sqlite3* db = nullptr;
        writer.get_data_manager_handle(&db); // throws an exception if handle is unset, so the return value is guaranteed not to be nullptr

        return sqlite3_operations::get_missing_serials<number, typename postintegration_items<number>::gauge_xfm2_213_item>(db);
      }


    template <typename number>
    std::set<unsigned int>
    data_manager_sqlite3<number>::get_missing_gauge_xfm2_312_serials(postintegration_writer<number>& writer)
      {
        // get sqlite3 handle to principal database
        sqlite3* db = nullptr;
        writer.get_data_manager_handle(&db); // throws an exception if handle is unset, so the return value is guaranteed not to be nullptr

        return sqlite3_operations::get_missing_serials<number, typename postintegration_items<number>::gauge_xfm2_312_item>(db);
      }


    template <typename number>
    void
      data_manager_sqlite3<number>::drop_twopf_re_configurations
        (transaction_manager& mgr, integration_writer<number>& writer, const std::set<unsigned int>& serials,
         const std::set<unsigned int>& missing, const twopf_kconfig_database& dbase)
      {
        // get sqlite3 handle to principal database
        sqlite3* db = nullptr;
        writer.get_data_manager_handle(&db); // throws an exception if handle is unset, so the return value is guaranteed not to be nullptr

        // only drop those serials that are actually missing (otherwise DELETE is expensive)
        std::set<unsigned int> drop_list;
        std::set_difference(serials.begin(), serials.end(), missing.begin(), missing.end(), std::inserter(drop_list, drop_list.begin()));

        sqlite3_operations::drop_k_configurations(mgr, db, writer, drop_list, dbase,
                                                  sqlite3_operations::data_traits<number, typename integration_items<number>::twopf_re_item>::sqlite_table());
      }


    template <typename number>
    void
      data_manager_sqlite3<number>::drop_twopf_im_configurations
      (transaction_manager& mgr, integration_writer<number>& writer, const std::set<unsigned int>& serials,
       const std::set<unsigned int>& missing, const twopf_kconfig_database& dbase)
      {
        // get sqlite3 handle to principal database
        sqlite3* db = nullptr;
        writer.get_data_manager_handle(&db); // throws an exception if handle is unset, so the return value is guaranteed not to be nullptr

        // only drop those serials that are actually missing (otherwise DELETE is expensive)
        std::set<unsigned int> drop_list;
        std::set_difference(serials.begin(), serials.end(), missing.begin(), missing.end(), std::inserter(drop_list, drop_list.begin()));

        sqlite3_operations::drop_k_configurations(mgr, db, writer, drop_list, dbase,
                                                  sqlite3_operations::data_traits<number, typename integration_items<number>::twopf_im_item>::sqlite_table());
      }


    template <typename number>
    void
    data_manager_sqlite3<number>::drop_twopf_si_re_configurations
      (transaction_manager& mgr, integration_writer <number>& writer, const std::set<unsigned int>& serials,
       const std::set<unsigned int>& missing, const twopf_kconfig_database& dbase)
      {
        // get sqlite3 handle to principal database
        sqlite3* db = nullptr;
        writer.get_data_manager_handle(&db); // throws an exception if handle is unset, so the return value is guaranteed not to be nullptr

        // only drop those serials that are actually missing (otherwise DELETE is expensive)
        std::set<unsigned int> drop_list;
        std::set_difference(serials.begin(), serials.end(), missing.begin(), missing.end(), std::inserter(drop_list, drop_list.begin()));

        sqlite3_operations::drop_k_configurations(mgr, db, writer, drop_list, dbase,
                                                  sqlite3_operations::data_traits<number, typename integration_items<number>::twopf_si_re_item>::sqlite_table());
      }


    template <typename number>
    void
    data_manager_sqlite3<number>::drop_tensor_twopf_configurations
      (transaction_manager& mgr, integration_writer<number>& writer, const std::set<unsigned int>& serials,
       const std::set<unsigned int>& missing, const twopf_kconfig_database& dbase)
      {
        // get sqlite3 handle to principal database
        sqlite3* db = nullptr;
        writer.get_data_manager_handle(&db); // throws an exception if handle is unset, so the return value is guaranteed not to be nullptr

        // only drop those serials that are actually missing (otherwise DELETE is expensive)
        std::set<unsigned int> drop_list;
        std::set_difference(serials.begin(), serials.end(), missing.begin(), missing.end(), std::inserter(drop_list, drop_list.begin()));

        sqlite3_operations::drop_k_configurations(mgr, db, writer, drop_list, dbase,
                                                  sqlite3_operations::data_traits<number, typename integration_items<number>::tensor_twopf_item>::sqlite_table());
      }


    template <typename number>
    void
    data_manager_sqlite3<number>::drop_tensor_twopf_si_configurations
      (transaction_manager& mgr, integration_writer <number>& writer, const std::set<unsigned int>& serials,
       const std::set<unsigned int>& missing, const twopf_kconfig_database& dbase)
      {
        // get sqlite3 handle to principal database
        sqlite3* db = nullptr;
        writer.get_data_manager_handle(&db); // throws an exception if handle is unset, so the return value is guaranteed not to be nullptr

        // only drop those serials that are actually missing (otherwise DELETE is expensive)
        std::set<unsigned int> drop_list;
        std::set_difference(serials.begin(), serials.end(), missing.begin(), missing.end(), std::inserter(drop_list, drop_list.begin()));

        sqlite3_operations::drop_k_configurations(mgr, db, writer, drop_list, dbase,
                                                  sqlite3_operations::data_traits<number, typename integration_items<number>::tensor_twopf_si_item>::sqlite_table());
      }


    template <typename number>
    void
    data_manager_sqlite3<number>::drop_threepf_momentum_configurations
      (transaction_manager& mgr, integration_writer <number>& writer, const std::set<unsigned int>& serials,
       const std::set<unsigned int>& missing, const threepf_kconfig_database& dbase)
      {
        // get sqlite3 handle to principal database
        sqlite3* db = nullptr;
        writer.get_data_manager_handle(&db); // throws an exception if handle is unset, so the return value is guaranteed not to be nullptr

        // only drop those serials that are actually missing (otherwise DELETE is expensive)
        std::set<unsigned int> drop_list;
        std::set_difference(serials.begin(), serials.end(), missing.begin(), missing.end(), std::inserter(drop_list, drop_list.begin()));

        sqlite3_operations::drop_k_configurations(mgr, db, writer, drop_list, dbase,
                                                  sqlite3_operations::data_traits<number, typename integration_items<number>::threepf_momentum_item>::sqlite_table());
      }


    template <typename number>
    void
    data_manager_sqlite3<number>::drop_threepf_deriv_configurations
      (transaction_manager& mgr, integration_writer <number>& writer, const std::set<unsigned int>& serials,
       const std::set<unsigned int>& missing, const threepf_kconfig_database& dbase)
      {
        // get sqlite3 handle to principal database
        sqlite3* db = nullptr;
        writer.get_data_manager_handle(&db); // throws an exception if handle is unset, so the return value is guaranteed not to be nullptr

        // only drop those serials that are actually missing (otherwise DELETE is expensive)
        std::set<unsigned int> drop_list;
        std::set_difference(serials.begin(), serials.end(), missing.begin(), missing.end(), std::inserter(drop_list, drop_list.begin()));

        sqlite3_operations::drop_k_configurations(mgr, db, writer, drop_list, dbase,
                                                  sqlite3_operations::data_traits<number, typename integration_items<number>::threepf_Nderiv_item>::sqlite_table());
      }


    template <typename number>
    void
    data_manager_sqlite3<number>::drop_zeta_twopf_configurations
    (transaction_manager& mgr, postintegration_writer<number>& writer, const std::set<unsigned int>& serials,
     const std::set<unsigned int>& missing, const twopf_kconfig_database& dbase)
      {
        // get sqlite3 handle to principal database
        sqlite3* db = nullptr;
        writer.get_data_manager_handle(&db); // throws an exception if handle is unset, so the return value is guaranteed not to be nullptr

        // only drop those serials that are actually missing (otherwise DELETE is expensive)
        std::set<unsigned int> drop_list;
        std::set_difference(serials.begin(), serials.end(), missing.begin(), missing.end(), std::inserter(drop_list, drop_list.begin()));

        sqlite3_operations::drop_k_configurations(mgr, db, writer, drop_list, dbase,
                                                  sqlite3_operations::data_traits<number, typename postintegration_items<number>::zeta_twopf_item>::sqlite_table());
      }


    template <typename number>
    void
    data_manager_sqlite3<number>::drop_zeta_threepf_configurations
      (transaction_manager& mgr, postintegration_writer<number>& writer, const std::set<unsigned int>& serials,
       const std::set<unsigned int>& missing, const threepf_kconfig_database& dbase)
      {
        // get sqlite3 handle to principal database
        sqlite3* db = nullptr;
        writer.get_data_manager_handle(&db); // throws an exception if handle is unset, so the return value is guaranteed not to be nullptr

        // only drop those serials that are actually missing (otherwise DELETE is expensive)
        std::set<unsigned int> drop_list;
        std::set_difference(serials.begin(), serials.end(), missing.begin(), missing.end(), std::inserter(drop_list, drop_list.begin()));

        sqlite3_operations::drop_k_configurations(mgr, db, writer, drop_list, dbase,
                                                  sqlite3_operations::data_traits<number, typename postintegration_items<number>::zeta_threepf_item>::sqlite_table());
      }


    template <typename number>
    void
    data_manager_sqlite3<number>::drop_gauge_xfm1_configurations
      (transaction_manager& mgr, postintegration_writer<number>& writer, const std::set<unsigned int>& serials,
       const std::set<unsigned int>& missing, const twopf_kconfig_database& dbase)
      {
        // get sqlite3 handle to principal database
        sqlite3* db = nullptr;
        writer.get_data_manager_handle(&db); // throws an exception if handle is unset, so the return value is guaranteed not to be nullptr

        // only drop those serials that are actually missing (otherwise DELETE is expensive)
        std::set<unsigned int> drop_list;
        std::set_difference(serials.begin(), serials.end(), missing.begin(), missing.end(), std::inserter(drop_list, drop_list.begin()));

        sqlite3_operations::drop_k_configurations(mgr, db, writer, drop_list, dbase,
                                                  sqlite3_operations::data_traits<number, typename postintegration_items<number>::gauge_xfm1_item>::sqlite_table());
      }


    template <typename number>
    void
    data_manager_sqlite3<number>::drop_gauge_xfm2_123_configurations
      (transaction_manager& mgr, postintegration_writer<number>& writer, const std::set<unsigned int>& serials,
       const std::set<unsigned int>& missing, const threepf_kconfig_database& dbase)
      {
        // get sqlite3 handle to principal database
        sqlite3* db = nullptr;
        writer.get_data_manager_handle(&db); // throws an exception if handle is unset, so the return value is guaranteed not to be nullptr

        // only drop those serials that are actually missing (otherwise DELETE is expensive)
        std::set<unsigned int> drop_list;
        std::set_difference(serials.begin(), serials.end(), missing.begin(), missing.end(), std::inserter(drop_list, drop_list.begin()));

        sqlite3_operations::drop_k_configurations(mgr, db, writer, drop_list, dbase,
                                                  sqlite3_operations::data_traits<number, typename postintegration_items<number>::gauge_xfm2_123_item>::sqlite_table());
      }


    template <typename number>
    void
    data_manager_sqlite3<number>::drop_gauge_xfm2_213_configurations
      (transaction_manager& mgr, postintegration_writer<number>& writer, const std::set<unsigned int>& serials,
       const std::set<unsigned int>& missing, const threepf_kconfig_database& dbase)
      {
        // get sqlite3 handle to principal database
        sqlite3* db = nullptr;
        writer.get_data_manager_handle(&db); // throws an exception if handle is unset, so the return value is guaranteed not to be nullptr

        // only drop those serials that are actually missing (otherwise DELETE is expensive)
        std::set<unsigned int> drop_list;
        std::set_difference(serials.begin(), serials.end(), missing.begin(), missing.end(), std::inserter(drop_list, drop_list.begin()));

        sqlite3_operations::drop_k_configurations(mgr, db, writer, drop_list, dbase,
                                                  sqlite3_operations::data_traits<number, typename postintegration_items<number>::gauge_xfm2_213_item>::sqlite_table());
      }


    template <typename number>
    void
    data_manager_sqlite3<number>::drop_gauge_xfm2_312_configurations
      (transaction_manager& mgr, postintegration_writer<number>& writer, const std::set<unsigned int>& serials,
       const std::set<unsigned int>& missing, const threepf_kconfig_database& dbase)
      {
        // get sqlite3 handle to principal database
        sqlite3* db = nullptr;
        writer.get_data_manager_handle(&db); // throws an exception if handle is unset, so the return value is guaranteed not to be nullptr

        // only drop those serials that are actually missing (otherwise DELETE is expensive)
        std::set<unsigned int> drop_list;
        std::set_difference(serials.begin(), serials.end(), missing.begin(), missing.end(), std::inserter(drop_list, drop_list.begin()));

        sqlite3_operations::drop_k_configurations(mgr, db, writer, drop_list, dbase,
                                                  sqlite3_operations::data_traits<number, typename postintegration_items<number>::gauge_xfm2_312_item>::sqlite_table());
      }


    template <typename number>
    void
    data_manager_sqlite3<number>::drop_statistics_configurations
      (transaction_manager& mgr, integration_writer<number>& writer, const std::set<unsigned int>& serials,
       const twopf_kconfig_database& dbase)
      {
        // get sqlite3 handle to principal database
        sqlite3* db = nullptr;
        writer.get_data_manager_handle(&db); // throws an exception if handle is unset, so the return value is guaranteed not to be nullptr

        std::set<unsigned int> present = sqlite3_operations::get_stored_serials<number, typename integration_items<number>::configuration_statistics>(db);
        std::set<unsigned int> drop_list;
        std::set_intersection(serials.begin(), serials.end(), present.begin(), present.end(), std::inserter(drop_list, drop_list.begin()));

        sqlite3_operations::drop_statistics(mgr, db, drop_list, dbase);
      }


    template <typename number>
    void
    data_manager_sqlite3<number>::drop_statistics_configurations
      (transaction_manager& mgr, integration_writer<number>& writer, const std::set<unsigned int>& serials,
       const threepf_kconfig_database& dbase)
      {
        // get sqlite3 handle to principal database
        sqlite3* db = nullptr;
        writer.get_data_manager_handle(&db); // throws an exception if handle is unset, so the return value is guaranteed not to be nullptr

        std::set<unsigned int> present = sqlite3_operations::get_stored_serials<number, typename integration_items<number>::configuration_statistics>(db);
        std::set<unsigned int> drop_list;
        std::set_intersection(serials.begin(), serials.end(), present.begin(), present.end(), std::inserter(drop_list, drop_list.begin()));

        sqlite3_operations::drop_statistics(mgr, db, drop_list, dbase);
      }


    template <typename number>
    void
    data_manager_sqlite3<number>::drop_initial_conditions_configurations
      (transaction_manager& mgr, integration_writer<number>& writer, const std::set<unsigned int>& serials,
       const twopf_kconfig_database& dbase)
      {
        // get sqlite3 handle to principal database
        sqlite3* db = nullptr;
        writer.get_data_manager_handle(&db); // throws an exception if handle is unset, so the return value is guaranteed not to be nullptr

        std::set<unsigned int> present = sqlite3_operations::get_stored_serials<number, typename integration_items<number>::ics_item>(db);
        std::set<unsigned int> drop_list;
        std::set_intersection(serials.begin(), serials.end(), present.begin(), present.end(), std::inserter(drop_list, drop_list.begin()));

        sqlite3_operations::drop_ics<number, typename integration_items<number>::ics_item, twopf_kconfig_database>(mgr, db, drop_list, dbase);
      }


    template <typename number>
    void
    data_manager_sqlite3<number>::drop_initial_conditions_configurations
      (transaction_manager& mgr, integration_writer<number>& writer, const std::set<unsigned int>& serials,
       const threepf_kconfig_database& dbase)
      {
        // get sqlite3 handle to principal database
        sqlite3* db = nullptr;
        writer.get_data_manager_handle(&db); // throws an exception if handle is unset, so the return value is guaranteed not to be nullptr

        std::set<unsigned int> present = sqlite3_operations::get_stored_serials<number, typename integration_items<number>::ics_item>(db);
        std::set<unsigned int> drop_list;
        std::set_intersection(serials.begin(), serials.end(), present.begin(), present.end(), std::inserter(drop_list, drop_list.begin()));

        sqlite3_operations::drop_ics<number, typename integration_items<number>::ics_item, threepf_kconfig_database>(mgr, db, drop_list, dbase);

        std::set<unsigned int> kt_present = sqlite3_operations::get_stored_serials<number, typename integration_items<number>::ics_kt_item>(db);
        std::set<unsigned int> kt_drop_list;
        std::set_intersection(serials.begin(), serials.end(), kt_present.begin(), kt_present.end(), std::inserter(kt_drop_list, kt_drop_list.begin()));

        sqlite3_operations::drop_ics<number, typename integration_items<number>::ics_kt_item, threepf_kconfig_database>(mgr, db, kt_drop_list, dbase);
      }


    // FINALIZE WRITERS


    template <typename number>
    void data_manager_sqlite3<number>::finalize_twopf_writer(integration_writer<number>& writer)
      {
        transaction_manager mgr = this->transaction_factory(writer);

        BOOST_LOG_SEV(writer.get_log(), base_writer::log_severity_level::notification)
          << '\n' << "** Performing finalization for twopf container '" << writer.get_abs_container_path().string() << "'";

        boost::timer::cpu_timer timer;

        // get sqlite3 handle to principal database
        sqlite3* db = nullptr;
        writer.get_data_manager_handle(&db); // throws an exception if handle is unset, so the return value is guaranteed not to be nullptr

        sqlite3_operations::finalize_twopf_writer(mgr, db);

        mgr.commit();

        // we move the journal mode to TRUNCATE before finalizing
        // the Xerial JDBC driver does not play nicely with WAL journals, which prevents use of eg. DataGrip
        // with containers that have been set to use WAL
        // (the journal mode can only be changed outside a transaction, so we have to do this after mgr.commit())
        sqlite3_operations::force_truncate_journal(db);

        timer.stop();
        BOOST_LOG_SEV(writer.get_log(), base_writer::log_severity_level::notification)
          << "** Finalization complete in time " << format_time(timer.elapsed().wall);
      }


    template <typename number>
    void data_manager_sqlite3<number>::finalize_threepf_writer(integration_writer<number>& writer)
      {
        transaction_manager mgr = this->transaction_factory(writer);

        BOOST_LOG_SEV(writer.get_log(), base_writer::log_severity_level::notification)
          << '\n' << "** Performing finalization for threepf container '" << writer.get_abs_container_path().string() << "'";

        boost::timer::cpu_timer timer;

        // get sqlite3 handle to principal database
        sqlite3* db = nullptr;
        writer.get_data_manager_handle(&db); // throws an exception if handle is unset, so the return value is guaranteed not to be nullptr

        sqlite3_operations::finalize_threepf_writer(mgr, db);

        mgr.commit();

        // we move the journal mode to TRUNCATE before finalizing
        // the Xerial JDBC driver does not play nicely with WAL journals, which prevents use of eg. DataGrip
        // with containers that have been set to use WAL
        // (the journal mode can only be changed outside a transaction, so we have to do this after mgr.commit())
        sqlite3_operations::force_truncate_journal(db);

        timer.stop();
        BOOST_LOG_SEV(writer.get_log(), base_writer::log_severity_level::notification)
          << "** Finalization complete in time " << format_time(timer.elapsed().wall);
      }


    template <typename number>
    void data_manager_sqlite3<number>::finalize_zeta_twopf_writer(postintegration_writer<number>& writer)
      {
        transaction_manager mgr = this->transaction_factory(writer);

        BOOST_LOG_SEV(writer.get_log(), base_writer::log_severity_level::notification)
          << '\n' << "** Performing finalization for zeta twopf container '" << writer.get_abs_container_path().string() << "'";

        boost::timer::cpu_timer timer;

        // get sqlite3 handle to principal database
        sqlite3* db = nullptr;
        writer.get_data_manager_handle(&db); // throws an exception if handle is unset, so the return value is guaranteed not to be nullptr

        sqlite3_operations::finalize_zeta_twopf_writer(mgr, db);

        mgr.commit();

        // we move the journal mode to TRUNCATE before finalizing
        // the Xerial JDBC driver does not play nicely with WAL journals, which prevents use of eg. DataGrip
        // with containers that have been set to use WAL
        // (the journal mode can only be changed outside a transaction, so we have to do this after mgr.commit())
        sqlite3_operations::force_truncate_journal(db);

        timer.stop();
        BOOST_LOG_SEV(writer.get_log(), base_writer::log_severity_level::notification)
          << "** Finalization complete in time " << format_time(timer.elapsed().wall);
      }


    template <typename number>
    void data_manager_sqlite3<number>::finalize_zeta_threepf_writer(postintegration_writer<number>& writer)
      {
        transaction_manager mgr = this->transaction_factory(writer);

        BOOST_LOG_SEV(writer.get_log(), base_writer::log_severity_level::notification)
          << '\n' << "** Performing finalization for zeta threepf container '" << writer.get_abs_container_path().string() << "'";

        boost::timer::cpu_timer timer;

        // get sqlite3 handle to principal database
        sqlite3* db = nullptr;
        writer.get_data_manager_handle(&db); // throws an exception if handle is unset, so the return value is guaranteed not to be nullptr

        sqlite3_operations::finalize_zeta_threepf_writer(mgr, db);

        mgr.commit();

        // we move the journal mode to TRUNCATE before finalizing
        // the Xerial JDBC driver does not play nicely with WAL journals, which prevents use of eg. DataGrip
        // with containers that have been set to use WAL
        // (the journal mode can only be changed outside a transaction, so we have to do this after mgr.commit())
        sqlite3_operations::force_truncate_journal(db);

        timer.stop();
        BOOST_LOG_SEV(writer.get_log(), base_writer::log_severity_level::notification)
          << "** Finalization complete in time " << format_time(timer.elapsed().wall);
      }


    template <typename number>
    void data_manager_sqlite3<number>::finalize_fNL_writer(postintegration_writer<number>& writer)
      {
        transaction_manager mgr = this->transaction_factory(writer);

        BOOST_LOG_SEV(writer.get_log(), base_writer::log_severity_level::notification)
          << '\n' << "** Performing finalization for fNL container '" << writer.get_abs_container_path().string() << "'";

        boost::timer::cpu_timer timer;

        // get sqlite3 handle to principal database
        sqlite3* db = nullptr;
        writer.get_data_manager_handle(&db); // throws an exception if handle is unset, so the return value is guaranteed not to be nullptr

        sqlite3_operations::finalize_fNL_writer(mgr, db);

        mgr.commit();

        // we move the journal mode to TRUNCATE before finalizing
        // the Xerial JDBC driver does not play nicely with WAL journals, which prevents use of eg. DataGrip
        // with containers that have been set to use WAL
        // (the journal mode can only be changed outside a transaction, so we have to do this after mgr.commit())
        sqlite3_operations::force_truncate_journal(db);

        timer.stop();
        BOOST_LOG_SEV(writer.get_log(), base_writer::log_severity_level::notification)
          << "** Finalization complete in time " << format_time(timer.elapsed().wall);
      }


    // DATAPIPES


    template <typename number>
    std::unique_ptr< datapipe<number> > data_manager_sqlite3<number>::create_datapipe(const boost::filesystem::path& logdir, const boost::filesystem::path& tempdir,
                                                                                      integration_content_finder<number>& integration_finder,
                                                                                      postintegration_content_finder<number>& postintegration_finder,
                                                                                      datapipe_dispatch_function<number>& dispatcher,
                                                                                      unsigned int worker, bool no_log)
      {
        // set up callback API
        typename datapipe<number>::utility_callbacks utilities(integration_finder, postintegration_finder, dispatcher);

        // set up datapipe
        return std::make_unique< datapipe<number> >(this->get_pipe_capacity(), logdir, tempdir, worker, *this, utilities, no_log);
      }


    template <typename number>
    void data_manager_sqlite3<number>::pull_time_config(datapipe<number>* pipe, const derived_data::SQL_time_query& query,
                                                        std::vector<time_config>& sample)
      {
        assert(pipe != nullptr);
        if(pipe == nullptr) throw runtime_exception(exception_type::RUNTIME_ERROR, CPPTRANSPORT_DATAMGR_NULL_DATAPIPE);

        sqlite3* db = nullptr;
        pipe->get_manager_handle(&db);    // throws an exception if the handle is unset, so safe to proceed; we can't get nullptr back

        sqlite3_operations::pull_time_config_sample(db, query, sample, pipe->get_worker_number());
      }


    template <typename number>
    void data_manager_sqlite3<number>::pull_kconfig_twopf(datapipe<number>* pipe, const derived_data::SQL_twopf_query& query,
                                                          std::vector<twopf_kconfig>& sample)
      {
        assert(pipe != nullptr);
        if(pipe == nullptr) throw runtime_exception(exception_type::RUNTIME_ERROR, CPPTRANSPORT_DATAMGR_NULL_DATAPIPE);

        sqlite3* db = nullptr;
        pipe->get_manager_handle(&db);    // throws an exception if the handle is unset, so safe to proceed; we can't get nullptr back

        sqlite3_operations::pull_twopf_kconfig_sample<number>(db, query, sample, pipe->get_worker_number());
      }


    template <typename number>
    void data_manager_sqlite3<number>::pull_kconfig_threepf(datapipe<number>* pipe, const derived_data::SQL_threepf_query& query,
                                                            std::vector<threepf_kconfig>& sample)
      {
        assert(pipe != nullptr);
        if(pipe == nullptr) throw runtime_exception(exception_type::RUNTIME_ERROR, CPPTRANSPORT_DATAMGR_NULL_DATAPIPE);

        sqlite3* db = nullptr;
        pipe->get_manager_handle(&db);    // throws an exception if the handle is unset, so safe to proceed; we can't get nullptr back

        sqlite3_operations::pull_threepf_kconfig_sample<number>(db, query, sample, pipe->get_worker_number());
      }


    template <typename number>
    void data_manager_sqlite3<number>::pull_background_time_sample(datapipe<number>* pipe, unsigned int id,
                                                                   const derived_data::SQL_query& query, std::vector<number>& sample)
      {
        assert(pipe != nullptr);
        if(pipe == nullptr) throw runtime_exception(exception_type::RUNTIME_ERROR, CPPTRANSPORT_DATAMGR_NULL_DATAPIPE);

        sqlite3* db = nullptr;
        pipe->get_manager_handle(&db);    // throws an exception if the handle is unset, so safe to proceed; we can't get nullptr back

        sqlite3_operations::pull_background_time_sample(db, id, query, sample, pipe->get_worker_number(), pipe->get_N_fields());
      }


    template <typename number>
    void data_manager_sqlite3<number>::pull_twopf_time_sample(datapipe<number>* pipe, unsigned int id,
                                                              const derived_data::SQL_query& query,
                                                              unsigned int k_serial, std::vector<number>& sample, twopf_type type)
      {
        assert(pipe != nullptr);
        if(pipe == nullptr) throw runtime_exception(exception_type::RUNTIME_ERROR, CPPTRANSPORT_DATAMGR_NULL_DATAPIPE);

        sqlite3* db = nullptr;
        pipe->get_manager_handle(&db);    // throws an exception if the handle is unset, so safe to proceed; we can't get nullptr back

        switch(type)
          {
            case twopf_type::real:
              {
                sqlite3_operations::pull_paged_time_sample<number, typename integration_items<number>::twopf_re_item>(db, id, query, k_serial, sample,
                                                                                                                      pipe->get_worker_number(), pipe->get_N_fields());
                break;
              }

            case twopf_type::imag:
              {
                sqlite3_operations::pull_paged_time_sample<number, typename integration_items<number>::twopf_im_item>(db, id, query, k_serial, sample,
                                                                                                                      pipe->get_worker_number(), pipe->get_N_fields());
                break;
              }
          }
      }


    template <typename number>
    void data_manager_sqlite3<number>::pull_threepf_time_sample(datapipe<number>* pipe, unsigned int id,
                                                                const derived_data::SQL_query& query,
                                                                unsigned int k_serial, std::vector<number>& sample, threepf_type type)
      {
        assert(pipe != nullptr);
        if(pipe == nullptr) throw runtime_exception(exception_type::RUNTIME_ERROR, CPPTRANSPORT_DATAMGR_NULL_DATAPIPE);

        sqlite3* db = nullptr;
        pipe->get_manager_handle(&db);    // throws an exception if the handle is unset, so safe to proceed; we can't get nullptr back

        switch(type)
          {
            case threepf_type::momentum:
              {
                sqlite3_operations::pull_paged_time_sample<number, typename integration_items<number>::threepf_momentum_item>(db, id, query, k_serial, sample,
                                                                                                                              pipe->get_worker_number(), pipe->get_N_fields());
                break;
              }

            case threepf_type::Nderiv:
              {
                sqlite3_operations::pull_paged_time_sample<number, typename integration_items<number>::threepf_Nderiv_item>(db, id, query, k_serial, sample,
                                                                                                                            pipe->get_worker_number(), pipe->get_N_fields());
                break;
              }
          }
      }


    template <typename number>
    void data_manager_sqlite3<number>::pull_tensor_twopf_time_sample(datapipe<number>* pipe, unsigned int id,
                                                                     const derived_data::SQL_query& query,
                                                                     unsigned int k_serial, std::vector<number>& sample)
      {
        assert(pipe != nullptr);
        if(pipe == nullptr) throw runtime_exception(exception_type::RUNTIME_ERROR, CPPTRANSPORT_DATAMGR_NULL_DATAPIPE);

        sqlite3* db = nullptr;
        pipe->get_manager_handle(&db);    // throws an exception if the handle is unset, so safe to proceed; we can't get nullptr back

        sqlite3_operations::pull_paged_time_sample<number, typename integration_items<number>::tensor_twopf_item>(db, id, query, k_serial, sample,
                                                                                                                  pipe->get_worker_number(), pipe->get_N_fields());
      }


    template <typename number>
    void data_manager_sqlite3<number>::pull_zeta_twopf_time_sample(datapipe<number>* pipe, const derived_data::SQL_query& query,
                                                                   unsigned int k_serial, std::vector<number>& sample)
      {
        assert(pipe != nullptr);
        if(pipe == nullptr) throw runtime_exception(exception_type::RUNTIME_ERROR, CPPTRANSPORT_DATAMGR_NULL_DATAPIPE);

        sqlite3* db = nullptr;
        pipe->get_manager_handle(&db);    // throws an exception if the handle is unset, so safe to proceed; we can't get nullptr back

        sqlite3_operations::pull_unpaged_time_sample<number, typename postintegration_items<number>::zeta_twopf_item>(db, query, k_serial, sample,
                                                                                                                      pipe->get_worker_number());
      }


    template <typename number>
    void data_manager_sqlite3<number>::pull_zeta_threepf_time_sample(datapipe<number>* pipe, const derived_data::SQL_query& query,
                                                                     unsigned int k_serial, std::vector<number>& sample)
      {
        assert(pipe != nullptr);
        if(pipe == nullptr) throw runtime_exception(exception_type::RUNTIME_ERROR, CPPTRANSPORT_DATAMGR_NULL_DATAPIPE);

        sqlite3* db = nullptr;
        pipe->get_manager_handle(&db);    // throws an exception if the handle is unset, so safe to proceed; we can't get nullptr back

        sqlite3_operations::pull_unpaged_time_sample<number, typename postintegration_items<number>::zeta_threepf_item>(db, query, k_serial, sample,
                                                                                                                        pipe->get_worker_number());
      }


    template <typename number>
    void data_manager_sqlite3<number>::pull_zeta_redbsp_time_sample(datapipe<number>* pipe, const derived_data::SQL_query& query,
                                                                    unsigned int k_serial, std::vector<number>& sample)
      {
        assert(pipe != nullptr);
        if(pipe == nullptr) throw runtime_exception(exception_type::RUNTIME_ERROR, CPPTRANSPORT_DATAMGR_NULL_DATAPIPE);

        sqlite3* db = nullptr;
        pipe->get_manager_handle(&db);    // throws an exception if the handle is unset, so safe to proceed; we can't get nullptr back

        sqlite3_operations::pull_unpaged_time_sample<number, typename postintegration_items<number>::zeta_redbsp_item>(db, query, k_serial, sample,
                                                                                                                       pipe->get_worker_number());
      }


    template <typename number>
    void data_manager_sqlite3<number>::pull_fNL_time_sample(datapipe<number>* pipe, const derived_data::SQL_query& query,
                                                            std::vector<number>& sample, derived_data::bispectrum_template type)
      {
        assert(pipe != nullptr);
        if(pipe == nullptr) throw runtime_exception(exception_type::RUNTIME_ERROR, CPPTRANSPORT_DATAMGR_NULL_DATAPIPE);

        sqlite3* db = nullptr;
        pipe->get_manager_handle(&db);    // throws an exception if the handle is unset, so safe to proceed; we can't get nullptr back

        sqlite3_operations::pull_fNL_time_sample(db, query, sample, pipe->get_worker_number(), type);
      }


    template <typename number>
    void data_manager_sqlite3<number>::pull_BT_time_sample(datapipe<number>* pipe, const derived_data::SQL_query& query,
                                                           std::vector<number>& sample, derived_data::bispectrum_template type)
      {
        assert(pipe != nullptr);
        if(pipe == nullptr) throw runtime_exception(exception_type::RUNTIME_ERROR, CPPTRANSPORT_DATAMGR_NULL_DATAPIPE);

        sqlite3* db = nullptr;
        pipe->get_manager_handle(&db);    // throws an exception if the handle is unset, so safe to proceed; we can't get nullptr back

        sqlite3_operations::pull_BT_time_sample(db, query, sample, pipe->get_worker_number(), type);
      }


    template <typename number>
    void data_manager_sqlite3<number>::pull_TT_time_sample(datapipe<number>* pipe, const derived_data::SQL_query& query,
                                                           std::vector<number>& sample, derived_data::bispectrum_template type)
      {
        assert(pipe != nullptr);
        if(pipe == nullptr) throw runtime_exception(exception_type::RUNTIME_ERROR, CPPTRANSPORT_DATAMGR_NULL_DATAPIPE);

        sqlite3* db = nullptr;
        pipe->get_manager_handle(&db);    // throws an exception if the handle is unset, so safe to proceed; we can't get nullptr back

        sqlite3_operations::pull_TT_time_sample(db, query, sample, pipe->get_worker_number(), type);
      }


    template <typename number>
    void data_manager_sqlite3<number>::pull_twopf_kconfig_sample(datapipe<number>* pipe, unsigned int id,
                                                                 const derived_data::SQL_query& query,
                                                                 unsigned int t_serial, std::vector<number>& sample, twopf_type type)
      {
        assert(pipe != nullptr);
        if(pipe == nullptr) throw runtime_exception(exception_type::RUNTIME_ERROR, CPPTRANSPORT_DATAMGR_NULL_DATAPIPE);

        sqlite3* db = nullptr;
        pipe->get_manager_handle(&db);    // throws an exception if the handle is unset, so safe to proceed; we can't get nullptr back

        switch(type)
          {
            case twopf_type::real:
              {
                sqlite3_operations::pull_paged_kconfig_sample<number, typename integration_items<number>::twopf_re_item>(db, id, query, t_serial, sample,
                                                                                                                         pipe->get_worker_number(), pipe->get_N_fields());
                break;
              }

            case twopf_type::imag:
              {
                sqlite3_operations::pull_paged_kconfig_sample<number, typename integration_items<number>::twopf_im_item>(db, id, query, t_serial, sample,
                                                                                                                         pipe->get_worker_number(), pipe->get_N_fields());
                break;
              }
          }
      }


    template <typename number>
    void data_manager_sqlite3<number>::pull_threepf_kconfig_sample(datapipe<number>* pipe, unsigned int id,
                                                                   const derived_data::SQL_query& query,
                                                                   unsigned int t_serial, std::vector<number>& sample, threepf_type type)
      {
        assert(pipe != nullptr);
        if(pipe == nullptr) throw runtime_exception(exception_type::RUNTIME_ERROR, CPPTRANSPORT_DATAMGR_NULL_DATAPIPE);

        sqlite3* db = nullptr;
        pipe->get_manager_handle(&db);    // throws an exception if the handle is unset, so safe to proceed; we can't get nullptr back

        switch(type)
          {
            case threepf_type::momentum:
              {
                sqlite3_operations::pull_paged_kconfig_sample<number, typename integration_items<number>::threepf_momentum_item>(db, id, query, t_serial, sample,
                                                                                                                                 pipe->get_worker_number(), pipe->get_N_fields());
                break;
              }

            case threepf_type::Nderiv:
              {
                sqlite3_operations::pull_paged_kconfig_sample<number, typename integration_items<number>::threepf_Nderiv_item>(db, id, query, t_serial, sample,
                                                                                                                               pipe->get_worker_number(), pipe->get_N_fields());
                break;
              }
          }
      }


    template <typename number>
    void data_manager_sqlite3<number>::pull_tensor_twopf_kconfig_sample(datapipe<number>* pipe, unsigned int id,
                                                                        const derived_data::SQL_query& query,
                                                                        unsigned int t_serial, std::vector<number>& sample)
      {
        assert(pipe != nullptr);
        if(pipe == nullptr) throw runtime_exception(exception_type::RUNTIME_ERROR, CPPTRANSPORT_DATAMGR_NULL_DATAPIPE);

        sqlite3* db = nullptr;
        pipe->get_manager_handle(&db);    // throws an exception if the handle is unset, so safe to proceed; we can't get nullptr back

        sqlite3_operations::pull_paged_kconfig_sample<number, typename integration_items<number>::tensor_twopf_item>(db, id, query, t_serial, sample,
                                                                                                                     pipe->get_worker_number(), pipe->get_N_fields());
      }


    template <typename number>
    void data_manager_sqlite3<number>::pull_zeta_twopf_kconfig_sample(datapipe<number>* pipe, const derived_data::SQL_query& query,
                                                                      unsigned int t_serial, std::vector<number>& sample)
      {
        assert(pipe != nullptr);
        if(pipe == nullptr) throw runtime_exception(exception_type::RUNTIME_ERROR, CPPTRANSPORT_DATAMGR_NULL_DATAPIPE);

        sqlite3* db = nullptr;
        pipe->get_manager_handle(&db);    // throws an exception if the handle is unset, so safe to proceed; we can't get nullptr back

        sqlite3_operations::pull_unpaged_kconfig_sample<number, typename postintegration_items<number>::zeta_twopf_item>(db, query, t_serial, sample,
                                                                                                                         pipe->get_worker_number());
      }


    template <typename number>
    void data_manager_sqlite3<number>::pull_zeta_threepf_kconfig_sample(datapipe<number>* pipe, const derived_data::SQL_query& query,
                                                                        unsigned int t_serial, std::vector<number>& sample)
      {
        assert(pipe != nullptr);
        if(pipe == nullptr) throw runtime_exception(exception_type::RUNTIME_ERROR, CPPTRANSPORT_DATAMGR_NULL_DATAPIPE);

        sqlite3* db = nullptr;
        pipe->get_manager_handle(&db);    // throws an exception if the handle is unset, so safe to proceed; we can't get nullptr back

        sqlite3_operations::pull_unpaged_kconfig_sample<number, typename postintegration_items<number>::zeta_threepf_item>(db, query, t_serial, sample,
                                                                                                                           pipe->get_worker_number());
      }


    template <typename number>
    void data_manager_sqlite3<number>::pull_zeta_redbsp_kconfig_sample(datapipe<number>* pipe, const derived_data::SQL_query& query,
                                                                       unsigned int t_serial, std::vector<number>& sample)
      {
        assert(pipe != nullptr);
        if(pipe == nullptr) throw runtime_exception(exception_type::RUNTIME_ERROR, CPPTRANSPORT_DATAMGR_NULL_DATAPIPE);

        sqlite3* db = nullptr;
        pipe->get_manager_handle(&db);    // throws an exception if the handle is unset, so safe to proceed; we can't get nullptr back

        sqlite3_operations::pull_unpaged_kconfig_sample<number, typename postintegration_items<number>::zeta_redbsp_item>(db, query, t_serial, sample,
                                                                                                                          pipe->get_worker_number());
      }


    template <typename number>
    void data_manager_sqlite3<number>::pull_k_statistics_sample(datapipe<number>* pipe, const derived_data::SQL_query& query,
                                                                std::vector<kconfiguration_statistics>& data)
      {
        assert(pipe != nullptr);
        if(pipe == nullptr) throw runtime_exception(exception_type::RUNTIME_ERROR, CPPTRANSPORT_DATAMGR_NULL_DATAPIPE);

        sqlite3* db = nullptr;
        pipe->get_manager_handle(&db);    // throws an exception if the handle is unset, so safe to proceed; we can't get nullptr back

        sqlite3_operations::pull_k_statistics_sample(db, query, data, pipe->get_worker_number());
      }


    template <typename number>
    void data_manager_sqlite3<number>::datapipe_attach_container(datapipe<number>* pipe, const boost::filesystem::path& ctr_path)
      {
        sqlite3* db = nullptr;

        int status = sqlite3_open_v2(ctr_path.string().c_str(), &db, SQLITE_OPEN_READONLY, nullptr);

        if(status != SQLITE_OK)
          {
            std::ostringstream msg;
            if(db != nullptr)
              {
                msg << CPPTRANSPORT_DATACTR_OPEN_A << " '" << ctr_path.string() << "' " << CPPTRANSPORT_DATACTR_OPEN_B << status << ": " << sqlite3_errmsg(db) << ")";
                sqlite3_close(db);
              }
            else
              {
                msg << CPPTRANSPORT_DATACTR_OPEN_A << " '" << ctr_path.string() << "' " << CPPTRANSPORT_DATACTR_OPEN_B << status << ")";
              }
            throw runtime_exception(exception_type::DATA_CONTAINER_ERROR, msg.str());
          }

        sqlite3_extended_result_codes(db, 1);

        // set performance-related options
        sqlite3_operations::consistency_pragmas(db);

        // remember this connexion
        this->open_containers.push_back(db);
        pipe->set_manager_handle(db);

        BOOST_LOG_SEV(pipe->get_log(), datapipe<number>::log_severity_level::normal) << "** Attached SQLite3 container '" << ctr_path.string() << "' to datapipe";
      }


    template <typename number>
    std::unique_ptr< content_group_record<integration_payload> >
    data_manager_sqlite3<number>::datapipe_attach_integration_content(datapipe<number>* pipe, integration_content_finder<number>& finder,
                                                                      const std::string& name, const std::list<std::string>& tags)
      {
        assert(pipe != nullptr);
        if(pipe == nullptr) throw runtime_exception(exception_type::RUNTIME_ERROR, CPPTRANSPORT_DATAMGR_NULL_DATAPIPE);

        // find a suitable content group for this task; will throw an exception if no suitable content group can be found
        std::unique_ptr< content_group_record<integration_payload> > group = finder(name, tags);

        integration_payload& payload = group->get_payload();

        // get path to the content group data container
        boost::filesystem::path ctr_path = group->get_abs_repo_path() / payload.get_container_path();

        this->datapipe_attach_container(pipe, ctr_path);

        return std::move(group);   // std::move required by GCC 5.2 although standard implies that copy elision should occur
      }


    template <typename number>
    std::unique_ptr< content_group_record<postintegration_payload> >
    data_manager_sqlite3<number>::datapipe_attach_postintegration_content(datapipe<number>* pipe, postintegration_content_finder<number>& finder,
                                                                          const std::string& name, const std::list<std::string>& tags)
      {
        assert(pipe != nullptr);
        if(pipe == nullptr) throw runtime_exception(exception_type::RUNTIME_ERROR, CPPTRANSPORT_DATAMGR_NULL_DATAPIPE);

        // find a suitable content group for this task; will throw an exception if no suitable content group can be found
        std::unique_ptr< content_group_record<postintegration_payload> > group = finder(name, tags);

        postintegration_payload& payload = group->get_payload();

        // get path to the content group data container
        boost::filesystem::path ctr_path = group->get_abs_repo_path() / payload.get_container_path();

        this->datapipe_attach_container(pipe, ctr_path);

        return std::move(group);   // std::move required by GCC 5.2 although standard implies that copy elision should occur
      }


    template <typename number>
    void data_manager_sqlite3<number>::datapipe_detach(datapipe<number>* pipe)
      {
        assert(pipe != nullptr);
        if(pipe == nullptr) throw runtime_exception(exception_type::RUNTIME_ERROR, CPPTRANSPORT_DATAMGR_NULL_DATAPIPE);

        sqlite3* db = nullptr;
        pipe->get_manager_handle(&db);
        this->open_containers.remove(db);
        sqlite3_close(db);

        BOOST_LOG_SEV(pipe->get_log(), datapipe<number>::log_severity_level::normal) << "** Detached SQLite3 container from datapipe";
      }


    template <typename number>
    worker_information_db data_manager_sqlite3<number>::read_worker_information(const boost::filesystem::path& ctr_path)
      {
        sqlite3* db = this->open_container(ctr_path);
        worker_information_db worker_db = sqlite3_operations::read_worker_table(db);
        this->close_container(db);

        return std::move(worker_db);
      }


    template <typename number>
    timing_db data_manager_sqlite3<number>::read_timing_information(const boost::filesystem::path& ctr_path)
      {
        sqlite3* db = this->open_container(ctr_path);
        timing_db timing_data = sqlite3_operations::read_statistics_table(db);
        this->close_container(db);

        return std::move(timing_data);
      }


    template <typename number>
    sqlite3* data_manager_sqlite3<number>::open_container(const boost::filesystem::path& ctr_path)
      {
        if(!boost::filesystem::exists(ctr_path))
          {
            std::ostringstream msg;
            msg << CPPTRANSPORT_DATAMGR_CONTAINER_NOT_EXIST << " '" << ctr_path.string() << "'";
            throw runtime_exception(exception_type::RUNTIME_ERROR, msg.str());
          }

        sqlite3* db = nullptr;

        int status = sqlite3_open_v2(ctr_path.string().c_str(), &db, SQLITE_OPEN_READONLY, nullptr);

        if(status != SQLITE_OK)
          {
            std::ostringstream msg;
            if(db != nullptr)
              {
                msg << CPPTRANSPORT_DATACTR_OPEN_A << " '" << ctr_path.string() << "' " << CPPTRANSPORT_DATACTR_OPEN_B << status << ": " << sqlite3_errmsg(db) << ")";
                sqlite3_close(db);
              }
            else
              {
                msg << CPPTRANSPORT_DATACTR_OPEN_A << " '" << ctr_path.string() << "' " << CPPTRANSPORT_DATACTR_OPEN_B << status << ")";
              }
            throw runtime_exception(exception_type::DATA_CONTAINER_ERROR, msg.str());
          }

        sqlite3_extended_result_codes(db, 1);

        // set performance-related options
        sqlite3_operations::consistency_pragmas(db);

        //
        this->open_containers.push_back(db);
        
        return db;
      }


    template <typename number>
    void data_manager_sqlite3<number>::close_container(sqlite3* db)
      {
        assert(db != nullptr);
        sqlite3_close(db);
        this->open_containers.remove(db);
      }


  }   // namespace transport


#endif //CPPTRANSPORT_DATA_MANAGER_SQLITE3_IMPL_H
