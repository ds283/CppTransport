//
// Created by David Seery on 04/07/2013.
// Copyright (c) 2013 University of Sussex. All rights reserved.
//
// To change the template use AppCode | Preferences | File Templates.
//

#ifndef __plot_gadget_H_
#define __plot_gadget_H_

#include <iostream>
#include <fstream>

#include <vector>
#include <string>


namespace transport
  {

    template <typename number>
    class plot_gadget
      {
      public:
        plot_gadget(std::string f = "pdf") : format(f), min_set(false), max_set(false), min_x(0.0), max_x(0.0) {}
        plot_gadget(std::string f, double min, double max) : format(f), min_set(true), min_x(min < max ? min : max), max_set(true), max_x(max > min ? max : min) {}

        virtual void plot(std::string output, std::string title,
                          const std::vector<double>& x, const std::vector< std::vector<number> >& ys, const std::vector<std::string>& labels,
                          std::string xlabel, std::string ylabel, bool logx, bool logy) = 0;

        virtual void               set_format(std::string f);         // set current output format (PDF, PNG, ... ) if supported by the gadget

        virtual const std::string& get_format();                      // get current output format

        virtual bool               latex_labels() { return(false); }  // gadget supports or expects labels in LaTeX format?
        // by default false, but can be overridden by derived classes

        virtual bool               get_min_x(double& min);
        virtual bool               get_max_x(double& max);
        virtual void               set_min_x(double min);
        virtual void               set_max_x(double max);

      protected:
        virtual bool               is_allowed(double x);

        std::string format;

        bool min_set;
        bool max_set;

        double min_x;
        double max_x;
      };


// IMPLEMENTATION -- PLOT_GADGET


    template <typename number>
    void plot_gadget<number>::set_format(std::string f)
      {
        this->format = f;
      }

    template <typename number>
    const std::string& plot_gadget<number>::get_format()
      {
        return(this->format);
      }

    template <typename number>
    bool plot_gadget<number>::get_min_x(double& min)
      {
        if(this->min_set) min = this->min_x;
        return(this->min_set);
      }

    template <typename number>
    bool plot_gadget<number>::get_max_x(double& max)
      {
        if(this->max_set) max = this->max_x;
        return(this->max_set);
      }

    template <typename number>
    void plot_gadget<number>::set_min_x(double min)
      {
        this->min_x   = min;
        this->min_set = true;

        if(this->max_set)
          {
            if(this->max_x < min) this->max_x = min;
          }
      }

    template <typename number>
    void plot_gadget<number>::set_max_x(double max)
      {
        this->max_x   = max;
        this->max_set = true;

        if(this->min_set)
          {
            if(this->min_x > max) this->min_x = max;
          }
      }

    template <typename number>
    bool plot_gadget<number>::is_allowed(double x)
      {
        bool allowed = true;

        if(allowed && this->min_set)
          {
            if(x < this->min_x) allowed = false;
          }
        if(allowed && this->max_set)
          {
            if(x > this->max_x) allowed = false;
          }

        return(allowed);
      }


// OTHER MISCELLANEOUS FUNCTIONS


    inline void open_tempfile(std::string& path)
      {
        path += "/XXXXXX";
        std::vector<char> dst_path(path.begin(), path.end());
        dst_path.push_back('\0');

        int fd = mkstemp(&dst_path[0]);
        if(fd != -1)
          {
            path.assign(dst_path.begin(), dst_path.end() - 1);

            std::ofstream f;
            f.open(path.c_str(), std::ios_base::trunc | std::ios_base::out);
            f.close();
          }
      }

  }   // namespace transport


#endif //__plot_maker_H_
