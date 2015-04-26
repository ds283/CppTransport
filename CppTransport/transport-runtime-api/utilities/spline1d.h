//
// Created by David Seery on 13/08/2013.
// Copyright (c) 2013-15 University of Sussex. All rights reserved.
//
//

#ifndef __CPP_TRANSPORT_SPLINE_H_
#define __CPP_TRANSPORT_SPLINE_H_


#include <vector>
#include <array>
#include <limits>

#include <assert.h>

#include "gsl/gsl_spline.h"


namespace transport
  {

    template <typename number>
    class spline1d
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! set up a 1d spline object with given data points
        spline1d(const std::vector<double>& x, const std::vector<number>& y);

		    //! prevent copying
		    spline1d(const spline1d<number>& obj);

        //! destroy object, releasing
        ~spline1d();


		    // INTERFACE -- OFFSETS

      public:

		    //! get current offset
		    number get_offset() const { return(this->offset); }

		    //! set new offset
		    void set_offset(number o) { this->offset = o; }


        // INTERFACE -- EVALUATE VALUES

      public:

        //! evaluate the spline at a given value of x
        number eval(double x);

        //! evaluate the spline at a given value of x
        number operator()(double x) { return(this->eval(x)); }


        // INTERFACE -- EVALUATE DERIVATIVES

      public:

        //! evaluate the derivative of the spline at a given value of x
        number eval_diff(double x);


		    // INTERFACE -- MISC DATA

      public:

		    //! get minimum x value
		    double get_min_x() const { return(this->min_x); }

		    //! get maximum x value
		    double get_max_x() const { return(this->max_x); }


        // INTERNAL DATA


      protected:

        //! pointer to GSL spline object
        gsl_spline*       spline;

        //! pointer to GSL accelerator cache
        gsl_interp_accel* accel;

		    //! offset to apply on evaluation
		    number offset;

		    //! pointer to array of xs
		    double* xs;

		    //! pointer to array of ys;
		    double* ys;

		    //! number of points
		    unsigned int N;

		    //! minimum x value
		    double min_x;

		    //! maximum x value
		    double max_x;

      };


    template <typename number>
    spline1d<number>::spline1d(const std::vector<double>& x, const std::vector<number>& y)
      : offset(0.0),
        min_x(std::numeric_limits<double>::max()),
        max_x(-std::numeric_limits<double>::max())
      {
        assert(x.size() == y.size());

        N  = x.size();

        spline = gsl_spline_alloc(gsl_interp_cspline, N);
        accel  = gsl_interp_accel_alloc();

        // allocate new arrays for x and y values
        xs = new double[N];
        ys = new double[N];

        // copy supplied data into our new arrays
        for(unsigned int i = 0; i < N; ++i)
          {
            // assume 'number' is explicitly castable to 'double'
            xs[i] = static_cast<double>(x[i]);
            ys[i] = (double)y[i];

		        if(xs[i] < min_x) min_x = xs[i];
		        if(xs[i] > max_x) max_x = xs[i];
          }

        // build GSL spline object and cache it
        gsl_spline_init(spline, xs, ys, N);
      }


		template <typename number>
		spline1d<number>::spline1d(const spline1d<number>& obj)
			: offset(obj.offset),
			  min_x(obj.min_x),
			  max_x(obj.max_x),
				N(obj.N)
			{
		    spline = gsl_spline_alloc(gsl_interp_cspline, N);
		    accel  = gsl_interp_accel_alloc();

				xs = new double[N];
				ys = new double[N];

				for(unsigned int i = 0; i < N; i++)
					{
						xs[i] = obj.xs[i];
						ys[i] = obj.ys[i];
					}

				gsl_spline_init(spline, xs, ys, N);
			}


    template <typename number>
    spline1d<number>::~spline1d()
      {
        gsl_spline_free(this->spline);
        gsl_interp_accel_free(this->accel);

        delete xs;
        delete ys;
      }


    template <typename number>
    number spline1d<number>::eval(double x)
      {
        return( static_cast<number>(gsl_spline_eval(this->spline, x, this->accel)) - this->offset );
      }


    template <typename number>
    number spline1d<number>::eval_diff(double x)
      {
        return( static_cast<number>(gsl_spline_eval_deriv(this->spline, x, this->accel)) );
      }


  }   // namespace transport


#endif // __CPP_TRANSPORT_SPLINE_H_