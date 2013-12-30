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

#include "transport/concepts/parameters.h"
#include "transport/db-xml/xml_serializable.h"
#include "transport/messages_en.h"


#define __CPP_TRANSPORT_NODE_INITIAL_CONDITIONS "initial-conditions"
#define __CPP_TRANSPORT_NODE_COORDINATE         "coordinate"
#define __CPP_TRANSPORT_ATTR_NAME               "name"
#define __CPP_TRANSPORT_NODE_NSTAR              "nstar"
#define __CPP_TRANSPORT_NODE_ICS_VALUES         "values"


namespace transport
  {

    template <typename number> class initial_conditions;

    template <typename number>
    std::ostream& operator<<(std::ostream& out, initial_conditions<number>& obj);

    template <typename number>
    std::ostream& operator<<(std::ostream& out, const initial_conditions<number>& obj);

    template <typename number>
    class initial_conditions: public xml_serializable  // WARNING: multiple inheritance, but we are only inheriting the interfaces
      {
      public:
        typedef std::function<void(const parameters<number>&, const std::vector<number>&, std::vector<number>&)> ics_validator;

        typedef std::function<void(const parameters<number>&, const std::vector<number>&, std::vector<number>&, double, double, unsigned int)> ics_finder;

        // construct initial conditions from directly-supplied data
        initial_conditions(const parameters<number>& p, const std::vector<number>& i, const std::vector<std::string>& n,
                           double Npre, ics_validator v);

        // construct initial conditioıns offset from directly-supplied data
        initial_conditions(const parameters<number>& p, const std::vector<number>& i, const std::vector<std::string>& n,
                           double Ninit, double Ncross, double Npre,
                           ics_validator v, ics_finder f);

        // return parameters used for these initial conditions
        const parameters<number>& get_params() const { return(this->params); }

        // return vector of initial conditions
        const std::vector<number>& get_vector() const { return(this->ics); }

        // return relative time of horizon-crossing
        const double get_Nstar() const { return(this->Nstar); }

        // XML SERIALIZATION INTERFACE

      public:
        void serialize_xml(DbXml::XmlEventWriter& writer);

      public:
        friend std::ostream& operator<< <>(std::ostream& out, initial_conditions<number>& obj);
        friend std::ostream& operator<< <>(std::ostream& out, const initial_conditions<number>& obj);

      protected:
        //! copy of parameters
        const parameters<number> params;

        //! values of fields and their derivatives
        std::vector<number> ics;
        //! names of fields and their derivatives
        std::vector<std::string> names;

        //! number of e-folds from initial conditions to horizon-crossing
        double Nstar;
      };


    template <typename number>
    initial_conditions<number>::initial_conditions(const parameters<number>& p, const std::vector<number>& i, const std::vector<std::string>& n,
                                                   double Npre, ics_validator v)
      : params(p), Nstar(Npre), names(n)
      {
        // validate supplied initial conditions - we rely on the validator to throw
        // an exception if the supplied number of ics is incorrect
        v(p, i, ics);
      }


    template <typename number>
    initial_conditions<number>::initial_conditions(const parameters<number>& p, const std::vector<number>& i, const std::vector<std::string>& n,
                                                   double Ninit, double Ncross, double Npre,
                                                   ics_validator v, ics_finder f)
      : params(p), Nstar(Npre), names(n)
      {
        std::vector<number> validated_ics;

        // validate supplied initial conditions
        v(p, i, validated_ics);

        // call supplied finder function to correctly offset these ics
        f(p, validated_ics, ics, Ninit, Ncross, Npre);
      }


    template <typename number>
    void initial_conditions<number>::serialize_xml(DbXml::XmlEventWriter& writer)
      {
        this->begin_node(writer, __CPP_TRANSPORT_NODE_INITIAL_CONDITIONS, false);
        this->write_value_node(writer, __CPP_TRANSPORT_NODE_NSTAR, this->Nstar);
        this->begin_node(writer, __CPP_TRANSPORT_NODE_ICS_VALUES, false);

        for(unsigned int i = 0; i < this->ics.size(); i++)
          {
            this->write_value_node(writer, __CPP_TRANSPORT_NODE_COORDINATE, this->ics[i], __CPP_TRANSPORT_ATTR_NAME, this->names[i]);
          }

        this->end_node(writer, __CPP_TRANSPORT_NODE_ICS_VALUES);
        this->end_node(writer, __CPP_TRANSPORT_NODE_INITIAL_CONDITIONS);
      }


    template <typename number>
    std::ostream& operator<<(std::ostream& out, const initial_conditions<number>& obj)
      {
        for(unsigned int i = 0; i < obj.ics.size(); i++)
          {
            out << "  " << obj.names[i] << " = " << obj.ics[i] << std::endl;
          }
        return(out);
      }


    template <typename number>
    std::ostream& operator<<(std::ostream& out, initial_conditions<number>& obj)
      {
        out << (const initial_conditions<number>)obj;
      }

  }


#endif //__initial_conditions_H_
