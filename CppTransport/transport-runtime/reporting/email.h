//
// Created by David Seery on 28/09/2016.
// --@@
// Copyright (c) 2016 University of Sussex. All rights reserved.
//
// This file is part of the CppTransport platform.
//
// CppTransport is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 2 of the License, or
// (at your option) any later version.
//
// CppTransport is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with CppTransport.  If not, see <http://www.gnu.org/licenses/>.
//
// @license: GPL-2
// @contributor: David Seery <D.Seery@sussex.ac.uk>
// --@@
//

#ifndef CPPTRANSPORT_EMAIL_H
#define CPPTRANSPORT_EMAIL_H


#include "transport-runtime/manager/environment.h"
#include "transport-runtime/manager/argument_cache.h"

#include "transport-runtime/utilities/host_information.h"

#include "transport-runtime/build_data.h"


namespace transport
  {
    
    namespace reporting
      {
    
        class email
          {
          
            // CONSTRUCTOR, DESTRUCTOR
            
          public:
            
            //! constructor
            email(local_environment& e, argument_cache& a);
            
            //! destructor is default
            ~email() = default;
            
            
            // INTERFACE -- CONTENT
            
          public:
    
            //! add address to 'to' field
            email& add_to(std::string to);
            
            //! set list of 'to' addresses
            email& set_to(std::list<std::string> to);
            
            //! set 'subject' field
            email& set_subject(std::string subject);
            
            //! set 'body' field
            email& set_body(std::string body);
            
            
            // INTERFACE -- OPERATIONS
            
          public:
            
            //! send email - no writer specified
            bool send() const;
            
            //! send email - writer specified for logging
            template <typename WriterObject>
            bool send(WriterObject& writer) const;
            
          private:
            
            //! build ; delimited recipients list
            std::string build_recipient_list() const;
            
            //! compose to field
            std::string compose_to() const;
            
            //! compose subject field
            std::string compose_subject() const;
            
            //! compose body
            std::string compose_body() const;

            
            // INTERNAL DATA
            
          private:
            
            //! to distribution list
            std::list< std::string > to;
            
            //! subject field
            std::string subject;
            
            //! message body
            std::string body;
            
            
            // AGENTS AND POLICY CLASSES
            
            //! reference to local environment
            local_environment& local_env;
            
            //! reference to argument cache
            argument_cache& arg_cache;
            
            //! host information
            host_information host_data;
        
          };
    
    
        email::email(local_environment& e, argument_cache& a)
          : local_env(e),
            arg_cache(a)
          {
          }
    
    
        email& email::add_to(std::string to)
          {
            this->to.emplace_back(std::move(to));
            return *this;
          }
    
    
        email& email::set_to(std::list<std::string> to)
          {
            this->to = std::move(to);
            return *this;
          }
    
    
        email& email::set_subject(std::string subject)
          {
            this->subject = std::move(subject);
            return *this;
          }
    
    
        email& email::set_body(std::string body)
          {
            this->body = std::move(body);
            return *this;
          }
    
    
        bool email::send() const
          {
            if(!this->local_env.has_sendmail()) return false;

            return this->local_env.execute_sendmail(this->compose_body(), this->build_recipient_list()) == EXIT_SUCCESS;
          }
    
    
        template <typename WriterObject>
        bool email::send(WriterObject& writer) const
          {
            bool rval = this->send();
        
            if(!rval)
              {
                BOOST_LOG_SEV(writer.get_log(), generic_writer::log_severity_level::error) << "!! Failed to send periodic report to '" << this->build_recipient_list() << "'";
              }
        
            return rval;
          }
    
    
        std::string email::compose_body() const
          {
            std::ostringstream body_text;
            
            body_text << "Subject: " << this->compose_subject() << '\n';
            
            if(!build_data::email_from.empty())
              {
                body_text << "From: " << build_data::email_from << '\n';
              }
            
            body_text << "To: " << this->compose_to() << '\n';
            
            body_text << "[" << CPPTRANSPORT_EMAIL_ISSUED_BY
                      << " " << this->local_env.get_userid()
                      << "@" << this->host_data.get_host_name()
                      << " " << CPPTRANSPORT_RUNTIME_API << "]" << '\n' << '\n';
            body_text << this->body;
            
            return body_text.str();
          }
    
    
        std::string email::build_recipient_list() const
          {
            std::ostringstream to_text;
            unsigned int count = 0;
            for(const std::string& addr : this->to)
              {
                if(count > 0) to_text << ";";
                to_text << addr;
                ++count;
              }
            
            return to_text.str();
          }
        
        
        std::string email::compose_to() const
          {
            std::ostringstream to_text;
            unsigned int count = 0;
            for(const std::string& addr : this->to)
              {
                if(count > 0) to_text << ", ";
                to_text << addr;
                ++count;
              }
            
            return to_text.str();
          }
    
    
        std::string email::compose_subject() const
          {
            std::ostringstream subject_text;
            subject_text << "[" << CPPTRANSPORT_NAME << "] " << this->subject;
            
            return subject_text.str();
          }
    
      }   // namespace reporting
    
  }   // namespace transport


#endif //CPPTRANSPORT_EMAIL_H
