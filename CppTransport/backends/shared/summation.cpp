//
// Created by David Seery on 17/12/2013.
// Copyright (c) 2013-15 University of Sussex. All rights reserved.
//


#include <assert.h>
#include <string>
#include <sstream>

#include <functional>

#include "summation.h"
#include "translation_unit.h"


#define BIND(x)  std::bind(&summation::X, this, std::placeholders::_1)
#define BIND3(X) std::bind(&summation::X, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)


namespace macro_packages
  {

    const std::vector<macro_packages::simple_rule> summation::get_pre_rules()
      {
        std::vector<macro_packages::simple_rule> package;

        return(package);
      }


    const std::vector<macro_packages::simple_rule> summation::get_post_rules()
      {
        std::vector<macro_packages::simple_rule> package;

        return(package);
      }


    const std::vector<macro_packages::index_rule> summation::get_index_rules()
      {
        std::vector<macro_packages::index_rule> package;

        const std::vector<replacement_rule_pre> pres =
                                                  { nullptr, nullptr, nullptr,
                                                    nullptr, nullptr, nullptr
                                                  };

        const std::vector<replacement_rule_post> posts =
                                                   { nullptr, nullptr, nullptr,
                                                     nullptr, nullptr, nullptr
                                                   };

        const std::vector<replacement_rule_index> rules =
                                                    { BIND3(null), BIND3(null), BIND3(null),
                                                      BIND3(null), BIND3(null), BIND3(null)
                                                    };

        const std::vector<std::string> names =
                                         { "SUM_COORDS", "SUM_FIELDS", "SUM_PARAMS",
                                           "SUM_2COORDS", "SUM_2FIELDS", "SUM_2PARAMS",
                                         };

        const std::vector<unsigned int> args =
                                          { 0, 0, 0,
                                            0, 0, 0
                                          };

        const std::vector<unsigned int> indices =
                                          { 1, 1, 1,
                                            2, 2, 2
                                          };

        const std::vector<unsigned int> ranges =
                                          { 2, 1, INDEX_RANGE_PARAMETER,
                                            2, 1, INDEX_RANGE_PARAMETER
                                          };

        assert(pres.size() == posts.size());
        assert(pres.size() == rules.size());
        assert(pres.size() == names.size());
        assert(pres.size() == args.size());
        assert(pres.size() == ranges.size());

        for(int i = 0; i < pres.size(); i++)
          {
            index_rule rule;

            rule.rule    = rules[i];
            rule.name    = names[i];
            rule.pre     = pres[i];
            rule.post    = posts[i];
            rule.args    = args[i];
            rule.indices = indices[i];
            rule.range   = ranges[i];

            package.push_back(rule);
          }

        return(package);
      }


    // *******************************************************************


    std::string summation::null(const std::vector<std::string>& args, std::vector<struct index_assignment> indices, void* state)
      {
        return("");
      }

  } // namespace macro_packages
