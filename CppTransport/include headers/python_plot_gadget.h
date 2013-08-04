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

#define DEFAULT_SCRIPT_OUTPUT (false)
#define DEFAULT_USE_LATEX     (false)


template <typename number>
bool check_log_scale(const std::vector<number>& x, bool strict)
  {
    bool ok = false;

    if(strict)
      {
        ok = true;
        for(int i = 0; i < x.size(); i++)
          {
            if(x[i] <= 0.0)
              {
                ok = false;   // require all values to be > 0
                break;
              }
          }
      }
    else
      {
        ok = false;
        for(int i = 0; i < x.size(); i++)
          {
            if(x[i] > 0.0)
              {
                ok = true;  // require at least one nonzero value
                break;
              }
          }
      }

    return(ok);
  }


template <typename number>
class python_plot_gadget : public plot_gadget<number>
  {
    public:
      python_plot_gadget(std::string i, std::string f = "pdf")
        : plot_gadget<number>(f), interpreter(i), script_output(DEFAULT_SCRIPT_OUTPUT), use_latex(DEFAULT_USE_LATEX) {}

      void plot(std::string output, std::string title,
                const std::vector<number>& x, const std::vector< std::vector<number> >& ys, const std::vector<std::string>& labels,
                std::string xlabel, std::string ylabel, bool logx = true, bool logy = true);

      // we want LaTeX labels to be provided
      bool latex_labels()             { return(true); }

      // the output can be a python script, if desired, for further editing
      // if so, set this option to 'true'
      bool get_script_output()        { return(this->script_output); }
      void set_script_output(bool s)  { this->script_output = s; }

      // control whether to use LaTeX to typeset all labels
      bool get_use_latex()            { return(this->use_latex); }
      void set_use_latex(bool s)      { this->use_latex = s; }

    protected:
      std::string interpreter;

      bool        script_output;
      bool        use_latex;
  };


template <typename number>
void python_plot_gadget<number>::plot(std::string output, std::string title,
  const std::vector<number>& x, const std::vector< std::vector<number> >& ys,
  const std::vector<std::string>& labels, std::string xlabel, std::string ylabel, bool logx, bool logy)
  {
    bool ok = true;
    if(logx)
      {
        if((ok = check_log_scale(x, true)) == false)
          {
            std::cout << __CPP_TRANSPORT_LOGX_ERROR_A << " '" << title << "' (" << output << "); "
                      << __CPP_TRANSPORT_LOGX_ERROR_B << std::endl;
          }
      }

    if(ok)
      {
        std::string filename = "/tmp";

        if(script_output)
          {
            filename = output + ".py";
          }
        else
          {
            open_tempfile(filename);
          }

        std::ofstream out;
        out.open(filename.c_str(), std::ios_base::trunc | std::ios_base::out);

        // x and ys should be the same
        assert(x.size() == ys.size());

        out << "import matplotlib.pyplot as plt" << std::endl;

        if(this->use_latex)
          {
            out << "plt.rc('text', usetex=True)" << std::endl;
          }

        out << "plt.figure()" << std::endl;

        if(logx) out << "plt.xscale('log')" << std::endl;
        if(logy) out << "plt.yscale('log')" << std::endl;

        out << "x = [ ";
        for(int i = 0; i < x.size(); i++)
          {
            out << (i > 0 ? ", " : "") << x[i];
          }
        out << " ]" << std::endl;

        std::vector<bool> plotted(labels.size());

        // there should be labels.size() lines stored in ys
        for(int i = 0; i < labels.size(); i++)
          {
            bool line_ok = true;
            std::vector<number> y_line(ys.size());

            for(int j = 0; j < ys.size(); j++)
              {
                y_line[j] = (logy ? fabs(ys[j][i]) : ys[j][i]);
              }

            if(logy)
              {
                if((line_ok = check_log_scale(y_line, false)) == false)
                  {
                    std::cout << __CPP_TRANSPORT_LOGY_ERROR_A << " '" << labels[i] << "' (" << output << "); "
                              << __CPP_TRANSPORT_LOGY_ERROR_B << std::endl;
                  }
              }

            if(line_ok)
              {
                out << "y" << i << " = [ ";

                for(int j = 0; j < ys.size(); j++)
                  {
                    assert(ys[j].size() == labels.size());

                    out << (j > 0 ? ", " : "") << y_line[j];
                  }
                out << " ]" << std::endl;

                out << "plt.errorbar(x, y" << i
                  << ", label=r'" << labels[i] << "')" << std::endl;

                plotted[i] = true;
              }
            else
              {
                plotted[i] = false;
              }
          }

        out << "ax = plt.gca()" << std::endl;
        out << "handles, labels = ax.get_legend_handles_labels()" << std::endl;

        out << "y_labels = [ ";
        for(int i = 0; i < labels.size(); i++)
          {
            if(plotted[i]) out << (i > 0 ? ", " : "") << "r'" << labels[i] << "'";
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

        if(!script_output)
          {
            system(("source ~/.profile; " + this->interpreter + " " + filename).c_str());
            remove(filename.c_str());
          }
      }
  }


#endif // __python_plot_gadget_H_
