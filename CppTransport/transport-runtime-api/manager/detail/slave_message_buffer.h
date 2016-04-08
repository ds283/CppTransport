//
// Created by David Seery on 08/04/2016.
// Copyright (c) 2016 University of Sussex. All rights reserved.
//

#ifndef CPPTRANSPORT_SLAVE_MESSAGE_BUFFER_H
#define CPPTRANSPORT_SLAVE_MESSAGE_BUFFER_H


#include <list>
#include <string>

#include "transport-runtime-api/manager/mpi_operations.h"

#include "boost/mpi.hpp"


namespace transport
  {

    class slave_message_buffer
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! constructor caches communicator objects
        slave_message_buffer(boost::mpi::environment& e, boost::mpi::communicator& w)
          : environment(e),
            world(w)
          {
          }

        //! disable copying
        slave_message_buffer(const slave_message_buffer&) = delete;

        //! destructor pushes all messages to master
        ~slave_message_buffer();


        // INTERFACE

      public:

        //! add context
        void push_context(std::string c) { this->context.push_back(std::move(c)); }

        //! pop context
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

      };


    void slave_message_buffer::push_back(std::string m)
      {
        std::ostringstream msg;
        msg << m;

        if(this->context.size() > 0)
          {
            msg << " (";
            unsigned int count = 0;
            for(const std::string& ctx : this->context)
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
            boost::mpi::request request = this->world.isend(MPI::RANK_MASTER, MPI::REPORT_ERROR, MPI::error_report(message));
            request.wait();
          }
      }


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
