//
// Created by David Seery on 27/12/2013.
// Copyright (c) 2013 University of Sussex. All rights reserved.
//


#ifndef __range_H_
#define __range_H_


#include <vector>

#include "math.h"

#include "transport/xml_serializable.h"
#include "transport/messages_en.h"

#include "dbxml/DbXml.hpp"


#define __CPP_TRANSPORT_NODE_RANGE        "value-range"
#define __CPP_TRANSPORT_NODE_MIN          "min"
#define __CPP_TRANSPORT_NODE_MAX          "max"
#define __CPP_TRANSPORT_NODE_STEPS        "steps"
#define __CPP_TRANSPORT_NODE_SPACING      "spacing"
#define __CPP_TRANSPORT_VALUE_LINEAR      "linear"
#define __CPP_TRANSPORT_VALUE_LOGARITHMIC "logarithmic"


namespace transport
  {

    template <typename value>
    class range: public xml_serializable
      {
      public:
        typedef enum { linear, logarithmic } spacing_type;

        // CONSTRUCTOR

      public:
        range(value mn, value mx, unsigned int st, spacing_type sp);

        // BASIC INTERROGATION

      public:
        value get_min()                       const { return(this->min); }
        value get_max()                       const { return(this->max); }
        unsigned int get_steps()              const { return(this->steps); }
        spacing_type get_spacing()            const { return(this->spacing); }
        const std::vector<value>& get_grid()  const { return(this->grid); }

        // XML SERIALIZATION INTERFACE

      public:
        void serialize_xml(DbXml::XmlEventWriter& writer);

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
        switch(sp)
          {
            case linear:
              for(unsigned int i = 0; i <= steps; i++)
                {
                  grid.push_back(min + (static_cast<double>(i)/steps)*(max-min));
                }
              break;

            case logarithmic:
              for(unsigned int i = 0; i <= steps; i++)
                {
                  grid.push_back(min * pow(max/min, static_cast<double>(i)/steps));
                }
              break;

            default:
              assert(false);
              throw std::invalid_argument(__CPP_TRANSPORT_RANGE_INVALID_SPACING);
          }
      }


    template <typename value>
    void range<value>::serialize_xml(DbXml::XmlEventWriter& writer)
      {
        this->begin_node(writer, __CPP_TRANSPORT_NODE_RANGE);
        this->write_value_node(writer, __CPP_TRANSPORT_NODE_MIN, this->min);
        this->write_value_node(writer, __CPP_TRANSPORT_NODE_MAX, this->max);
        this->write_value_node(writer, __CPP_TRANSPORT_NODE_STEPS, this->steps);
        this->write_value_node(writer, __CPP_TRANSPORT_NODE_SPACING, (this->spacing == linear) ? __CPP_TRANSPORT_VALUE_LINEAR : _CPP_TRANSPORT_VALUE_LOGARITHMIC);
        this->end_node(writer, __CPP_TRANSPORT_NODE_RANGE);
      }


  }   // namespace transport



#endif //__range_H_
