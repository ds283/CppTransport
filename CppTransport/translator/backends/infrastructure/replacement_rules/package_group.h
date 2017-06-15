//
// Created by David Seery on 05/12/2013.
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


#ifndef CPPTRANSPORT_PACKAGE_GROUP_H
#define CPPTRANSPORT_PACKAGE_GROUP_H


#include <memory>

#include "macro.h"
#include "ginac_cache.h"
#include "replacement_rule_package.h"
#include "directive_package.h"
#include "concepts/tensor_factory.h"
#include "buffer.h"
#include "cse.h"
#include "lambda_manager.h"
#include "error.h"


template <typename PrinterAgent, typename CSEAgent, typename LambdaAgent>
class PackageGroupComponentAgent
  {

    // CONSTRUCTOR, DESTRUCTOR

  public:

    //! constructor
    PackageGroupComponentAgent() = default;

    //! destructor
    ~PackageGroupComponentAgent() = default;


    // SERVICES

  public:

    //! make a printer agent
    std::unique_ptr<PrinterAgent> make_printer_agent() const
      { return std::make_unique<PrinterAgent>(); }

    //! make a CSE agent
    std::unique_ptr<CSEAgent> make_cse_agent(language_printer& p, translator_data& d) const
      { return std::make_unique<CSEAgent>(0, p, d); }

    //! make a lambda agent
    std::unique_ptr<LambdaAgent> make_lambda_agent(language_printer& p, translator_data& d) const
      { return std::make_unique<LambdaAgent>(0, p, d, std::move(make_cse_agent(p, d))); }

  };


class package_group
  {
    
    // TYPES
    
  protected:
    
    //! database of replacement rule packages
    typedef std::list< std::unique_ptr<macro_packages::replacement_rule_package> > replacement_rule_database;
    
    //! database of directive packages
    typedef std::list< std::unique_ptr<macro_packages::directive_package> > directive_database;
    

		// CONSTRUCTOR, DESTRUCTOR

  public:

    //! constructor accepts a generic ComponentFactory object whose job is to
    //! construct the langugae printer, CSE and lambda management agents that we need
    template <typename ComponentFactory>
    package_group(translator_data& p, tensor_factory& fctry, ComponentFactory F);

    //! destructor reports statistics for replacements associated with this group
    virtual ~package_group();


		// INTERFACE - REPORT END OF INPUT

  public:

    //! report end of input; the default behaviour is to loop through all registered rule packages,
    //! passing on the notification. However, this can be overridden by implementations if desired.
		virtual void report_end_of_input();


		// INTERFACE - GET RULES ASSOCIATED WITH THIS GROUP OF MACRO PACKAGES

  public:

    // return references to our ruleset and directiveset caches

    //! return reference to pre-rules
    const pre_ruleset& get_pre_ruleset() const { return this->pre_rules; }

    //! return reference to post-rules
    const post_ruleset& get_post_ruleset() const { return this->post_rules; }

    //! return reference to index-rules
    const index_ruleset& get_index_ruleset() const { return this->index_rules; }
    
    //! return reference to simple directives
    const simple_directiveset& get_simple_directiveset() const { return this->simple_directives; }
    
    //! return reference to index directives
    const index_directiveset& get_index_directiveset() const { return this->index_directives; }


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
    void add_rule_package(Args&& ... args);
    
    //! register a directive package
    template <typename PackageType, typename ... Args>
    void add_directive_package(Args&& ... args);
    
    //! rebuild pre-ruleset
    void build_pre_ruleset();

    //! rebuild post-ruleset
    void build_post_ruleset();

    //! rebuild index ruleset
    void build_index_ruleset();
    
    //! rebuild simple directiveset
    void build_simple_directiveset();
    
    //! rebuild index directiveset
    void build_index_directiveset();
    
    //! generic rule to rebuild an indexset or directiveset
    template <typename SourceDatabase, typename DestinationContainer, typename RuleGetter>
    void build_set(const SourceDatabase& src, DestinationContainer& dest, RuleGetter get_rules);

    
		// INTERNAL DATA

  protected:


    // PAYLOAD DATA (provided by parent translator)

    //! data payload
    translator_data& data_payload;


    // AGENTS

    //! polymorphic reference to tensor factory; exactly which factory is involved
    //! may depend on what kind of model is being processed, eg. canonical, noncanonical
    tensor_factory& fctry;

    //! polymorphic pointer to language printer; as above, depends on what kind
    //! of model is being processed
    std::unique_ptr<language_printer> l_printer;

    //! polymorphic pointer to CSE worker; as above exactly which CSE scheme is
    //! involved may depend what kind of model is being processed
    //! must be declared *after* l_printer, since depends on l_printer during construction
    std::unique_ptr<cse> cse_worker;

    //! lambda manager; has to be held as a managed pointer since depends on
    //! the language printer, which isn't known until construction
    //! must be declared *after* l_printer, since depends on l_printer during construction
    std::unique_ptr<lambda_manager> lambda_mgr;


    // MACRO PACKAGE CACHE

    //! set of replacement rule packages
    replacement_rule_database rule_packages;
    
    //! set of directive packages
    directive_database directive_packages;


    // RULE CACHE, BUILT BY AGGREGATING RULES FROM MACRO PACKAGES

    // all these are held as raw pointers because
    // we have no ownership in these objects.
    // They are owned by the underlying replacement_rule_package

    //! rules for pre-macros
    pre_ruleset pre_rules;

    //! rules for post-macros
    post_ruleset post_rules;

    //! rules for index macros
    index_ruleset index_rules;
    
    //! rules for simple directives
    simple_directiveset simple_directives;
    
    //! rules for index directives
    index_directiveset index_directives;


    // STATISTICS AND METADATA

    //! have macro replacement statistics been reported?
    bool statistics_reported;

    //! time taken to replace macros
    boost::timer::nanosecond_type macro_replacement_time;

    //! time taken to tokenize template
    boost::timer::nanosecond_type macro_tokenization_time;

  };


template <typename ComponentFactory>
package_group::package_group(translator_data& p, tensor_factory& _f, ComponentFactory F)
  : data_payload(p),
    statistics_reported(false),
    fctry(_f),
    l_printer(F.make_printer_agent()),
    cse_worker(F.make_cse_agent(*l_printer, p)),
    lambda_mgr(F.make_lambda_agent(*l_printer, p))
  {
  }


template <typename PackageType, typename ... Args>
void package_group::add_rule_package(Args&& ... args)
  {
    // establish that everything has been set up correctly
    assert(this->cse_worker);

    // construct a new package of the specified type, forwarding any arguments we were given
    std::unique_ptr< macro_packages::replacement_rule_package > pkg =
      std::make_unique<PackageType>(this->fctry, *this->cse_worker, *this->lambda_mgr, std::forward<Args>(args) ...);
    this->rule_packages.push_back(std::move(pkg));

    // rebuild ruleset caches
    this->build_pre_ruleset();
    this->build_post_ruleset();
    this->build_index_ruleset();
  }


template <typename PackageType, typename ... Args>
void package_group::add_directive_package(Args&& ... args)
  {
    // construct new package of the specified type
    std::unique_ptr< macro_packages::directive_package > pkg =
      std::make_unique<PackageType>(std::forward<Args>(args) ...);
    this->directive_packages.push_back(std::move(pkg));
    
    // rebuild directiveset caches
    this->build_simple_directiveset();
    this->build_index_directiveset();
  }


#endif //CPPTRANSPORT_PACKAGE_GROUP_H
