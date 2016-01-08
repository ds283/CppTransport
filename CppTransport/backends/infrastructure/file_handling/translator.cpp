//
// Created by David Seery on 09/12/2013.
// Copyright (c) 2013-15 University of Sussex. All rights reserved.
//


#include <assert.h>

#include <vector>

#include "translator.h"
#include "buffer.h"
#include "package_group_factory.h"
#include "make_tensor_factory.h"

#include "formatter.h"


translator::translator(translator_data& payload)
  : data_payload(payload)
  {
    cache = std::make_unique<expression_cache>();
  }


translator::~translator()
	{
    std::ostringstream msg;
    msg << this->cache->get_hits() << " " << MESSAGE_EXPRESSION_CACHE_HITS
		    << ", " << this->cache->get_misses() << " " << MESSAGE_EXPRESSION_CACHE_MISSES
		    << " (" << MESSAGE_EXPRESSION_CACHE_QUERY_TIME << " " << format_time(this->cache->get_query_time()) << ")";

    this->print_advisory(msg.str());
	}


void translator::print_advisory(const std::string& msg)
	{
    this->data_payload.message(msg);
	}


unsigned int translator::translate(const std::string& in, const error_context& ctx, const std::string& out, enum process_type type, filter_function* filter)
  {
		buffer buf(out);

    unsigned int rval = this->translate(in, ctx, buf, type, filter);

    return(rval);
  }


unsigned int translator::translate(const std::string& in, const error_context& ctx, buffer& buf, enum process_type type, filter_function* filter)
  {
    unsigned int            rval = 0;
    boost::filesystem::path template_in;

    finder& path = this->data_payload.get_finder();

		// try to find a template corresponding to the input filename
    if(path.fqpn(in + ".h", template_in) || path.fqpn(in, template_in))   // leaves fully-qualified path-name in 'template_in', if exists; short-circuit evaluation means value not overwritten
      {
        rval += this->process(template_in, buf, type, filter);
      }
    else
      {
        std::ostringstream msg;
        msg << ERROR_MISSING_TEMPLATE << " '" << in << ".h'";

        ctx.error(msg.str());
      }

    return(rval);
  }


unsigned int translator::process(const boost::filesystem::path& in, buffer& buf, enum process_type type, filter_function* filter)
  {
    unsigned int replacements = 0;
    std::ifstream inf;

    inf.open(in.string().c_str());
    if(inf.is_open() && !inf.fail())
      {
		    // emit advisory that translation is underway
        std::ostringstream translation_msg;
        translation_msg << MESSAGE_TRANSLATING << " '" << in.string() << "'";
		    if(!buf.is_memory())
			    {
		        translation_msg << " " << MESSAGE_TRANSLATING_TO << " '" << buf.get_filename() << "'";
			    }
        this->data_payload.message(translation_msg.str());

		    // decide which backend and API version are required
        error_context err_context(this->data_payload.get_stack(), this->data_payload.get_error_handler(), this->data_payload.get_warning_handler());
        backend_data backend(inf, in, err_context);

        if(backend)
          {
            if(backend.get_min_version() <= CPPTRANSPORT_NUMERIC_VERSION)
              {
                // Generate an appropriate backend

                // A backend consists of a set of macro replacement rules which collectively comprise a 'package group'.
                // The result is returned as a managed pointer, using std::unique_ptr<>
                std::unique_ptr<tensor_factory> factory = make_tensor_factory(backend, this->data_payload, *this->cache);
                std::unique_ptr<package_group> package = package_group_factory(in, backend, this->data_payload, *factory);

                // generate a macro replacement agent based on this package group
                macro_agent agent(this->data_payload, *package, BACKEND_MACRO_PREFIX, BACKEND_LINE_SPLIT_EQUAL, BACKEND_LINE_SPLIT_SUM_EQUAL);

                // push this input file to the top of the filestack
                output_stack& os = this->data_payload.get_stack();
                os.push(in, buf, agent, type);  // current line number is automatically set to 2 (accounting for the header line)

                bool annotate = this->data_payload.annotate();

                while(!inf.eof() && !inf.fail())
                  {
                    std::string line;
                    // read in a line from the template
                    std::getline(inf, line);

                    // apply macro replacement to this line, keeping track of how many replacements are performed
                    // result is supplied as a std::shared_ptr<> because we don't want to have to take copies
                    // of a large array of strings
                    unsigned int new_replacements = 0;
                    std::unique_ptr< std::list<std::string> > line_list = agent.apply(line, new_replacements);
                    replacements += new_replacements;

                    if(line_list)
                      {
                        std::ostringstream continuation_tag;
                        language_printer& printer = package->get_language_printer();

                        continuation_tag << ANNOTATE_EXPANSION_OF_LINE << " " << os.get_line();

                        unsigned int c = 0;
                        for(const std::string& l : *line_list)
                          {
                            std::string out_line = l + (annotate && c > 0 ? " " + printer.comment(continuation_tag.str()) : "");

                            if(filter != nullptr) buf.write_to_end((*filter)(out_line));
                            else                  buf.write_to_end(out_line);

                            ++c;
                          }
                      }

                    os.increment_line();
                  }

                // report end of input to the backend;
                // this enables it to do any tidying-up which may be required,
                // such as depositing temporaries to a temporary pool
                package->report_end_of_input();
                os.pop();

                // emit advisory that translation is complete
                std::ostringstream finished_msg;
                finished_msg << MESSAGE_TRANSLATION_RESULT << " " << replacements << " " << MESSAGE_MACRO_REPLACEMENTS;
                this->data_payload.message(finished_msg.str());

                // report time spent doing macro replacement
                package->report_macro_metadata(agent.get_total_work_time(), agent.get_tokenization_time());

                // package will report on time and memory use when it goes out of scope and is destroyed
              }
          }
        else  // we can't handle this template -- the API version required is too new
          {
            std::ostringstream msg;
            msg << ERROR_TEMPLATE_TOO_RECENT_A << " " << in << " " << ERROR_TEMPLATE_TOO_RECENT_B << backend.get_min_version() << ")";

            error_context err_context(this->data_payload.get_stack(), this->data_payload.get_error_handler(), this->data_payload.get_warning_handler());
            err_context.error(msg.str());
          }
      }
    else  // failed to open the input file
      {
        std::ostringstream msg;
        msg << ERROR_READING_TEMPLATE << " " << in;

        error_context err_context(this->data_payload.get_stack(), this->data_payload.get_error_handler(), this->data_payload.get_warning_handler());
        err_context.error(msg.str());
      }

    inf.close();

    return(replacements);
  }
