//
// Created by David Seery on 09/12/2013.
// Copyright (c) 2013-15 University of Sussex. All rights reserved.
//


#ifndef __translator_H_
#define __translator_H_


#include "buffer.h"
#include "output_stack.h"

#include "ginac_cache.h"
#include "formatter.h"


enum expression_item_types
	{
    sr_U_item, U1_item, U2_item, U3_item, A_item, B_item, C_item, M_item, zxfm1_item, zxfm2_item, dN1_item, dN2_item
	};


// need forward reference to avoid circularity
class translation_unit;

typedef std::function<std::string(const std::string&)> filter_function;

class translator
  {

  public:

		// constructor
    translator(translation_unit* tu);

		// destructor
    ~translator();


		// INTERFACE - TRANSLATION

  public:

    // translate from template 'in', depositing output in the file 'out'
		// implemented internally by constructed a buffer and calling the next variant
    unsigned int translate(const std::string& in, const std::string& out, enum process_type type, filter_function* filter=nullptr);

		// translate from template 'in', depositing output in the supplied buffer 'buf'
    unsigned int translate(const std::string& in, buffer& buf, enum process_type type, filter_function* filter=nullptr);


		// INTERFACE - UTILITY FUNCTIONS

  public:

		// print an advisory message to standard output
		// whether the message is actually printing can depend on the current verbosity setting
		// (and maybe other things in future)
		void print_advisory(const std::string& msg);


		// INTERNAL API

  protected:

    // internal API to process a file
    unsigned int process(const std::string in, buffer& buf, enum process_type type, filter_function* filter);

    // parse the header line from a template, tokenizing it into 'backend' and 'minimum version' data
    void parse_header_line(const std::string in, const std::string line, std::string& backend, double& minver);


		// INTERNAL DATA

  private:

		//! point back to parent translation unit
    translation_unit* unit;

		//! expression cache for this translation unit
		ginac_cache<expression_item_types, DEFAULT_GINAC_CACHE_SIZE> cache;

  };


#endif //__translator_H_
