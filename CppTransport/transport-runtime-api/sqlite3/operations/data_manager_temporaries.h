//
// Created by David Seery on 27/04/15.
// Copyright (c) 2015-2016 University of Sussex. All rights reserved.
//


#ifndef __data_manager_temporaries_H_
#define __data_manager_temporaries_H_


#include "transport-runtime-api/sqlite3/operations/data_manager_common.h"
#include "transport-runtime-api/sqlite3/operations/data_manager_create.h"


namespace transport
	{

		namespace sqlite3_operations
			{

		    // Create a temporary container for a twopf integration
				template <typename number>
		    sqlite3* create_temp_twopf_container(const boost::filesystem::path& container, unsigned int Nfields, bool collect_stats, bool collect_ics)
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

		        // create the necessary tables
		        create_worker_info_table(db, foreign_keys_type::no_foreign_keys);
		        if(collect_stats) create_stats_table(db, foreign_keys_type::no_foreign_keys, kconfiguration_type::twopf_configs);
		        if(collect_ics) create_ics_table<number, typename integration_items<number>::ics_item>(db, Nfields, foreign_keys_type::no_foreign_keys, kconfiguration_type::twopf_configs);
		        create_backg_table<number, typename integration_items<number>::backg_item>(db, Nfields, foreign_keys_type::no_foreign_keys);
		        create_paged_table<number, typename integration_items<number>::twopf_re_item>(db, Nfields, foreign_keys_type::no_foreign_keys);
            create_paged_table<number, typename integration_items<number>::tensor_twopf_item>(db, Nfields, foreign_keys_type::no_foreign_keys);

		        return(db);
			    }


		    // Create a temporary container for a threepf integration
				template <typename number>
		    sqlite3* create_temp_threepf_container(const boost::filesystem::path& container, unsigned int Nfields, bool collect_stats, bool collect_ics)
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

		        // create the necessary tables
		        create_worker_info_table(db, foreign_keys_type::no_foreign_keys);
		        if(collect_stats) create_stats_table(db, foreign_keys_type::no_foreign_keys, kconfiguration_type::threepf_configs);
		        if(collect_ics)
			        {
		            create_ics_table<number, typename integration_items<number>::ics_item>(db, Nfields, foreign_keys_type::no_foreign_keys, kconfiguration_type::threepf_configs);
		            create_ics_table<number, typename integration_items<number>::ics_kt_item>(db, Nfields, foreign_keys_type::no_foreign_keys, kconfiguration_type::threepf_configs);
			        }
		        create_backg_table<number, typename integration_items<number>::backg_item>(db, Nfields, foreign_keys_type::no_foreign_keys);
		        create_paged_table<number, typename integration_items<number>::twopf_re_item>(db, Nfields, foreign_keys_type::no_foreign_keys);
		        create_paged_table<number, typename integration_items<number>::twopf_im_item>(db, Nfields, foreign_keys_type::no_foreign_keys);
            create_paged_table<number, typename integration_items<number>::tensor_twopf_item>(db, Nfields, foreign_keys_type::no_foreign_keys);
            create_paged_table<number, typename integration_items<number>::threepf_momentum_item>(db, Nfields, foreign_keys_type::no_foreign_keys, kconfiguration_type::threepf_configs);
            create_paged_table<number, typename integration_items<number>::threepf_Nderiv_item>(db, Nfields, foreign_keys_type::no_foreign_keys, kconfiguration_type::threepf_configs);

		        return(db);
			    }


		    // Create a temporary container for a zeta twopf
        template <typename number>
		    sqlite3* create_temp_zeta_twopf_container(const boost::filesystem::path& container, unsigned int Nfields)
			    {
		        sqlite3* db = nullptr;

		        int status = sqlite3_open_v2(container.string().c_str(), &db, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, nullptr);

		        if(status != SQLITE_OK)
			        {
		            std::ostringstream msg;
		            if(db != nullptr)
			            {
		                msg
                      << CPPTRANSPORT_DATACTR_TEMPCTR_FAIL_A << " '" << container.string() << "' "
			                << CPPTRANSPORT_DATACTR_TEMPCTR_FAIL_B << status << ": " << sqlite3_errmsg(db) << ")";
		                sqlite3_close(db);
			            }
		            else
			            {
		                msg
                      << CPPTRANSPORT_DATACTR_TEMPCTR_FAIL_A << " '" << container.string() << "' "
			                << CPPTRANSPORT_DATACTR_TEMPCTR_FAIL_B << status << ")";
			            }
		            throw runtime_exception(exception_type::DATA_CONTAINER_ERROR, msg.str());
			        }

		        // create the necessary tables
		        create_zeta_twopf_table(db, foreign_keys_type::no_foreign_keys);
            create_paged_table<number, typename postintegration_items<number>::gauge_xfm1_item>(db, Nfields, foreign_keys_type::no_foreign_keys);

		        return(db);
			    }


		    // Create a temporary container for a zeta threepf
        template <typename number>
		    sqlite3* create_temp_zeta_threepf_container(const boost::filesystem::path& container, unsigned int Nfields)
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

		        // create the necessary tables
		        create_zeta_twopf_table(db, foreign_keys_type::no_foreign_keys);
		        create_zeta_threepf_table(db, foreign_keys_type::no_foreign_keys);
            create_paged_table<number, typename postintegration_items<number>::gauge_xfm1_item>(db, Nfields, foreign_keys_type::no_foreign_keys);
            create_paged_table<number, typename postintegration_items<number>::gauge_xfm2_123_item>(db, Nfields, foreign_keys_type::no_foreign_keys);
            create_paged_table<number, typename postintegration_items<number>::gauge_xfm2_213_item>(db, Nfields, foreign_keys_type::no_foreign_keys);
            create_paged_table<number, typename postintegration_items<number>::gauge_xfm2_312_item>(db, Nfields, foreign_keys_type::no_foreign_keys);

		        return(db);
			    }


		    // Create a temporary container for a zeta threepf
		    sqlite3* create_temp_fNL_container(const boost::filesystem::path& container, derived_data::template_type type)
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

		        // create the necessary tables
		        create_fNL_table(db, type, foreign_keys_type::no_foreign_keys);

		        return(db);
			    }

			}   // namespace sqlite3_operations

	}   // namespace transport

#endif //__data_manager_temporaries_H_
