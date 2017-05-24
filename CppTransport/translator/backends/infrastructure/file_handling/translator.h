//
// Created by David Seery on 09/12/2013.
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


#ifndef CPPTRANSPORT_TRANSLATOR_H
#define CPPTRANSPORT_TRANSLATOR_H


#include <iostream>

#include "buffer.h"
#include "output_stack.h"
#include "translator_data.h"
#include "expression_cache.h"
#include "backend_data.h"

#include "ginac_cache.h"
#include "formatter.h"


typedef std::function<std::string(const std::string&)> filter_function;


class translator
  {

  public:

		// constructor
    translator(translator_data& payload);

		// destructor
    ~translator();


		// INTERFACE - TRANSLATION

  public:

    // translate from template 'in', depositing output in the file 'out'
		// implemented internally by constructed a buffer and calling the next variant
    unsigned int translate(const std::string& in, const error_context& ctx, const std::string& out, process_type type, filter_function* filter=nullptr);

		// translate from template 'in', depositing output in the supplied buffer 'buf'
    unsigned int translate(const std::string& in, const error_context& ctx, buffer& buf, process_type type, filter_function* filter=nullptr);


		// INTERFACE - UTILITY FUNCTIONS

  public:

		// print an advisory message to standard output
		// whether the message is actually printed can depend on the current verbosity setting
		// (and maybe other things in future)
		void print_advisory(const std::string& msg);


		// INTERNAL API

  protected:

    // internal API to process a file
    unsigned int process(const boost::filesystem::path& in, buffer& buf, process_type type, filter_function* filter);


		// INTERNAL DATA

  private:

    //! translator_data payload
    translator_data& data_payload;

		//! expression cache for this translator; has to be a pointer because we use it in the destructor
		std::unique_ptr<expression_cache> cache;

  };


#endif //CPPTRANSPORT_TRANSLATOR_H
