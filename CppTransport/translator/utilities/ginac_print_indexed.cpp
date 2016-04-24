//
// Created by David Seery on 23/12/2015.
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

#include "ginac_print_indexed.h"


void ginac_print_indexed(const GiNaC::indexed& delta, const GiNaC::print_csrc& c, unsigned int level);


void ginac_print_indexed(const GiNaC::indexed& expr, const GiNaC::print_csrc& c, unsigned int level)
  {
    if(expr.nops() > 0)
      {
        // check whether this indexed expression can be converted to a Kronecker delta expression
        if(GiNaC::is_a<GiNaC::tensdelta>(expr.op(0)) && expr.nops() == 3)
          {
            GiNaC::ex idx1_ex = expr.op(1);
            GiNaC::ex idx2_ex = expr.op(2);

            GiNaC::idx idx1 = GiNaC::ex_to<GiNaC::idx>(idx1_ex);
            GiNaC::idx idx2 = GiNaC::ex_to<GiNaC::idx>(idx2_ex);

            c.s << "(";
            idx1.get_value().print(c, level);
            c.s << " == ";
            idx2.get_value().print(c, level);
            c.s << " ? 1 : 0)";
          }
        else    // revert to default GiNaC behaviour
          {
            expr.op(0).print(c, level);

            GiNaC::exvector idxs = expr.get_indices();
            for(GiNaC::exvector::const_iterator t = idxs.begin(); t != idxs.end(); ++t)
              {
                t->print(c, level);
              }
          }
      }
  }


void set_up_ginac_printing()
  {
    GiNaC::set_print_func<GiNaC::indexed, GiNaC::print_csrc>(ginac_print_indexed);
  }
