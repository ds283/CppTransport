//
// Created by David Seery on 11/03/15.
// Copyright (c) 2015 University of Sussex. All rights reserved.
//

#include <assert.h>
#include <sstream>
#include <stdexcept>

#include "macro_tokenizer.h"


token_list::token_list(const std::string& input,
                       const std::string& prefix,
                       const std::vector<macro_packages::simple_rule>& pre,
                       const std::vector<macro_packages::simple_rule>& post,
                       const std::vector<macro_packages::index_rule>& index)
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

						this->tokens.push_back(new text_token(string_literal));
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
										this->tokens.push_back(new free_index_token(input[position]));
										this->add_index(input[position]);
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

										            std::vector<std::string> arg_list;
												        if(rule.args > 0) arg_list = this->get_argument_list(input, candidate, position, rule.args);

												        this->tokens.push_back(new simple_macro_token(candidate, arg_list, rule, simple_macro_token::pre));
											        }
										        else if(this->check_for_match(candidate, post, false))
											        {
										            // we matched a simple post-macro
										            const macro_packages::simple_rule& rule = this->find_match(candidate, post);

										            // found a match -- move position past the candidate
										            position += candidate_length;

										            // shouldn't find an index list
										            this->check_no_index_list(input, candidate, position);

										            std::vector<std::string> arg_list;
										            if(rule.args > 0) arg_list = this->get_argument_list(input, candidate, position, rule.args);

												        this->tokens.push_back(new simple_macro_token(candidate, arg_list, rule, simple_macro_token::post));
											        }
										        else if(this->check_for_match(candidate, index, false))
											        {
										            // we matched an index macro
										            const macro_packages::index_rule& rule = this->find_match(candidate, index);

										            // found a match -- move position past the candidate
										            position += candidate_length;

												        // should find an index list
										            std::vector<index_abstract> idx_list = this->get_index_list(input, candidate, position, rule.indices, rule.range);

										            std::vector<std::string> arg_list;
										            if(rule.args > 0) arg_list = this->get_argument_list(input, candidate, position, rule.args);

												        this->tokens.push_back(new index_macro_token(candidate, idx_list, arg_list, rule));
											        }
										        else  // something has gone wrong
											        {
												        // we didn't find an exact match after all; we only matched a substring
												        // assume it was a free index after all
										            this->tokens.push_back(new free_index_token(input[position]));
										            this->add_index(input[position]);
										            position++;
											        }
											    }
								        catch(std::runtime_error& xe)
									        {
										        // something whent wrong
										        // assume it was a free index after all
								            this->tokens.push_back(new free_index_token(input[position]));
								            this->add_index(input[position]);
								            position++;
									        }
											}
										else  // no it doesn't; there's no match. It must have been a free index
											{
												this->tokens.push_back(new free_index_token(input[position]));
										    this->add_index(input[position]);
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

						    this->tokens.push_back(new text_token(string_literal));
							}
					}
			}
	}


token_list::~token_list()
	{
		for(std::list< generic_token* >::iterator t = this->tokens.begin(); t != this->tokens.end(); ++t)
			{
				delete *t;
			}
	}


template <typename Rule>
bool token_list::check_for_match(const std::string& candidate, const std::vector<Rule>& rule_list, bool allow_substring)
	{
		bool found = false;

		for(typename std::vector<Rule>::const_iterator t = rule_list.begin(); !found && t != rule_list.end(); ++t)
			{
				if(candidate.length() <= (*t).name.length())
					{
				    std::string name_substring = allow_substring ? (*t).name.substr(0, candidate.length()) : (*t).name;
						if(candidate == name_substring)
							{
								found = true;
							}
					}
			}

		return(found);
	}


template <typename Rule>
const Rule& token_list::find_match(const std::string& candidate, const std::vector<Rule>& rule_list)
	{
		for(typename std::vector<Rule>::const_iterator t = rule_list.begin(); t != rule_list.end(); ++t)
			{
				if(candidate == (*t).name)
					{
						return(*t);
					}
			}

		throw std::runtime_error("Unexpectedly failed to match any macro");
	}


void token_list::check_no_index_list(const std::string& input, const std::string& candidate, size_t& position)
	{
    if(position < input.length() && input[position] == '[')
	    {
        std::cout << "ERROR: didn't expect index list following simple macro '" << candidate << "'; skipping" << '\n';
        position++;
        while(position < input.length() && input[position] != ']')
	        {
            position++;
	        }
        if(position < input.length()) position++;
	    }
	}


std::vector<std::string> token_list::get_argument_list(const std::string& input, const std::string& candidate, size_t& position, unsigned int expected_args)
	{
    std::vector<std::string> arg_list;

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
						position++;
			    }

				if(!(position < input.length() && input[position] == '}'))
					{
				    std::cout << ERROR_EXPECTED_CLOSE_ARGUMENT_LIST << " '" << candidate << "'";
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
		    std::cout << ERROR_EXPECTED_OPEN_ARGUMENT_LIST << " '" << candidate << "'";
			}

		if(arg_list.size() != expected_args)
			{
		    std::cout << ERROR_WRONG_ARGUMENT_COUNT << " '" << candidate << "' (" << ERROR_EXPECTED_ARGUMENT_COUNT << " " << expected_args << ")" << '\n';

		    while(arg_list.size() < expected_args)
			    {
		        arg_list.push_back(std::string(""));
			    }

			}

		return(arg_list);
	}


std::vector<index_abstract> token_list::get_index_list(const std::string& input, const std::string& candidate, size_t& position, unsigned int expected_indices, unsigned int range)
	{
    std::vector<index_abstract> idx_list;

		if(position < input.length() && input[position] == '[')
			{
				position++;

				while(position < input.length() && input[position] != ']')
					{
						if(isalnum(input[position]))
							{
								struct index_abstract index;

								index.label = input[position];
								assert(range = identify_index(index.label));
								index.range = range;

								idx_list.push_back(index);
								this->add_index(index);
							}
						else
							{
						    std::cout << ERROR_EXPECTED_INDEX_LABEL_A << " '" << candidate << "', " << ERROR_EXPECTED_INDEX_LABEL_B << " '" << input[position] << "'" << '\n';
							}

						position++;
					}

				if(!(position < input.length() && input[position] == ']'))
					{
				    std::cout << ERROR_EXPECTED_CLOSE_ARGUMENT_LIST << " '" << candidate << "'" << '\n';
					}
				else
					{
						position++;   // skip closing ]
					}
			}
		else    // expected to find an index list
			{
		    std::cout << ERROR_EXPECTED_OPEN_INDEX_LIST << " '" << candidate << "'" << '\n';
			}

		if(idx_list.size() != expected_indices)
			{
		    std::cout << ERROR_WRONG_INDEX_COUNT << " '" << candidate << "' (" << ERROR_EXPECTED_INDEX_COUNT << " " << expected_indices << ")";
			}

		return(idx_list);
	}


void token_list::add_index(char label)
	{
    index_abstract index;

		index.label = label;
		index.range = identify_index(label);

		add_index(index);
	}


void token_list::add_index(const index_abstract& index)
	{
		bool found = false;

		for(std::vector<index_abstract>::iterator t = this->indices.begin(); !found && t != this->indices.end(); ++t)
			{
				if((*t).label == index.label && (*t).range == index.range)
					{
						found = true;
					}
			}

		if(!found)
			{
				this->indices.push_back(index);
			}
	}


unsigned int token_list::evaluate_macros(simple_macro_token::macro_type type)
	{
		unsigned int replacements = 0;

		for(std::list<generic_token*>::iterator t = this->tokens.begin(); t != this->tokens.end(); ++t)
			{
				simple_macro_token* token;

				if((token = dynamic_cast<simple_macro_token*>(*t)) != nullptr)
					{
						if(token->get_type() == type)
							{
								token->evaluate();
								replacements++;
							}
					}
			}

		return(replacements);
	}


unsigned int token_list::evaluate_macros(const std::vector<index_assignment>& a)
	{
		unsigned int replacements = 0;

		for(std::list<generic_token*>::iterator t = this->tokens.begin(); t != this->tokens.end(); ++t)
			{
				free_index_token* f_token;
				index_macro_token* i_token;

				if((f_token = dynamic_cast<free_index_token*>(*t)) != nullptr)
					{
						f_token->evaluate(a);
						replacements++;
					}
				else if((i_token = dynamic_cast<index_macro_token*>(*t)) != nullptr)
					{
						i_token->evaluate(a);
						replacements++;
					}
			}

		return(replacements);
}


std::string token_list::to_string()
	{
    std::string output;

		for(std::list<generic_token*>::iterator t = this->tokens.begin(); t != this->tokens.end(); ++t)
			{
				output += (*t)->to_string();
			}

		return(output);
	}


// TOKEN IMPLEMENTATION


token_list::generic_token::generic_token(const std::string& c)
	: conversion(c)
	{
	}


token_list::text_token::text_token(const std::string& l)
	: generic_token(l)
	{
	}


token_list::free_index_token::free_index_token(const struct index_abstract& i)
	: generic_token(std::string(1, i.label)),
    index(i)
	{
	}


void token_list::free_index_token::evaluate(const std::vector<index_assignment>& a)
	{
    bool found = false;

    for(std::vector<index_assignment>::const_iterator t = a.begin(); !found && t != a.end(); ++t)
	    {
        if((*t).label == this->index.label)
	        {
            std::ostringstream cnv;
            cnv << index_numeric(*t);
            this->conversion = cnv.str();
            found = true;
	        }
	    }

    if(!found)
	    {
        throw std::runtime_error("Missing index assignment!");
	    }
	}


token_list::simple_macro_token::simple_macro_token(const std::string& m, const std::vector<std::string>& a,
                                                   const macro_packages::simple_rule& r, token_list::simple_macro_token::macro_type t)
	: generic_token(m),
    name(m),
    args(a),
    rule(r),
    type(t)
	{
	}


void token_list::simple_macro_token::evaluate()
	{
		// evaluate the macro, and cache the result
		this->conversion = this->rule.rule(this->args);
	}


token_list::index_macro_token::index_macro_token(const std::string& m, const std::vector<index_abstract>& i, const std::vector<std::string>& a, const macro_packages::index_rule& r)
	: generic_token(m),
    name(m),
    args(a),
    indices(i),
    rule(r),
    state(nullptr)
	{
    if(rule.pre != nullptr)
	    {
        state = (rule.pre)(args);
	    }
	}


token_list::index_macro_token::~index_macro_token()
	{
    if(this->rule.post != nullptr)
	    {
        (this->rule.post)(this->state);
	    }
	}


void token_list::index_macro_token::evaluate(const std::vector<index_assignment>& a)
	{
    std::vector<struct index_assignment> index_values;

		for(std::vector<index_abstract>::const_iterator t = this->indices.begin(); t != indices.end(); ++t)
			{
				bool found = false;
				for(std::vector<index_assignment>::const_iterator u = a.begin(); !found && u != a.end(); ++u)
					{
						if((*u).label == (*t).label)
							{
								index_values.push_back(*u);
								found = true;
							}
					}

				if(!found)
					{
						throw std::runtime_error("Missing index assignment!");
					}
			}

		this->conversion = (this->rule.rule)(this->args, index_values, this->state);
	}
