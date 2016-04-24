//
// Created by David Seery on 18/12/2015.
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


#include <vector>
#include <sstream>

#include "backend_data.h"
#include "msg_en.h"

#include "boost/algorithm/string.hpp"
#include "boost/lexical_cast.hpp"


constexpr auto BACKEND_TOKEN = "backend";
constexpr auto MINVER_TOKEN  = "minver";


backend_data::backend_data(std::ifstream& inf, const boost::filesystem::path& in, error_context& err_context)
  : type(model_type::canonical)
  {
    std::string line;
    std::getline(inf, line);

    std::vector<std::string> tokens;
    boost::split(tokens, line, boost::is_any_of(" ,:;="));

    if(tokens.size() <= 1)
      {
        std::ostringstream msg;
        msg << ERROR_IMPROPER_TEMPLATE_HEADER << " " << in;
        err_context.error(msg.str());
      }

    for(int i = 1; i < tokens.size(); ++i)
      {
        if(boost::to_lower_copy(tokens[i]) == BACKEND_TOKEN)
          {
            if(i + 1 < tokens.size())
              {
                if(name)
                  {
                    std::ostringstream msg;
                    msg << WARNING_DUPLICATE_TEMPLATE_BACKEND << " " << in;
                    err_context.warn(msg.str());
                  }
                else
                  {
                    name = tokens[++i];
                  }
              }
            else
              {
                std::ostringstream msg;
                msg << ERROR_EXPECTED_TEMPLATE_BACKEND << " " << in;
                err_context.error(msg.str());
              }
          }
        else if(boost::to_lower_copy(tokens[i]) == MINVER_TOKEN)
          {
            if(i + 1 < tokens.size())
              {
                if(minver)
                  {
                    std::ostringstream msg;
                    msg << WARNING_DUPLICATE_TEMPLATE_MINVER << " " << in;
                    err_context.warn(msg.str());
                  }
                else
                  {
                    minver = boost::lexical_cast<double>(tokens[++i]);
                  }
              }
            else
              {
                std::ostringstream msg;
                msg << ERROR_EXPECTED_TEMPLATE_MINVER << " " << in;
                err_context.error(msg.str());
              }
          }
      }

    if(!name || !minver)
      {
        std::ostringstream msg;
        msg << ERROR_IMPROPER_TEMPLATE_HEADER << " " << in;
        err_context.error(msg.str());
      }
  }
