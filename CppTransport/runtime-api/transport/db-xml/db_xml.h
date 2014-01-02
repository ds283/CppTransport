//
// Created by David Seery on 27/12/2013.
// Copyright (c) 2013 University of Sussex. All rights reserved.
//


#ifndef __db_xml_H_
#define __db_xml_H_


#include <functional>
#include <algorithm>
#include <map>

#include "dbxml/db_cxx.h"
#include "dbxml/dbxml/DbXml.hpp"

#include "transport/exceptions.h"
#include "transport/messages_en.h"


// the Berkeley DB XML API wants all string provided as (const unsigned char*) types
// Here, this is done with a reinterpret_cast which may be a slight blunt solution
#define __CPP_TRANSPORT_DBXML_STRING(str) reinterpret_cast<const unsigned char*>(str)


#define __CPP_TRANSPORT_XQUERY_VALUES    "distinct-values"
#define __CPP_TRANSPORT_XQUERY_SELF      "."
#define __CPP_TRANSPORT_XQUERY_SEPARATOR "/"
#define __CPP_TRANSPORT_XQUERY_WILDCARD  "*"


namespace dbxml_delegate
  {

    namespace named_list
      {
        typedef std::map<std::string, unsigned int> ordering;

        inline ordering make_ordering(const std::vector<std::string>& order)
          {
            ordering order_map;

            for(unsigned int i = 0; i < order.size(); i++)
              {
                order_map.insert(std::make_pair(order[i], i));
              }

            return(order_map);
          }

        template <typename number> class comparator;

        template <typename number>
        class element
          {
          public:
            element(std::string n, number p)
              : value(p), name(n)
              {
              }

            number get_value() const { return(this->value); }
            const std::string& get_name() const { return(this->name); }

            friend class comparator<number>;

          protected:
            number value;
            std::string name;
          };

        template <typename number>
        class comparator
          {
          public:
            comparator(const ordering& o)
              : order_map(o)
              {
              }

            bool operator()(const element<number>& a, const element<number>& b)
              {
                auto t_a = this->order_map.find(a.name);
                auto t_b = this->order_map.find(b.name);

                if(t_a == this->order_map.end() || t_b == this->order_map.end()) throw transport::runtime_exception(transport::runtime_exception::RUNTIME_ERROR, __CPP_TRANSPORT_NAMED_LIST_FAIL);

                return((*t_a) < (*t_b));
              }

          protected:
            const ordering& order_map;
          };

      } // namespace named_list


    inline
    DbXml::XmlValue extract_single_node(const std::string& query, DbXml::XmlManager* mgr, DbXml::XmlValue& value, const std::string& excpt_msg)
      {
        DbXml::XmlQueryContext ctx = mgr->createQueryContext();

        DbXml::XmlQueryExpression expr = mgr->prepare(query, ctx);

        DbXml::XmlResults results = expr.execute(value, ctx);

        if(results.size() != 1)
          {
            std::ostringstream msg;
            msg << excpt_msg << __CPP_TRANSPORT_RUN_REPAIR;
            throw transport::runtime_exception(transport::runtime_exception::BADLY_FORMED_XML, excpt_msg);
          }

        DbXml::XmlValue node;
        results.next(node);

        return(node);
      }

  }   // namespace dbxml_delegate


#endif //__db_xml_H_
