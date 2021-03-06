//
// Created by David Seery on 13/08/2013.
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
//

#ifndef CPPTRANSPORT_SPLINE_H
#define CPPTRANSPORT_SPLINE_H


#include <vector>
#include <array>
#include <limits>
#include <type_traits>
#include <assert.h>

#include "SPLINTER/datatable.h"
#include "SPLINTER/bspline.h"
#include "SPLINTER/bsplinebuilder.h"


namespace transport
  {

    template <typename number>
    class spline1d
      {
        
        // TYPES
        
      public:
        
        //! spline y-values are 'number'
        using value_type = number;
        
        //! spline x-values are 'double'
        using domain_type = double;

        
        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! set up a 1d spline object with given data points
        spline1d(const std::vector<domain_type>& x, const std::vector<value_type>& y);

        //! destructor is default
        ~spline1d() = default;

        
		    // INTERFACE -- OFFSETS

      public:

		    //! get current offset
		    value_type get_offset() const { return(this->offset); }

		    //! set new offset
		    void set_offset(value_type o) { this->offset = o; }


        // INTERFACE -- EVALUATE VALUES

      public:

        //! evaluate the spline at a given value of x
        value_type eval(domain_type x);

        //! evaluate the spline at a given value of x
        value_type operator()(domain_type x) { return this->eval(x); }


        // INTERFACE -- EVALUATE DERIVATIVES

      public:

        //! evaluate the derivative of the spline at a given value of x
        value_type eval_diff(domain_type x);


		    // INTERFACE -- MISC DATA

      public:

		    //! get minimum x value
		    domain_type get_min_x() const { return(this->min_x); }

		    //! get maximum x value
		    domain_type get_max_x() const { return(this->max_x); }


        // WRITE TO STREAM

      public:

        //! write self to stream
        void write(std::ostream& out) const;


        // INTERNAL DATA


      protected:

        //! pointer to spline object; its lifetime is managed by std::shared_ptr,
        //! which also deals correctly with taking copies.
        //! This is a raw BSpline, meaning that it goes through each of the data points.
        //! We use it to interpolate values.
        std::shared_ptr<SPLINTER::BSpline> b_spline;

        //! pointer to penalized-B spline object.
        //! This is smoothed, so it need not go through every data point.
        //! We use it to interpolate derivatives
        std::shared_ptr<SPLINTER::BSpline> p_spline;

        //! DataTable instance
        SPLINTER::DataTable table;

		    //! offset to apply on evaluation
		    value_type offset;

		    //! number of points
		    size_t N;

		    //! minimum x value
		    domain_type min_x;

		    //! maximum x value
		    domain_type max_x;

      };


    template <typename number>
    spline1d<number>::spline1d(const std::vector<domain_type>& x, const std::vector<value_type>& y)
      : offset(0.0),
        min_x(std::numeric_limits<domain_type>::max()),
        max_x(-std::numeric_limits<domain_type>::max())
      {
        assert(x.size() == y.size());

        N  = x.size();

        // copy supplied data into Splinter DataTable
        for(unsigned int i = 0; i < N; ++i)
          {
            // y may need an explicit downcast to domain_type
            table.addSample(x[i], static_cast<domain_type>(y[i]));

		        if(x[i] < min_x) min_x = x[i];
		        if(x[i] > max_x) max_x = x[i];
          }

        try
	        {
            b_spline = std::make_shared<SPLINTER::BSpline>(SPLINTER::BSpline::Builder(table).degree(3).build());
            p_spline = std::make_shared<SPLINTER::BSpline>(SPLINTER::BSpline::Builder(table).degree(3).smoothing(SPLINTER::BSpline::Smoothing::PSPLINE).alpha(0.03).build());
	        }
				catch(SPLINTER::Exception& xe)
					{
						throw runtime_exception(exception_type::SPLINE_ERROR, xe.what());
					}
      }


    template <typename number>
    typename spline1d<number>::value_type spline1d<number>::eval(domain_type x)
      {
        SPLINTER::DenseVector xv(1);
        xv(0) = x;

        return( static_cast<value_type>(this->b_spline->eval(xv)) - this->offset );
      }


    template <typename number>
    typename spline1d<number>::value_type spline1d<number>::eval_diff(domain_type x)
      {
        SPLINTER::DenseVector xv(1);
        xv(0) = x;

        return( static_cast<value_type>((this->p_spline->evalJacobian(xv))(0)) );
      }


    template <typename number>
    void spline1d<number>::write(std::ostream& out) const
      {
        for(std::multiset<SPLINTER::DataPoint>::const_iterator t = this->table.cbegin(); t != this->table.cend(); ++t)
          {
            assert(t->getDimX() == 1);
            out << "x = " << t->getX()[0] << ", y = " << t->getY() << '\n';
          }
      }


    template <typename number, typename Char, typename Traits>
    std::basic_ostream<Char, Traits>& operator<<(std::basic_ostream<Char, Traits>& out, const spline1d<number>& obj)
      {
        obj.write(out);
        return(out);
      };


  }   // namespace transport


#endif // CPPTRANSPORT_SPLINE_H
