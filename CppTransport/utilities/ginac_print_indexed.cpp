//
// Created by David Seery on 23/12/2015.
// Copyright (c) 2015 University of Sussex. All rights reserved.
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
