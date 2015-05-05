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

#include "datatable.h"
#include "bspline.h"
#include "pspline.h"

namespace transport
  {

    template <typename number>
    class spline1d
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! set up a 1d spline object with given data points
        spline1d(const std::vector<double>& x, const std::vector<number>& y);

        //! destructor is default
        ~spline1d() = default;

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

        //! pointer to spline object; its lifetime is managed by std::shared_ptr,
        //! which also deals correctly with taking copies.
        //! This is a raw BSpline, meaning that it goes through each of the data points.
        //! We use it to interpolate values.
        std::shared_ptr<Splinter::BSpline> b_spline;

        //! pointer to penalized-B spline object.
        //! This is smoothed, so it need not go through every data point.
        //! We use it to interpolate derivatives
        std::shared_ptr<Splinter::PSpline> p_spline;

        //! DataTable instance
        Splinter::DataTable table;

		    //! offset to apply on evaluation
		    number offset;

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

        // copy supplied data into Splinter DataTable
        for(unsigned int i = 0; i < N; ++i)
          {
            table.addSample(x[i], y[i]);

		        if(x[i] < min_x) min_x = x[i];
		        if(x[i] > max_x) max_x = x[i];
          }

        b_spline = std::make_shared<Splinter::BSpline>(table, Splinter::BSplineType::CUBIC_FREE);
        p_spline = std::make_shared<Splinter::PSpline>(table, 0.03);
      }


    template <typename number>
    number spline1d<number>::eval(double x)
      {
        Splinter::DenseVector xv(1);
        xv(0) = x;

        return( static_cast<number>(this->b_spline->eval(xv)) - this->offset );
      }


    template <typename number>
    number spline1d<number>::eval_diff(double x)
      {
        Splinter::DenseVector xv(1);
        xv(0) = x;

        return( static_cast<number>((this->p_spline->evalJacobian(xv))(0)) );
      }


  }   // namespace transport


#endif // __CPP_TRANSPORT_SPLINE_H_