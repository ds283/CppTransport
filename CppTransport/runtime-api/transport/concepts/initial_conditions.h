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

    template <typename number, unsigned int N>
    class initial_conditions: public flattener<N>, public xml_serializable;

    template <typename number, unsigned int N>
    std::ostream& operator<<(std::ostream& out, initial_conditions<number, N>& obj);

    template <typename number, unsigned int N>
    class initial_conditions: public flattener<N>, public xml_serializable  // WARNING: multiple inheritance, but we are only inheriting the interfaces
      {
      public:
        typedef std::function<void(const parameters<number>&, const std::vector<number>&, std::vector<number>&)> ics_validator;

        typedef std::function<void(const parameters<number>&, const std::vector<number>&, std::vector<number>&, double, double, double)> ics_finder;

        // construct initial conditions from directly-supplied data
        initial_conditions(const std::vector<number>& i, const std::vector<std::string>& n,
                           double Npre, ics_validator v);

        // construct initial conditions offset from directly-supplied data
        initial_conditions(const std::vector<number>& i, const std::vector<std::string>& n,
                           double Ninit, double Ncross, double Npre,
                           ics_validator v, ics_finder f);

        // return vector of initial conditions
        const std::vector<number>& get_vector() const { return(this->ics); }

        // return relative time of horizon-crossing
        const double get_Nstar() const { return(this->Nstar); }

        // XML SERIALIZATION INTERFACE

      public:
        void serialize_xml(DbXml::XmlEventWriter& writer);

      public:
        friend std::ostream& operator<< <>(std::ostream& out, initial_conditions<number>& obj);

      protected:
        std::vector<number> ics;        // values of fields and their derivatives at the initial time
        std::vector<std::string> names; // names for fields and their derivatives

        double Nstar;                    // number of e-folds from initial conditions to horizon-crossing
      };


    template <typename number, unsigned int N>
    initial_conditions<number, N>::initial_conditions(const std::vector<number>& i, const std::vector<std::string>& n,
                                                      double Npre, ics_validator v)
      : Nstar(Npre), names(n)
      {
        assert(i.size() == N || i.size() == 2*N);
        assert(n.size() == 2*N);

        if((i.size() == N || i.size() == 2*N) && n.size() == 2*N)
          {
            // validate supplied initial conditions
            v(i, ics);
          }
        else
          {
            throw std::invalid_argument(__CPP_TRANSPORT_ICS_MISMATCH);
          }
      }


    template <typename number, unsigned int N>
    initial_conditions<number, N>::initial_conditions(const std::vector<number>& i, const std::vector<std::string>& n,
                                                      double Ninit, double Ncross, double Npre,
                                                      ics_validator v, ics_finder f)
      : Nstar(Npre), names(n)
      {
        assert(i.size() == N || i.size() == 2*N);
        assert(n.size() == 2*N);

        if((i.size() == N || i.size() == 2*N) && n.size() == 2*N)
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


    template <typename number, unsigned int N>
    void initial_conditions<number, N>::serialize_xml(DbXml::XmlEventWriter& writer)
      {
        this->begin_node(writer, __CPP_TRANSPORT_NODE_INITIAL_CONDITIONS);
        this->write_value_node(writer, __CPP_TRANSPORT_NODE_NSTAR, this->N);
        this->begin_node(writer, __CPP_TRANSPORT_NODE_ICS_VALUES);

        assert(this->ics.size() == N);

        for(unsigned int i = 0; i < N; i++)
          {
            this->write_value_node(writer, __CPP_TRANSPORT_NODE_COORDINATE, this->ics[i], __CPP_TRANSPORT_ATTR_NAME, this->names[i]);
          }

        this->end_node(writer, __CPP_TRANSPORT_NODE_ICS_VALUES);
        this->end_node(writer, __CPP_TRANSPORT_NODE_INITIAL_CONDITIONS);
      }


    template <typename number, unsigned int N>
    std::ostream& operator<<(std::ostream& out, initial_conditions<number>& obj)
      {
        assert(obj.ics.size() == N);

        for(unsigned int i = 0; i < N; i++)
          {
            out << "  " << obj.names[i] << " = " << obj.ics[i] << std::endl;
          }
        return(out);
      }

  }


#endif //__initial_conditions_H_
