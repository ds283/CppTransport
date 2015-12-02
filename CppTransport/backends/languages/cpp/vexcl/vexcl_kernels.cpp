//
// Created by David Seery on 08/12/2013.
// Copyright (c) 2013-15 University of Sussex. All rights reserved.
//


#include <assert.h>
#include <string>
#include <sstream>

#include <functional>

#include "vexcl_kernels.h"
#include "macro.h"
#include "translation_unit.h"

#include "to_printable.h"

#define BIND(X) std::bind(&vexcl_kernels::X, this, std::placeholders::_1)


namespace cpp
  {

    const std::vector<macro_packages::simple_rule> vexcl_kernels::get_pre_rules()
      {
        std::vector<macro_packages::simple_rule> package;

        const std::vector<replacement_rule_simple> rules =
                                                     { BIND(import_kernel)
                                                     };

        const std::vector<std::string> names =
                                         { "IMPORT_KERNEL"
                                         };

        const std::vector<unsigned int> args =
                                          { 3
                                          };

        assert(rules.size() == names.size());
        assert(rules.size() == args.size());

        for(int i = 0; i < rules.size(); ++i)
          {
            macro_packages::simple_rule rule;

            rule.rule = rules[i];
            rule.args = args[i];
            rule.name = names[i];

            package.push_back(rule);
          }

        return(package);
      }


    const std::vector<macro_packages::simple_rule> vexcl_kernels::get_post_rules()
      {
        std::vector<macro_packages::simple_rule> package;

        return(package);
      }


    const std::vector<macro_packages::index_rule> vexcl_kernels::get_index_rules()
      {
        std::vector<macro_packages::index_rule> package;

        return(package);
      }


    // *******************************************************************


    std::string vexcl_kernels::import_kernel(const std::vector<std::string>& args)
      {
        assert(args.size() == 3);

        std::string rval = "";

        if(args.size() >= 1)
          {
            std::string kernel_file = args[0];

            output_stack& os = this->data_payload.get_stack();
            translator t(this->data_payload);

            buffer& buf = os.top_buffer();
            enum process_type type = os.top_process_type();

            // set up a new in-memory buffer to hold the translated kernel
            buffer kernel_buffer;

            // inherit current decoration (line prefixes, skipping blank lines) from parent buffer
            kernel_buffer.inherit_decoration(buf);

            // the lines of the kernel have to be quoted with ", so ask buffer to set up new delimiters
            // also skip blank lines, which [usually] don't help in stringized kernels
            kernel_buffer.push_delimiter(OUTPUT_VEXCL_KERNEL_LINE_PRE, OUTPUT_VEXCL_KERNEL_LINE_POST);
            kernel_buffer.push_skip_blank(true);

            // set up a filter to ensure we emit only printable strings.
            // The bound arguments here are:
            // * 'false' disables quoting by to_printable())
            // * 'true' allows newlines to pass through
            std::function<std::string(const std::string&)> filter = std::bind(to_printable, std::placeholders::_1, false, true);

            // translate into our new in-memory buffer, preserving the type of translation (ie. core or implementation)
            error_context ctx(this->data_payload.get_stack(), this->data_payload.get_error_handler(), this->data_payload.get_warning_handler());
            unsigned int replacements = t.translate(kernel_file, ctx, kernel_buffer, type, &filter);

            // merge new buffer into old one
            buf.merge(kernel_buffer);

            // output kernel name string
            if(args.size() >= 2)
              {
                buf.write_to_end(OUTPUT_VEXCL_KERNEL_PRE + args[1] + OUTPUT_VEXCL_KERNEL_POST);
              }
            else
              {
                buf.write_to_end(OUTPUT_VEXCL_UNKNOWN_KERNEL);
              }

            // output trailing content, which must appear *before* the location of the new pool, if required
            if(args.size() >= 3)
              {
                buf.write_to_end(args[2]);
                buf.write_to_end("");  // skip a line to avoid collisions with the new pool (set below)
              }

            // mark this point as the beginning of a new pool
            rval = this->printer.comment(OUTPUT_KERNEL_LOCATION);
          }

        return(rval);
      }

  } // namespace cpp
