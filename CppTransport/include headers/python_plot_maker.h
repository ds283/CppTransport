//
// Created by David Seery on 04/07/2013.
// Copyright (c) 2013 University of Sussex. All rights reserved.
//
// To change the template use AppCode | Preferences | File Templates.
//



#ifndef __python_plot_maker_H_
#define __python_plot_maker_H_

#include <iostream>
#include <assert.h>

#include <stdio.h>
#include <math.h>

#include "plot_maker.h"


template <typename number>
class python_plot_maker : public plot_maker<number>
  {
    public:
      python_plot_maker(std::string i) : interpreter(i) {}

      void plot(std::string output, std::string title,
                const std::vector<number>& x, const std::vector< std::vector<number> >& ys, const std::vector<std::string>& labels,
                std::string xlabel, std::string ylabel, bool logx = true, bool logy = true);

    protected:
      std::string interpreter;
  };


template <typename number>
void python_plot_maker<number>::plot(std::string output, std::string title,
  const std::vector<number>& x, const std::vector< std::vector<number> >& ys,
  const std::vector<std::string>& labels, std::string xlabel, std::string ylabel, bool logx, bool logy)
  {
    std::string filename = "/tmp";
    open_tempfile(filename);

    std::ofstream out;
    out.open(filename.c_str(), std::ios_base::trunc | std::ios_base::out);
	 
	 // x and ys should be the same
	 assert(x.size() == ys.size());
	 
    out << "import matplotlib.pyplot as plt" << std::endl;
    out << "plt.figure()" << std::endl;

    if(logx) out << "plt.xscale('log')" << std::endl;
    if(logy) out << "plt.yscale('log')" << std::endl;

    out << "x = [ ";
    for(int i = 0; i < x.size(); i++)
      {
        out << (i > 0 ? ", " : "") << x[i];
      }
    out << " ]" << std::endl;
	 
	 // there should be labels.size() lines stored in ys
    for(int i = 0; i < labels.size(); i++)
      {
        out << "y" << i << " = [ ";

        for(int j = 0; j < ys.size(); j++)
          {
            assert(ys[j].size() == labels.size());

            out << (j > 0 ? ", " : "") << (logy ? fabs(ys[j][i]) : ys[j][i]);
          }
        out << " ]" << std::endl;

        out << "plt.errorbar(x, y" << i
            << ", label=r'" << labels[i] << "')" << std::endl;
      }

    out << "ax = plt.gca()" << std::endl;
    out << "handles, labels = ax.get_legend_handles_labels()" << std::endl;

    out << "y_labels = [ ";
    for(int i = 0; i < labels.size(); i++)
      {
        out << (i > 0 ? ", " : "") << "r'" << labels[i] << "'";
      }
    out << " ]" << std::endl;

    out << "plt.legend(handles, y_labels, frameon=False)" << std::endl;

    out << "plt.xlabel(r'"  << xlabel << "')" << std::endl;
    out << "plt.ylabel(r'"  << ylabel << "')" << std::endl;
    out << "plt.title(r'"   << title  << "')" << std::endl;
    out << "plt.savefig('"  << output << "')" << std::endl;
    out << "plt.close()"    << std::endl;

    out.close();

    system((this->interpreter + " " + filename).c_str());
    remove(filename.c_str());
  }


#endif //__python_plot_maker_H_
