//
// Created by David Seery on 17/07/2013.
// Copyright (c) 2013 University of Sussex. All rights reserved.
//
// To change the template use AppCode | Preferences | File Templates.
//



#ifndef __asciitable_plot_gadget_H_
#define __asciitable_plot_gadget_H_

#include <iostream>
#include <assert.h>

#include <stdio.h>
#include <math.h>

#include "gnuplot_i.hpp"

#include "plot_gadget.h"
#include "messages_en.h"
#include "asciitable.h"


template <typename number>
class asciitable_plot_gadget : public plot_gadget<number>
  {
    public:
      asciitable_plot_gadget() : plot_gadget<number>("") {}

      void plot(std::string output, std::string title,
        const std::vector<number>& x, const std::vector< std::vector<number> >& ys, const std::vector<std::string>& labels,
        std::string xlabel, std::string ylabel, bool logx = true, bool logy = true);

      virtual void set_format(std::string f);
  };


template <typename number>
void asciitable_plot_gadget<number>::set_format(std::string f)
  {
    // do nothing; we can only output .ps format
  }


template <typename number>
void asciitable_plot_gadget<number>::plot(std::string output, std::string title,
  const std::vector<number>& x, const std::vector< std::vector<number> >& ys,
  const std::vector<std::string>& labels, std::string xlabel, std::string ylabel, bool logx, bool logy)
  {
    // x and ys should be the same size
    assert(x.size() == ys.size());

    std::ofstream out;
    out.open(output + ".txt");
    if(out.is_open())
      {
        transport::asciitable<number> writer(out);

        writer.write("N", labels, x, ys);
        // currently do nothing with xlabel, ylabel, title, logx, logy
      }
    else
      {
        std::cout << __CPP_TRANSPORT_ASCIITABLE_OUTPUT << " '" << output << "'" << std::endl;
      }
  }


#endif //__asciitable_plot_gadget_H_
