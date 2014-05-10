//
// Created by David Seery on 27/12/2013.
// Copyright (c) 2013 University of Sussex. All rights reserved.
//


#ifndef __range_H_
#define __range_H_


#include <vector>
#include <stdexcept>

#include "math.h"

#include "transport-runtime-api/serialization/serializable.h"
#include "transport-runtime-api/messages.h"
#include "exceptions.h"


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
        //! Construct a null range object. Used in 'task' objects as placeholders when storing the original wavenumber grid.
        range();

        // BASIC INTERROGATION

      public:
        value get_min()                       const { return(this->min); }
        value get_max()                       const { return(this->max); }
        unsigned int get_steps()              const { return(this->steps); }
        unsigned int size()                   const { return(this->grid.size()); }
        spacing_type get_spacing()            const { return(this->spacing); }
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

        // SERIALIZATION INTERFACE

      public:
        void serialize(serialization_writer& writer) const;

        friend std::ostream& operator<< <>(std::ostream& out, const range<value>& obj);

        // INTERNAL DATA

      protected:
        value min;
        value max;
        unsigned int steps;
        spacing_type spacing;

        std::vector<value> grid;
      };


    template <typename value>
    range<value>::range(value mn, value mx, unsigned int st, spacing_type sp)
      : min(mn), max(mx), steps(st), spacing(sp)
      {
        assert(sp != INTERNAL__null_range_object);

        switch(sp)
          {
            case linear:
            case INTERNAL__null_range_object:
              {
                for(unsigned int i = 0; i <= steps; i++)
                  {
                    grid.push_back(min + (static_cast<double>(i)/steps)*(max-min));
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
                double shifted_max = max - (min-1.0);
                for(unsigned int i = 0; i <= steps; i++)
                  {
                    grid.push_back(min-1.0 + static_cast<value>(pow(shifted_max, static_cast<double>(i)/steps)));
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
    range<value>::range()
      : min(static_cast<value>(0.0)), max(static_cast<value>(0.0)), steps(0), spacing(INTERNAL__null_range_object)
      {
      }


    template <typename value>
    void range<value>::serialize(serialization_writer& writer) const
      {
        if(this->spacing == INTERNAL__null_range_object) throw std::runtime_error(__CPP_TRANSPORT_SERIALIZE_NULL_RANGE);

        this->write_value_node(writer, __CPP_TRANSPORT_NODE_MIN, this->min);
        this->write_value_node(writer, __CPP_TRANSPORT_NODE_MAX, this->max);
        this->write_value_node(writer, __CPP_TRANSPORT_NODE_STEPS, this->steps);

		    switch(this->spacing)
			    {
		        case linear:
			        this->write_value_node(writer, __CPP_TRANSPORT_NODE_SPACING, std::string(__CPP_TRANSPORT_VALUE_LINEAR));
				      break;

		        case logarithmic:
			        this->write_value_node(writer, __CPP_TRANSPORT_NODE_SPACING, std::string(__CPP_TRANSPORT_VALUE_LOGARITHMIC));
				      break;

		        default:
			        assert(false);
		          throw runtime_exception(runtime_exception::SERIALIZATION_ERROR, __CPP_TRANSPORT_RANGE_INVALID_SPACING);
			    }
      }


		namespace
			{

				namespace range
					{

						template <typename value>
						transport::range<value> deserialize(serialization_reader* reader)
							{
								double min, max;
								unsigned int steps;
						    std::string spacing;

								reader->read_value(__CPP_TRANSPORT_NODE_MIN, min);
								reader->read_value(__CPP_TRANSPORT_NODE_MAX, max);
								reader->read_value(__CPP_TRANSPORT_NODE_STEPS, steps);
								reader->read_value(__CPP_TRANSPORT_NODE_SPACING, spacing);

						    std::cerr << "Range group read spacing = " << spacing << std::endl;

						    typename transport::range<value>::spacing_type type = transport::range<value>::linear;

								if(spacing == __CPP_TRANSPORT_VALUE_LINEAR) type = transport::range<value>::linear;
								else if(spacing == __CPP_TRANSPORT_VALUE_LOGARITHMIC) type = transport::range<value>::logarithmic;
								else throw runtime_exception(runtime_exception::REPOSITORY_ERROR, __CPP_TRANSPORT_BADLY_FORMED_RANGE);

							  return(transport::range<value>(static_cast<value>(min), static_cast<value>(max), steps, type));
							}

					}   // namespace range

			}   // unnamed namespace


    template <typename value>
    std::ostream& operator<<(std::ostream& out, const range<value>& obj)
      {
        out << __CPP_TRANSPORT_RANGE_A << obj.steps
            << __CPP_TRANSPORT_RANGE_B << (obj.spacing == range<value>::linear ? __CPP_TRANSPORT_RANGE_LINEAR : __CPP_TRANSPORT_RANGE_LOGARITHMIC)
            << __CPP_TRANSPORT_RANGE_C << obj.min << ", " << __CPP_TRANSPORT_RANGE_D << obj.max << std::endl;
        return(out);
      }


  }   // namespace transport



#endif //__range_H_
