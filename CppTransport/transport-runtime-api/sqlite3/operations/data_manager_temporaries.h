//
// Created by David Seery on 27/04/15.
// Copyright (c) 2015 University of Sussex. All rights reserved.
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
		    sqlite3* create_temp_twopf_container(const boost::filesystem::path& container, unsigned int Nfields, bool collect_stats, bool collect_ics)
			    {
		        sqlite3* db = nullptr;

		        int status = sqlite3_open_v2(container.string().c_str(), &db, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, nullptr);

		        if(status != SQLITE_OK)
			        {
		            std::ostringstream msg;
		            if(db != nullptr)
			            {
		                msg << __CPP_TRANSPORT_DATACTR_TEMPCTR_FAIL_A << " '" << container.string() << "' "
			                << __CPP_TRANSPORT_DATACTR_TEMPCTR_FAIL_B << status << ": " << sqlite3_errmsg(db) << ")";
		                sqlite3_close(db);
			            }
		            else
			            {
		                msg << __CPP_TRANSPORT_DATACTR_TEMPCTR_FAIL_A << " '" << container.string() << "' "
			                << __CPP_TRANSPORT_DATACTR_TEMPCTR_FAIL_B << status << ")";
			            }
		            throw runtime_exception(runtime_exception::DATA_CONTAINER_ERROR, msg.str());
			        }

		        // create the necessary tables
		        create_worker_info_table(db, no_foreign_keys);
		        if(collect_stats) create_stats_table(db, no_foreign_keys, twopf_configs);
		        if(collect_ics) create_ics_table(db, Nfields, no_foreign_keys, twopf_configs, default_ics);
		        create_backg_table(db, Nfields, no_foreign_keys);
		        create_twopf_table(db, Nfields, real_twopf, no_foreign_keys);
		        create_tensor_twopf_table(db, no_foreign_keys);

		        return(db);
			    }


		    // Create a temporary container for a threepf integration
		    sqlite3* create_temp_threepf_container(const boost::filesystem::path& container, unsigned int Nfields, bool collect_stats, bool collect_ics)
			    {
		        sqlite3* db = nullptr;

		        int status = sqlite3_open_v2(container.string().c_str(), &db, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, nullptr);

		        if(status != SQLITE_OK)
			        {
		            std::ostringstream msg;
		            if(db != nullptr)
			            {
		                msg << __CPP_TRANSPORT_DATACTR_TEMPCTR_FAIL_A << " '" << container.string() << "' "
			                << __CPP_TRANSPORT_DATACTR_TEMPCTR_FAIL_B << status << ": " << sqlite3_errmsg(db) << ")";
		                sqlite3_close(db);
			            }
		            else
			            {
		                msg << __CPP_TRANSPORT_DATACTR_TEMPCTR_FAIL_A << " '" << container.string() << "' "
			                << __CPP_TRANSPORT_DATACTR_TEMPCTR_FAIL_B << status << ")";
			            }
		            throw runtime_exception(runtime_exception::DATA_CONTAINER_ERROR, msg.str());
			        }

		        // create the necessary tables
		        create_worker_info_table(db, no_foreign_keys);
		        if(collect_stats) create_stats_table(db, no_foreign_keys, threepf_configs);
		        if(collect_ics)
			        {
		            create_ics_table(db, Nfields, no_foreign_keys, threepf_configs, default_ics);
		            create_ics_table(db, Nfields, no_foreign_keys, threepf_configs, kt_ics);
			        }
		        create_backg_table(db, Nfields, no_foreign_keys);
		        create_twopf_table(db, Nfields, real_twopf, no_foreign_keys);
		        create_twopf_table(db, Nfields, imag_twopf, no_foreign_keys);
		        create_tensor_twopf_table(db, no_foreign_keys);
		        create_threepf_table(db, Nfields, no_foreign_keys);

		        return(db);
			    }


		    // Create a temporary container for a zeta twopf
		    sqlite3* create_temp_zeta_twopf_container(const boost::filesystem::path& container)
			    {
		        sqlite3* db = nullptr;

		        int status = sqlite3_open_v2(container.string().c_str(), &db, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, nullptr);

		        if(status != SQLITE_OK)
			        {
		            std::ostringstream msg;
		            if(db != nullptr)
			            {
		                msg << __CPP_TRANSPORT_DATACTR_TEMPCTR_FAIL_A << " '" << container.string() << "' "
			                << __CPP_TRANSPORT_DATACTR_TEMPCTR_FAIL_B << status << ": " << sqlite3_errmsg(db) << ")";
		                sqlite3_close(db);
			            }
		            else
			            {
		                msg << __CPP_TRANSPORT_DATACTR_TEMPCTR_FAIL_A << " '" << container.string() << "' "
			                << __CPP_TRANSPORT_DATACTR_TEMPCTR_FAIL_B << status << ")";
			            }
		            throw runtime_exception(runtime_exception::DATA_CONTAINER_ERROR, msg.str());
			        }

		        // create the necessary tables
		        create_zeta_twopf_table(db, no_foreign_keys);

		        return(db);
			    }


		    // Create a temporary container for a zeta threepf
		    sqlite3* create_temp_zeta_threepf_container(const boost::filesystem::path& container)
			    {
		        sqlite3* db = nullptr;

		        int status = sqlite3_open_v2(container.string().c_str(), &db, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, nullptr);

		        if(status != SQLITE_OK)
			        {
		            std::ostringstream msg;
		            if(db != nullptr)
			            {
		                msg << __CPP_TRANSPORT_DATACTR_TEMPCTR_FAIL_A << " '" << container.string() << "' "
			                << __CPP_TRANSPORT_DATACTR_TEMPCTR_FAIL_B << status << ": " << sqlite3_errmsg(db) << ")";
		                sqlite3_close(db);
			            }
		            else
			            {
		                msg << __CPP_TRANSPORT_DATACTR_TEMPCTR_FAIL_A << " '" << container.string() << "' "
			                << __CPP_TRANSPORT_DATACTR_TEMPCTR_FAIL_B << status << ")";
			            }
		            throw runtime_exception(runtime_exception::DATA_CONTAINER_ERROR, msg.str());
			        }

		        // create the necessary tables
		        create_zeta_twopf_table(db, no_foreign_keys);
		        create_zeta_threepf_table(db, no_foreign_keys);
		        create_zeta_reduced_bispectrum_table(db, no_foreign_keys);

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
		                msg << __CPP_TRANSPORT_DATACTR_TEMPCTR_FAIL_A << " '" << container.string() << "' "
			                << __CPP_TRANSPORT_DATACTR_TEMPCTR_FAIL_B << status << ": " << sqlite3_errmsg(db) << ")";
		                sqlite3_close(db);
			            }
		            else
			            {
		                msg << __CPP_TRANSPORT_DATACTR_TEMPCTR_FAIL_A << " '" << container.string() << "' "
			                << __CPP_TRANSPORT_DATACTR_TEMPCTR_FAIL_B << status << ")";
			            }
		            throw runtime_exception(runtime_exception::DATA_CONTAINER_ERROR, msg.str());
			        }

		        // create the necessary tables
		        create_fNL_table(db, type, no_foreign_keys);

		        return(db);
			    }

			}   // namespace sqlite3_operations

	}   // namespace transport

#endif //__data_manager_temporaries_H_
