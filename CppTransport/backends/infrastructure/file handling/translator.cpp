//
// Created by David Seery on 09/12/2013.
// Copyright (c) 2013 University of Sussex. All rights reserved.
//


#include <assert.h>

#include "boost/algorithm/string.hpp"
#include "boost/range/algorithm/remove_if.hpp"
#include "boost/lexical_cast.hpp"

#include "buffer.h"
#include "package_group_factory.h"
#include "msg_en.h"

#include "translator.h"
#include "translation_unit.h"


#define BACKEND_TOKEN "backend"
#define MINVER_TOKEN  "minver"


translator::translator(translation_unit* tu)
  : unit(tu)
  {
    assert(unit != nullptr);
  }


unsigned int translator::translate(const std::string in, const std::string out, enum process_type type, filter_function* filter)
  {
    buffer* buf = new buffer;
    unsigned int rval = this->translate(in, out, type, buf, filter);
    delete buf;

    return(rval);
  }


unsigned int translator::translate(const std::string in, const std::string out, enum process_type type, buffer* buf, filter_function* filter)
  {
    unsigned int rval = 0;
    std::string  template_in;

    finder* path = this->unit->get_finder();

    if(path->fqpn(in + ".h", template_in) == true)    // leaves fully qualified pathname in template_in if it exists
      {
        rval += this->process(template_in, out, type, buf, filter);
      }
    else if(path->fqpn(in, template_in) == true)
      {
        rval += this->process(template_in, out, type, buf, filter);
      }
    else
      {
        std::ostringstream msg;
        msg << ERROR_MISSING_TEMPLATE << " '" << in << ".h'";
        error(msg.str());
      }

    return(rval);
  }


unsigned int translator::process(const std::string in, const std::string out, enum process_type type, buffer* buf, filter_function* filter)
  {
    unsigned int  rval = 0;
    std::ifstream inf;

    inf.open(in.c_str());
    if(inf.is_open() && !inf.fail())
      {
        std::string line;
        std::string backend;
        double minver;

        std::getline(inf, line);
        this->parse_header_line(in, line, backend, minver);

        if(minver <= CPPTRANSPORT_NUMERIC_VERSION)
          {
            // want to generate an object on the stack *before* we call package_group,
            // because the constructors of replacement_rule_package objects may depend
            // on it being there
            output_stack* os  = this->unit->get_stack();
            os->push(out, in, buf, type);  // current line number is automatically set to 1

            // generate an appropriate backend
            package_group* package = package_group_factory(backend, this->unit);

            if(package != nullptr)
              {
                macro_package* ms = new macro_package(this->unit, package, BACKEND_MACRO_PREFIX, BACKEND_LINE_SPLIT);

                os->push_top_data(ms, package);

                while(inf.eof() == false && inf.fail() == false)
                  {
                    std::getline(inf, line);
                    rval += ms->apply(line);

                    if(filter != nullptr)
                      {
                        buf->write_to_end((*filter)(line));
                      }
                    else
                      {
                        buf->write_to_end(line);
                      }
                    os->increment_line();
                  }

                // if we are required to output the buffer, do so.
                // any temporaries will be automatically flushed.
                // if we are not required to output, temporaries
                // will be flushed when the package_group is destroyed
                // below
                if(out != "") buf->emit(out);
              }
            else
              {
                std::ostringstream msg;
                msg << ERROR_TEMPLATE_BACKEND_A << " '" << in << "' " << ERROR_TEMPLATE_BACKEND_B << " '" << backend << "'";
                error(msg.str());
              }

            // note that destruction of the package_group must happen
            // before the output stack is adjusted, because replacement_rule_packages
            // may depend on the stack contents
            delete package;
            os->pop();
          }
        else
          {
            std::ostringstream msg;
            msg << ERROR_TEMPLATE_TOO_RECENT_A << " '" << in << "' " << ERROR_TEMPLATE_TOO_RECENT_B << minver << ")";
            error(msg.str());
          }
      }
    else
      {
        std::ostringstream msg;
        msg << ERROR_READING_TEMPLATE << " '" << in << "'";
        error(msg.str());
      }

    inf.close();

    return(rval);
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

