//
// Created by David Seery on 13/08/2013.
// Copyright (c) 2013 University of Sussex. All rights reserved.
//
// To change the template use AppCode | Preferences | File Templates.
//

#ifndef __CPP_TRANSPORT_SPLINE_H_
#define __CPP_TRANSPORT_SPLINE_H_


#include <vector>
#include <array>

#include "gsl/gsl_spline.h"


namespace transport
  {

    template <typename number>
    class spline1d
      {
      public:
        spline1d(const std::vector<double>& x, const std::vector<number>& y);
        ~spline1d();

        number eval(double x);

      protected:
        gsl_spline*       spline;
        gsl_interp_accel* accel;
      };


    template <typename number>
    spline1d<number>::spline1d(const std::vector<double>& x, const std::vector<number>& y)
      {
        assert(x.size() == y.size());

        spline = gsl_spline_alloc(gsl_interp_cspline, x.size());
        accel  = gsl_interp_accel_alloc();

        double* xs = new double[x.size()];
        double* ys = new double[x.size()];

        for(int i = 0; i < x.size(); i++)
          {
            // assume 'number' is explicitly castable to 'double'
            xs[i] = x[i];
            ys[i] = (double)y[i];
          }

        gsl_spline_init(spline, xs, ys, x.size());
      }


    template <typename number>
    spline1d<number>::~spline1d()
      {
        gsl_spline_free(this->spline);
        gsl_interp_accel_free(this->accel);
      }


    template <typename number>
    number spline1d<number>::eval(double x)
      {
        return((number)gsl_spline_eval(this->spline, x, this->accel));
      }


  }   // namespace transport


#endif // __CPP_TRANSPORT_SPLINE_H_