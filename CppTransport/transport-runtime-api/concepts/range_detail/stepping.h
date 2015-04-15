//
// Created by David Seery on 15/04/15.
// Copyright (c) 2015 University of Sussex. All rights reserved.
//


#ifndef __range_stepping_H_
#define __range_stepping_H_


#include "transport-runtime-api/concepts/range_detail/common.h"
#include "transport-runtime-api/concepts/range_detail/abstract.h"


#define __CPP_TRANSPORT_NODE_MIN                 "min"
#define __CPP_TRANSPORT_NODE_MAX                 "max"
#define __CPP_TRANSPORT_NODE_STEPS               "steps"
#define __CPP_TRANSPORT_NODE_SPACING             "spacing"
#define __CPP_TRANSPORT_VALUE_LINEAR             "linear"
#define __CPP_TRANSPORT_VALUE_LOGARITHMIC_BOTTOM "logarithmic"
#define __CPP_TRANSPORT_VALUE_LOGARITHMIC_TOP    "logarithmic-top"


namespace transport
	{

    template <typename value> class stepping_range;

    template <typename value>
    std::ostream& operator<<(std::ostream& out, const stepping_range<value>& obj);

    typedef enum { linear_stepping, logarithmic_bottom_stepping, logarithmic_top_stepping } range_spacing_type;


    template <typename value>
    class stepping_range: public range<value>
	    {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! Construct a range object with specified minimum & maximum values, number of steps and spacing type.
        stepping_range(value mn, value mx, unsigned int st, range_spacing_type sp= linear_stepping);

        //! Deserialization constructor
        stepping_range(Json::Value& reader);


        // INTERFACE

      public:

        //! Get minimum entry
        virtual value get_min()                      const override { return(this->min); }

        //! Get maximum entry
        virtual value get_max()                      const override { return(this->max); }

        //! Get number of steps
        virtual unsigned int get_steps()             const override { return(this->steps); }

        //! Get number of entries
        virtual unsigned int size()                  const override { return(this->grid.size()); }

        //! Is a simple, linear range?
        virtual bool is_simple_linear()              const override { return(this->spacing == linear_stepping); }

        //! Get grid of entries
        virtual const std::vector<value>& get_grid() const override { return(this->grid); }

        value operator[](unsigned int d)             const override;


        // POPULATE GRID

      protected:

        //! fill out the grid entries
        void populate_grid(void);


        // CLONE -- implements a 'range<>' interface

      public:

        virtual stepping_range<value>* clone() const override { return new stepping_range<value>(dynamic_cast<const stepping_range<value>&>(*this)); }


        // SERIALIZATION INTERFACE -- implements a 'serializable' interface

      public:

        //! Serialize this object
        virtual void serialize(Json::Value& writer) const override;

        friend std::ostream& operator<< <>(std::ostream& out, const stepping_range<value>& obj);


        // INTERNAL DATA

      protected:

        //! Minimium value
        value min;

        //! Maximum value
        value max;

        //! Number of steps
        unsigned int steps;

        //! Spacing type
        range_spacing_type spacing;

        //! Grid of values
        std::vector<value> grid;

	    };


    template <typename value>
    stepping_range<value>::stepping_range(value mn, value mx, unsigned int st, range_spacing_type sp)
	    : min(mn), max(mx), steps(st), spacing(sp)
	    {
        this->populate_grid();
	    }


    template <typename value>
    stepping_range<value>::stepping_range(Json::Value& reader)
	    {
        double m = reader[__CPP_TRANSPORT_NODE_MIN].asDouble();
        min = static_cast<value>(m);

        m = reader[__CPP_TRANSPORT_NODE_MAX].asDouble();
        max = static_cast<value>(m);

        steps = reader[__CPP_TRANSPORT_NODE_STEPS].asUInt();

        std::string spc_string = reader[__CPP_TRANSPORT_NODE_SPACING].asString();

        if(spc_string == __CPP_TRANSPORT_VALUE_LINEAR)                  spacing = linear_stepping;
        else if(spc_string == __CPP_TRANSPORT_VALUE_LOGARITHMIC_BOTTOM) spacing = logarithmic_bottom_stepping;
        else if(spc_string == __CPP_TRANSPORT_VALUE_LOGARITHMIC_TOP)    spacing = logarithmic_top_stepping;
        else throw runtime_exception(runtime_exception::SERIALIZATION_ERROR, __CPP_TRANSPORT_BADLY_FORMED_RANGE);

        this->populate_grid();
	    }


    template <typename value>
    value stepping_range<value>::operator[](unsigned int d) const
	    {
        assert(d < this->grid.size());
        if(d < this->grid.size())
	        {
            return (this->grid[d]);
	        }
        else
	        {
            throw std::out_of_range(__CPP_TRANSPORT_RANGE_RANGE);
	        }
	    }


    template <typename value>
    void stepping_range<value>::populate_grid(void)
	    {
        this->grid.clear();
        this->grid.reserve(this->steps+1);

        switch(this->spacing)
	        {
            case linear_stepping:
	            {
                for(unsigned int i = 0; i <= this->steps; i++)
	                {
                    this->grid.push_back(min + (static_cast<double>(i)/this->steps)*(this->max-this->min));
	                }
                break;
	            }

            // the logarithmically-spaced interval isn't translation-invariant
            // (logarithmic spacing between 10 and 20 isn't the same as logarithmic
            // spacing between 110 and 120)
            // to standardize, and also to allow log-spacing for ranges that contain 0,
            // we shift the range to begin at 1, perform the log-spacing, and then
            // reverse the shift

            // logarithmic-bottom is log-spaced at the bottom end of the interval

            case logarithmic_bottom_stepping:
	            {
                double shifted_max = this->max - (this->min-1.0);
                for(unsigned int i = 0; i <= this->steps; i++)
	                {
                    grid.push_back(this->min-1.0 + static_cast<value>(pow(shifted_max, static_cast<double>(i)/this->steps)));
	                }
                break;
	            }

            // logarithmic-top is log-spaced at the top end of the interval

            case logarithmic_top_stepping:
	            {
                double shifted_max = this->max - (this->min-1.0);
                for(unsigned int i = 0; i <= this->steps; i++)
	                {
                    grid.push_back(shifted_max - static_cast<value>(pow(shifted_max, static_cast<double>(i)/this->steps)));
	                }
                // the result is out-of-order, but it will be sorted below
                break;
	            };

            default:
	            {
                assert(false);
                throw runtime_exception(runtime_exception::RUNTIME_ERROR, __CPP_TRANSPORT_RANGE_INVALID_SPACING);
	            }
	        }

        struct GridSorter
	        {
            bool operator()(const value& a, const value& b)
	            {
                return(a < b);
	            }
	        };

        std::sort(this->grid.begin(), this->grid.end(), GridSorter());
	    }

    template <typename value>
    void stepping_range<value>::serialize(Json::Value& writer) const
	    {
        writer[__CPP_TRANSPORT_NODE_RANGE_TYPE] = std::string(__CPP_TRANSPORT_NODE_RANGE_STEPPING);

        writer[__CPP_TRANSPORT_NODE_MIN]   = static_cast<double>(this->min);
        writer[__CPP_TRANSPORT_NODE_MAX]   = static_cast<double>(this->max);
        writer[__CPP_TRANSPORT_NODE_STEPS] = this->steps;

        switch(this->spacing)
	        {
            case linear_stepping:
	            writer[__CPP_TRANSPORT_NODE_SPACING] = std::string(__CPP_TRANSPORT_VALUE_LINEAR);
	            break;

            case logarithmic_bottom_stepping:
	            writer[__CPP_TRANSPORT_NODE_SPACING] = std::string(__CPP_TRANSPORT_VALUE_LOGARITHMIC_BOTTOM);
              break;

            case logarithmic_top_stepping:
	            writer[__CPP_TRANSPORT_NODE_SPACING] = std::string(__CPP_TRANSPORT_VALUE_LOGARITHMIC_TOP);
              break;

            default:
	            assert(false);
              throw runtime_exception(runtime_exception::SERIALIZATION_ERROR, __CPP_TRANSPORT_RANGE_INVALID_SPACING);
	        }
	    }


    template <typename value>
    std::ostream& operator<<(std::ostream& out, const stepping_range<value>& obj)
	    {
        out << __CPP_TRANSPORT_RANGE_A << obj.steps;
        out << __CPP_TRANSPORT_RANGE_B;

        if(obj.spacing == linear_stepping)                  out << __CPP_TRANSPORT_RANGE_LINEAR;
        else if(obj.spacing == logarithmic_bottom_stepping) out << __CPP_TRANSPORT_RANGE_LOGARITHMIC_BOTTOM;
        else if(obj.spacing == logarithmic_top_stepping)    out << __CPP_TRANSPORT_RANGE_LOGARITHMIC_TOP;

        out << __CPP_TRANSPORT_RANGE_C << obj.min << ", " << __CPP_TRANSPORT_RANGE_D << obj.max << std::endl;

        for(unsigned int i = 0; i < obj.grid.size(); i++)
	        {
            out << i << ". " << obj.grid[i] << std::endl;
	        }

        return(out);
	    }

	}   // namespace transport


#endif //__range_stepping_H_
