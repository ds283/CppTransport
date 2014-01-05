//
// Created by David Seery on 28/12/2013.
// Copyright (c) 2013 University of Sussex. All rights reserved.
//


#ifndef __parameters_H_
#define __parameters_H_


#include <assert.h>
#include <vector>
#include <functional>
#include <stdexcept>

#include "transport/db-xml/xml_serializable.h"
#include "transport/exceptions.h"
#include "transport/messages_en.h"

#include "boost/lexical_cast.hpp"


#define __CPP_TRANSPORT_NODE_PARAMETERS "parameters"
#define __CPP_TRANSPORT_NODE_MPLANCK    "mplanck"
#define __CPP_TRANSPORT_NODE_PRM_VALUES "values"
#define __CPP_TRANSPORT_NODE_PARAMETER  "parameter"
#define __CPP_TRANSPORT_ATTR_NAME       "name"


namespace transport
  {

    // functions to extract information from XML schema
    namespace parameters_dbxml
      {

        template <typename number>
        void extract_M_Planck(DbXml::XmlManager* mgr, DbXml::XmlValue& value, number& M_Planck)
          {
            // run a query to pick out the M_Planck node
            std::ostringstream query;
            query << __CPP_TRANSPORT_XQUERY_VALUES << "(" << __CPP_TRANSPORT_XQUERY_SELF << __CPP_TRANSPORT_XQUERY_SEPARATOR
              << __CPP_TRANSPORT_NODE_MPLANCK << ")";

            DbXml::XmlValue node = dbxml_helper::extract_single_node(query.str(), mgr, value, __CPP_TRANSPORT_BADLY_FORMED_PARAMS);

            M_Planck = boost::lexical_cast<number>(node.asString());
          }


        template <typename number>
        void extract_parameters(DbXml::XmlManager* mgr, DbXml::XmlValue& value, std::vector<number>& p, std::vector<std::string>& n,
                                const std::vector<std::string>& ordering)
          {
            // run a query to pick out the parameter values block
            std::ostringstream query;
            query << __CPP_TRANSPORT_XQUERY_SELF << __CPP_TRANSPORT_XQUERY_SEPARATOR
              << __CPP_TRANSPORT_NODE_PRM_VALUES;

            DbXml::XmlValue node = dbxml_helper::extract_single_node(query.str(), mgr, value, __CPP_TRANSPORT_BADLY_FORMED_PARAMS);

            if(node.getLocalName() != __CPP_TRANSPORT_NODE_PRM_VALUES) throw runtime_exception(runtime_exception::BADLY_FORMED_XML, __CPP_TRANSPORT_BADLY_FORMED_PARAMS);
            std::vector< dbxml_helper::named_list::element<number> > temporary_list;

            DbXml::XmlValue child = node.getFirstChild();
            while(child.getType() != DbXml::XmlValue::NONE)
              {
                DbXml::XmlResults attrs = child.getAttributes();
                if(attrs.size() != 1) throw runtime_exception(runtime_exception::BADLY_FORMED_XML, __CPP_TRANSPORT_BADLY_FORMED_PARAMS);

                DbXml::XmlValue name;
                attrs.next(name);
                if(name.getLocalName() != __CPP_TRANSPORT_ATTR_NAME) throw runtime_exception(runtime_exception::BADLY_FORMED_XML, __CPP_TRANSPORT_BADLY_FORMED_PARAMS);

                DbXml::XmlValue value = child.getFirstChild();
                if(!(value.getType() == DbXml::XmlValue::NODE && value.getNodeType() == DbXml::XmlValue::TEXT_NODE)) throw runtime_exception(runtime_exception::BADLY_FORMED_XML, __CPP_TRANSPORT_BADLY_FORMED_PARAMS);

                temporary_list.push_back(dbxml_helper::named_list::element<number>(name.getNodeValue(),
                                                                                     boost::lexical_cast<number>(value.getNodeValue())));

                child = child.getNextSibling();
              }

            if(temporary_list.size() != ordering.size()) throw runtime_exception(runtime_exception::BADLY_FORMED_XML, __CPP_TRANSPORT_BADLY_FORMED_PARAMS);

            dbxml_helper::named_list::ordering order_map = dbxml_helper::named_list::make_ordering(ordering);
            dbxml_helper::named_list::comparator<number> cmp(order_map);
            std::sort(temporary_list.begin(), temporary_list.end(), cmp);

            for(unsigned int i = 0; i < temporary_list.size(); i++)
              {
                p.push_back((temporary_list[i]).get_value());
                n.push_back((temporary_list[i]).get_name());
              }
          }


        template <typename number>
        void extract(DbXml::XmlManager* mgr, DbXml::XmlValue& value,
                     number& M_Planck, std::vector<number>& p, std::vector<std::string>& n,
                     const std::vector<std::string>& ordering)
          {
            // run a query to find the parameters XML block from this schema
            std::ostringstream query;
            query << __CPP_TRANSPORT_XQUERY_SELF << __CPP_TRANSPORT_XQUERY_SEPARATOR
              << __CPP_TRANSPORT_NODE_PARAMETERS;

            DbXml::XmlValue node = dbxml_helper::extract_single_node(query.str(), mgr, value, __CPP_TRANSPORT_BADLY_FORMED_PARAMS);

            extract_M_Planck(mgr, node, M_Planck);
            extract_parameters(mgr, node, p, n, ordering);
          }

      }   // namespace parameters_dbxml

    template <typename number> class parameters;

    template <typename number>
    std::ostream& operator<<(std::ostream& out, const parameters<number>& obj);

    template <typename number>
    class parameters: public xml_serializable
      {
      public:
        typedef std::function<void(const std::vector<number>&, std::vector<number>&)> params_validator;

        // CONSTRUCTOR, DESTRUCTOR

        //! Construct 'parameter' object from explicit
        parameters(number Mp, const std::vector<number>& p, const std::vector<std::string>& n, params_validator v);

        //! Return std::vector of parameters
        const std::vector<number>& get_vector() const { return(this->params); }

        //! Return value of M_Planck
        number get_Mp() const { return(this->M_Planck); }

        // XML SERIALIZATION INTERFACE

      public:
        void serialize_xml(DbXml::XmlEventWriter& writer) const;

      public:
        friend std::ostream& operator<< <>(std::ostream& out, const parameters<number>& obj);

        // INTERNAL DATA

      protected:
        //! std::vector representing values of parameters
        std::vector<number> params;
        //! std::vector representing names of parameters
        std::vector<std::string> names;

        //! Value of M_Planck, which sets the scale for all units
        number M_Planck;
      };


    template <typename number>
    parameters<number>::parameters(number Mp, const std::vector<number>& p, const std::vector<std::string>& n, params_validator v)
      : M_Planck(Mp), names(n)
      {
        assert(p.size() == n.size());

        if(M_Planck <= 0.0) throw std::invalid_argument(__CPP_TRANSPORT_MPLANCK_NEGATIVE);

        // validate supplied parameters
        if(p.size() == n.size()) v(p, params);
        else throw std::invalid_argument(__CPP_TRANSPORT_PARAMS_MISMATCH);
      }


    template <typename number>
    void parameters<number>::serialize_xml(DbXml::XmlEventWriter& writer) const
      {
        assert(this->params.size() == this->names.size());

        this->begin_node(writer, __CPP_TRANSPORT_NODE_PARAMETERS, false);

        this->write_value_node(writer, __CPP_TRANSPORT_NODE_MPLANCK, this->M_Planck);

        this->begin_node(writer, __CPP_TRANSPORT_NODE_PRM_VALUES, false);

        if(this->params.size() == this->names.size())
          {
            for(unsigned int i = 0; i < this->params.size(); i++)
              {
                this->write_value_node(writer, __CPP_TRANSPORT_NODE_PARAMETER, this->params[i], __CPP_TRANSPORT_ATTR_NAME, this->names[i]);
              }
          }
        else
          {
            throw std::out_of_range(__CPP_TRANSPORT_PARAM_DATA_MISMATCH);
          }

        this->end_node(writer, __CPP_TRANSPORT_NODE_PRM_VALUES);

        this->end_node(writer, __CPP_TRANSPORT_NODE_PARAMETERS);
      }


    template <typename number>
    std::ostream& operator<<(std::ostream& out, const parameters<number>& obj)
      {
        out << __CPP_TRANSPORT_PARAMS_TAG << std::endl;
        out << "  " << __CPP_TRANSPORT_MPLANCK_TAG << obj.M_Planck << std::endl;
        for(unsigned int i = 0; i < obj.params.size(); i++)
          {
            out << "  " << obj.names[i] << " = " << obj.params[i] << std::endl;
          }
      }

  }



#endif //__parameters_H_
