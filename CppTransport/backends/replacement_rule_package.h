//
// Created by David Seery on 25/06/2013.
// Copyright (c) 2013 University of Sussex. All rights reserved.
//
// To change the template use AppCode | Preferences | File Templates.
//

#ifndef __replacement_rule_package_H_
#define __replacement_rule_package_H_


#include "input.h"
#include "macro.h"


namespace macro_packages
  {

    template <typename rule_type>
    class rule_package
      {
      public:
        std::vector<rule_type>             rules;
        std::vector<replacement_rule_pre>  pre;
        std::vector<replacement_rule_post> post;
        std::vector<std::string>           names;
        unsigned int                       N;
      };


    // abstract replacement_rule_package class
    class replacement_rule_package
      {
      public:
        replacement_rule_package(replacement_data &d)
        : data(d)
          {
          }

        // these methods must be overridden by derived classes which implement the replacement_rule_package concept
        const rule_package<replacement_rule_simple>& get_simple_rules() = 0;
        const rule_package<replacement_rule_index>&  get_index_rules()  = 0;

      protected:
        replacement_data &data;
      };
  }


#endif // __replacement_rule_package_H_