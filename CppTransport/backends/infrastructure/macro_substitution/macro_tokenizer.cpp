//
// Created by David Seery on 11/03/15.
// Copyright (c) 2015 University of Sussex. All rights reserved.
//


#include <assert.h>
#include <sstream>
#include <stdexcept>

#include "macro_tokenizer.h"


constexpr unsigned int MAX_TOKEN_ERRORS = 4;


token_list::token_list(const std::string& input, const std::string& prefix,
                       unsigned int nf, unsigned int np,
                       const std::vector<macro_packages::simple_rule>& pre,
                       const std::vector<macro_packages::simple_rule>& post,
                       const std::vector<macro_packages::index_rule>& index,
                       translator_data& d)
  : num_fields(nf),
    num_params(np),
    data_payload(d),
    num_prevent_unroll(0),
    num_force_unroll(0),
    input_string(std::make_shared<std::string>(input))
	{
		size_t position = 0;

		while(position < input.length())
			{
				if(input[position] != prefix[0])  // not a candidate to be a macro or index
					{
				    std::string string_literal(1, input[position++]);
						while(position < input.length() && input[position] != prefix[0])
							{
								string_literal += input[position++];
							}

            error_context ctx(this->data_payload.get_stack(), input_string, position, this->data_payload.get_error_handler(), this->data_payload.get_warning_handler());
            std::unique_ptr<token_list_impl::text_token> tok = std::make_unique<token_list_impl::text_token>(string_literal, ctx);
						this->tokens.push_back(std::move(tok));     // transfers ownership
					}
				else  // possible macro or index
					{
						// first, check if we match the full prefix
						unsigned int count = 0;
						while(position + count < input.length() && count < prefix.length() && input[position+count] == prefix[count])
							{
								count++;
							}

						if(count >= prefix.length())   // we matched the full prefix
							{
								position += prefix.length();

								// try to match the longest macro we can

								// is there only one character left in the input string? if so, this must be a free index
								if(position+1 >= input.length())
									{
                    abstract_index_list::const_iterator idx = this->add_index(input[position]);

                    error_context ctx(this->data_payload.get_stack(), input_string, position, this->data_payload.get_error_handler(), this->data_payload.get_warning_handler());
                    std::unique_ptr<token_list_impl::free_index_token> tok = std::make_unique<token_list_impl::free_index_token>(idx, ctx);
                    this->free_index_tokens.push_back(tok.get());
										this->tokens.push_back(std::move(tok));     // transfers ownership
										position++;
									}
								else  // more than one character left, so could be free index or a macro
									{
										// build the first macro candidate -- the first two characters after the prefix
								    std::string candidate(1, input[position]);
										candidate += input[position+1];

										// does this candidate (potentially) match any macros? (we can match a substring)
										if(this->check_for_match(candidate, pre) || this->check_for_match(candidate, post) || this->check_for_match(candidate, index))
											{
												// yes, there was a match. Now greedily match the longest macro we can
										    size_t candidate_length = 2;

										    while(position + candidate_length < input.length()
											        && input[position + candidate_length] != '['
											        && input[position + candidate_length] != '{'
											        && (isalnum(input[position + candidate_length]) || input[position + candidate_length] == '_')
											        && (this->check_for_match(candidate + input[position + candidate_length], pre)
										              || this->check_for_match(candidate + input[position + candidate_length], post)
										              || this->check_for_match(candidate + input[position + candidate_length], index)))
											    {
										        candidate += input[position + candidate_length];
										        candidate_length++;
											    }

										    try
											    {
										        if(this->check_for_match(candidate, pre, false))
											        {
										            // we matched a simple pre-macro
										            const macro_packages::simple_rule& rule = this->find_match(candidate, pre);

												        // found a match -- move position past the candidate
										            position += candidate_length;

										            // shouldn't find an index list
										            this->check_no_index_list(input, candidate, position);

										            macro_argument_list arg_list;
												        if(rule.get_number_args() > 0) arg_list = this->get_argument_list(input, candidate, position, rule.get_number_args());

                                error_context ctx(this->data_payload.get_stack(), input_string, position, this->data_payload.get_error_handler(), this->data_payload.get_warning_handler());
                                std::unique_ptr<token_list_impl::simple_macro_token> tok = std::make_unique<token_list_impl::simple_macro_token>(candidate, arg_list, rule, simple_macro_type::pre, ctx);
                                if(arg_list.size() != rule.get_number_args()) tok->mark_silent();   // constructor will raise error; further errors will be suppressed
                                this->simple_macro_tokens.push_back(tok.get());
												        this->tokens.push_back(std::move(tok));     // transfers ownership
											        }
										        else if(this->check_for_match(candidate, post, false))
											        {
										            // we matched a simple post-macro
										            const macro_packages::simple_rule& rule = this->find_match(candidate, post);

										            // found a match -- move position past the candidate
										            position += candidate_length;

										            // shouldn't find an index list
										            this->check_no_index_list(input, candidate, position);

										            macro_argument_list arg_list;
										            if(rule.get_number_args() > 0) arg_list = this->get_argument_list(input, candidate, position, rule.get_number_args());

                                error_context ctx(this->data_payload.get_stack(), input_string, position, this->data_payload.get_error_handler(), this->data_payload.get_warning_handler());
                                std::unique_ptr<token_list_impl::simple_macro_token> tok = std::make_unique<token_list_impl::simple_macro_token>(candidate, arg_list, rule, simple_macro_type::post, ctx);
                                if(arg_list.size() != rule.get_number_args()) tok->mark_silent();   // constructor will raise error; further errors will be suppressed
                                this->simple_macro_tokens.push_back(tok.get());
												        this->tokens.push_back(std::move(tok));     // transfers ownership
											        }
										        else if(this->check_for_match(candidate, index, false))
											        {
										            // we matched an index macro
										            const macro_packages::index_rule& rule = this->find_match(candidate, index);

										            // found a match -- move position past the candidate
										            position += candidate_length;

												        // should find an index list
										            abstract_index_list idx_list = this->get_index_list(input, candidate, position, rule.get_number_indices(), rule.get_index_class());

										            macro_argument_list arg_list;
										            if(rule.get_number_args() > 0) arg_list = this->get_argument_list(input, candidate, position, rule.get_number_args());

                                // apply unroll flags, either inherited from the macro or by allowing suffixies
                                if(rule.get_unroll() == unroll_behaviour::force)
                                  {
                                    ++this->num_force_unroll;
                                  }
                                else if(rule.get_unroll() == unroll_behaviour::prevent)
                                  {
                                    ++this->num_prevent_unroll;
                                  }
                                else if(position < input.length() && input[position] == '@')    // check for 'force unroll' suffix if macro is neutral; ignore suffixes otherwise
                                  {
                                    ++position;
                                    ++this->num_force_unroll;
                                  }
                                else if(position < input.length() && input[position] == '^')    // check for 'prevent unroll' suffix if macro is neutral
                                  {
                                    ++position;
                                    ++this->num_prevent_unroll;
                                  }

                                error_context ctx(this->data_payload.get_stack(), input_string, position, this->data_payload.get_error_handler(), this->data_payload.get_warning_handler());

                                if(this->num_force_unroll > 0 && this->num_prevent_unroll > 0)
                                  {
                                    std::ostringstream msg;
                                    msg << ERROR_INCOMPATIBLE_UNROLL << " '" << candidate << "'";
                                    ctx.error(msg.str());
                                  }

                                std::unique_ptr<token_list_impl::index_macro_token> tok = std::make_unique<token_list_impl::index_macro_token>(candidate, idx_list, arg_list, rule, ctx);
                                if(arg_list.size() != rule.get_number_args() || idx_list.size() != rule.get_number_indices()) tok->mark_silent();   // constructor will raise error; further errors will be suppressed
                                this->index_macro_tokens.push_back(tok.get());
												        this->tokens.push_back(std::move(tok));     // transfers ownership
											        }
										        else  // something has gone wrong
											        {
												        // we didn't find an exact match after all; we only matched a substring
												        // assume it was a free index after all
                                abstract_index_list::const_iterator idx = this->add_index(input[position]);

                                error_context ctx(this->data_payload.get_stack(), input_string, position, this->data_payload.get_error_handler(), this->data_payload.get_warning_handler());
                                std::unique_ptr<token_list_impl::free_index_token> tok = std::make_unique<token_list_impl::free_index_token>(idx, ctx);
                                this->free_index_tokens.push_back(tok.get());
										            this->tokens.push_back(std::move(tok));     // transfers ownership

										            position++;
											        }
											    }
								        catch(std::runtime_error& xe)
									        {
										        // something went wrong
										        // assume it was a free index after all
                            abstract_index_list::const_iterator idx = this->add_index(input[position]);

                            error_context ctx(this->data_payload.get_stack(), input_string, position, this->data_payload.get_error_handler(), this->data_payload.get_warning_handler());
                            std::unique_ptr<token_list_impl::free_index_token> tok = std::make_unique<token_list_impl::free_index_token>(idx, ctx);
                            this->free_index_tokens.push_back(tok.get());
								            this->tokens.push_back(std::move(tok));     // transfers ownership

								            position++;
									        }
											}
										else  // no it doesn't; there's no match. It must have been a free index
											{
                        abstract_index_list::const_iterator idx = this->add_index(input[position]);

                        error_context ctx(this->data_payload.get_stack(), input_string, position, this->data_payload.get_error_handler(), this->data_payload.get_warning_handler());
                        std::unique_ptr<token_list_impl::free_index_token> tok = std::make_unique<token_list_impl::free_index_token>(idx, ctx);
                        this->free_index_tokens.push_back(tok.get());
												this->tokens.push_back(std::move(tok));     // transfers ownership

												position++;
											}
									}
							}
						else                          // we did *not* match the full prefix; treat this as literal text
							{
						    std::string string_literal(1, input[position++]);
						    while(position < input.length() && input[position] != prefix[0])
							    {
						        string_literal += input[position++];
							    }

                error_context ctx(this->data_payload.get_stack(), input_string, position, this->data_payload.get_error_handler(), this->data_payload.get_warning_handler());
                std::unique_ptr<token_list_impl::text_token> tok = std::make_unique<token_list_impl::text_token>(string_literal, ctx);
						    this->tokens.push_back(std::move(tok));     // transfers ownership
							}
					}
			}
	}


template <typename Rule>
bool token_list::check_for_match(const std::string& candidate, const std::vector<Rule>& rule_list, bool allow_substring)
	{
		bool found = false;

    for(const Rule& t : rule_list)
			{
				if(candidate.length() <= t.get_name().length())
					{
				    std::string name_substring = allow_substring ? t.get_name().substr(0, candidate.length()) : t.get_name();
						if(candidate == name_substring)
							{
								found = true;
                break;
							}
					}
			}

		return(found);
	}


namespace macro_tokenizer_impl
  {

    template <typename Rule>
    class RuleNameComparator
      {

      public:

        RuleNameComparator(const std::string n)
          : name(std::move(n))
          {
          }

        bool operator()(const Rule& r)
          {
            return(this->name == r.get_name());
          }

      private:

        std::string name;

      };

  }


template <typename Rule>
const Rule& token_list::find_match(const std::string& candidate, const std::vector<Rule>& rule_list)
	{
    typename std::vector<Rule>::const_iterator t = std::find_if(rule_list.begin(), rule_list.end(), macro_tokenizer_impl::RuleNameComparator<Rule>(candidate));

    if(t != rule_list.end()) return(*t);

		throw std::runtime_error(ERROR_TOKENIZE_NO_MACRO_MATCH);
	}


void token_list::check_no_index_list(const std::string& input, const std::string& candidate, size_t& position)
	{
    if(position < input.length() && input[position] == '[')
	    {
        std::ostringstream msg;
        msg << ERROR_TOKENIZE_UNEXPECTED_LIST << " '" << candidate << "'; " << ERROR_TOKENIZE_SKIPPING;
        error_context ctx(this->data_payload.get_stack(), this->input_string, position, this->data_payload.get_error_handler(), this->data_payload.get_warning_handler());
        ctx.error(msg.str());

        position++;
        while(position < input.length() && input[position] != ']')
	        {
            position++;
	        }
        if(position < input.length()) position++;
	    }
	}


macro_argument_list token_list::get_argument_list(const std::string& input, const std::string& candidate, size_t& position, unsigned int expected_args)
	{
    macro_argument_list arg_list;

		if(position < input.length() && input[position] == '{')
			{
				position++;
		    std::string arg;

		    while(position < input.length() && input[position] != '}')
			    {
				    if(input[position] == ',')
					    {
						    arg_list.push_back(arg);
						    arg.clear();
					    }
				    else if(input[position] == '"' && arg.length() == 0)
					    {
						    position++; // skip the opening quote

				        while(position < input.length() && input[position] != '"')
					        {
				            arg += input[position++];
					        }

				        // check closing quote is in place
				        if(!(position < input.length() && input[position] == '"'))
					        {
				            std::cout << ERROR_EXPECTED_CLOSE_ARGUMENT_QUOTE << " '" << candidate << "'";
					        }
						    // closing quote will be skipped by position++ below
					    }
				    else
					    {
						    if(!isspace(input[position]))
							    {
								    arg += input[position];
							    }
					    }

						++position;
			    }

				if(!(position < input.length() && input[position] == '}'))
					{
            std::ostringstream msg;
            msg << ERROR_EXPECTED_CLOSE_ARGUMENT_LIST << " '" << candidate << "'";
            error_context ctx(this->data_payload.get_stack(), this->input_string, position, this->data_payload.get_error_handler(), this->data_payload.get_warning_handler());
            ctx.error(msg.str());
					}
				else
					{
						position++;   // skip closing }
					}

				if(arg.length() > 0)
					{
						arg_list.push_back(arg);
					}
			}
		else  // expected to find an argument list
			{
        std::ostringstream msg;
		    msg << ERROR_EXPECTED_OPEN_ARGUMENT_LIST << " '" << candidate << "'";
        error_context ctx(this->data_payload.get_stack(), this->input_string, position, this->data_payload.get_error_handler(), this->data_payload.get_warning_handler());
        ctx.error(msg.str());
			}

		return(arg_list);
	}


abstract_index_list token_list::get_index_list(const std::string& input, const std::string& candidate, size_t& position, unsigned int expected_indices, enum index_class range)
	{
    abstract_index_list idx_list;

		if(position < input.length() && input[position] == '[')
			{
				position++;   // matched opening bracket of index list

        // while not at end of index list, scan for indices
				while(position < input.length() && input[position] != ']')
					{
            error_context ctx(this->data_payload.get_stack(), this->input_string, position, this->data_payload.get_error_handler(), this->data_payload.get_warning_handler());

						if(isalnum(input[position]))
							{
                // add to index list for this macro if we haven't already seen it
                // the constructor for abstract_index will assign a suitable type
                std::pair< abstract_index_list::iterator, bool > result = idx_list.emplace_back(std::make_pair(input[position],
                                                                                                               std::make_shared<abstract_index>(input[position], this->num_fields, this->num_params)));    // will guess suitable index class

                // if the index was new, add to list for this entire tokenization job
                // if the index has already been seen for a previous macro then add_index() will do nothing provided
                // the index type is consistent
								if(result.second) this->add_index(*result.first, ctx);

                // check for consistency with the intended index type, as supplied by the rule definition
                if(result.first->get_class() != range)
                  {
                    std::ostringstream msg;
                    msg << WARNING_INDEX_TYPE_MISMATCH << " '" << result.first->get_label() << "'";
                    ctx.warn(msg.str());
                  }
							}
						else
							{
                std::ostringstream msg;
						    msg << ERROR_EXPECTED_INDEX_LABEL_A << " '" << candidate << "', " << ERROR_EXPECTED_INDEX_LABEL_B << " '" << input[position] << "'";
                error_context ctx(this->data_payload.get_stack(), this->input_string, position, this->data_payload.get_error_handler(), this->data_payload.get_warning_handler());
                ctx.error(msg.str());
							}

						position++;
					}

				if(!(position < input.length() && input[position] == ']'))
					{
            std::ostringstream msg;
				    msg << ERROR_EXPECTED_CLOSE_INDEX_LIST << " '" << candidate << "'";
            error_context ctx(this->data_payload.get_stack(), this->input_string, position, this->data_payload.get_error_handler(), this->data_payload.get_warning_handler());
            ctx.error(msg.str());
					}
				else
					{
						position++;   // skip closing ]
					}
			}
		else    // expected to find an index list
			{
        std::ostringstream msg;
		    msg << ERROR_EXPECTED_OPEN_INDEX_LIST << " '" << candidate << "'";
        error_context ctx(this->data_payload.get_stack(), this->input_string, position, this->data_payload.get_error_handler(), this->data_payload.get_warning_handler());
        ctx.error(msg.str());
			}

		return(idx_list);
	}


abstract_index_list::const_iterator token_list::add_index(char label)
	{
    // emplace does nothing if a record already exists
    return (this->indices.emplace_back(std::make_pair(label, std::make_unique<abstract_index>(label, this->num_fields, this->num_params)))).first;
	}


abstract_index_list::const_iterator token_list::add_index(const abstract_index& index, error_context& ctx)
	{
    // emplace does nothing if a record already exists;
    // we want to ensure class compatibility, so we have to take this responsibility on ourselves
    abstract_index_list::const_iterator t = this->indices.find(index.get_label());

    if(t != this->indices.end())
      {
        if(index.get_class() != t->get_class())
          {
            std::ostringstream msg;
            msg << ERROR_TOKENIZE_INDEX_MISMATCH << " '" << index.get_label() << "'";
            ctx.error(msg.str());
          }
        return(t);
      }
    else
      {
        return (this->indices.emplace_back(index.get_label(), std::make_unique<abstract_index>(index))).first;
      }
	}


unsigned int token_list::evaluate_macros(simple_macro_type type)
	{
		unsigned int replacements = 0;

    for(token_list_impl::simple_macro_token* t : this->simple_macro_tokens)
			{
        if(t->get_type() == type)
          {
            t->evaluate();
            replacements++;
          }
			}

		return(replacements);
	}


unsigned int token_list::evaluate_macros(const assignment_list& a)
	{
		unsigned int replacements = 0;

    for(token_list_impl::free_index_token*& t : this->free_index_tokens)
      {
        t->evaluate(a);
        replacements++;
      }

    for(token_list_impl::index_macro_token*& t : this->index_macro_tokens)
      {
        t->evaluate(a);
        replacements++;
      }

		return(replacements);
}


std::string token_list::to_string()
	{
    std::string output;

    for(const std::unique_ptr< token_list_impl::generic_token >& t : this->tokens)
			{
				output += t->to_string();
			}

		return(output);
	}


enum unroll_behaviour token_list::unroll_status() const
  {
    if(this->num_force_unroll > 0 && this->num_prevent_unroll == 0) return unroll_behaviour::force;
    if(this->num_force_unroll == 0 && this->num_prevent_unroll > 0) return unroll_behaviour::prevent;

    // if we are in an inconsistent state then an error will already have been raised, so do nothing here
    return unroll_behaviour::allow;
  }



// TOKEN IMPLEMENTATION


token_list_impl::generic_token::generic_token(const std::string& c, error_context ec)
	: conversion(c),
    err_ctx(std::move(ec)),
    num_errors(0),
    silent(false)
	{
	}


void token_list_impl::generic_token::error(const std::string& msg)
  {
    if(!this->silent)
      {
        this->err_ctx.error(msg);
      }

    ++this->num_errors;

    if(!this->silent && this->num_errors >= MAX_TOKEN_ERRORS)
      {
        this->err_ctx.warn(ERROR_TOKENIZE_TOO_MANY_ERRORS);
        this->silent = true;
      }
  }


token_list_impl::text_token::text_token(const std::string& l, error_context ec)
	: generic_token(l, std::move(ec))
	{
	}


token_list_impl::free_index_token::free_index_token(abstract_index_list::const_iterator& it, error_context ec)
	: generic_token(std::string(1, it->get_label()), std::move(ec)),
    index(*it)
	{
	}


void token_list_impl::free_index_token::evaluate(const assignment_list& a)
	{
    assignment_list::const_iterator it = a.find(this->index.get_label());
    if(it == a.end()) throw std::runtime_error(ERROR_MISSING_INDEX_ASSIGNMENT);

    std::ostringstream cnv;
    cnv << it->get_numeric_value();
    this->conversion = cnv.str();
	}


token_list_impl::simple_macro_token::simple_macro_token(const std::string& m, const macro_argument_list& a,
                                                        const macro_packages::simple_rule& r, simple_macro_type t,
                                                        error_context ec)
	: generic_token(m, std::move(ec)),
    name(m),
    args(a),
    rule(r),
    type(t)
	{
	}


void token_list_impl::simple_macro_token::evaluate()
	{
		// evaluate the macro, and cache the result
    try
      {
        this->conversion = this->rule(this->args);
      }
    catch(macro_packages::rule_apply_fail& xe)
      {
        this->error(xe.what());
      }
	}


token_list_impl::index_macro_token::index_macro_token(const std::string& m, const abstract_index_list i,
                                                      const macro_argument_list& a, const macro_packages::index_rule& r,
                                                      error_context ec)
	: generic_token(m, std::move(ec)),
    name(m),
    args(a),
    indices(std::move(i)),
    rule(r)
	{
    try
      {
        // state assumes ownership of the CSE-map returned from the pre-rule
        state = this->rule.pre_evaluate(args);
      }
    catch(macro_packages::rule_apply_fail& xe)
      {
        this->error(xe.what());
      }
	}


token_list_impl::index_macro_token::~index_macro_token()
	{
    try
      {
        this->rule.post_evaluate(this->state.get());
      }
    catch(macro_packages::rule_apply_fail& xe)
      {
        this->error(xe.what());
      }

    // release CSE map held by this macro if it hasn't already been done
    this->state.release();
	}


void token_list_impl::index_macro_token::evaluate(const assignment_list& a)
	{
    // strip out the index assignment -- just for the indices this macro requires;
    // preserves ordering
    assignment_list index_values;

    for(const abstract_index& idx : this->indices)
      {
        assignment_list::const_iterator it = a.find(idx.get_label());
        if(it == a.end()) throw std::runtime_error(ERROR_MISSING_INDEX_ASSIGNMENT);

        index_values.emplace_back(std::make_pair(idx.get_label(), std::make_shared<assignment_record>(*it)));
      }

    try
      {
        this->conversion = this->rule(this->args, index_values, this->state.get());
      }
    catch(macro_packages::rule_apply_fail& xe)
      {
        this->error(xe.what());
      }
	}
