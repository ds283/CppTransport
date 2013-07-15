//
// Created by David Seery on 04/07/2013.
// Copyright (c) 2013 University of Sussex. All rights reserved.
//
// To change the template use AppCode | Preferences | File Templates.
//



#ifndef __gnuplot_plot_maker_H_
#define __gnuplot_plot_maker_H_

#include <iostream>
#include <assert.h>

#include <stdio.h>
#include <math.h>

#include "gnuplot_i.hpp"

#include "plot_maker.h"


template <typename number>
class gnuplot_plot_maker : public plot_maker<number>
  {
    public:
      void plot(std::string output, std::string title,
        const std::vector<number>& x, const std::vector< std::vector<number> >& ys, const std::vector<std::string>& labels,
        std::string xlabel, std::string ylabel, bool logx = true, bool logy = true);
  };


template <typename number>
void gnuplot_plot_maker<number>::plot(std::string output, std::string title,
  const std::vector<number>& x, const std::vector< std::vector<number> >& ys,
  const std::vector<std::string>& labels, std::string xlabel, std::string ylabel, bool logx, bool logy)
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

      }
    catch (GnuplotException ge)
      {
        std::cerr << ge.what() << std::endl;
      }

  }


#endif //__python_plot_maker_H_
