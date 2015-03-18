//
// Created by David Seery on 09/12/2013.
// Copyright (c) 2013-15 University of Sussex. All rights reserved.
//


#include <assert.h>

#include <vector>

#include "boost/algorithm/string.hpp"
#include "boost/range/algorithm/remove_if.hpp"
#include "boost/lexical_cast.hpp"

#include "buffer.h"
#include "package_group_factory.h"


#define BACKEND_TOKEN "backend"
#define MINVER_TOKEN  "minver"


translator::translator(translation_unit* tu)
  : unit(tu)
  {
    assert(unit != nullptr);
  }


translator::~translator()
	{
    std::ostringstream msg;
    msg << this->cache.get_hits() << " " << MESSAGE_EXPRESSION_CACHE_HITS
		    << ", " << this->cache.get_misses() << " " << MESSAGE_EXPRESSION_CACHE_MISSES
		    << " (" << MESSAGE_EXPRESSION_CACHE_QUERY_TIME << " " << format_time(this->cache.get_query_time()) << ")";

    this->print_advisory(msg.str());
	}


void translator::print_advisory(const std::string& msg)
	{
    this->unit->print_advisory(msg);
	}


unsigned int translator::translate(const std::string& in, const std::string& out, enum process_type type, filter_function* filter)
  {
		buffer buf(out);

    unsigned int rval = this->translate(in, buf, type, filter);

    return(rval);
  }


unsigned int translator::translate(const std::string& in, buffer& buf, enum process_type type, filter_function* filter)
  {
    unsigned int rval = 0;
    std::string  template_in;

    finder* path = this->unit->get_finder();

		// try to find a template corresponding to the input filename
    if(path->fqpn(in + ".h", template_in))    // leaves fully qualified pathname in template_in if it exists
      {
        rval += this->process(template_in, buf, type, filter);
      }
    else if(path->fqpn(in, template_in))
      {
        rval += this->process(template_in, buf, type, filter);
      }
    else
      {
        std::ostringstream msg;
        msg << ERROR_MISSING_TEMPLATE << " '" << in << ".h'";
        error(msg.str());
      }

    return(rval);
  }


unsigned int translator::process(const std::string in, buffer& buf, enum process_type type, filter_function* filter)
  {
    unsigned int replacements = 0;
    std::ifstream inf;

    inf.open(in.c_str());
    if(inf.is_open() && !inf.fail())
      {
        std::string line;
        std::string backend;
        double minver;

		    // emit advisory that translation is underway
        std::ostringstream translation_msg;
        translation_msg << MESSAGE_TRANSLATING << " '" << in << "'";
		    if(!buf.is_memory())
			    {
		        translation_msg << " " << MESSAGE_TRANSLATING_TO << " '" << buf.get_filename() << "'";
			    }
        this->unit->print_advisory(translation_msg.str());

		    // decide which backend and API version are required
        std::getline(inf, line);
        this->parse_header_line(in, line, backend, minver);

        if(minver <= CPPTRANSPORT_NUMERIC_VERSION)
          {
            // generate an appropriate backend
		        // this consists of a set of macro replacement rules which collectively comprise a 'package group'
            std::shared_ptr<package_group> package = package_group_factory(in, backend, this->unit, this->cache);

            // generate a macro replacement agent based on this package group
            macro_agent agent(this->unit, package, BACKEND_MACRO_PREFIX, BACKEND_LINE_SPLIT);

            // push this input file to the top of the filestack
            output_stack* os  = this->unit->get_stack();
            os->push(in, buf, agent, type);  // current line number is automatically set to 1

            while(!inf.eof() && !inf.fail())
              {
                // read in a line from the template
                std::getline(inf, line);

                // apply macro replacement to this line, keeping track of how many replacements are performed
                unsigned int new_replacements = 0;
                std::shared_ptr< std::vector<std::string> > line_list = agent.apply(line, new_replacements);
                replacements += new_replacements;

                std::ostringstream continuation_tag;
                continuation_tag << " " << package->get_comment_separator() << " " << MESSAGE_EXPANSION_OF_LINE << " " << os->get_line();

                unsigned int c = 0;
                for(std::vector<std::string>::const_iterator l = line_list->begin(); l != line_list->end(); l++, c++)
                  {
                    std::string out_line = *l + (c > 0 ? continuation_tag.str() : "");

                    if(filter != nullptr) buf.write_to_end((*filter)(out_line));
                    else                  buf.write_to_end(out_line);
                  }

                os->increment_line();
              }

		        // report end of input to the backend;
		        // this enables it to do any tidying-up which may be required,
		        // such as depositing temporaries to a temporary pool
		        package->report_end_of_input();
            os->pop();

            // emit advisory that translation is complete
            std::ostringstream finished_msg;
            finished_msg << MESSAGE_TRANSLATION_RESULT << " " << replacements << " " << MESSAGE_MACRO_REPLACEMENTS;
            this->unit->print_advisory(finished_msg.str());

            // report time spent doing macro replacement
            package->report_macro_metadata(agent.get_total_time(), agent.get_tokenization_time());

		        // package will report on time and memory use when it goes out of scope and is destroyed
          }
        else  // we can't handle this template -- the API version required is too new
          {
            std::ostringstream msg;
            msg << ERROR_TEMPLATE_TOO_RECENT_A << " '" << in << "' " << ERROR_TEMPLATE_TOO_RECENT_B << minver << ")";
            error(msg.str());
          }
      }
    else  // failed to open the input file
      {
        std::ostringstream msg;
        msg << ERROR_READING_TEMPLATE << " '" << in << "'";
        error(msg.str());
      }

    inf.close();

    return(replacements);
  }


void translator::parse_header_line(const std::string in, const std::string line, std::string& backend, double& minver)
  {
    std::vector<std::string> tokens;
    boost::split(tokens, line, boost::is_any_of(" ,:;="));

    bool backend_set = false;
    bool minver_set  = false;

    if(tokens.size() <= 1)
      {
        std::ostringstream msg;
        msg << ERROR_IMPROPER_TEMPLATE_HEADER << " '" << in << "'";
        error(msg.str());
      }

    for(int i = 1; i < tokens.size(); i++)
      {
        if(boost::to_lower_copy(tokens[i]) == BACKEND_TOKEN)
          {
            if(i+1 < tokens.size())
              {
                if(backend_set)
                  {
                    std::ostringstream msg;
                    msg << WARNING_DUPLICATE_TEMPLATE_BACKEND << " '" << in << "'";
                    warn(msg.str());
                  }
                else
                  {
                    backend     = tokens[++i];
                    backend_set = true;
                  }
              }
            else
              {
                std::ostringstream msg;
                msg << ERROR_EXPECTED_TEMPLATE_BACKEND << " '" << in << "'";
                error(msg.str());
              }
          }
        else if(boost::to_lower_copy(tokens[i]) == MINVER_TOKEN)
          {
            if(i+1 < tokens.size())
              {
                if(minver_set)
                  {
                    std::ostringstream msg;
                    msg << WARNING_DUPLICATE_TEMPLATE_MINVER << " '" << in << "'";
                    warn(msg.str());
                  }
                else
                  {
                    minver     = boost::lexical_cast<double>(tokens[++i]);
                    minver_set = true;
                  }
              }
            else
              {
                std::ostringstream msg;
                msg << ERROR_EXPECTED_TEMPLATE_MINVER << " '" << in << "'";
                error(msg.str());
              }
          }
      }

    if(!backend_set || !minver_set)
      {
        std::ostringstream msg;
        msg << ERROR_IMPROPER_TEMPLATE_HEADER << " '" << in << "'";
        error(msg.str());
      }
  }

