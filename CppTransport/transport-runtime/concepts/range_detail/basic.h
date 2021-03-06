//
// Created by David Seery on 15/04/15.
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


#ifndef CPPTRANSPORT_RANGE_STEPPING_H
#define CPPTRANSPORT_RANGE_STEPPING_H


#include "transport-runtime/concepts/range_detail/common.h"
#include "transport-runtime/concepts/range_detail/abstract.h"


#define CPPTRANSPORT_NODE_MIN                 "min"
#define CPPTRANSPORT_NODE_MAX                 "max"
#define CPPTRANSPORT_NODE_STEPS               "steps"
#define CPPTRANSPORT_NODE_SPACING             "spacing"
#define CPPTRANSPORT_VALUE_LINEAR             "linear"
#define CPPTRANSPORT_VALUE_LOGARITHMIC_BOTTOM "logarithmic"
#define CPPTRANSPORT_VALUE_LOGARITHMIC_TOP    "logarithmic-top"


namespace transport
	{


    enum class spacing { linear, log_bottom, log_top };


    template <typename value=double>
    class basic_range: public range<value>
	    {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! Construct a range object with specified minimum & maximum values, number of steps and spacing type.
        basic_range(value mn, value mx, unsigned int st, spacing sp=spacing::linear);

        //! Deserialization constructor
        basic_range(Json::Value& reader);


        // INTERFACE

      public:

        //! Get minimum entry
        virtual value get_min()                      override       { return(this->min); }

        //! Get maximum entry
        virtual value get_max()                      override       { return(this->max); }

        //! Get number of steps
        virtual unsigned int get_steps()             override       { return(this->steps); }

        //! Get number of entries
        virtual unsigned int size()                  override       { return(this->grid.size()); }

        //! Get spacing type
        spacing get_spacing_type()                   const          { return(this->type); }

        //! Is a simple, linear range?
        virtual bool is_simple_linear()              const override { return(this->type == spacing::linear); }

        //! Get grid of entries
        virtual const std::vector<value>& get_grid() override       { return(this->grid); }

        value operator[](unsigned int d)             override;


        // POPULATE GRID

      protected:

        //! fill out the grid entries
        void populate_grid(void);

        //! fill out grid entries -- linear spacing
        void populate_linear_grid(void);

        //! fill out grid entries -- logarithmic bottom-spacing
        void populate_log_bottom_grid(void);

        //! fill out grid entries -- logarithmic top-spacing
        void populate_log_top_grid(void);


        // CLONE -- implements a 'range<>' interface

      public:

        virtual basic_range<value>* clone() const override { return new basic_range<value>(dynamic_cast<const basic_range<value>&>(*this)); }


        // SERIALIZATION INTERFACE -- implements a 'serializable' interface

      public:

        //! Serialize this object
        virtual void serialize(Json::Value& writer) const override;


        // WRITE TO A STREAM

      public:

        //! write self to stream
        template <typename Stream> void write(Stream& out);


        // INTERNAL DATA

      protected:

        //! Minimium value
        value min;

        //! Maximum value
        value max;

        //! Number of steps
        unsigned int steps;

        //! Spacing type
        spacing type;

        //! Grid of values
        std::vector<value> grid;

	    };


    template <typename value>
    basic_range<value>::basic_range(value mn, value mx, unsigned int st, spacing sp)
	    : min(mn), max(mx), steps(st), type(sp)
	    {
        this->populate_grid();
	    }


    template <typename value>
    basic_range<value>::basic_range(Json::Value& reader)
	    {
        double m = reader[CPPTRANSPORT_NODE_MIN].asDouble();
        min = static_cast<value>(m);

        m = reader[CPPTRANSPORT_NODE_MAX].asDouble();
        max = static_cast<value>(m);

        steps = reader[CPPTRANSPORT_NODE_STEPS].asUInt();

        std::string spc_string = reader[CPPTRANSPORT_NODE_SPACING].asString();

        if(spc_string == CPPTRANSPORT_VALUE_LINEAR)                  type = spacing::linear;
        else if(spc_string == CPPTRANSPORT_VALUE_LOGARITHMIC_BOTTOM) type = spacing::log_bottom;
        else if(spc_string == CPPTRANSPORT_VALUE_LOGARITHMIC_TOP)    type = spacing::log_top;
        else throw runtime_exception(exception_type::SERIALIZATION_ERROR, CPPTRANSPORT_BADLY_FORMED_RANGE);

        this->populate_grid();
	    }


    template <typename value>
    value basic_range<value>::operator[](unsigned int d)
	    {
        assert(d < this->grid.size());
        if(d < this->grid.size())
	        {
            return (this->grid[d]);
	        }
        else
	        {
            throw std::out_of_range(CPPTRANSPORT_RANGE_RANGE);
	        }
	    }


    template <typename value>
    void basic_range<value>::populate_grid(void)
	    {
        this->grid.clear();
        this->grid.reserve(this->steps+1);

        switch(this->type)
	        {
            case spacing::linear:
              this->populate_linear_grid();
	            break;

            case spacing::log_bottom:
	            this->populate_log_bottom_grid();
              break;

            case spacing::log_top:
              this->populate_log_top_grid();
              break;
	        }

        // sort grid into order and remove duplicates, eg. if user (accidentally) set top and bottom limits to be the same
        std::sort(this->grid.begin(), this->grid.end());
        auto last = std::unique(this->grid.begin(), this->grid.end(), aggregation_range_impl::DuplicateRemovalPredicate<value>(1E-10));
        this->grid.erase(last, this->grid.end());
	    }


    template <typename value>
    void basic_range<value>::populate_linear_grid(void)
      {
        if(this->steps == 0)
          {
            this->grid.push_back(min);
          }
        else
          {
            for(unsigned int i = 0; i <= this->steps; ++i)
              {
                value v = this->min + (static_cast<value>(i)/this->steps)*(this->max-this->min);
                if(!std::isnan(v)) this->grid.push_back(v);
              }
          }
      }


    template <typename value>
    void basic_range<value>::populate_log_bottom_grid(void)
      {
        if(this->steps == 0)
          {
            this->grid.push_back(min);
          }
        else
          {
            // if max and min are both positive, perform log-spacing as we expect
            if(this->max > 0.0 && this->min > 0.0)
              {
                for(unsigned int i = 0; i <= this->steps; ++i)
                  {
                    value v = this->min * static_cast<value>(std::pow(this->max/this->min, static_cast<double>(i)/this->steps));
                    if(!std::isnan(v)) this->grid.push_back(v);
                  }
              }
            else
              // otherwise, need to think of something else.
              // we shift the range to begin at 1, perform the log-spacing, and then
              // reverse the shift
              {
                double shifted_max = this->max - (this->min-1.0);
                for(unsigned int i = 0; i <= this->steps; ++i)
                  {
                    value v = this->min-1.0 + static_cast<value>(std::pow(shifted_max, static_cast<double>(i)/this->steps));
                    if(!std::isnan(v)) this->grid.push_back(v);
                  }
              }
          }
      }


    template <typename value>
    void basic_range<value>::populate_log_top_grid(void)
      {
        if(this->steps == 0)
          {
            this->grid.push_back(min);
          }
        else
          {
            if(this->max > 0.0 && this->min > 0.0)
              {
                for(unsigned int i = 0; i <= this->steps; ++i)
                  {
                    value v = this->max + this->min - this->min * static_cast<value>(std::pow(this->max/this->min, static_cast<double>(i)/this->steps));
                    if(!std::isnan(v)) this->grid.push_back(v);
                  }
              }
            else
              {
                double shifted_max = this->max - (this->min-1.0);
                for(unsigned int i = 0; i <= this->steps; ++i)
                  {
                    value v = this->max+1.0 - static_cast<value>(std::pow(shifted_max, static_cast<double>(i)/this->steps));
                    if(!std::isnan(v)) this->grid.push_back(v);
                  }
              }
            // the result is out-of-order, but it will be sorted below
          }
      }


    template <typename value>
    void basic_range<value>::serialize(Json::Value& writer) const
	    {
        writer[CPPTRANSPORT_NODE_RANGE_TYPE] = std::string(CPPTRANSPORT_NODE_RANGE_STEPPING);

        writer[CPPTRANSPORT_NODE_MIN]   = static_cast<double>(this->min);
        writer[CPPTRANSPORT_NODE_MAX]   = static_cast<double>(this->max);
        writer[CPPTRANSPORT_NODE_STEPS] = this->steps;

        switch(this->type)
	        {
            case spacing::linear:
	            writer[CPPTRANSPORT_NODE_SPACING] = std::string(CPPTRANSPORT_VALUE_LINEAR);
	            break;

            case spacing::log_bottom:
	            writer[CPPTRANSPORT_NODE_SPACING] = std::string(CPPTRANSPORT_VALUE_LOGARITHMIC_BOTTOM);
              break;

            case spacing::log_top:
	            writer[CPPTRANSPORT_NODE_SPACING] = std::string(CPPTRANSPORT_VALUE_LOGARITHMIC_TOP);
              break;
	        }
	    }


    template <typename value>
    template <typename Stream>
    void basic_range<value>::write(Stream& out)
      {
        out << CPPTRANSPORT_STEPPING_RANGE_A << this->get_steps() << CPPTRANSPORT_STEPPING_RANGE_B;

        spacing type = this->get_spacing_type();

        if(type == spacing::linear)                  out << CPPTRANSPORT_STEPPING_RANGE_LINEAR;
        else if(type == spacing::log_bottom) out << CPPTRANSPORT_STEPPING_RANGE_LOGARITHMIC_BOTTOM;
        else if(type == spacing::log_top)    out << CPPTRANSPORT_STEPPING_RANGE_LOGARITHMIC_TOP;

        out << CPPTRANSPORT_STEPPING_RANGE_C << this->get_min() << ", " << CPPTRANSPORT_STEPPING_RANGE_D << this->get_max() << '\n';

        const std::vector<value> grid = this->get_grid();

        out << CPPTRANSPORT_STEPPING_RANGE_E << '\n';
        for(unsigned int i = 0; i < grid.size(); ++i)
          {
            out << i << ". " << grid[i] << '\n';
          }
      }

    // can't make basic_range as const since some methods, eg. get_steps(), get_grid() are not marked const
    // (for the benefit of aggregate_range, where lazy evaluation of the grid means these methods
    // sometimes need to modify the internal state)
    template <typename value, typename Char, typename Traits>
    std::basic_ostream<Char, Traits>& operator<<(std::basic_ostream<Char, Traits>& out, basic_range<value>& obj)
	    {
        obj.write(out);
        return(out);
	    }


    // can't make basic_range as const since some methods, eg. get_steps(), get_grid() are not marked const
    // (for the benefit of aggregate_range, where lazy evaluation of the grid means these methods
    // sometimes need to modify the internal state)
    template <typename value, typename Char, typename Traits, typename Allocator>
    boost::log::basic_formatting_ostream<Char, Traits, Allocator>& operator<<(boost::log::basic_formatting_ostream<Char, Traits, Allocator>& out, basic_range<value>& obj)
      {
        obj.write(out);
        return(out);
      }

	}   // namespace transport


#endif //CPPTRANSPORT_RANGE_STEPPING_H
