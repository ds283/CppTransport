//
// Created by David Seery on 09/12/2013.
// Copyright (c) 2013-15 University of Sussex. All rights reserved.
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
    unsigned int translate(const std::string& in, const error_context& ctx, const std::string& out, enum process_type type, filter_function* filter=nullptr);

		// translate from template 'in', depositing output in the supplied buffer 'buf'
    unsigned int translate(const std::string& in, const error_context& ctx, buffer& buf, enum process_type type, filter_function* filter=nullptr);


		// INTERFACE - UTILITY FUNCTIONS

  public:

		// print an advisory message to standard output
		// whether the message is actually printed can depend on the current verbosity setting
		// (and maybe other things in future)
		void print_advisory(const std::string& msg);


		// INTERNAL API

  protected:

    // internal API to process a file
    unsigned int process(const boost::filesystem::path& in, buffer& buf, enum process_type type, filter_function* filter);


		// INTERNAL DATA

  private:

    //! translator_data payload
    translator_data& data_payload;

		//! expression cache for this translator
		ginac_cache<expression_item_types, DEFAULT_GINAC_CACHE_SIZE> cache;

  };


#endif //CPPTRANSPORT_TRANSLATOR_H
