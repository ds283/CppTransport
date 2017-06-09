//
// Created by David Seery on 11/03/15.
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


#ifndef CPPTRANSPORT_MACRO_TOKENIZER_H
#define CPPTRANSPORT_MACRO_TOKENIZER_H

#include <iostream>
#include <vector>
#include <list>
#include <unordered_map>
#include <string>
#include <functional>

#include "core.h"
#include "error_context.h"
#include "index_assignment.h"
#include "index_literal.h"
#include "replacement_rule_definitions.h"
#include "directive_definitions.h"
#include "macro_types.h"
#include "translator_data.h"
#include "macro_tokens.h"

#include "boost/optional.hpp"


// forward-declare package_group, if needed
class package_group;


//! tokenize a string and supply methods on the resulting list of tokens
class token_list
  {

    // CONSTRUCTOR, DESTRUCTOR

  public:

    //! build a token list from an input string
    //! the supplied package_group determined which replacement rules and directives are recognized during
    //! tokenization, and the index_ruleset 'lr' provides a list of local rules (eg. injected by the user
    //! using the $SET directive).
    //! If the optional index_literal_database 'v' is provided then it is used to validate the properties
    //! of recognized indices -- eg. RHS indices that match LHS indices should also match the variance
    //! specified on the LHS
    token_list(const std::string& in, const std::string& pfx, unsigned int nf, unsigned int np,
               const package_group& pkg, const index_ruleset& lr, translator_data& d,
               boost::optional<index_literal_database&> v=boost::none, bool s=false);

    // suppress default copy constructor
    token_list(const token_list& obj) = delete;

    //! destructor is default
    ~token_list() = default;


    // INTERFACE -- UTILITIES

  public:

    //! get number of tokens
    unsigned int size() const { return (this->tokens.size()); }


    //! reset token list to pristine state, as if no evaluations had taken place
    //! principally used to force call to post-hooks and reset pre-hooks
    void reset();


    // INTERFACE -- MACRO EVALUATION AND CONVERSION

  public:

    //! convert to string form, using evaluated versions of each macro
    std::string to_string();

    //! evaluate simple macros of a specific type, and cache the result.
    //! We only want to do this once if possible, since macro evaluation may be expensive.
    unsigned int evaluate_macros(simple_macro_type type);

    //! evaluate index macros on a specific assignment and cache the result;
    //! used when unrolling index sets
    unsigned int evaluate_macros(const assignment_list& a);

    //! evaluate index macros on an abstract assignment and cache the result;
    //! used when planting code as a 'for'-loop
    unsigned int evaluate_macros();

    //! evaluate index macros on an abstract assignment determined by the given substitution rule
    unsigned int evaluate_macros(const index_remap_rule& rule);


    // INDEX SERVICES

  public:

    //! get database of abstract indices identified during tokenization
    const abstract_index_database& get_index_database() const { return this->index_db; }

    //! get list of index instances identified during tokenization
    const index_literal_list& get_index_declarations() const { return this->index_decls; }


    // INTERFACE -- INDEX SET UNROLLING

  public:

    //! does this token set prevent, force or allow unrolling?
    unroll_behaviour unroll_status() const;
    
    
    // INTERFACE -- DIRECTIVES
    
  public:
    
    //! does the tokenized form contain directives?
    bool is_directive() const { return !this->simple_directive_tokens.empty() || !this->index_directive_tokens.empty(); }


    // INTERNAL API

  protected:

    //! tokenize a macro or free index
    template <typename ContextFactory>
    std::pair<std::unique_ptr<token_list_impl::generic_token>, size_t>
    match_macro_or_index(const size_t position, ContextFactory make_context);

    //! build an index literal token
    template <typename ContextFactory>
    std::pair<std::unique_ptr<token_list_impl::index_literal_token>, size_t>
    make_index_literal(const size_t position, ContextFactory make_context);

    //! build a simple macro token
    template <typename RuleSet, typename ContextFactory>
    std::pair<std::unique_ptr<token_list_impl::simple_macro_token>, size_t>
    make_simple_macro(const std::string& macro, const size_t position, const RuleSet& rules, simple_macro_type type,
                      ContextFactory make_context);

    //! build an index macro token
    template <typename RuleSet, typename ContextFactory>
    std::pair<std::unique_ptr<token_list_impl::index_macro_token>, size_t>
    make_index_macro(const std::string& macro, const size_t position, const RuleSet& rules, ContextFactory make_context);

    //! add an index to our internal list
    abstract_index_database::iterator add_index(char label);
    
    //! build a simple directive token
    template <typename RuleSet, typename ContextFactory>
    std::pair<std::unique_ptr<token_list_impl::simple_directive_token>, size_t>
    make_simple_directive(const std::string& macro, const size_t position, const RuleSet& rules,
                              ContextFactory make_context);
    
    //! build an index directive token
    template <typename RuleSet, typename ContextFactory>
    std::pair<std::unique_ptr<token_list_impl::index_directive_token>, size_t>
    make_index_directive(const std::string& macro, const size_t position, const RuleSet& rules,
                             ContextFactory make_context);


    // VALIDATION

  protected:
    
    //! validate whether it's OK to process a replacement rule (directives and rules should not be mixed)
    template <typename ContextFactory>
    bool validate_replacement_rule(const std::string& macro, const size_t position, ContextFactory make_context);
    
    //! validate whether it's OK to process a directive (directives and rules should not be mixed)
    template <typename ContextFactory>
    bool validate_directive(const std::string& macro, const size_t position, ContextFactory make_context);

    //! validate an index_literal against a supplied database
    void validate_index_literal(index_literal& l);
    
    //! validate variance assignment
    void validate_index_properties(index_literal& l);
    

    // INTERNAL DATA

  protected:


    // PAYLOAD DATA (provided to us by owning translator)

    //! tokenized input string; ownership is shared with any error contexts which we generate
    std::shared_ptr<std::string> input_string;

    //! reference to translator data payload
    translator_data& data_payload;

    //! reference to replacement rule package
    const package_group& package;


    // SETTINGS

    //! rule prefix
    const std::string prefix;

    //! validation database, if supplied
    boost::optional< index_literal_database& > validation_db;

    //! is validation strict? ie., should we reject indices not in the validation database?
    const bool strict;


    // RULE SET CACHES

    //! pre-rules
    const pre_ruleset& pre;

    //! post-rules
    const post_ruleset& post;

    //! index rules
    const index_ruleset& index;

    //! locally-defined index rules
    const index_ruleset& local_rules;

    //! simple directives
    const simple_directiveset& simp_dir;

    //! index directives
    const index_directiveset& ind_dir;


    // TOKEN DATA

    typedef std::list< std::unique_ptr< token_list_impl::generic_token > > token_database;
    typedef std::list< std::reference_wrapper< token_list_impl::simple_macro_token > > simple_macro_token_database;
    typedef std::list< std::reference_wrapper< token_list_impl::index_macro_token > > index_macro_token_database;
    typedef std::list< std::reference_wrapper< token_list_impl::index_literal_token > > index_literal_token_database;
    typedef std::list< std::reference_wrapper< token_list_impl::simple_directive_token > > simple_directive_token_database;
    typedef std::list< std::reference_wrapper< token_list_impl::index_directive_token > > index_directive_token_database;

    //! tokenized version of input
    token_database tokens;

    //! auxiliary list of simple macro tokens
    simple_macro_token_database simple_macro_tokens;

    //! auxiliary list of index macro tokens
    index_macro_token_database index_macro_tokens;

    //! auxiliary list of index literal tokens
    index_literal_token_database index_literal_tokens;
    
    //! auxiliary list of simple directive tokens
    simple_directive_token_database simple_directive_tokens;
    
    //! axiliary list of index directive tokens
    index_directive_token_database index_directive_tokens;


    // MACRO DATA

    //! list of macro names preventing loop unroll
    std::list<std::string> prevent_unroll;

    //! list of macro names forcing loop unroll
    std::list<std::string> force_unroll;
    

    // INDEX DATA (maintains information about indices encountered in this entire line)

    //! master database of indices found in input line
    abstract_index_database index_db;

    //! list of index instances found in input line, in parse order
    index_literal_list index_decls;


    // CACHE DATA ABOUT MODEL

    //! cache number of fields
    const unsigned int num_fields;

    //! cache number of parameters
    const unsigned int num_params;

  };

#endif //CPPTRANSPORT_MACRO_TOKENIZER_H
