//
// Created by David Seery on 27/12/2013.
// Copyright (c) 2013 University of Sussex. All rights reserved.
//


#ifndef __xml_serializable_H_
#define __xml_serializable_H_


#include <list>
#include <string>

#include "boost/lexical_cast.hpp"

#include "dbxml/DbXml.hpp"

#include "transport/db-xml/db_xml.h"


namespace transport
  {

    class xml_serializable
      {
      public:
        virtual ~xml_serializable()
          {
          }

        // SERIALIZATION METHOD

        virtual void serialize_xml(DbXml::XmlEventWriter& writer) = 0;

        // STANDARD SERIALIZATION METHODS

      protected:

        // begin XML node (its type is recorded on the stack)
        void begin_node(DbXml::XmlEventWriter& writer, const std::string& name, bool empty=false);

        // end the current XML node
        void end_node(DbXml::XmlEventWriter& writer, const std::string& name);

        // write a node with a single value type
        template <typename T>
        void write_value_node(DbXml::XmlEventWriter& writer, const std::string& name, T value);

        // INTERNAL DATA

      private:

        std::list<std::string> xml_stack;
      };


    void xml_serializable::begin_node(DbXml::XmlEventWriter& writer, const std::string& name, bool empty)
      {
        this->xml_stack.push_front(name);

        writer.writeStartElement(__CPP_TRANSPORT_DBXML_STRING(name.c_str()), nullptr, nullptr, 0, empty);
      }


    void xml_serializable::end_node(DbXml::XmlEventWriter& writer, const std::string& name)
      {
        if(this->xml_stack.size() > 0 && this->xml_stack.front() == name)
          {
            this->xml_stack.pop_front();

            writer.writeEndElement(__CPP_TRANSPORT_DBXML_STRING(name.c_str()), nullptr, nullptr);
          }
        else
          {
            throw std::invalid_argument(__CPP_TRANSPORT_XML_MISMATCH);
          }
      }


    template <typename T>
    void xml_serializable::write_value_node(DbXml::XmlEventWriter& writer, const std::string& name, T value)
      {
        this->begin_node(writer, name);

        std::string val = boost::lexical_cast<std::string>(value);
        writer.writeText(DbXml::XmlEventReader::XmlEventType::Characters, __CPP_TRANSPORT_DBXML_STRING(val.c_str()), val.length());

        this->end_node(writer, name);
      }

  }


#endif //__xml_serializable_H_
