//
// Created by David Seery on 04/07/2013.
// Copyright (c) 2013 University of Sussex. All rights reserved.
//
// To change the template use AppCode | Preferences | File Templates.
//



#ifndef __python_plot_gadget_H_
#define __python_plot_gadget_H_

#include <iostream>
#include <assert.h>

#include <stdio.h>
#include <math.h>

#include "plot_gadget.h"


template <typename number>
class python_plot_gadget : public plot_gadget<number>
  {
    public:
      python_plot_gadget(std::string i, std::string f = "pdf") : plot_gadget<number>(f), interpreter(i) {}

      void plot(std::string output, std::string title,
                const std::vector<number>& x, const std::vector< std::vector<number> >& ys, const std::vector<std::string>& labels,
                std::string xlabel, std::string ylabel, bool logx = true, bool logy = true);

      bool latex_labels() { return(true); }

    protected:
      std::string interpreter;
  };


template <typename number>
void python_plot_gadget<number>::plot(std::string output, std::string title,
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
    out << "plt.savefig('"  << output;
    if(this->format != "")
      {
        out << "." << this->format;
      }
    out << "')" << std::endl;
    out << "plt.close()"    << std::endl;

    out.close();

    system((this->interpreter + " " + filename).c_str());
    remove(filename.c_str());
  }


#endif // __python_plot_gadget_H_
