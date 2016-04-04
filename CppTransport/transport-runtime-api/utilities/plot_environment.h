//
// Created by David Seery on 14/03/2016.
// Copyright (c) 2016 University of Sussex. All rights reserved.
//

#ifndef CPPTRANSPORT_PLOT_ENVIRONMENT_H
#define CPPTRANSPORT_PLOT_ENVIRONMENT_H


#include <fstream>

#include "transport-runtime-api/manager/environment.h"
#include "transport-runtime-api/manager/argument_cache.h"


namespace transport
  {

    class plot_environment
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! constructor
        plot_environment(local_environment& e, argument_cache& a)
          : env(e),
            args(a)
          {
          }

        //! destructor is default
        ~plot_environment() = default;


        // INTERFACE

      public:

        //! write plot environment
        void write_environment(std::ofstream& outf);


        // INTERNAL DATA

      private:

        //! local environment
        local_environment& env;

        //! argument cache
        argument_cache& args;

      };


    void plot_environment::write_environment(std::ofstream& outf)
      {
        if(!env.has_python()) return;
        if(!env.has_matplotlib()) return;

        plot_style style = this->args.get_plot_environment();

        outf << "import matplotlib.pyplot as plt" << '\n';

        switch(style)
          {
            case plot_style::raw_matplotlib:
              break;

            case plot_style::matplotlib_ggplot:
              {
                if(env.has_matplotlib_style_sheets())
                  {
                    outf << "plt.style.use('ggplot')" << '\n';
                  }
                break;
              }

            case plot_style::matplotlib_ticks:
              {
                if(env.has_matplotlib_style_sheets())
                  {
                    outf << "plt.style.use('ticks')" << '\n';
                  }
                break;
              }

            case plot_style::seaborn:
              {
                if(env.has_seaborn())
                  {
                    outf << "import seaborn as sns" << '\n';
                  }
                break;
              }
          }
      }


  }   // namespace transport


#endif //CPPTRANSPORT_PLOT_ENVIRONMENT_H
