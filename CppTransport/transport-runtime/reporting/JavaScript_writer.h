//
// Created by David Seery on 31/03/2016.
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

#ifndef CPPTRANSPORT_JAVASCRIPT_WRITER_H
#define CPPTRANSPORT_JAVASCRIPT_WRITER_H


#include <iostream>
#include <fstream>
#include <sstream>

#include "transport-runtime/messages.h"
#include "transport-runtime/defaults.h"
#include "transport-runtime/exceptions.h"

#include "boost/filesystem/operations.hpp"


namespace transport
  {

    namespace reporting
      {

        class JavaScript_writer
          {

            // CONSTRUCTOR, DESTRUCTOR

          public:

            //! constructor
            JavaScript_writer(boost::filesystem::path p);

            //! destructor closes file
            ~JavaScript_writer();


            // INTERFACE

          public:

            //! write a line to the file
            void write(std::string line);


            // INTERNAL DATA

          private:

            //! output file name
            const boost::filesystem::path output_file;

            //! output handle
            std::ofstream out;

          };


        JavaScript_writer::JavaScript_writer(boost::filesystem::path p)
          : output_file(p)
          {
            out.open(output_file.string(), std::ios::out | std::ios::trunc);

            if(!out.is_open() || out.fail())
              {
                std::ostringstream msg;
                msg << CPPTRANSPORT_HTML_CANT_OPEN_FILE << " '" << output_file.string() << "'";
                throw runtime_exception(exception_type::REPORTING_ERROR, msg.str());
              }
          }


        JavaScript_writer::~JavaScript_writer()
          {
            this->out.close();
          }


        void JavaScript_writer::write(std::string line)
          {
            this->out << line << '\n';
          }


      }   // namespace reporting

  }   // namespace transport


#endif //CPPTRANSPORT_JAVASCRIPT_WRITER_H
