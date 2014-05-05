//
// Created by David Seery on 04/07/2013.
// Copyright (c) 2013 University of Sussex. All rights reserved.
//
// To change the template use AppCode | Preferences | File Templates.
//



#ifndef __gnuplot_plot_gadget_H_
#define __gnuplot_plot_gadget_H_

#include <iostream>
#include <assert.h>

#include <stdio.h>
#include <math.h>

#include "gnuplot_i.hpp"

#include "transport/plotgadgets/plot_gadget.h"
#include "transport/messages.h"


namespace transport
  {

    template <typename number>
    class gnuplot_plot_gadget : public plot_gadget<number>
      {
      public:
        gnuplot_plot_gadget() : plot_gadget<number>("") {}

        void plot(std::string output, std::string title,
                  const std::vector<number>& x, const std::vector< std::vector<number> >& ys, const std::vector<std::string>& labels,
                  std::string xlabel, std::string ylabel, bool logx = true, bool logy = true, const std::string tag = "");

        virtual void set_format(std::string f);
      };


    template <typename number>
    void gnuplot_plot_gadget<number>::set_format(std::string f)
      {
        // do nothing; we can only output .ps format
      }


    template <typename number>
    void gnuplot_plot_gadget<number>::plot(std::string output, std::string title,
                                           const std::vector<number>& x, const std::vector< std::vector<number> >& ys,
                                           const std::vector<std::string>& labels, std::string xlabel, std::string ylabel,
                                            bool logx, bool logy, const std::string tag)
      {
        // x and ys should be the same size
        assert(x.size() == ys.size());

        try
          {
            Gnuplot gplot;

            if(logx) gplot.set_xlogscale();
            if(logy) gplot.set_ylogscale();

            gplot.set_legend("inside right top");

            gplot.set_xlabel(xlabel);
            gplot.set_ylabel(ylabel);
            gplot.set_title(title);

            for(int i = 0; i < labels.size(); i++)
              {
                std::vector<number> y(ys.size());

                for(int j = 0; j < ys.size(); j++)
                  {
                    assert(ys[j].size() == labels.size());

                    y[j] = (logy ? fabs(ys[j][i]) : ys[j][i]);
                  }

                gplot.set_smooth().plot_xy(x, y, labels[i]);
              }

            if(this->format != "")  // but this should never really happen; we can only handle .ps format
              {
                output += "." + this->format;
              }
//        gplot.savetops(output);
          }
        catch (GnuplotException ge)
          {
            std::cerr << ge.what() << std::endl;
          }
      }

  }   // namespace transport


#endif // __gnuplot_plot_gadget_H_
