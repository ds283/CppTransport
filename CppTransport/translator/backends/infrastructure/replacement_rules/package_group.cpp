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


package_group::~package_group()
  {
    if(!this->data_payload.get_argument_cache().show_profiling()) return;

		if(this->statistics_reported)
			{
		    std::ostringstream msg;

				msg << MESSAGE_MACRO_TIME << " " << format_time(this->macro_replacement_time)
						<< ", " << MESSAGE_TOKENIZATION_TIME << " " << format_time(this->macro_tokenization_time)
						<< " (" << MESSAGE_SYMBOLIC_COMPUTE_TIME << " " << format_time(this->fctry.get_symbolic_compute_time())
			      << ", " << MESSAGE_CSE_TIME << " " << format_time(this->cse_worker->get_cse_time()) << ")";

		    this->data_payload.message(msg.str());
			}

    auto hits = this->lambda_mgr->get_hits();
    auto misses = this->lambda_mgr->get_misses();

    if(hits + misses > 0)
      {
        double hit_rate = static_cast<double>(hits) / (static_cast<double>(hits) + static_cast<double>(misses));
        
        std::ostringstream lambda_msg;
        lambda_msg << hits << " " << (hits == 1 ? MESSAGE_LAMBDA_CACHE_HIT : MESSAGE_LAMBDA_CACHE_HITS)
                   << ", " << misses << " " << MESSAGE_LAMBDA_CACHE_MISSES;
        auto prec = lambda_msg.precision();
        lambda_msg.precision(3);
        lambda_msg << " (" << 100.0*hit_rate << "%)";
        lambda_msg.precision(prec);
        lambda_msg << " (" << MESSAGE_LAMBDA_CACHE_QUERY_TIME << " " << format_time(this->lambda_mgr->get_query_time())
                   << ", " << MESSAGE_LAMBDA_CACHE_INSERT_TIME << " " << format_time(this->lambda_mgr->get_insert_time())
                   << ")";
        this->data_payload.message(lambda_msg.str());
      }
  }


template <typename SourceDatabase, typename DestinationContainer, typename RuleGetter>
void package_group::build_set(const SourceDatabase& src, DestinationContainer& dest, RuleGetter get_rules)
  {
    dest.clear();
    
    // loop over all packages in the source database
    for(const auto& pkg : src)
      {
        // extract the rules provided by this package
        const auto& rules = get_rules(pkg);
        
        // now push references to all of these rules into the destination container
        dest.reserve(dest.size() + rules.size());
        for(const auto& rule : rules)
          {
            dest.emplace_back(std::ref(*rule));
          }
      }
  };


void package_group::build_pre_ruleset()
  {
    this->build_set(this->rule_packages, this->pre_rules, [](const auto& pkg) -> auto& { return pkg->get_pre_rules(); });
  }


void package_group::build_post_ruleset()
  {
    this->build_set(this->rule_packages, this->post_rules, [](const auto& pkg) -> auto& { return pkg->get_post_rules(); });
  }


void package_group::build_index_ruleset()
  {
    this->build_set(this->rule_packages, this->index_rules, [](const auto& pkg) -> auto& { return pkg->get_index_rules(); });
  }


void package_group::build_simple_directiveset()
  {
    this->build_set(this->directive_packages, this->simple_directives, [](const auto& pkg) -> auto& { return pkg->get_simple(); });
  }


void package_group::build_index_directiveset()
  {
    this->build_set(this->directive_packages, this->index_directives, [](const auto& pkg) -> auto& { return pkg->get_index(); });
  }


void package_group::report_end_of_input()
  {
    // we report end-of-input to replacement rule packages, but (currently) not directive packages
    for(auto& pkg : this->rule_packages)
      {
        pkg->report_end_of_input();
      }
  }
