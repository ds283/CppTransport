//
// Created by David Seery on 27/12/2013.
// Copyright (c) 2013-15 University of Sussex. All rights reserved.
//


#ifndef __range_H_
#define __range_H_


#include <vector>
#include <stdexcept>

#include "math.h"

#include "transport-runtime-api/serialization/serializable.h"
#include "transport-runtime-api/messages.h"
#include "transport-runtime-api/exceptions.h"


#define __CPP_TRANSPORT_NODE_MIN          "min"
#define __CPP_TRANSPORT_NODE_MAX          "max"
#define __CPP_TRANSPORT_NODE_STEPS        "steps"
#define __CPP_TRANSPORT_NODE_SPACING      "spacing"
#define __CPP_TRANSPORT_VALUE_LINEAR      "linear"
#define __CPP_TRANSPORT_VALUE_LOGARITHMIC "logarithmic"


namespace transport
  {

    template <typename value> class range;

    template <typename value>
    std::ostream& operator<<(std::ostream& out, const range<value>& obj);

    template <typename value>
    class range: public serializable
      {
      public:

        typedef enum { linear, logarithmic, INTERNAL__null_range_object } spacing_type;


        // CONSTRUCTOR

      public:
        //! Construct a range object with specified minimum & maximum values, number of steps and spacing type.
        range(value mn, value mx, unsigned int st, spacing_type sp=linear);

        //! Deserialization constructor
        range(Json::Value& reader);

        //! Construct a null range object. Used in 'task' objects as placeholders when storing the original wavenumber grid.
        range();


        // BASIC INTERROGATION

      public:

        //! Get minimum entry
        value get_min()                       const { return(this->min); }

        //! Get maximum entry
        value get_max()                       const { return(this->max); }

        //! Get number of steps
        unsigned int get_steps()              const { return(this->steps); }

        //! Get number of entries
        unsigned int size()                   const { return(this->grid.size()); }

        //! Get spacing type
        spacing_type get_spacing()            const { return(this->spacing); }

        //! Get grid of entries
        const std::vector<value>& get_grid()  const { return(this->grid); }

        value operator[](unsigned int d) const
          {
            assert(d < this->grid.size());
            if(d < this->grid.size())
              {
                return(this->grid[d]);
              }
            else
              {
                throw std::out_of_range(__CPP_TRANSPORT_RANGE_RANGE);
              }
          }


        // POPULATE GRID

      protected:

        //! fill out the grid entries
        void populate_grid(void);


        // SERIALIZATION INTERFACE

      public:

        //! Serialize this object
        virtual void serialize(Json::Value& writer) const override;

        friend std::ostream& operator<< <>(std::ostream& out, const range<value>& obj);


        // INTERNAL DATA

      protected:

        //! Minimium value
        value min;

        //! Maximum value
        value max;

        //! Numnber of steps
        unsigned int steps;

        //! Spacing type
        spacing_type spacing;

        //! Grid of values
        std::vector<value> grid;
      };


    template <typename value>
    range<value>::range(value mn, value mx, unsigned int st, spacing_type sp)
      : min(mn), max(mx), steps(st), spacing(sp)
      {
        assert(sp != INTERNAL__null_range_object);

        this->populate_grid();
      }


    template <typename value>
    range<value>::range()
      : min(static_cast<value>(0.0)), max(static_cast<value>(0.0)), steps(0), spacing(INTERNAL__null_range_object)
      {
      }


    template <typename value>
    range<value>::range(Json::Value& reader)
      {
        double m = reader[__CPP_TRANSPORT_NODE_MIN].asDouble();
        min = static_cast<value>(m);

        m = reader[__CPP_TRANSPORT_NODE_MAX].asDouble();
        max = static_cast<value>(m);

        steps = reader[__CPP_TRANSPORT_NODE_STEPS].asUInt();

        std::string spc_string = reader[__CPP_TRANSPORT_NODE_SPACING].asString();

        if(spc_string == __CPP_TRANSPORT_VALUE_LINEAR) spacing = linear;
        else if(spc_string == __CPP_TRANSPORT_VALUE_LOGARITHMIC) spacing = logarithmic;
        else throw runtime_exception(runtime_exception::SERIALIZATION_ERROR, __CPP_TRANSPORT_BADLY_FORMED_RANGE);

        this->populate_grid();
      }


    template <typename value>
    void range<value>::populate_grid(void)
      {
        this->grid.clear();
        this->grid.reserve(this->steps+1);

        switch(this->spacing)
          {
            case linear:
            case INTERNAL__null_range_object:
              {
                for(unsigned int i = 0; i <= this->steps; i++)
                  {
                    this->grid.push_back(min + (static_cast<double>(i)/this->steps)*(this->max-this->min));
                  }
                break;
              }

            // the logarithmically-spaced interval isn't translation-invariant
            // (logarithmic spacing between 10 and 20 doesn't isn't the same as logarithmic
            // spacing between 110 and 120)
            // to standardize, and also to allow log-spacing for ranges that contain 0,
            // we shift the range to begin at 1, perform the log-spacing, and then
            // reverse the shift
            case logarithmic:
              {
                double shifted_max = this->max - (this->min-1.0);
                for(unsigned int i = 0; i <= this->steps; i++)
                  {
                    grid.push_back(this->min-1.0 + static_cast<value>(pow(shifted_max, static_cast<double>(i)/this->steps)));
                  }
                break;
              }

            default:
              {
                assert(false);
                throw runtime_exception(runtime_exception::RUNTIME_ERROR, __CPP_TRANSPORT_RANGE_INVALID_SPACING);
              }
          }
      }

    template <typename value>
    void range<value>::serialize(Json::Value& writer) const
      {
        if(this->spacing == INTERNAL__null_range_object) throw std::runtime_error(__CPP_TRANSPORT_SERIALIZE_NULL_RANGE);

        writer[__CPP_TRANSPORT_NODE_MIN] = static_cast<double>(this->min);
        writer[__CPP_TRANSPORT_NODE_MAX] = static_cast<double>(this->max);
        writer[__CPP_TRANSPORT_NODE_STEPS] = this->steps;

		    switch(this->spacing)
			    {
		        case linear:
			        writer[__CPP_TRANSPORT_NODE_SPACING] = std::string(__CPP_TRANSPORT_VALUE_LINEAR);
				      break;

		        case logarithmic:
			        writer[__CPP_TRANSPORT_NODE_SPACING] = std::string(__CPP_TRANSPORT_VALUE_LOGARITHMIC);
				      break;

		        default:
			        assert(false);
		          throw runtime_exception(runtime_exception::SERIALIZATION_ERROR, __CPP_TRANSPORT_RANGE_INVALID_SPACING);
			    }
      }


    template <typename value>
    std::ostream& operator<<(std::ostream& out, const range<value>& obj)
      {
        out << __CPP_TRANSPORT_RANGE_A << obj.steps
            << __CPP_TRANSPORT_RANGE_B << (obj.spacing == range<value>::linear ? __CPP_TRANSPORT_RANGE_LINEAR : __CPP_TRANSPORT_RANGE_LOGARITHMIC)
            << __CPP_TRANSPORT_RANGE_C << obj.min << ", " << __CPP_TRANSPORT_RANGE_D << obj.max << std::endl;

		    for(unsigned int i = 0; i < obj.grid.size(); i++)
			    {
				    out << i << ". " << obj.grid[i] << std::endl;
			    }

        return(out);
      }


  }   // namespace transport



#endif //__range_H_
