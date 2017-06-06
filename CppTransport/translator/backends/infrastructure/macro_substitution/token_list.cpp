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
#include <stdexcept>

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
        std::string arg;

        while(current_position < input.length() && input[current_position] != '}')
          {
            if(input[current_position] == ',')
              {
                error_context ctx = make_context(current_position - arg.length(), current_position);
                arg_list.emplace_back(arg, ctx);
                arg.clear();
              }
            else if(input[current_position] == '"' && arg.length() == 0)
              {
                ++current_position; // skip the opening quote

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

                // closing quote will be skipped by position++ below
              }
            else
              {
                // skip over white space, but add non-white-space characters to current argument string
                if(!isspace(input[current_position]))
                  {
                    arg += input[current_position];
                  }
              }

            ++current_position;
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

        // emplace last argument if it has not already been done
        if(arg.length() > 0)
          {
            error_context ctx = make_context(current_position - arg.length(), current_position);
            arg_list.emplace_back(arg, ctx);
          }

        return std::make_pair(arg_list, current_position);
      }


    // get a macro index list
    template <typename ContextFactory, typename IndexHandler>
    std::pair<abstract_index_list, size_t>
    get_index_list(const std::string& input, const std::string& candidate, const size_t position,
                   ContextFactory make_context, IndexHandler add_index)
      {
        abstract_index_list idx_list;
        size_t current_position = position;

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

            if(isalnum(input[current_position]))
              {
                add_index(idx_list, input[current_position], ctx);
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


token_list::token_list(const std::string& input, const std::string& prefix, const unsigned int nf, const unsigned int np,
                       const package_group& package, const index_ruleset& local_rules, translator_data& d)
  : num_fields(nf),
    num_params(np),
    data_payload(d),
    input_string(std::make_shared<std::string>(input))
	{
    const pre_ruleset& pre = package.get_pre_ruleset();
    const post_ruleset& post = package.get_post_ruleset();
    const index_ruleset& index = package.get_index_ruleset();

    auto make_context = [&](unsigned int start, unsigned int end) -> auto
      { return this->data_payload.make_error_context(input_string, start, end); };

    // loop over line, trying to match tokens
    size_t position = 0;
		while(position < input.length())
			{
        std::unique_ptr<token_list_impl::generic_token> tok;

				if(input[position] != prefix[0]) // doesn't start with first symbol from prefix, so not a candidate to be a macro or index
					{
            std::tie(tok, position) = match_string_literal(input, position, prefix[0], make_context);
					}
				else  // possible macro or index
					{
            if(check_match_prefix(input, position, prefix))
              {
								position += prefix.length();

                std::tie(tok, position) = this->match_macro_or_index(input, position, pre, post, index, local_rules,
                                                                     make_context);
							}
						else // we did *not* match the full prefix; treat this as literal text
							{
                std::tie(tok, position) = match_string_literal(input, position, prefix[0], make_context);
							}
					}

        this->tokens.push_back(std::move(tok)); // transfers ownership of tok to the token list
			}
	}


template <typename ContextFactory>
std::pair<std::unique_ptr<token_list_impl::generic_token>, size_t>
token_list::match_macro_or_index(const std::string& input, const size_t position, const pre_ruleset& pre,
                                 const post_ruleset& post, const index_ruleset& index, const index_ruleset& local_rules,
                                 ContextFactory make_context)
  {
    // try to match the longest macro we can from the current position, or if we cannot match a macro
    // then identify an index literal

    // first, is there only one character left in the input string? if so, this must be an index literal
    bool possible_match = position+1 < input.length();
    std::string candidate(1, input[position]);

    // otherwise, build a macro candidate from the next two characters and check whether it can match
    // and macros in the ruleset (we can match a substring at this stage)
    if(possible_match)
      {
        candidate += input[position+1];
        possible_match = check_for_match(candidate, pre) || check_for_match(candidate, post)
                         || check_for_match(candidate, index) || check_for_match(candidate, local_rules);
      }

    // if no possible match then this must be an index literal, so build it and return
    if(!possible_match)
      {
        auto tok = this->make_index_literal(input, position, make_context);
        return std::make_pair(std::move(tok), position+1);
      }

    // There is a possible match. Now greedily match the longest macro we can
    size_t candidate_length = 2;

    while(position + candidate_length < input.length()
          && input[position + candidate_length] != '['
          && input[position + candidate_length] != '{'
          && (isalnum(input[position + candidate_length]) || input[position + candidate_length] == '_')
          && (check_for_match(candidate + input[position + candidate_length], pre)
              || check_for_match(candidate + input[position + candidate_length], post)
              || check_for_match(candidate + input[position + candidate_length], index)
              || check_for_match(candidate + input[position + candidate_length], local_rules)))
      {
        candidate += input[position + candidate_length];
        ++candidate_length;
      }

    // now determine what we have found
    try
      {
        if(check_for_match(candidate, pre, false))
          {
            // we matched a simple pre-macro
            return this->make_simple_macro(input, candidate, position, pre, simple_macro_type::pre, make_context);
          }
        else if(check_for_match(candidate, post, false))
          {
            // we matched a simple post-macro
            return this->make_simple_macro(input, candidate, position, post, simple_macro_type::post, make_context);
          }
        else if(check_for_match(candidate, index, false))
          {
            // we matched an index macro
            return this->make_index_macro(input, candidate, position, index, make_context);
          }
        else if(check_for_match(candidate, local_rules, false))
          {
            // we matched a locally-defined macro
            return this->make_index_macro(input, candidate, position, local_rules, make_context);
          }
        else  // something has gone wrong
          {
            // we didn't find an exact match after all; we only matched a substring
            // assume it was a free index after all
            auto tok = this->make_index_literal(input, position, make_context);
            return std::make_pair(std::move(tok), position+1);
          }
      }
    catch(std::runtime_error& xe)
      {
        // something went wrong
        // assume it was an index literal after all
        auto tok = this->make_index_literal(input, position, make_context);
        return std::make_pair(std::move(tok), position+1);
      }
  }


template <typename ContextFactory>
std::unique_ptr<token_list_impl::index_literal_token>
token_list::make_index_literal(const std::string& input, const size_t position, ContextFactory make_context)
  {
    abstract_index_list::const_iterator idx = this->add_index(input[position]);

    auto tok = std::make_unique<token_list_impl::index_literal_token>(idx, make_context(position, position+1));
    this->index_literal_tokens.push_back(std::ref(*tok));

    return tok;
  }


template <typename RuleSet, typename ContextFactory>
std::pair<std::unique_ptr<token_list_impl::simple_macro_token>, size_t>
token_list::make_simple_macro(const std::string& input, const std::string& macro, const size_t position,
                              const RuleSet& rules, simple_macro_type type, ContextFactory make_context)
  {
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
token_list::make_index_macro(const std::string& input, const std::string& macro, const size_t position,
                             const RuleSet& rules, ContextFactory make_context)
  {
    // find rule for this macro
    auto& rule = find_match(macro, rules);

    // found a match -- move position past the candidate
    size_t current_position = position + macro.length();

    // should find an index list
    abstract_index_list idx_list;
    auto add_index = [&](abstract_index_list& idxs, char label, error_context& ctx) -> void
      {
        // add to index list if we haven't already seen it;
        // the constructor for abstract_index will assign a suitable type based on the index symbol
        std::pair<abstract_index_list::iterator, bool> result = idxs.emplace_back(
          std::make_pair(label, std::make_shared<abstract_index>(label, this->num_fields, this->num_params)));

        // if the index was new, add to list for this entire tokenization job unless this
        // if the index has already been seen for a previous macro then add_index() will do nothing provided
        // the index type is consistent
        if(result.second && !rule.is_directive()) this->add_index(*result.first, ctx);
      };
    std::tie(idx_list, current_position) = get_index_list(input, macro, current_position, make_context, add_index);

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


abstract_index_list::const_iterator token_list::add_index(char label)
	{
    // emplace does nothing if a record already exists
    return (this->indices.emplace_back(
      std::make_pair(label, std::make_unique<abstract_index>(label, this->num_fields, this->num_params)))).first;
	}


abstract_index_list::const_iterator token_list::add_index(const abstract_index& index, error_context& ctx)
	{
    // check whether an existing record for this index exiosts
    abstract_index_list::const_iterator t = this->indices.find(index.get_label());

    if(t == this->indices.end())
      {
        // no record currently exists, so emplace a copy in the database and return
        // an iterator to it
        return (this->indices.emplace_back(index.get_label(), std::make_unique<abstract_index>(index))).first;
      }

    // a record did previously exist, so check its class and report an error if they do not agree
    if(index.get_class() != t->get_class())
      {
        std::ostringstream msg;
        msg << ERROR_TOKENIZE_INDEX_MISMATCH << " '" << index.get_label() << "'";
        ctx.error(msg.str());
      }

    // return iterator to existing record
    return(t);
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


unsigned int token_list::evaluate_macros(const assignment_list& a)
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

