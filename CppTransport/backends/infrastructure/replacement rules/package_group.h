//
// Created by David Seery on 05/12/2013.
// Copyright (c) 2013-15 University of Sussex. All rights reserved.
//



#ifndef __package_group_H_
#define __package_group_H_


#include "macro.h"
#include "ginac_cache.h"
#include "replacement_rule_package.h"
#include "u_tensor_factory.h"
#include "buffer.h"
#include "cse.h"
#include "error.h"


// need a forward reference to avoid circularity
class translation_unit;

class package_group
  {

		// CONSTRUCTOR, DESTRUCTOR

  public:

    package_group(translation_unit* u, const std::string& cmnt, ginac_cache<expression_item_types, DEFAULT_GINAC_CACHE_SIZE>& cache);

    virtual ~package_group();


		// INTERFACE - ERRORS AND WARNINGS

  public:

    void                                                 error              (const std::string msg);
    void                                                 warn               (const std::string msg);


		// INTERFACE - GET RULES ASSOCIATED WITH THIS GROUP OF MACRO PACKAGES

  public:

    // return references to our internal ruleset caches
    // TODO find some way to prevent them being changed explicitly - they can change *indirectly* by rebuilding the cache, so is it ok to use const?
    std::vector<macro_packages::simple_rule>&            get_pre_ruleset    ();
    std::vector<macro_packages::simple_rule>&            get_post_ruleset   ();
    std::vector<macro_packages::index_rule>&             get_index_ruleset  ();


		// INTERFACE - QUERY DATA ABOUT THE BACKEND

  public:

		// make a comment appropriate for this backend
		const std::string&                                   get_comment_separator() const { return(this->comment_string); }


		// INTERFACE - STATISTICS

  public:

		// report macro replacement time data
		void report_macro_metadata(boost::timer::nanosecond_type m, boost::timer::nanosecond_type t) { this->statistics_reported = true; this->macro_replacement_time = m; this->macro_tokenization_time = t; }


		// INTERNAL API

  protected:

    void                                                 push_back          (macro_packages::replacement_rule_package* package);

    void                                                 build_pre_ruleset  ();
    void                                                 build_post_ruleset ();
    void                                                 build_index_ruleset();


		// INTERNAL DATA

  protected:

    translation_unit*                                    unit;
    u_tensor_factory*                                    u_factory;
    cse*                                                 cse_worker;  // should be set by implementations
    flattener*                                           fl;

    std::list<macro_packages::replacement_rule_package*> packages;
		std::string                                          comment_string;

    std::vector<macro_packages::simple_rule>             pre_ruleset;
    std::vector<macro_packages::simple_rule>             post_ruleset;
    std::vector<macro_packages::index_rule>              index_ruleset;

		// statistics
		bool statistics_reported;
		boost::timer::nanosecond_type macro_replacement_time;
		boost::timer::nanosecond_type macro_tokenization_time;

  };


#endif //__package_group_H_
