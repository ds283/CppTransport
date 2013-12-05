//
// Created by David Seery on 25/06/2013.
// Copyright (c) 2013 University of Sussex. All rights reserved.
//
// To change the template use AppCode | Preferences | File Templates.
//


#include <assert.h>

#include <vector>
#include <list>

#include "boost/algorithm/string.hpp"

#include "core.h"
#include "cpp_backend.h"
#include "macro.h"

#include "cpp_macros.h"
#include "fundamental.h"
#include "flow_tensors.h"
#include "utensors.h"
#include "lagrangian_tensors.h"
#include "temporary_pool.h"
#include "gauge_xfm.h"

#include "cpp_printer.h"

#include "cpp_cse.h"
#include "flatten.h"


// ******************************************************************

static bool         apply_replacement  (const std::string& input, const std::string& output,
                                        u_tensor_factory* u_factory, cse& temp_factory, flattener& fl,
                                        const struct input& data, script* source, finder& path, bool cse);
static std::string  strip_dot_h        (const std::string& pathname);
static std::string  leafname           (const std::string& pathname);

static bool         process            (replacement_data& d);


// ******************************************************************


// generate C++ output
// returns 'false' if this fails
bool cpp_backend(const struct input& data, finder& path, bool do_cse)
  {
    bool rval = true;

    script* source = data.driver->get_script();

    u_tensor_factory* u_factory    = make_u_tensor_factory(source);
    cpp::cpp_cse      temp_factory = new cpp::cpp_cse(0, ginac_printer(&cpp::print));

    flattener         fl(1);  // dimension of flattener will be set later

    rval = apply_replacement(source->get_core(), data.core_output, u_factory, temp_factory, fl, data, source, path, do_cse);
    if(rval) rval = apply_replacement(source->get_implementation(), data.implementation_output, u_factory, temp_factory, fl, data, source, path, do_cse);

    delete u_factory;
    delete temp_factory;

    return(rval);
  }


static bool apply_replacement(const std::string& input, const std::string& output,
                              u_tensor_factory* u_factory, cse& temp_factory, flattener& fl,
                              const struct input& data, script* source, finder& path, bool do_cse)
  {
    bool rval = true;
    std::string h_template;

    if(input == "")
      {
        error(ERROR_NO_CPP_TEMPLATE);
        rval = false;
      }
    else
      {
        if((rval = path.fqpn(input + ".h", h_template)) == false)
          {
            std::ostringstream msg;
            msg << ERROR_MISSING_CPP_HEADER << " '" << input << ".h'";
            error(msg.str());
          }
      }

    if(rval)
      {
        // set up a buffer to allow insertion of temporaries into the output stream
        std::list<std::string> buffer;

        // set up replacement data
        replacement_data d(buffer, temp_factory, fl);

        d.source        = source;
        d.source_file   = data.name;

        d.u_factory     = u_factory;

        d.do_cse        = do_cse;

        d.output_file   = strip_dot_h(output) + ".h";
        d.core_file     = strip_dot_h(data.core_output) + ".h";
        d.guard         = boost::to_upper_copy(leafname(output));
        d.template_file = h_template;

        d.unique        = 0;

        d.num_fields    = source->get_number_fields();

        // set up a deque of inclusions for error reporting purposes
        struct inclusion inc;
        inc.line = 0;               // line number is irrelevant; we just set it to zero
        inc.name = d.template_file;
        d.path.push_back(inc);
        d.current_line = 1;

        class macro_package ms(source->get_number_fields(), source->get_number_params(), source->get_indexorder(),
          BACKEND_MACRO_PREFIX, BACKEND_LINE_SPLIT, d,
          NUMBER_PRE_MACROS, pre_macros, pre_macro_args, pre_macro_replacements,
          NUMBER_POST_MACROS, post_macros, post_macro_args, post_macro_replacements,
          NUMBER_INDEX_MACROS, index_macros, index_macro_indices, index_macro_ranges, index_macro_args,
                               index_macro_replacements, index_macro_pre, index_macro_post);

        d.ms = &ms;

        rval = process(d);
      }

    return(rval);
  }


static std::string strip_dot_h(const std::string& pathname)
  {
    std::string rval;

    if(pathname.substr(pathname.length() - 3) == ".h")
      {
        rval = pathname.substr(0, pathname.length() - 3);
      }
    else if(pathname.substr(pathname.length() - 5) == ".hpp")
      {
        rval = pathname.substr(0, pathname.length() - 5);
      }
    else
      {
        rval = pathname;
      }

    return(rval);
  }


static std::string leafname(const std::string& pathname)
  {
    std::string rval = pathname;
    size_t      pos;

    while((pos = rval.find("/")) != std::string::npos)
      {
        rval.erase(0, pos+1);
      }

    while((pos = rval.find(".")) != std::string::npos)
      {
        rval.erase(pos+1, rval.length()-pos-1);
      }

    return(rval);
  }


// ******************************************************************


static bool process(replacement_data& d)
  {
    bool rval = true;

    std::ofstream out;
    out.open(d.output_file.c_str());
    if(out.fail())
      {
        std::ostringstream msg;
        msg << ERROR_BACKEND_OUTPUT << " '" << d.output_file << "'";
        error(msg.str());
      }

    std::ifstream in;
    in.open(d.template_file.c_str());

    if(in.is_open())
      {
        while(in.eof() == false && in.fail() == false)
          {
            std::string line;
            std::getline(in, line);

            d.ms->apply(line);
            d.buffer.push_back(line);

            d.current_line++;
          }
     
        // flush any remaining temporaries
        deposit_temporaries(d);

        for(std::list<std::string>::iterator it = d.buffer.begin(); it != d.buffer.end() && out.fail() == false; it++)
          {
            out << *it << std::endl;
          }
        if(out.fail())
          {
            std::ostringstream msg;
            msg << ERROR_CPP_BACKEND_WRITE << "'" << d.output_file << "'";
            error(msg.str());
          }
      }
    else
      {
        std::ostringstream msg;
        msg << ERROR_CPP_TEMPLATE_READ << " '" << d.template_file << "'";
        error(msg.str());
        rval = false;
      }

    if(rval == true) rval = !out.fail();

    in.close();
    out.close();

    return(rval);
  }


// ********************************************************************************


