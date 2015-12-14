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

    package_group(translator_data& p, const std::string cmnt, const std::string opb, const std::string clb,
                  unsigned int brind, unsigned int bkind,
                  ginac_cache<expression_item_types, DEFAULT_GINAC_CACHE_SIZE>& cache);

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
    virtual const std::string& get_comment_separator() const { return(this->comment_string); }

    //! get open-brace appropriate for this backend (if exists)
    virtual const std::string& get_open_brace() const { return(this->open_brace); }

    //! get close-brace appropriate for this backend (if exists)
    virtual const std::string& get_close_brace() const { return(this->close_brace); }

    //! get brace indent
    virtual unsigned int get_brace_indent() const { return(this->brace_indent); }

    //! get block indent
    virtual unsigned int get_block_indent() const { return(this->block_indent); }

    //! plant a 'for' loop appropriate for this backend; should be supplied by a concrete class
    virtual std::string plant_for_loop(const std::string& loop_variable, unsigned int min, unsigned int max) const = 0;


		// INTERFACE - STATISTICS

  public:

		// report macro replacement time data
		void report_macro_metadata(boost::timer::nanosecond_type m, boost::timer::nanosecond_type t)
      {
        this->statistics_reported = true;
        this->macro_replacement_time = m;
        this->macro_tokenization_time = t;
      }


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


    // PAYLOAD DATA (provided by parent translator)

    //! data payload
    translator_data& data_payload;


    // AGENTS

    //! u-tensor factory
    std::unique_ptr<u_tensor_factory> u_factory;

    //! CSE worker
    std::unique_ptr<cse> cse_worker;  // should be set by implementations

    //! flattener
    std::unique_ptr<flattener> fl;


    // CODE GENERATION DATA

    //! comment delimiter
    std::string comment_string;

    //! open brace
    std::string open_brace;

    //! close brace
    std::string close_brace;

    //! brace-level indent
    unsigned int brace_indent;

    //! block-level indent
    unsigned int block_indent;


    // MACRO PACKAGE CACHE

    std::list< std::unique_ptr<macro_packages::replacement_rule_package> > packages;


    // RULE CACHE, BUILD BY AGGREGATING RULES FROM MACRO PACKAGES

    //! rules for pre-macros
    std::vector<macro_packages::simple_rule> pre_ruleset;

    //! rules for post-macros
    std::vector<macro_packages::simple_rule> post_ruleset;

    //! rules for index macros
    std::vector<macro_packages::index_rule>  index_ruleset;


    // STATISTICS AND METADATA

    //! have macro replacement statistics been reported?
    bool statistics_reported;

    //! time taken to replace macros
    boost::timer::nanosecond_type macro_replacement_time;

    //! time taken to tokenize template
    boost::timer::nanosecond_type macro_tokenization_time;

  };


#endif //__package_group_H_
