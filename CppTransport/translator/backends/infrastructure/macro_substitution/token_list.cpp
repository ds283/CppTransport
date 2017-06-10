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


#include <assert.h>
#include <sstream>

#include "token_list.h"
#include "package_group.h"


namespace macro_tokenizer_impl
  {


    // match a text token from the string starting at position
    template <typename ContextFactory>
    std::pair< std::unique_ptr<token_list_impl::text_token>, size_t >
    match_string_literal(const std::string& input, const size_t position, char terminal, ContextFactory make_context)
      {
        size_t current = position;

        // shift first character of text token into a std::string and move pointer past it
        std::string string_literal(1, input[current++]);

        while(current < input.length() && input[current] != terminal)
          {
            string_literal += input[current++];
          }

        return std::make_pair(
          std::make_unique<token_list_impl::text_token>(string_literal, make_context(position, current)), current);
      };


    // check whether we match the full macro prefix, starting at position
    bool check_match_prefix(const std::string& input, const size_t position, const std::string& prefix)
      {
        size_t count = 0;

        while(position + count < input.length() && count < prefix.length() && input[position+count] == prefix[count])
          {
            ++count;
          }

        return count >= prefix.length();
      }


    // get a macro argument list
    template <typename ContextFactory>
    std::pair<macro_argument_list, size_t>
    get_argument_list(const std::string& input, const std::string& macro, const size_t position,
                      ContextFactory make_context)
      {
        macro_argument_list arg_list;
        size_t current_position = position;

        // check that an argument list is present
        if(current_position >= input.length() || input[current_position] != '{')
          {
            std::ostringstream msg;
            msg << ERROR_EXPECTED_OPEN_ARGUMENT_LIST << " '" << macro << "'";
            error_context ctx = make_context(current_position, current_position+1);
            ctx.error(msg.str());

            return std::make_pair(arg_list, current_position);
          }

        ++current_position;   // move past opening bracket of index list
        
        while(current_position < input.length() && input[current_position] != '}')
          {
            // skip over any white space characters
            while(current_position < input.length() && isspace(input[current_position]))
              {
                ++current_position;
              }
            
            if(input[current_position] == ',')
              {
                std::ostringstream msg;
                msg  << ERROR_UNEXPECTED_COMMA << " '" << macro << "'";
                error_context ctx = make_context(current_position, current_position+1);
                ctx.error(msg.str());
              }
            else if(input[current_position] == '"')
              {
                size_t start = current_position;
                std::string arg;
    
                // skip opening quote
                ++current_position;

                while(current_position < input.length() && input[current_position] != '"')
                  {
                    arg += input[current_position++];
                  }

                // check closing quote is in place
                if(!(current_position < input.length() && input[current_position] == '"'))
                  {
                    std::ostringstream msg;
                    msg  << ERROR_EXPECTED_CLOSE_ARGUMENT_QUOTE << " '" << macro << "'";
                    error_context ctx = make_context(current_position, current_position+1);
                    ctx.error(msg.str());
                  }
                else
                  {
                    ++current_position;
                  }
    
                error_context ctx = make_context(start, current_position);
                arg_list.emplace_back(arg, ctx);
              }
            else
              {
                // now at the start of an argument
                size_t start = current_position;
                std::string arg;
    
                while(current_position < input.length()
                      && input[current_position] != ','
                      && input[current_position] != '}'
                      && !isspace(input[current_position]))
                  {
                    arg += input[current_position];
                    ++current_position;
                  }
    
                error_context ctx = make_context(start, current_position);
                arg_list.emplace_back(arg, ctx);
              }

            // skip over any white space characters
            while(current_position < input.length() && isspace(input[current_position]))
              {
                ++current_position;
              }
            
            // look for argument separator and wrap up
            if(input[current_position] == ',')
              {
                ++current_position;
                continue;
              }
            else if(input[current_position] == '}')
              {
                continue;
              }
    
            std::ostringstream msg;
            msg  << ERROR_EXPECTED_COMMA << " '" << macro << "'";
            error_context ctx = make_context(current_position, current_position+1);
            ctx.error(msg.str());
          }

        // complain if missing closing bracket
        if(!(current_position < input.length() && input[current_position] == '}'))
          {
            std::ostringstream msg;
            msg << ERROR_EXPECTED_CLOSE_ARGUMENT_LIST << " '" << macro << "'";
            error_context ctx = make_context(current_position, current_position + 1);
            ctx.error(msg.str());
          }
        else
          {
            ++current_position;   // skip closing bracket '}'
          }

        return std::make_pair(arg_list, current_position);
      }


    // get a macro index list
    template <typename ContextFactory, typename IndexHandler, typename IndexValidator, typename PropertiesValidator>
    std::pair<index_literal_list, size_t>
    get_index_list(const std::string& input, const std::string& candidate, const size_t position,
                   ContextFactory make_context, IndexHandler add_index, PropertiesValidator validate_properties,
                   IndexValidator validate_index)
      {
        index_literal_list idx_list;
        size_t current_position = position;
        variance v = variance::none;

        // check that an index list is present
        if(current_position >= input.length() || input[current_position] != '[')
          {
            std::ostringstream msg;
            msg << ERROR_EXPECTED_OPEN_INDEX_LIST << " '" << candidate << "'";
            error_context ctx = make_context(current_position, current_position+1);
            ctx.error(msg.str());

            return std::make_pair(idx_list, current_position);
          }

        ++current_position;   // move past opening bracket of index list

        // while not at end of index list, scan for indices
        while(current_position < input.length() && input[current_position] != ']')
          {
            error_context ctx = make_context(current_position, current_position + 1);

            if(input[current_position] == '^')
              {
                v = variance::contravariant;
              }
            else if(input[current_position] == '_')
              {
                v = variance::covariant;
              }
            else if(isalnum(input[current_position]))
              {
                // push this index into the main database and receive a reference to its record
                abstract_index& idx = add_index(input[current_position], ctx);

                // construct index literal
                auto l = std::make_shared<index_literal>(idx, ctx, v);

                // validate its properties (currently its variance)
                validate_properties(*l);
    
                // validate this literal against pre-supplied database of acceptable
                // kernel names (no-op if no validation database was supplied)
                validate_index(*l);

                // move index literal record into central list
                idx_list.push_back(std::move(l));
              }
            else
              {
                std::ostringstream msg;
                msg << ERROR_EXPECTED_INDEX_LABEL_A << " '" << candidate << "', " << ERROR_EXPECTED_INDEX_LABEL_B << " '"
                    << input[current_position] << "'";
                ctx.error(msg.str());
              }

            ++current_position;
          }

        // complain if missing closing bracket ']'
        if(current_position >= input.length() || input[current_position] != ']')
          {
            std::ostringstream msg;
            msg << ERROR_EXPECTED_CLOSE_INDEX_LIST << " '" << candidate << "'";
            error_context ctx = make_context(current_position, current_position + 1);
            ctx.error(msg.str());
          }
        else
          {
            ++current_position;   // skip closing bracket ']'
          }

        return std::make_pair(idx_list, current_position);
      }


    // check no index list exists in the input string
    template <typename ErrorHandler>
    size_t check_no_index_list(const std::string& input, const size_t position, ErrorHandler err)
      {
        size_t current_position = position;

        // if we see the opening bracket '[' belonging to an index list, complain
        if(current_position < input.length() && input[current_position] == '[')
          {
            // invoke error handler
            err(current_position);

            // skip over the index list, until we find a closing bracket ']'
            ++current_position;
            while(current_position < input.length() && input[current_position] != ']')
              {
                ++current_position;
              }

            // skip past final bracket if needed
            if(current_position < input.length()) ++current_position;
          }

        return current_position;
      }


    template <typename RuleItem>
    class RuleNameComparator
      {

      public:

        RuleNameComparator(const std::string n, bool s=false)
          : name(std::move(n)),
            allow_substring(s)
          {
          }


        bool operator()(const RuleItem& r)
          {
            const auto& T = r.get();

            std::string test_string = this->allow_substring ? T.get_name().substr(0, this->name.length())
                                                            : T.get_name();
            return this->name == test_string;
          }


      private:

        //! name to match
        std::string name;

        //! allow matching to a substring of each rule name?
        bool allow_substring;

      };


    template <typename RuleSet>
    bool check_for_match(const std::string& candidate, const RuleSet& rule_list, bool allow_substring = true)
      {
        auto t = std::find_if(rule_list.begin(), rule_list.end(),
                              RuleNameComparator<typename RuleSet::value_type>(candidate, allow_substring));

        return t != rule_list.end();
      }


    template <typename RuleSet>
    auto find_match(const std::string& candidate, const RuleSet& rule_list) -> decltype(rule_list.front().get()) const
      {
        auto t = std::find_if(rule_list.begin(), rule_list.end(),
                              RuleNameComparator<typename RuleSet::value_type>(candidate));

        if(t != rule_list.end()) return(t->get());

        throw std::runtime_error(ERROR_TOKENIZE_NO_MACRO_MATCH);
      }

  }   // namespace macro_tokenizer_impl


using namespace macro_tokenizer_impl;


token_list::token_list(const std::string& in, const std::string& pfx, unsigned int nf, unsigned int np,
                       const package_group& pkg, const index_ruleset& lr, translator_data& d,
                       boost::optional<index_literal_database&> v, bool s)
  : num_fields(nf),
    num_params(np),
    data_payload(d),
    package(pkg),
    input_string(std::make_shared<std::string>(in)),
    prefix(pfx),
    validation_db(v),
    strict(s),
    pre(pkg.get_pre_ruleset()),
    post(pkg.get_post_ruleset()),
    index(pkg.get_index_ruleset()),
    simp_dir(pkg.get_simple_directiveset()),
    ind_dir(pkg.get_index_directiveset()),
    local_rules(lr)
	{
    auto make_context = [&](unsigned int start, unsigned int end) -> auto
      { return this->data_payload.make_error_context(input_string, start, end); };

    // loop over line, trying to match tokens
    size_t position = 0;
		while(position < in.length())
			{
        std::unique_ptr<token_list_impl::generic_token> tok;

				if(in[position] != pfx[0]) // doesn't start with first symbol from prefix, so not a candidate to be a macro or index
					{
            std::tie(tok, position) = match_string_literal(*input_string, position, prefix[0], make_context);
					}
				else  // possible macro, directive or index
					{
            if(check_match_prefix(in, position, pfx))
              {
								position += pfx.length();

                std::tie(tok, position) =
                  this->match_macro_or_index(position, make_context);
							}
						else // we did *not* match the full prefix; treat this as literal text
							{
                std::tie(tok, position) = match_string_literal(in, position, pfx[0], make_context);
							}
					}

        // if a token was recognized, push it onto token list
        if(tok) this->tokens.push_back(std::move(tok));
			}
	}


template <typename ContextFactory>
std::pair<std::unique_ptr<token_list_impl::generic_token>, size_t>
token_list::match_macro_or_index(const size_t position, ContextFactory make_context)
  {
    // try to match the longest macro we can from the current position, or if we cannot match a macro
    // then identify an index literal
    const std::string& input = *this->input_string;

    // first, is there only one character left in the input string? if so, this must be an index literal
    bool possible_match = position+1 < input.length();
    std::string candidate(1, input[position]);

    // otherwise, build a macro candidate from the next two characters and check whether it can match
    // any macros in the ruleset (we can match a substring at this stage)
    if(possible_match)
      {
        candidate += input[position+1];
        possible_match = check_for_match(candidate, this->pre) || check_for_match(candidate, this->post)
                         || check_for_match(candidate, this->index) || check_for_match(candidate, this->local_rules)
                         || check_for_match(candidate, this->simp_dir) || check_for_match(candidate, this->ind_dir);
      }

    // if no possible match then this must be an index literal, so build it and return
    if(!possible_match) return this->make_index_literal(position, make_context);

    // There is a possible match. Now greedily match the *longest* macro we can, from any available package
    size_t candidate_length = 2;
    
    while(position + candidate_length < input.length()
          && input[position + candidate_length] != '['
          && input[position + candidate_length] != '{'
          && (isalnum(input[position + candidate_length]) || input[position + candidate_length] == '_')
          && (check_for_match(candidate + input[position + candidate_length], this->pre)
              || check_for_match(candidate + input[position + candidate_length], this->post)
              || check_for_match(candidate + input[position + candidate_length], this->index)
              || check_for_match(candidate + input[position + candidate_length], this->local_rules)
              || check_for_match(candidate + input[position + candidate_length], this->simp_dir)
              || check_for_match(candidate + input[position + candidate_length], this->ind_dir)))
      {
        candidate += input[position + candidate_length];
        ++candidate_length;
      }

    // now determine what we have found
    try
      {
        if(check_for_match(candidate, this->pre, false))
          {
            // we matched a simple pre-macro
            return this->make_simple_macro(candidate, position, this->pre, simple_macro_type::pre, make_context);
          }
        else if(check_for_match(candidate, this->post, false))
          {
            // we matched a simple post-macro
            return this->make_simple_macro(candidate, position, this->post, simple_macro_type::post, make_context);
          }
        else if(check_for_match(candidate, this->index, false))
          {
            // we matched an index macro
            return this->make_index_macro(candidate, position, this->index, make_context);
          }
        else if(check_for_match(candidate, this->local_rules, false))
          {
            // we matched a locally-defined macro
            return this->make_index_macro(candidate, position, this->local_rules, make_context);
          }
        else if(check_for_match(candidate, this->simp_dir, false))
          {
            // we matched a simple directive
            return this->make_simple_directive(candidate, position, this->simp_dir, make_context);
          }
        else if(check_for_match(candidate, this->ind_dir, false))
          {
            // we matched an index directive
            return this->make_index_directive(candidate, position, this->ind_dir, make_context);
          }
        else  // something has gone wrong
          {
            // we didn't find an exact match after all; we only matched a substring
            // assume it was an index literal after all
            return this->make_index_literal(position, make_context);
          }
      }
    catch(std::runtime_error& xe)
      {
        // something went wrong
        // assume it was an index literal after all
        return this->make_index_literal(position, make_context);
      }
  }


template <typename ContextFactory>
std::pair<std::unique_ptr<token_list_impl::index_literal_token>, size_t>
token_list::make_index_literal(const size_t position, ContextFactory make_context)
  {
    const std::string& input = *this->input_string;
    size_t current_position = position;
    
    variance v = variance::none;

    while(current_position < input.length() &&
          (input[current_position] == '^' || input[current_position] == '_'))
      {
        if(input[current_position] == '^') v = variance::contravariant;
        if(input[current_position] == '_') v = variance::covariant;
        ++current_position;
      }
    
    // check that an index kernel letter is available to be read
    if(current_position >= input.length())
      {
        std::ostringstream msg;
        auto ctx = make_context(current_position-1, current_position);
        ctx.error(ERROR_INDEX_LITERAL_NO_KERNEL);
        
        std::unique_ptr<token_list_impl::index_literal_token> tok;
        return std::make_pair(std::move(tok), current_position);
      }
    
    // insert an abstract index with this kernel letter into the main database
    abstract_index_database::iterator idx = this->add_index(input[current_position]);

    // step past the kernel letter
    ++current_position;

    // generate a record of this index instance, keeping the variance information
    error_context ctx = make_context(position, current_position);
    index_literal l(*idx, ctx, v);
    this->validate_index_properties(l);

    // validate this literal against pre-supplied database of acceptable
    // kernel names (no-op if no validation database was supplied)
    this->validate_index_literal(l);

    // generate a token corresponding to this literal
    auto tok = std::make_unique<token_list_impl::index_literal_token>(l, ctx);
    this->index_literal_tokens.push_back(std::ref(*tok));

    // push this literal onto the declaration list (makes a new copy)
    this->index_decls.emplace_back(std::make_shared<index_literal>(l));

    return std::make_pair(std::move(tok), current_position);
  }


template <typename RuleSet, typename ContextFactory>
std::pair<std::unique_ptr<token_list_impl::simple_macro_token>, size_t>
token_list::make_simple_macro(const std::string& macro, const size_t position, const RuleSet& rules, simple_macro_type type,
                              ContextFactory make_context)
  {
    const std::string& input = *this->input_string;

    // check whether we've previously seen a directive and therefore replacement rules should be disallowed
    if(!this->validate_replacement_rule(macro, position, make_context))
      {
        std::unique_ptr<token_list_impl::simple_macro_token> tok;
        return std::make_pair(std::move(tok), position + macro.length());
      }
    
    // find rule for this macro
    auto& rule = find_match(macro, rules);

    // move position past the macro name
    size_t current_position = position + macro.length();

    // shouldn't find an index list
    auto err_handler = [&](size_t p) -> void
      {
        std::ostringstream msg;
        msg << ERROR_TOKENIZE_UNEXPECTED_LIST << " '" << macro << "'; " << ERROR_TOKENIZE_SKIPPING;
        auto ctx = make_context(p, p+1);
        ctx.error(msg.str());
      };
    current_position = check_no_index_list(input, current_position, err_handler);

    // get argument list, if one is expected
    macro_argument_list arg_list;
    if(rule.get_number_args() > 0)
      std::tie(arg_list, current_position) = get_argument_list(input, macro, current_position, make_context);

    // build token
    auto tok = std::make_unique<token_list_impl::simple_macro_token>(macro, arg_list, rule, type,
                                                                     make_context(position, current_position));
    this->simple_macro_tokens.push_back(*tok);

    return std::make_pair(std::move(tok), current_position);
  }


template <typename RuleSet, typename ContextFactory>
std::pair<std::unique_ptr<token_list_impl::index_macro_token>, size_t>
token_list::make_index_macro(const std::string& macro, const size_t position, const RuleSet& rules, ContextFactory make_context)
  {
    const std::string& input = *this->input_string;

    // check whether we've previously seen a directive and therefore replacement rules should be disallowed
    if(!this->validate_replacement_rule(macro, position, make_context))
      {
        std::unique_ptr<token_list_impl::index_macro_token> tok;
        return std::make_pair(std::move(tok), position + macro.length());
      }

    // find rule for this macro
    auto& rule = find_match(macro, rules);

    // found a match -- move position past the candidate
    size_t current_position = position + macro.length();

    // should find an index list
    index_literal_list idx_list;

    auto add_index = [&](char l, error_context& ctx) -> auto&
      {
        // add this index to the main database; if it has already been seen the we will just get
        // an iterator to the original record. We return a reference to the abstract_index& record
        auto idx = this->add_index(l);
        return *idx;
      };
    
    auto validate_index = [&](index_literal& l) -> void
      {
        this->validate_index_literal(l);
        this->index_decls.emplace_back(std::make_shared<index_literal>(l));
      };

    auto validate_properties = [&](index_literal& l) -> void { this->validate_index_properties(l); };

    std::tie(idx_list, current_position) =
      get_index_list(input, macro, current_position, make_context, add_index, validate_properties, validate_index);

    // may find an argument list
    macro_argument_list arg_list;
    if(rule.get_number_args() > 0)
      std::tie(arg_list, current_position) = get_argument_list(input, macro, current_position, make_context);

    // determine unroll status flags, either inherited from the macro or by allowing a suffix to the argument list
    if(rule.get_unroll() == unroll_behaviour::force)
      {
        this->force_unroll.push_back(macro);
      }
    else if(rule.get_unroll() == unroll_behaviour::prevent)
      {
        this->prevent_unroll.push_back(macro);
      }
    else if(current_position < input.length() && input[current_position] == '|')
      // check for 'force unroll' suffix if macro is neutral; ignore suffixes otherwise
      {
        ++current_position;
        this->force_unroll.push_back(macro);
      }
    else if(position < input.length() && input[position] == '@')
      // check for 'prevent unroll' suffix if macro is neutral
      {
        ++current_position;
        this->prevent_unroll.push_back(macro);
      }

    error_context ctx = make_context(position, current_position);

    if(!this->force_unroll.empty() && !this->prevent_unroll.empty())
      {
        std::ostringstream msg;
        msg << ERROR_INCOMPATIBLE_UNROLL << " '" << macro << "'";
        ctx.error(msg.str());
      }

    auto tok = std::make_unique<token_list_impl::index_macro_token>(macro, idx_list, arg_list, rule, ctx);

    this->index_macro_tokens.push_back(std::ref(*tok));

    return std::make_pair(std::move(tok), current_position);
  }


template <typename RuleSet, typename ContextFactory>
std::pair<std::unique_ptr<token_list_impl::simple_directive_token>, size_t>
token_list::make_simple_directive(const std::string& macro, const size_t position, const RuleSet& rules,
                                  ContextFactory make_context)
  {
    const std::string& input = *this->input_string;

    // check whether we've previously seen a replacement rule and therefore directives should be disallowed
    if(!this->validate_directive(macro, position, make_context))
      {
        std::unique_ptr<token_list_impl::simple_directive_token> tok;
        return std::make_pair(std::move(tok), position + macro.length());
      }

    // find rule for this directive
    auto& rule = find_match(macro, rules);
    
    // move position past the macro name
    size_t current_position = position + macro.length();
    
    // shouldn't have an index list
    auto err_handler = [&](size_t p) -> void
      {
        std::ostringstream msg;
        msg << ERROR_TOKENIZE_UNEXPECTED_LIST << " '" << macro << "'; " << ERROR_TOKENIZE_SKIPPING;
        auto ctx = make_context(p, p+1);
        ctx.error(msg.str());
      };
    current_position = check_no_index_list(input, current_position, err_handler);
    
    // get argument list, if one is expected
    macro_argument_list arg_list;
    if(rule.get_number_args() > 0)
      std::tie(arg_list, current_position) = get_argument_list(input, macro, current_position, make_context);
    
    // build token
    auto tok = std::make_unique<token_list_impl::simple_directive_token>(macro, arg_list, rule,
                                                                         make_context(position, current_position));
    this->simple_directive_tokens.push_back(*tok);
    
    return std::make_pair(std::move(tok), current_position);
  }


template <typename RuleSet, typename ContextFactory>
std::pair<std::unique_ptr<token_list_impl::index_directive_token>, size_t>
token_list::make_index_directive(const std::string& macro, const size_t position, const RuleSet& rules,
                                 ContextFactory make_context)
  {
    const std::string& input = *this->input_string;

    // check whether we've previously seen a replacement rule and therefore directives should be disallowed
    if(!this->validate_directive(macro, position, make_context))
      {
        std::unique_ptr<token_list_impl::index_directive_token> tok;
        return std::make_pair(std::move(tok), position + macro.length());
      }

    // find rule for this directive
    auto& rule = find_match(macro, rules);
    
    // found a match -- move position past the candidate
    size_t current_position = position + macro.length();
    
    // should find an index list
    index_literal_list idx_list;

    auto add_index = [&](char l, error_context& ctx) -> auto&
      {
        // add this index to the main database; if it has already been seen the we will just get
        // an iterator to the original record. We return a reference to the abstract_index& record
        auto idx = this->add_index(l);
        return *idx;
      };

    auto validate_index = [&](index_literal& l) -> void
      {
        this->validate_index_literal(l);
        this->index_decls.emplace_back(std::make_shared<index_literal>(l));
      };
    
    auto validate_properties = [&](index_literal& l) -> void { this->validate_index_properties(l); };

    std::tie(idx_list, current_position) =
      get_index_list(input, macro, current_position, make_context, add_index, validate_properties, validate_index);
    
    // may find an argument list
    macro_argument_list arg_list;
    if(rule.get_number_args() > 0)
      std::tie(arg_list, current_position) = get_argument_list(input, macro, current_position, make_context);
    
    auto tok = std::make_unique<token_list_impl::index_directive_token>(macro, idx_list, arg_list, rule,
                                                                        make_context(position, current_position));
    
    this->index_directive_tokens.push_back(std::ref(*tok));
    
    return std::make_pair(std::move(tok), current_position);
  }


abstract_index_database::iterator token_list::add_index(char label)
	{
    // emplace does nothing if a record already exists
    return (this->index_db.emplace_back(
      std::make_pair(label, std::make_unique<abstract_index>(label, this->num_fields, this->num_params)))).first;
	}


unsigned int token_list::evaluate_macros(simple_macro_type type)
	{
		unsigned int replacements = 0;

    for(auto& t : this->simple_macro_tokens)
			{
        auto& T = t.get();

        if(T.get_type() == type)
          {
            T.evaluate();
            ++replacements;
          }
			}

		return(replacements);
	}


unsigned int token_list::evaluate_macros(const indices_assignment& a)
	{
		unsigned int replacements = 0;

    for(auto& t : this->index_literal_tokens)
      {
        auto& T = t.get();
        T.evaluate(a);
        ++replacements;
      }

    for(auto& t : this->index_macro_tokens)
      {
        auto& T = t.get();
        T.evaluate_unroll(a);
        ++replacements;
      }

		return(replacements);
}


unsigned int token_list::evaluate_macros()
  {
    unsigned int replacements = 0;

    for(auto& t : this->index_literal_tokens)
      {
        auto& T = t.get();
        T.evaluate();
        ++replacements;
      }

    for(auto& t : this->index_macro_tokens)
      {
        auto& T = t.get();
        T.evaluate_roll();
        ++replacements;
      }

    return(replacements);
  }


unsigned int token_list::evaluate_macros(const index_remap_rule& rule)
  {
    unsigned int replacements = 0;

    for(auto& t : this->index_literal_tokens)
      {
        auto& T = t.get();
        T.evaluate(rule);
        ++replacements;
      }

    for(auto& t : this->index_macro_tokens)
      {
        auto& T = t.get();
        T.evaluate_roll(rule);
        ++replacements;
      }

    return(replacements);
  }


std::string token_list::to_string()
	{
    std::string output;

    for(const auto& t : this->tokens)
			{
				output += t->to_string();
			}

		return(output);
	}


unroll_behaviour token_list::unroll_status() const
  {
    if(this->force_unroll.size() > 0 && this->prevent_unroll.size() == 0) return unroll_behaviour::force;
    if(this->force_unroll.size() == 0 && this->prevent_unroll.size() > 0) return unroll_behaviour::prevent;

    // if we are in an inconsistent state then an error will already have been raised, so do nothing here
    return unroll_behaviour::allow;
  }


void token_list::reset()
  {
    // reset initialization status of all index macro tokens
    for(auto& t : this->index_macro_tokens)
      {
        auto& T = t.get();
        T.reset();
      }
  }


template <typename ContextFactory>
bool token_list::validate_replacement_rule(const std::string& macro, const size_t position, ContextFactory make_context)
  {
    if(this->simple_directive_tokens.empty() && this->index_directive_tokens.empty()) return true;
    
    error_context ctx = make_context(position, position + macro.length());
    ctx.error(ERROR_RULE_AFTER_DIRECTIVE);
    return false;
  }


template <typename ContextFactory>
bool token_list::validate_directive(const std::string& macro, const size_t position, ContextFactory make_context)
  {
    if(this->simple_macro_tokens.empty() && this->index_macro_tokens.empty() && this->index_literal_tokens.empty()) return true;
    
    error_context ctx = make_context(position, position + macro.length());
    ctx.error(ERROR_DIRECTIVE_AFTER_RULE);
    return false;
  }


void token_list::validate_index_literal(index_literal& l)
  {
    if(!this->validation_db) return;

    const abstract_index& idx = l;

    // if this index isn't in the validation database then there is nothing to do: return immediately
    auto t = this->validation_db->find(idx.get_label());
    if(t == this->validation_db->end())
      {
        if(this->strict)
          {
            std::ostringstream msg;
            msg << ERROR_INDEX_NOT_VALID << " '" << l.to_string() << "'";
            l.get_declaration_point().error(msg.str());
          }

        return;
      }

    // otherwise, verify that this index literal matches the expected properties of the database version
    index_literal& validation_copy = *t->second;

    // check whether validation copy agrees with the version supplied
    if(!std::equal_to<index_literal>()(validation_copy, l))
      {
        std::ostringstream msg;
        msg << ERROR_INDEX_VALIDATION_FAIL << " '" << validation_copy.to_string() << "'";
        l.get_declaration_point().error(msg.str());

        validation_copy.get_declaration_point().warn(NOTIFY_INDEX_DECLARATION_WAS);
      }
  }


void token_list::validate_index_properties(index_literal& l)
  {
    const abstract_index& idx = l.get();
    
    // if a parameter index, drop any variance qualifier and return
    if(idx.get_class() == index_class::parameter)
      {
        if(l.get_variance() != variance::none)
          {
            l.set_variance(variance::none);
            l.get_declaration_point().warn(NOTIFY_PARAMETER_VARIANCE_IGNORED);
          }
        return;
      }
    
    // if a canonical model, warn that index variance is ignored
    if(this->data_payload.model.get_lagrangian_type() == model_type::canonical)
      {
        if(l.get_variance() != variance::none)
          {
            l.set_variance(variance::none);
            l.get_declaration_point().warn(NOTIFY_CANONICAL_IGNORES_VARIANCE);
          }
        return;
      }
    
    // assume a nontrivial metric model
    
    // if a nontrivial metric model, all indices should have a variance assignment
    if(l.get_variance() == variance::none)
      {
        l.set_variance(variance::contravariant);
        l.get_declaration_point().error(ERROR_NONTRIVIAL_REQUIRES_VARIANCE);
      }
  }
