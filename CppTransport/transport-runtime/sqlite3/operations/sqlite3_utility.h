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


#ifndef CPPTRANSPORT_SQLITE3_UTILITY_H
#define CPPTRANSPORT_SQLITE3_UTILITY_H


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


				// apply PRAGMAs to optimize performance
				inline void performance_pragmas(sqlite3* db)
					{
						// SQLite performance choices:
						// http://blog.devart.com/increasing-sqlite-performance.html
						// https://wiki.mozilla.org/Performance/Avoid_SQLite_In_Your_Next_Firefox_Feature#Important_Pragmas
						// http://stackoverflow.com/questions/784173/what-are-the-performance-characteristics-of-sqlite-with-very-large-database-file

						// attempt to speed up insertions by disabling foreign key constraints
						// during initial writes
						char* errmsg;
						sqlite3_exec(db, "PRAGMA foreign_keys = OFF;", nullptr, nullptr, &errmsg);

						// if write-ahead log mode is disabled (as it must be if a network filing system is in play)
						// then put journal into truncate mode
						// otherwise, enable to write-ahead log
#ifndef CPPTRANSPORT_NETWORK_FILESYSTEM
						sqlite3_exec(db, "PRAGMA journal_mode = WAL;", nullptr, nullptr, &errmsg);
#else
						sqlite3_exec(db, "PRAGMA journal_mode = TRUNCATE;", nullptr, nullptr, &errmsg);
#endif

						// force temporary objects to be stored in memory, for speed
						sqlite3_exec(db, "PRAGMA temp_store = MEMORY;", nullptr, nullptr, &errmsg);

						// set SYNCHRONOUS mode to 'Normal' rather than 'Full'
						sqlite3_exec(db, "PRAGMA synchronous = NORMAL;", nullptr, nullptr, &errmsg);

						// CACHE_SIZE unlikely to make much difference except in windows
						sqlite3_exec(db, "PRAGMA cache_size = 10000;", nullptr, nullptr, &errmsg);

						// PAGE_SIZE unlikely to make much difference except in windows
						sqlite3_exec(db, "PRAGMA page_size = 4096;", nullptr, nullptr, &errmsg);
					}


				// apply PRAGMAs to optimize performance
				inline void reading_pragmas(sqlite3* db)
					{
						// SQLite performance choices:
						// http://blog.devart.com/increasing-sqlite-performance.html
						// https://wiki.mozilla.org/Performance/Avoid_SQLite_In_Your_Next_Firefox_Feature#Important_Pragmas
						// http://stackoverflow.com/questions/784173/what-are-the-performance-characteristics-of-sqlite-with-very-large-database-file

						char* errmsg;
						sqlite3_exec(db, "PRAGMA foreign_keys = ON;", nullptr, nullptr, &errmsg);

						// if write-ahead log mode is disabled (as it must be if a network filing system is in play)
						// then put journal into truncate mode
						// otherwise, enable to write-ahead log
#ifndef CPPTRANSPORT_NETWORK_FILESYSTEM
						sqlite3_exec(db, "PRAGMA journal_mode = WAL;", nullptr, nullptr, &errmsg);
#else
						sqlite3_exec(db, "PRAGMA journal_mode = TRUNCATE;", nullptr, nullptr, &errmsg);
#endif

						// force temporary objects to be stored in memory, for speed
						sqlite3_exec(db, "PRAGMA temp_store = MEMORY;", nullptr, nullptr, &errmsg);

						// set SYNCHRONOUS mode to 'Normal' rather than 'Full'
						sqlite3_exec(db, "PRAGMA synchronous = NORMAL;", nullptr, nullptr, &errmsg);

						// CACHE_SIZE unlikely to make much difference except in windows
						sqlite3_exec(db, "PRAGMA cache_size = 10000;", nullptr, nullptr, &errmsg);

						// PAGE_SIZE unlikely to make much difference except in windows
						sqlite3_exec(db, "PRAGMA page_size = 4096;", nullptr, nullptr, &errmsg);
					}


			}   // namespace sqlite3_operations

	}   // namespace transport


#endif //CPPTRANSPORT_SQLITE3_UTILITY_H
