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

#include "transport-runtime-api/exceptions.h"
#include "transport-runtime-api/messages.h"


// the Berkeley DB XML API wants all string provided as (const unsigned char*) types
// Here, this is done with a reinterpret_cast which may be a slightly blunt solution
#define __CPP_TRANSPORT_DBXML_STRING(str) reinterpret_cast<const unsigned char*>(str)


namespace dbxml_helper
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

                return((*t_a).second < (*t_b).second);
              }

          protected:
            const ordering& order_map;
          };

      } // namespace named_list

    namespace xquery
      {

        namespace
          {

            inline std::string build_node(const std::string& name)
              {
                return(name);
              }


            template <typename... nodes>
            std::string build_node(const std::string& name, nodes... more_nodes)
              {
                return(name + static_cast<std::string>("/") + build_node(more_nodes...));
              }

          }   // unnamed namepsace


        inline std::string document(const std::string& ctr, const std::string doc)
          {
            return(static_cast<std::string>("doc(\"dbxml:/") + ctr + static_cast<std::string>("/") + doc + static_cast<std::string>("\")"));
          }

        template <typename... nodes>
        std::string node_self(nodes... more_nodes)
          {
            return("./" + build_node(more_nodes...));
          }


        template <typename... nodes>
        std::string node_root(nodes... more_nodes)
          {
            return("/" + build_node(more_nodes...));
          }


        template <typename... nodes>
        std::string value_self(nodes... more_nodes)
          {
            return(static_cast<std::string>("distinct-values(") + node_self(more_nodes...) + static_cast<std::string>(")"));
          }


        template <typename... nodes>
        std::string value_root(nodes... more_nodes)
          {
            return(static_cast<std::string>("distinct-values(") + node_root(more_nodes...) + static_cast<std::string>(")"));
          }


        template <typename... nodes>
        std::string update(const std::string& doc_selector, const std::string& new_value, nodes... more_nodes)
          {
            return(static_cast<std::string>("replace value of node ") + doc_selector + static_cast<std::string>("/") + build_node(more_nodes...) + static_cast<std::string>(" with \"") + new_value + static_cast<std::string>("\""));
          }



        typedef enum { before, after, as_first, as_last, into } insertion_position;


        template <typename... nodes>
        std::string insert(const std::string& doc_selector, const std::string& new_node, insertion_position pos, nodes... more_nodes)
          {
            std::string keywords = "into";

            if(pos == before) keywords = "before";
            else if(pos == after) keywords = "after";
            else if(pos == as_first) keywords = "as first into";
            else if(pos == as_last) keywords = "as last into";
            else if(pos == into) keywords = "into";

            return(static_cast<std::string>("insert nodes ") + new_node + static_cast<std::string>(" ") + keywords + static_cast<std::string>(" ") + doc_selector + static_cast<std::string>("/") + build_node(more_nodes...));
          }

      } // namespace xquery


    inline
    DbXml::XmlValue extract_single_node(const std::string& query, DbXml::XmlManager* mgr, DbXml::XmlValue& value, const std::string& excpt_msg)
      {
        DbXml::XmlQueryContext ctx = mgr->createQueryContext();
        DbXml::XmlQueryExpression expr = mgr->prepare(query, ctx);

        DbXml::XmlValue node;
        try
          {
            DbXml::XmlResults results = expr.execute(value, ctx);

            if(results.size() != 1)
              {
                std::ostringstream msg;
                msg << excpt_msg << __CPP_TRANSPORT_RUN_REPAIR;
                throw transport::runtime_exception(transport::runtime_exception::REPOSITORY_BACKEND_ERROR, msg.str());
              }

            results.next(node);
          }
        catch (DbXml::XmlException& xe)
          {
            std::ostringstream msg;
            msg << excpt_msg << " (DBXML code=" << xe.getExceptionCode() << ": " << xe.what() << ")";
            throw transport::runtime_exception(transport::runtime_exception::REPOSITORY_ERROR, msg.str());
          }

        return(node);
      }


  }   // namespace dbxml_helper


#endif //__db_xml_H_
