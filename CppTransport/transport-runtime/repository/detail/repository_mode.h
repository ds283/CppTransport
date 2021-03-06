//
// Created by David Seery on 03/04/2016.
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

#ifndef CPPTRANSPORT_REPOSITORY_MODE_H
#define CPPTRANSPORT_REPOSITORY_MODE_H


namespace transport
  {

    // specify read/write mode for an entire repository
    enum class repository_mode
      {
        readonly,
        readwrite
      };


    inline std::string to_string(repository_mode m)
      {
        switch(m)
          {
            case repository_mode::readonly:
              {
                return std::string(CPPTRANSPORT_REPOSITORY_MODE_READONLY);
              }

            case repository_mode::readwrite:
              {
                return std::string(CPPTRANSPORT_REPOSITORY_MODE_READWRITE);
              }
          }
      }

  }


#endif //CPPTRANSPORT_REPOSITORY_MODE_H
