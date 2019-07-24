//
// Created by David Seery on 14/03/2016.
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

#ifndef CPPTRANSPORT_PLOT_ENVIRONMENT_H
#define CPPTRANSPORT_PLOT_ENVIRONMENT_H


#include <fstream>

#include "transport-runtime/manager/environment.h"
#include "transport-runtime/manager/argument_cache.h"


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

        matplotlib_backend backend = this->args.get_matplotlib_backend();

        switch(backend)
          {
            case matplotlib_backend::unset:
              break;

            case matplotlib_backend::Agg:
              {
                outf << "import matplotlib as mpl" << '\n';
                outf << "mpl.use('Agg')" << '\n';
                break;
              }

            case matplotlib_backend::Cairo:
              {
                outf << "import matplotlib as mpl" << '\n';
                outf << "mpl.use('Cairo')" << '\n';
                break;
              }

            case matplotlib_backend::MacOSX:
              {
                outf << "import matplotlib as mpl" << '\n';
                outf << "mpl.use('MacOSX')" << '\n';
                break;
              }

            case matplotlib_backend::PDF:
              {
                outf << "import matplotlib as mpl" << '\n';
                outf << "mpl.use('PDF')" << '\n';
                break;
              }
          }

        plot_style style = this->args.get_plot_environment();

        outf << "import matplotlib.pyplot as plt" << '\n';
        outf << "import matplotlib.colors as col" << '\n';

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
                else
                  {
                    outf << "# matplotlib 'ggplot' style requested, but omitted since not detected in environment" << '\n';
                  }
                break;
              }

            case plot_style::matplotlib_ticks:
              {
                if(env.has_matplotlib_style_sheets())
                  {
                    outf << "plt.style.use('ticks')" << '\n';
                  }
                else
                  {
                    outf << "# matplotlib 'ticks' style requested, but omitted since not detected in environment" << '\n';
                  }
                break;
              }

            case plot_style::seaborn:
              {
                if(env.has_seaborn())
                  {
                    outf << "from pandas.plotting import register_matplotlib_converters" << '\n';
                    outf << "register_matplotlib_converters()" << '\n';
                    outf << "import seaborn as sns" << '\n';
                    outf << "sns.set()" << '\n';
                  }
                else
                  {
                    outf << "# seaborn plot style requested, but omitted since not detected in environment" << '\n';
                  }
                break;
              }
          }
      }


  }   // namespace transport


#endif //CPPTRANSPORT_PLOT_ENVIRONMENT_H
