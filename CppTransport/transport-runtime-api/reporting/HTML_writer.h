//
// Created by David Seery on 31/03/2016.
// Copyright (c) 2016 University of Sussex. All rights reserved.
//

#ifndef CPPTRANSPORT_HTML_WRITER_H
#define CPPTRANSPORT_HTML_WRITER_H


#include <iostream>
#include <fstream>
#include <sstream>

#include <stack>
#include <map>

#include "transport-runtime-api/messages.h"
#include "transport-runtime-api/defaults.h"
#include "transport-runtime-api/exceptions.h"

#include "boost/filesystem/operations.hpp"


namespace transport
  {

    namespace reporting
      {

        class HTML_element
          {

            // CONSTRUCTOR, DESTRUCTOR

          public:

            //! constructor is default
            HTML_element() = default;

            //! destructor is default
            virtual ~HTML_element() = default;


            // INTERFACE

          public:

            //! write self to a stream
            virtual void write(std::ostream& out, const std::string& indent) const = 0;

            //! clone self
            virtual HTML_element* clone() const = 0;

          };


        class HTML_string: public HTML_element
          {

            // CONSTRUCTOR, DESTRUCTOR

          public:

            //! constructor
            HTML_string(std::string v)
              : value(std::move(v)),
                is_bold(false)
              {
              }

            //! destructor is default
            virtual ~HTML_string() = default;


            // INTERFACE

          public:

            //! mark as bold
            void bold() { this->is_bold = true; }


            // INTERFACE -- from HTML_element

          public:

            //! write self to stream
            void write(std::ostream& out, const std::string& indent) const override final;

            //! clone self
            HTML_string* clone() const override final { return new HTML_string(static_cast<const HTML_string&>(*this)); }


            // INTERNAL DATA

          private:

            //! value
            std::string value;

            //! typeset string in bold?
            bool is_bold;

          };


        void HTML_string::write(std::ostream& out, const std::string& indent) const
          {
            out << indent;
            if(this->is_bold) out << "<b>";
            out << value;
            if(this->is_bold) out << "</b>";
            out << '\n';
          }


        class HTML_node: public HTML_element
          {

            // CONSTRUCTOR, DESTRUCTOR

          public:

            //! constructor
            HTML_node(std::string t, bool c=true)
              : tag(std::move(t)),
                has_closing_tag(c)
              {
              }

            //! convenience constructor for node containing a string
            HTML_node(std::string t, std::string e, bool c=true)
              : HTML_node(std::move(t), c)
              {
                HTML_string str(e);
                this->add_element(str);
              }

            //! convenience construct for node containing a string
            //! const char* version for string literals; otherwise they implicitly convert to bool
            HTML_node(std::string t, const char* e, bool c=true)
              : HTML_node(t, std::string(e), c)
              {
              }

            //! destructor is default
            virtual ~HTML_node() = default;


            // INTERFACE

          public:

            //! add an attribute with value
            HTML_node& add_attribute(std::string name, std::string value);

            //! add an attribute without value
            HTML_node& add_attribute(std::string name);

            //! add an element to this node's content
            HTML_node& add_element(const HTML_element& element);


            // INTERFACE -- from HTML_element

          public:

            //! write self to stream
            void write(std::ostream& out, const std::string& indent) const override final;

            //! clone self
            HTML_node* clone() const override final { return new HTML_node(static_cast<const HTML_node&>(*this)); }


            // INTERNAL DATA

          private:

            //! HTML tag
            const std::string tag;

            //! does this element require a closing tag?
            const bool has_closing_tag;

            //! typedef for attribute database
            typedef std::map< std::string, std::string > attribute_list;

            //! list of attributes
            attribute_list attributes;

            //! node content; elements are managed using shared pointers,
            //! so ownership is shared with any other HTML_nodes which
            //! contain the same elements
            std::list< std::shared_ptr<HTML_element> > content;

          };


        HTML_node& HTML_node::add_attribute(std::string name, std::string value)
          {
            // insertion will have no effect if an existing attribute with the same name exists;
            // in that case, the earliest provided attribute is used
            this->attributes.insert(std::make_pair(name, value));

            return(*this);
          }


        HTML_node& HTML_node::add_attribute(std::string name)
          {
            // insertion will have no effect if an existing attribute with the same name exists;
            // in that case, the earliest provided attribute is used
            this->attributes.insert(std::make_pair(name, std::string()));

            return(*this);
          }


        HTML_node& HTML_node::add_element(const HTML_element& element)
          {
            this->content.emplace_back(std::shared_ptr<HTML_element>(element.clone()));
            return(*this);
          }


        void HTML_node::write(std::ostream& out, const std::string& indent) const
          {
            // write opening tag with attributes if present
            out << indent << "<" << tag;

            for(const attribute_list::value_type& attribute : this->attributes)
              {
                if(attribute.second.empty())
                  {
                    out << " " << attribute.first;
                  }
                else
                  {
                    out << " " << attribute.first << "=\"" << attribute.second << "\"";
                  }
              }

            out << ">" << '\n';

            // write content, with new indent given by old indent plus standard 4 spaces
            std::string new_indent = indent + "    ";
            for(const std::shared_ptr<HTML_element>& element : this->content)
              {
                element->write(out, new_indent);
              }

            // write closing tag
            if(this->has_closing_tag)
              {
                out << indent << "</" << tag << ">" << '\n';
              }
          }


        class HTML_writer
          {

            // CONSTRUCTOR, DESTRUCTOR

          public:

            //! constructor
            HTML_writer(boost::filesystem::path p, std::string t);

            //! destructor commits everything to the file
            ~HTML_writer();


            // INTERFACE

          public:

            //! supply a HTML element representing the document body
            void add_body(HTML_element& root);

            //! add a stylesheet
            void add_stylesheet(boost::filesystem::path p);

            //! add a JavaScript file
            void add_JavaScript(boost::filesystem::path p);


            // INTERNAL API

          protected:

            //! write HTML header section
            void write_header(const std::string& indent);

            //! write HTML body section
            void write_body(const std::string& indent);


            // INTERNAL DATA

          private:

            //! output file name
            const boost::filesystem::path output_file;

            //! document title
            const std::string title;

            //! output handle
            std::ofstream out;

            //! list of stylesheets
            std::list< boost::filesystem::path > stylesheets;

            //! list of JavaScripts
            std::list< boost::filesystem::path > scripts;

            //! HTML element representing the document body
            std::unique_ptr< HTML_element > body_element;

          };


        HTML_writer::HTML_writer(boost::filesystem::path p, std::string t)
          : output_file(p),
            title(std::move(t))
          {
            out.open(output_file.string(), std::ios::out | std::ios::trunc);

            if(!out.is_open() || out.fail())
              {
                std::ostringstream msg;
                msg << CPPTRANSPORT_HTML_CANT_OPEN_FILE << " '" << output_file.string() << "'";
                throw runtime_exception(exception_type::REPORTING_ERROR, msg.str());
              }
          }


        HTML_writer::~HTML_writer()
          {
            // write header declaring this to be HTML5 content
            this->out << "<!DOCTYPE html>" << '\n';

            //! write main HTML tag
            this->out << "<html lang=\"en\">" << '\n';

            // write header followed by body, each with a standard 4-space indent
            this->write_header("    ");
            this->write_body("    ");

            //! close main HTML tag
            this->out << "</html>" << '\n';

            //! close stream
            this->out.close();
          }


        void HTML_writer::write_header(const std::string& indent)
          {
            HTML_node head("head");

            for(const boost::filesystem::path& p : this->stylesheets)
              {
                HTML_node script("link", false);
                script.add_attribute("rel", "stylesheet").add_attribute("type", "text/css").add_attribute("href",
                                                                                                          p.string());
                head.add_element(script);
              }

            head.write(this->out, indent);
          }


        void HTML_writer::write_body(const std::string& indent)
          {
            HTML_node body("body");
            body.add_attribute("role", "document");

            // SET UP TITLE, CHARSET AND VIEWPORT ELEMENTS

            HTML_node title("title", this->title);

            HTML_node charset("meta", false);
            charset.add_attribute("charset", "utf-8");

            HTML_node viewport("meta", false);
            viewport.add_attribute("name", "viewport").add_attribute("content", "width=device-width, initial-scale=1");

            body.add_element(title).add_element(charset).add_element(viewport);

            // ATTACH DOCUMENT BODY IF IT EXISTS

            if(this->body_element) body.add_element(*this->body_element);

            // ATTACH FOOTER TO LOAD JAVASCRIPT FILES
            // (comes at end so pages load faster)

            for(const boost::filesystem::path& p : this->scripts)
              {
                HTML_node script("script");
                script.add_attribute("src", p.string());
                body.add_element(script);
              }

            body.write(this->out, indent);
          }


        void HTML_writer::add_body(HTML_element& root)
          {
            this->body_element.reset(root.clone());
          }


        void HTML_writer::add_stylesheet(boost::filesystem::path p)
          {
            this->stylesheets.emplace_back(std::move(p));
          }


        void HTML_writer::add_JavaScript(boost::filesystem::path p)
          {
            this->scripts.emplace_back(std::move(p));
          }


      }   // namespace reporting

  }   // namespace transport

#endif //CPPTRANSPORT_HTML_WRITER_H
