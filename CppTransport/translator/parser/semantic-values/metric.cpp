//
// Created by David Seery on 30/05/2017.
// --@@
// Copyright (c) 2017 University of Sussex. All rights reserved.
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


#include "metric.h"


field_metric::field_metric()
  : prefactor(boost::none)
  {
  }


field_metric& field_metric::multiply(const GiNaC::ex& f)
  {
    if(this->prefactor) this->prefactor = *this->prefactor * f;
    else this->prefactor = f;
    
    return *this;
  }


field_metric& field_metric::set_prefactor(const GiNaC::ex& expr)
  {
    this->prefactor = expr;
    return *this;
  }


field_metric_base::field_metric_base()
  : field_metric()
  {
  }


GiNaC::ex field_metric_base::operator()(field_metric::index_type idx) const
  {
    auto canonical_idx = canonicalize(idx);
    
    // search for an entry corresponding to this index combination
    auto t = this->components.find(canonical_idx);

    // if none, value is zero
    if(t == this->components.end()) return GiNaC::ex(0);
    
    // otherwise, return prefactor multiplied by value
    if(this->prefactor) return *this->prefactor * *t->second;
    return *t->second;
  }


field_metric_base& field_metric_base::set_component(field_metric::index_type idx, const GiNaC::ex& expr,
                                                    const y::lexeme_type& l)
  {
    using emplace_result = std::pair< component_database::iterator, bool >;
    
    auto canonical_idx = canonicalize(idx);
    
    // attempt to emplace value
    // will fail if a value has already been assigned
    emplace_result res = this->components.emplace(std::piecewise_construct,
                                                  std::forward_as_tuple(canonical_idx),
                                                  std::forward_as_tuple(expr, l.get_error_context()));
    
    // return normally if the value was actually emplaced
    if(res.second) return *this;
    
    component_database::const_iterator t = this->components.find(canonical_idx);
    if(t == this->components.end())
      {
        l.error(ERROR_COMPONENT_DATABASE_INCONSISTENT);
        throw parse_error(ERROR_COMPONENT_DATABASE_INCONSISTENT);
      }
    
    std::ostringstream msg;
    msg << ERROR_COMPONENT_REDEFINITION << " (" << idx.first << "," << idx.second << ")";
    l.get_error_context().error(msg.str());
    
    t->second.get_declaration_point().warn(NOTIFY_DUPLICATION_DEFINITION_WAS);
    throw parse_error(msg.str());
  }


field_metric_binop::field_metric_binop(field_metric_binop::op O_, std::shared_ptr<field_metric> A_, std::shared_ptr<field_metric> B_)
  : field_metric(),
    O(O_),
    A(A_),
    B(B_)
  {
  }


GiNaC::ex field_metric_binop::operator()(field_metric::index_type idx) const
  {
    GiNaC::ex e;
    switch(this->O)
      {
        case op::add: e = (*A)(idx) + (*B)(idx); break;
        case op::sub: e = (*A)(idx) - (*B)(idx); break;
      }
    
    if(this->prefactor) return *this->prefactor * e;
    return e;
  }
