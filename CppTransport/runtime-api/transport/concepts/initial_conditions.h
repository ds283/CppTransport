//
// Created by David Seery on 22/12/2013.
// Copyright (c) 2013 University of Sussex. All rights reserved.
//


#ifndef __initial_conditions_H_
#define __initial_conditions_H_


#include <assert.h>
#include <vector>
#include <functional>
#include <stdexcept>

#include "transport/db-xml/xml_serializable.h"
#include "transport/messages_en.h"


#define __CPP_TRANSPORT_NODE_INITIAL_CONDITIONS "initial-conditions"
#define __CPP_TRANSPORT_NODE_COORDINATE         "coordinate"
#define __CPP_TRANSPORT_ATTR_NAME               "name"
#define __CPP_TRANSPORT_NODE_NSTAR              "nstar"
#define __CPP_TRANSPORT_NODE_ICS_VALUES         "values"


namespace transport
  {

    template <typename number>
    class initial_conditions: public xml_serializable
      {
      public:
        typedef std::function<void(const std::vector<number>&, std::vector<number>&)> ics_validator;

        typedef std::function<void(const std::vector<number>&, std::vector<number>&, double, double, double)> ics_finder;

        // construct initial conditions from directly-supplied data
        initial_conditions(const std::vector<number>& i, const std::vector<std::string>& n,
                           double Npre, ics_validator v);

        // construct initial conditions offset from directly-supplied data
        initial_conditions(const std::vector<number>& i, const std::vector<std::string>& n,
                           double Ninit, double Ncross, double Npre,
                           ics_validator v, ics_finder f);

        const std::vector<number>& get_ics() const { return(this->ics); }

        // XML SERIALIZATION INTERFACE

      public:
        void serialize_xml(DbXml::XmlEventWriter& writer);

      protected:
        std::vector<number> ics;        // values of fields and their derivatives at the initial time
        std::vector<std::string> names; // names for fields and their derivatives

        double Nstar;                    // number of e-folds from initial conditions to horizon-crossing
      };


    template <typename number>
    initial_conditions<number>::initial_conditions(const std::vector<number>& i, const std::vector<std::string>& n,
                                                   double Npre, ics_validator v)
      : Nstar(Npre), names(n)
      {
        assert(i.size() == n.size());

        if(i.size() == n.size())
          {
            // validate supplied initial conditions
            v(i, ics);
          }
        else
          {
            throw std::invalid_argument(__CPP_TRANSPORT_ICS_MISMATCH);
          }
      }


    template <typename number>
    initial_conditions<number>::initial_conditions(const std::vector<number>& i, const std::vector<std::string>& n,
                                                   double Ninit, double Ncross, double Npre,
                                                   ics_validator v, ics_finder f)
      : Nstar(Npre), names(n)
      {
        assert(i.size() == n.size());

        if(i.size() == n.size())
          {
            std::vector<number> validated_ics;

            // validate supplied initial conditions
            v(i, validated_ics);

            // call supplied finder function to correctly offset these ics
            f(validated_ics, ics, Ninit, Ncross, Npre);
          }
        else
          {
            throw std::invalid_argument(__CPP_TRANSPORT_ICS_MISMATCH);
          }
      }


    template <typename number>
    void initial_conditions<number>::serialize_xml(DbXml::XmlEventWriter& writer)
      {
        this->begin_node(writer, __CPP_TRANSPORT_NODE_INITIAL_CONDITIONS);
        this->write_value_node(writer, __CPP_TRANSPORT_NODE_NSTAR, this->N);
        this->begin_node(writer, __CPP_TRANSPORT_NODE_ICS_VALUES);
        for(unsigned int i = 0; i < this->ics.size(); i++)
          {
            this->write_value_node(writer, __CPP_TRANSPORT_NODE_COORDINATE, this->ics[i], __CPP_TRANSPORT_ATTR_NAME, this->names[i]);
          }
        this->end_node(writer, __CPP_TRANSPORT_NODE_ICS_VALUES);
        this->end_node(writer, __CPP_TRANSPORT_NODE_INITIAL_CONDITIONS);
      }

  }


#endif //__initial_conditions_H_
