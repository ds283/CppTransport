//
// Created by David Seery on 04/12/2013.
// Copyright (c) 2013-15 University of Sussex. All rights reserved.
//



#ifndef __macropackage_fundamental_H_
#define __macropackage_fundamental_H_


#include <string>

#include "msg_en.h"
#include "replacement_rule_package.h"


namespace macro_packages
  {

    class fundamental: public replacement_rule_package
      {
      public:
        fundamental(translation_unit* u, language_printer& p,
                    std::string ls = "{", std::string le = "}", std::string lsep = ",", std::string lpad = " ",
                    std::string t = OUTPUT_CPPTRANSPORT_TAG,
                    std::string ge = "_H_",
                    std::string tf = OUTPUT_TIME_FORMAT)  // output format currently unused, since switch to boost date & time library
          : list_start(ls), list_end(le), list_separator(lsep), list_pad(lpad), tag(t), guard_terminator(ge), time_format(tf),
            unique(0),
            replacement_rule_package(u, p)
          {
          }

        const std::vector<simple_rule> get_pre_rules();
        const std::vector<simple_rule> get_post_rules();
        const std::vector<index_rule>  get_index_rules();

      protected:
        std::string  list_start;
        std::string  list_end;
        std::string  list_separator;
        std::string  list_pad;
        std::string  tag;
        std::string  guard_terminator;

        std::string  time_format;

        unsigned int unique;

        std::string stringize_list         (std::vector<std::string> list);
        std::string stringize_number       (double number);

        // PRE macros
        std::string replace_tool          (const std::vector<std::string> &args);
        std::string replace_version       (const std::vector<std::string> &args);
        std::string replace_guard         (const std::vector<std::string> &args);
        std::string replace_date          (const std::vector<std::string> &args);
        std::string replace_source        (const std::vector<std::string> &args);

        std::string replace_uid           (const std::vector<std::string> &args);

        std::string replace_name          (const std::vector<std::string> &args);
        std::string replace_author        (const std::vector<std::string> &args);
        std::string replace_tag           (const std::vector<std::string> &args);
        std::string replace_model         (const std::vector<std::string> &args);

        std::string replace_header        (const std::vector<std::string> &args);
        std::string replace_core          (const std::vector<std::string> &args);

        std::string replace_number_fields (const std::vector<std::string> &args);
        std::string replace_number_params (const std::vector<std::string> &args);

        std::string replace_field_list    (const std::vector<std::string> &args);
        std::string replace_latex_list    (const std::vector<std::string> &args);
        std::string replace_param_list    (const std::vector<std::string> &args);
        std::string replace_platx_list    (const std::vector<std::string> &args);
        std::string replace_state_list    (const std::vector<std::string> &args);

        std::string replace_b_abs_err     (const std::vector<std::string> &args);
        std::string replace_b_rel_err     (const std::vector<std::string> &args);
        std::string replace_b_step        (const std::vector<std::string> &args);
        std::string replace_b_stepper     (const std::vector<std::string> &args);

        std::string replace_p_abs_err     (const std::vector<std::string> &args);
        std::string replace_p_rel_err     (const std::vector<std::string> &args);
        std::string replace_p_step        (const std::vector<std::string> &args);
        std::string replace_p_stepper     (const std::vector<std::string> &args);

        // POST macros
        std::string replace_unique        (const std::vector<std::string> &args);

      };

  } // namespace macro_packages


#endif //__macropackage_fundamental_H_
