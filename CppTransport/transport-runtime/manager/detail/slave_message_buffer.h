//
// Created by David Seery on 08/04/2016.
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

#ifndef CPPTRANSPORT_SLAVE_MESSAGE_BUFFER_H
#define CPPTRANSPORT_SLAVE_MESSAGE_BUFFER_H


#include <list>
#include <string>

#include "transport-runtime/manager/mpi_operations.h"

#include "boost/mpi.hpp"


namespace transport
  {

    class slave_message_buffer
      {

        // ASSOCIATED TYPES

        using notify_handler = std::function<void(const std::string&)>;

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! constructor caches communicator objects;
        //! notify_handler h is a callback used to journal messages in the logs
        slave_message_buffer(boost::mpi::environment& e, boost::mpi::communicator& w,
                             notify_handler h)
          : environment(e),
            world(w),
            handler(std::move(h))
          {
          }

        //! disable copying
        slave_message_buffer(const slave_message_buffer&) = delete;

        //! destructor pushes all messages to master process,
        //! and waits for replies before exiting
        ~slave_message_buffer();


        // INTERFACE

      public:

        //! Push context.
        //! In practice, context management is handled by the slave_message_context handler
        //! defined below.
        //! The context string itself can be any informative name, but clients usually set it
        //! equal to the currently executing task.
        void push_context(std::string c) { this->context.push_back(std::move(c)); }

        //! Pop context.
        //! In practice, context management is handled by the slave_message_context handler
        //! defined below.
        //! The context string itself can be any informative name, but clients usually set it
        //! equal to the currently executing task.
        void pop_context() { if(this->context.size() > 0) this->context.pop_back(); }

        //! add message
        void push_back(std::string m);


        // INTERNAL DATA

      private:

        //! cached MPI environment
        boost::mpi::environment& environment;

        //! cached MPI communicator
        boost::mpi::communicator& world;

        //! message buffer
        std::list<std::string> messages;

        //! context stack
        std::list<std::string> context;

        //! callback object, used eg. for journalling messages in a log
        notify_handler handler;

      };


    void slave_message_buffer::push_back(std::string m)
      {
        std::ostringstream msg;
        msg << m;

        if(this->context.size() > 0)
          {
            msg << " (";
            unsigned int count = 0;
            for(const auto& ctx : this->context)
              {
                if(count > 0) msg << ", ";
                msg << ctx;
                ++count;
              }
            msg << ")";
          }

        this->messages.emplace_back(msg.str());
      }


    slave_message_buffer::~slave_message_buffer()
      {
        // send requests synchronously so they appear in the correct order
        for(const std::string& message : this->messages)
          {
            // journal error message
            this->handler(message);

            // push message to master and await acknowledgment
            boost::mpi::request request = this->world.isend(MPI::RANK_MASTER, MPI::REPORT_ERROR, MPI::error_report(message));
            request.wait();
          }
      }


    //! slave_message_context is a context handler for slave_message_buffer
    //! It captures a slave_message_buffer object and a context string, and handles push/pop of the
    //! context. The context string itself can be any informative name, but clients usually
    //! set it equal to the currently executing task
    class slave_message_context
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! constructor pushes context
        slave_message_context(slave_message_buffer& b, std::string ctx);

        //! disable copying
        slave_message_context(const slave_message_context&) = delete;

        //! destructor pops context
        ~slave_message_context();


        // INTERNAL DATA

      private:

        //! message handler
        slave_message_buffer& buffer;

      };


    slave_message_context::slave_message_context(slave_message_buffer& b, std::string ctx)
      : buffer(b)
      {
        buffer.push_context(ctx);
      }


    slave_message_context::~slave_message_context()
      {
        this->buffer.pop_context();
      }

  }   // namespace transport


#endif //CPPTRANSPORT_SLAVE_MESSAGE_BUFFER_H
