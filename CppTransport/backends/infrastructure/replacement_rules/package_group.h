//
// Created by David Seery on 05/12/2013.
// Copyright (c) 2013-15 University of Sussex. All rights reserved.
//



#ifndef __package_group_H_
#define __package_group_H_

#include <memory>

#include "macro.h"
#include "ginac_cache.h"
#include "replacement_rule_package.h"
#include "u_tensor_factory.h"
#include "buffer.h"
#include "cse.h"
#include "error.h"


class package_group
  {

		// CONSTRUCTOR, DESTRUCTOR

  public:

    package_group(translator_data& p, const std::string& cmnt, ginac_cache<expression_item_types, DEFAULT_GINAC_CACHE_SIZE>& cache);

    virtual ~package_group();


		// INTERFACE - REPORT END OF INPUT

  public:

    //! report end of input; the default behaviour is to loop through all registered rule packages,
    //! passing on the notification. However, this can be overridden by implementations if desired.
		virtual void report_end_of_input();


		// INTERFACE - GET RULES ASSOCIATED WITH THIS GROUP OF MACRO PACKAGES

  public:

    // return references to our ruleset caches
    // TODO find some way to prevent them being changed explicitly - they can change *indirectly* by rebuilding the cache, so is it ok to use const?

    //! return reference to pre-rules
    std::vector<macro_packages::simple_rule>& get_pre_ruleset();

    //! return reference to post-rules
    std::vector<macro_packages::simple_rule>& get_post_ruleset();

    //! return reference to index-rules
    std::vector<macro_packages::index_rule>& get_index_ruleset();


		// INTERFACE - QUERY DATA ABOUT THE BACKEND

  public:

		//! make a comment appropriate for this backend
    const std::string& get_comment_separator() const { return (this->comment_string); }


		// INTERFACE - STATISTICS

  public:

		// report macro replacement time data
		void report_macro_metadata(boost::timer::nanosecond_type m, boost::timer::nanosecond_type t) { this->statistics_reported = true; this->macro_replacement_time = m; this->macro_tokenization_time = t; }


		// INTERNAL API

  protected:

    //! register a replacement rule package, transfer its ownership to ourselves, and populate it
    //! with details about the u-tensor factory and CSE worker
    void push_back(std::unique_ptr<macro_packages::replacement_rule_package>&& package);

    //! rebuild pre-ruleset
    void build_pre_ruleset();

    //! rebuild post-ruleset
    void build_post_ruleset();

    //! rebuild index ruleset
    void build_index_ruleset();


		// INTERNAL DATA

  protected:

    translator_data& data_payload;
    std::unique_ptr<u_tensor_factory> u_factory;
    std::unique_ptr<cse>              cse_worker;  // should be set by implementations
    std::unique_ptr<flattener>        fl;

    std::list< std::unique_ptr<macro_packages::replacement_rule_package> > packages;
    std::string                                                            comment_string;

    std::vector<macro_packages::simple_rule> pre_ruleset;
    std::vector<macro_packages::simple_rule> post_ruleset;
    std::vector<macro_packages::index_rule>  index_ruleset;

    // statistics
    bool                          statistics_reported;
    boost::timer::nanosecond_type macro_replacement_time;
    boost::timer::nanosecond_type macro_tokenization_time;

  };


#endif //__package_group_H_
