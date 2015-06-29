//
// Created by David Seery on 30/03/15.
// Copyright (c) 2015 University of Sussex. All rights reserved.
//


#ifndef __sqlite3_utility_H_
#define __sqlite3_utility_H_


#include "transport-runtime-api/messages.h"


namespace transport
	{

		namespace sqlite3_operations
			{

		    // error-check an exec statement
		    inline void exec(sqlite3* db, const std::string& stmt, const std::string& err)
			    {
		        char* errmsg;

		        int status = sqlite3_exec(db, stmt.c_str(), nullptr, nullptr, &errmsg);

		        if(status != SQLITE_OK)
			        {
		            std::ostringstream msg;
		            msg << err << errmsg << ") [status=" << status << "]";
		            throw runtime_exception(runtime_exception::DATA_CONTAINER_ERROR, msg.str());
			        }
			    }


		    // error-check an exec statement
		    inline void exec(sqlite3* db, const std::string& stmt)
			    {
		        char* errmsg;

		        int status = sqlite3_exec(db, stmt.c_str(), nullptr, nullptr, &errmsg);

		        if(status != SQLITE_OK)
			        {
		            std::ostringstream msg;
		            msg << CPPTRANSPORT_SQLITE_UTILITY_ERROR << " " << errmsg << " [status=" << status << "]";
		            throw runtime_exception(runtime_exception::DATA_CONTAINER_ERROR, msg.str());
			        }
			    }


		    // error-check a non-exec statement
		    inline void check_stmt(sqlite3* db, int status, const std::string& err, int check_code=SQLITE_OK)
			    {
		        if(status != check_code)
			        {
		            std::ostringstream msg;
		            msg << err << sqlite3_errmsg(db) << ") [status=" << status << "]";
		            throw runtime_exception(runtime_exception::DATA_CONTAINER_ERROR, msg.str());
			        }
			    }


		    // error-check a non-exec statement
		    inline void check_stmt(sqlite3* db, int status, int check_code=SQLITE_OK)
			    {
		        if(status != check_code)
			        {
		            std::ostringstream msg;
		            msg << CPPTRANSPORT_SQLITE_UTILITY_ERROR << " " << sqlite3_errmsg(db) << " [status=" << status << "]";
		            std::cerr << msg.str() << '\n';
				        assert(false);
		            throw runtime_exception(runtime_exception::DATA_CONTAINER_ERROR, msg.str());
			        }
			    }

			}   // namespace sqlite3_operations

	}   // namespace transport


#endif //__sqlite3_utility_H_
