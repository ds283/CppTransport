//
// Created by David Seery on 05/12/2013.
// Copyright (c) 2016 University of Sussex. All rights reserved.
//


#ifndef CPPTRANSPORT_PACKAGE_GROUP_H
#define CPPTRANSPORT_PACKAGE_GROUP_H


#include <memory>

#include "macro.h"
#include "ginac_cache.h"
#include "replacement_rule_package.h"
#include "concepts/tensor_factory.h"
#include "buffer.h"
#include "cse.h"
#include "lambda_manager.h"
#include "error.h"


class package_group
  {

		// CONSTRUCTOR, DESTRUCTOR

  public:

    package_group(translator_data& p, tensor_factory& fctry);

    virtual ~package_group();


		// INTERFACE - REPORT END OF INPUT

  public:

    //! report end of input; the default behaviour is to loop through all registered rule packages,
    //! passing on the notification. However, this can be overridden by implementations if desired.
		virtual void report_end_of_input();


		// INTERFACE - GET RULES ASSOCIATED WITH THIS GROUP OF MACRO PACKAGES

  public:

    // return references to our ruleset caches

    //! return reference to pre-rules
    std::vector<macro_packages::replacement_rule_simple*>& get_pre_ruleset()   { return(this->pre_ruleset); }

    //! return reference to post-rules
    std::vector<macro_packages::replacement_rule_simple*>& get_post_ruleset()  { return(this->post_ruleset); }

    //! return reference to index-rules
    std::vector<macro_packages::replacement_rule_index*>& get_index_ruleset()  { return(this->index_ruleset); }


		// INTERFACE - GET LANGUAGE PRINTER FOR THIS PACKAGE GROUP

  public:

    //! return reference to language printer
    language_printer& get_language_printer() { return(*this->l_printer); }    // will throw exception if l_printer has not been set


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
    template <typename PackageType, typename ... Args>
    void add_package(Args&& ... args);

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

    //! polymorphic reference to tensor factory; exactly which factory is involved
    //! may depend on what kind of model is being processed, eg. canonical, noncanonical
    tensor_factory& fctry;

    //! polymorphic pointer to CSE worker; as above exactly which CSE scheme is
    //! involved may depend what kind of model is being processed
    std::unique_ptr<cse> cse_worker;  // should be set by implementations

    //! lambda manager; has to be held as a managed pointer since depends on
    //! the language printer, which isn't known until construction
    std::unique_ptr<lambda_manager> lambda_mgr;

    //! polymorphic pointer to language printer; as above, depends on what kind
    //! of model is being processed
    std::unique_ptr<language_printer> l_printer;  // should be set by implementations


    // MACRO PACKAGE CACHE

    std::list< std::unique_ptr<macro_packages::replacement_rule_package> > packages;


    // RULE CACHE, BUILD BY AGGREGATING RULES FROM MACRO PACKAGES

    // all these are held as raw pointers because
    // we have no ownership in these objects.
    // They are owned by the underylying replacement_rule_package

    // rules for pre-macros
    std::vector<macro_packages::replacement_rule_simple*> pre_ruleset;

    //! rules for post-macros
    std::vector<macro_packages::replacement_rule_simple*> post_ruleset;

    //! rules for index macros
    std::vector<macro_packages::replacement_rule_index*> index_ruleset;


    // STATISTICS AND METADATA

    //! have macro replacement statistics been reported?
    bool statistics_reported;

    //! time taken to replace macros
    boost::timer::nanosecond_type macro_replacement_time;

    //! time taken to tokenize template
    boost::timer::nanosecond_type macro_tokenization_time;

  };


template <typename PackageType, typename ... Args>
void package_group::add_package(Args&& ... args)
  {
    // establish that everything has been set up correctly
    assert(this->cse_worker);

    // construct a new package of the specified type, forwarding any arguments we were given
    std::unique_ptr< macro_packages::replacement_rule_package> pkg = std::make_unique<PackageType>(this->fctry, *this->cse_worker, *this->lambda_mgr, std::forward<Args>(args) ...);
    this->packages.push_back(std::move(pkg));

    // rebuild ruleset caches
    this->build_pre_ruleset();
    this->build_post_ruleset();
    this->build_index_ruleset();
  }


#endif //CPPTRANSPORT_PACKAGE_GROUP_H
