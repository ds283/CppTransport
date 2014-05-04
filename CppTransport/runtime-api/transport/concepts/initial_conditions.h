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
#include "transport/manager/serializable.h"
#include "transport/messages_en.h"

#include "transport/utilities/random_string.h"


#define __CPP_TRANSPORT_NODE_INITIAL_CONDITIONS "initial-conditions"
#define __CPP_TRANSPORT_NODE_COORDINATE         "coordinate"
#define __CPP_TRANSPORT_ATTR_NAME               "name"
#define __CPP_TRANSPORT_NODE_NSTAR              "nstar"
#define __CPP_TRANSPORT_NODE_ICS_VALUES         "values"


namespace transport
  {

    // functions to extract information from XML schema
    namespace ics_dbxml
      {

        void extract_Nstar(DbXml::XmlManager* mgr, DbXml::XmlValue& value, double& Nstar)
          {
            // run a query to pick out the Nstar node
            std::string query = dbxml_helper::xquery::value_self(__CPP_TRANSPORT_NODE_NSTAR);

            DbXml::XmlValue node = dbxml_helper::extract_single_node(query, mgr, value, __CPP_TRANSPORT_BADLY_FORMED_ICS);

            Nstar = boost::lexical_cast<double>(node.asString());
          }


        template <typename number>
        void extract_coords(DbXml::XmlManager* mgr, DbXml::XmlValue& value, std::vector<number>& c, std::vector<std::string>& n,
                            const std::vector<std::string>& ordering)
          {
            // run a query to pick out the initial-condition values block
            std::string query = dbxml_helper::xquery::node_self(__CPP_TRANSPORT_NODE_ICS_VALUES);

            DbXml::XmlValue node = dbxml_helper::extract_single_node(query, mgr, value, __CPP_TRANSPORT_BADLY_FORMED_ICS);

            if(node.getLocalName() != __CPP_TRANSPORT_NODE_ICS_VALUES) throw runtime_exception(runtime_exception::BADLY_FORMED_XML, __CPP_TRANSPORT_BADLY_FORMED_ICS);
            std::vector< dbxml_helper::named_list::element<number> > temporary_list;

            DbXml::XmlValue child = node.getFirstChild();
            while(child.getType() != DbXml::XmlValue::NONE)
              {
                DbXml::XmlResults attrs = child.getAttributes();
                if(attrs.size() != 1) throw runtime_exception(runtime_exception::BADLY_FORMED_XML, __CPP_TRANSPORT_BADLY_FORMED_ICS);

                DbXml::XmlValue name;
                attrs.next(name);
                if(name.getLocalName() != __CPP_TRANSPORT_ATTR_NAME) throw runtime_exception(runtime_exception::BADLY_FORMED_XML, __CPP_TRANSPORT_BADLY_FORMED_ICS);

                DbXml::XmlValue value = child.getFirstChild();
                if(value.getNodeType() != DbXml::XmlValue::TEXT_NODE) throw runtime_exception(runtime_exception::BADLY_FORMED_XML, __CPP_TRANSPORT_BADLY_FORMED_ICS);

                temporary_list.push_back(dbxml_helper::named_list::element<number>(name.getNodeValue(),
                                                                                     boost::lexical_cast<number>(value.getNodeValue())));

                child = child.getNextSibling();
              }

            if(temporary_list.size() != ordering.size()) throw runtime_exception(runtime_exception::BADLY_FORMED_XML, __CPP_TRANSPORT_BADLY_FORMED_ICS);

            dbxml_helper::named_list::ordering order_map = dbxml_helper::named_list::make_ordering(ordering);
            dbxml_helper::named_list::comparator<number> cmp(order_map);
            std::sort(temporary_list.begin(), temporary_list.end(), cmp);

            for(unsigned int i = 0; i < temporary_list.size(); i++)
              {
                c.push_back((temporary_list[i]).get_value());
                n.push_back((temporary_list[i]).get_name());
              }
          }


        template <typename number>
        void extract(DbXml::XmlManager* mgr, DbXml::XmlValue& value, double& Nstar,
                     std::vector<number>& c, std::vector<std::string>& n,
                     const std::vector<std::string>& ordering)
          {
            // run a query to find the initial conditions XML block from this schema
            std::string query = dbxml_helper::xquery::node_self(__CPP_TRANSPORT_NODE_INITIAL_CONDITIONS);

            DbXml::XmlValue node = dbxml_helper::extract_single_node(query, mgr, value, __CPP_TRANSPORT_BADLY_FORMED_ICS);

            extract_Nstar(mgr, node, Nstar);
            extract_coords(mgr, node, c, n, ordering);
          }

      }   // namespace ics_dbxml

    template <typename number> class initial_conditions;

    template <typename number>
    std::ostream& operator<<(std::ostream& out, const initial_conditions<number>& obj);

    template <typename number>
    class initial_conditions: public serializable
      {
      public:
        typedef std::function<void(const parameters<number>&, const std::vector<number>&, std::vector<number>&)> ics_validator;

        typedef std::function<void(const parameters<number>&, const std::vector<number>&, std::vector<number>&, double, double, unsigned int)> ics_finder;

        //! Construct named initial conditions from directly-supplied data
        initial_conditions(const std::string& nm, const parameters<number>& p,
                           const std::vector<number>& i, const std::vector<std::string>& n,
                           double Npre, ics_validator v);

        //! Construct anonymized initial conditions from directly-supplied data
        initial_conditions(const parameters<number>& p,
                           const std::vector<number>& i, const std::vector<std::string>& n,
                           double Npre, ics_validator v)
          : initial_conditions(random_string(), p, i, n, Npre, v)
          {
          }

        //! Construct named initial conditions offset from directly-supplied data using a supplied model
        initial_conditions(const std::string& nm, const parameters<number>& p,
                           const std::vector<number>& i, const std::vector<std::string>& n,
                           double Ninit, double Ncross, double Npre,
                           ics_validator v, ics_finder f);

        //! Construct anonymized initial conditions offset from directly-supplied data using a supplied model
        initial_conditions(const parameters<number>& p,
                           const std::vector<number>& i, const std::vector<std::string>& n,
                           double Ninit, double Ncross, double Npre,
                           ics_validator v, ics_finder f)
          : initial_conditions(random_string(), p, i, n, Ninit, Ncross, Npre, v, f)
          {
          }

        //! Return parameters associated with these initial conditions
        const parameters<number>& get_params() const { return(this->params); }

        //! Return std::vector of initial conditions
        const std::vector<number>& get_vector() const { return(this->ics); }

        //! Return relative time of horizon-crossing
        const double get_Nstar() const { return(this->Nstar); }

        //! Return name
        const std::string& get_name() const { return(this->name); }

        // SERIALIZATION INTERFACE

      public:
        void serialize(serialization_writer& writer) const;

      public:
        friend std::ostream& operator<< <>(std::ostream& out, const initial_conditions<number>& obj);

      protected:
        //! name of this ics/params combination, used for tagging with deposited in a repository
        const std::string name;

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
    initial_conditions<number>::initial_conditions(const std::string& nm, const parameters<number>& p,
                                                   const std::vector<number>& i, const std::vector<std::string>& n,
                                                   double Npre, ics_validator v)
      : name(nm), params(p), Nstar(Npre), names(n)
      {
        // validate supplied initial conditions - we rely on the validator to throw
        // an exception if the supplied number of ics is incorrect
        v(params, i, ics);
      }


    template <typename number>
    initial_conditions<number>::initial_conditions(const std::string& nm, const parameters<number>& p,
                                                   const std::vector<number>& i, const std::vector<std::string>& n,
                                                   double Ninit, double Ncross, double Npre,
                                                   ics_validator v, ics_finder f)
      : name(nm), params(p), Nstar(Npre), names(n)
      {
        std::vector<number> validated_ics;

        // validate supplied initial conditions
        v(p, i, validated_ics);

        // call supplied finder function to correctly offset these ics
        f(p, validated_ics, ics, Ninit, Ncross, Npre);
      }


    template <typename number>
    void initial_conditions<number>::serialize(serialization_writer& writer) const
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

        this->params.serialize(writer);
      }


    template <typename number>
    std::ostream& operator<<(std::ostream& out, const initial_conditions<number>& obj)
      {
        out << obj.params << std::endl;

        assert(obj.ics.size() == obj.names.size());

        out << __CPP_TRANSPORT_ICS_TAG << std::endl;
        for(unsigned int i = 0; i < obj.ics.size(); i++)
          {
            out << "  " << obj.names[i] << " = " << obj.ics[i] << std::endl;
          }

        return(out);
      }

  }


#endif //__initial_conditions_H_
