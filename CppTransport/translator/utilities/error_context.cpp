//
// Created by David Seery on 01/12/2015.
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

#include <stdexcept>

#include "error_context.h"

#include "msg_en.h"


error_context::error_context(filestack& p, std::shared_ptr<std::string> ln, unsigned int sp, unsigned int ep,
                             error_handler e, warning_handler w)
  : stack(p.clone()),
    line(std::move(ln)),
    start_position(sp),
    end_position(ep),
    err_handle(std::move(e)),
    wrn_handle(std::move(w)),
    full_context(true)
  {
  }


error_context::error_context(filestack& p, error_context::error_handler e, error_context::warning_handler w)
  : stack(p.clone()),
    err_handle(std::move(e)),
    wrn_handle(std::move(w)),
    full_context(false)
  {
  }


void error_context::error(const std::string& msg) const
  {
    if(!this->err_handle) throw std::runtime_error(ERROR_NO_ERROR_HANDLER);
    this->err_handle(msg, *this);
  }


void error_context::warn(const std::string& msg) const
  {
    if(!this->wrn_handle) throw std::runtime_error(ERROR_NO_WARNING_HANDLER);
    this->wrn_handle(msg, *this);
  }


const filestack& error_context::get_filestack() const
  {
    if(!this->stack) throw std::runtime_error(ERROR_NO_STACK_REGISTERED);
    return(*this->stack);
  }


const std::string& error_context::get_line() const
  {
    if(!this->line || !this->full_context) throw std::runtime_error(ERROR_NOT_FULL_CONTEXT);
    return(*this->line);
  }


size_t error_context::get_context_start_position() const
  {
    if(!this->full_context) throw std::runtime_error(ERROR_NOT_FULL_CONTEXT);

    if(this->start_position < this->line->length()) return this->start_position;
    return this->line->length()-1;
  }


size_t error_context::get_context_end_position() const
  {
    if(!this->full_context) throw std::runtime_error(ERROR_NOT_FULL_CONTEXT);

    if(this->end_position > this->start_position && this->end_position < this->line->length()+1) return this->end_position;
    if(this->start_position < this->line->length()) return this->start_position+1;
    return this->line->length();
  }
