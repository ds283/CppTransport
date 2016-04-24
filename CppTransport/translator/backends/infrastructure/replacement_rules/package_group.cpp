//
// Created by David Seery on 05/12/2013.
// --@@
// Copyright (c) 2016 University of Sussex. All rights reserved.
//
// This file is part of the CppTransport platform.
//
// CppTransport is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 2 of the License, or
// (at your option) any later version.
//
// CppTransport is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with CppTransport.  If not, see <http://www.gnu.org/licenses/>.
//
// @license: GPL-2
// @contributor: David Seery <D.Seery@sussex.ac.uk>
// --@@
//


#include "package_group.h"

#include "formatter.h"


package_group::package_group(translator_data& p, tensor_factory& _f)
  : data_payload(p),
		statistics_reported(false),
    fctry(_f)
  {
  }


package_group::~package_group()
  {
		if(this->statistics_reported)
			{
		    std::ostringstream msg;

				msg << MESSAGE_MACRO_TIME << " " << format_time(this->macro_replacement_time)
						<< ", " << MESSAGE_TOKENIZATION_TIME << " " << format_time(this->macro_tokenization_time)
						<< " (" << MESSAGE_SYMBOLIC_COMPUTE_TIME << " " << format_time(this->fctry.get_symbolic_compute_time())
			      << ", " << MESSAGE_CSE_TIME << " " << format_time(this->cse_worker->get_cse_time()) << ")";

		    this->data_payload.message(msg.str());
			}
  }


void package_group::build_pre_ruleset()
  {
    this->pre_ruleset.clear();

    for(std::unique_ptr<macro_packages::replacement_rule_package>& pkg : this->packages)
      {
        const std::vector< std::unique_ptr<macro_packages::replacement_rule_simple> >& rules = pkg->get_pre_rules();

        this->pre_ruleset.reserve(this->pre_ruleset.size() + rules.size());
        for(const std::unique_ptr<macro_packages::replacement_rule_simple>& rule : rules)
          {
            this->pre_ruleset.emplace_back(rule.get());
          }
      }
  }


void package_group::build_post_ruleset()
  {
    this->post_ruleset.clear();

    for(std::unique_ptr<macro_packages::replacement_rule_package>& pkg : this->packages)
      {
        const std::vector< std::unique_ptr<macro_packages::replacement_rule_simple> >& rules = pkg->get_post_rules();

        this->post_ruleset.reserve(this->post_ruleset.size() + rules.size());
        for(const std::unique_ptr<macro_packages::replacement_rule_simple>& rule : rules)
          {
            this->post_ruleset.emplace_back(rule.get());
          }
      }
  }


void package_group::build_index_ruleset()
  {
    this->index_ruleset.clear();

    for(std::unique_ptr<macro_packages::replacement_rule_package>& pkg : this->packages)
      {
        const std::vector< std::unique_ptr<macro_packages::replacement_rule_index> >& rules = pkg->get_index_rules();

        this->index_ruleset.reserve(this->index_ruleset.size() + rules.size());
        for(const std::unique_ptr<macro_packages::replacement_rule_index>& rule : rules)
          {
            this->index_ruleset.emplace_back(rule.get());
          }
      }
  }


void package_group::report_end_of_input()
  {
    for(std::unique_ptr<macro_packages::replacement_rule_package>& pkg : this->packages)
      {
        pkg->report_end_of_input();
      }
  }
