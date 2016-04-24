//
// Created by David Seery on 30/03/15.
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


#ifndef __sqlite3_utility_H_
#define __sqlite3_utility_H_


#include "transport-runtime/messages.h"


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
		            throw runtime_exception(exception_type::DATA_CONTAINER_ERROR, msg.str());
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
		            throw runtime_exception(exception_type::DATA_CONTAINER_ERROR, msg.str());
			        }
			    }


		    // error-check a non-exec statement
		    inline void check_stmt(sqlite3* db, int status, const std::string& err, int check_code=SQLITE_OK)
			    {
		        if(status != check_code)
			        {
		            std::ostringstream msg;
		            msg << err << sqlite3_errmsg(db) << ") [status=" << status << "]";
		            throw runtime_exception(exception_type::DATA_CONTAINER_ERROR, msg.str());
			        }
			    }


		    // error-check a non-exec statement
		    inline void check_stmt(sqlite3* db, int status, int check_code=SQLITE_OK)
			    {
		        if(status != check_code)
			        {
		            std::ostringstream msg;
		            msg << CPPTRANSPORT_SQLITE_UTILITY_ERROR << " " << sqlite3_errmsg(db) << " [status=" << status << "]";
//		            std::cerr << msg.str() << '\n';
//				        assert(false);
		            throw runtime_exception(exception_type::DATA_CONTAINER_ERROR, msg.str());
			        }
			    }

			}   // namespace sqlite3_operations

	}   // namespace transport


#endif //__sqlite3_utility_H_
