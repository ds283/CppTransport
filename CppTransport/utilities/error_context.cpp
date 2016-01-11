//
// Created by David Seery on 01/12/2015.
// Copyright (c) 2013-2016 University of Sussex. All rights reserved.
//

#include <stdexcept>

#include "error_context.h"

#include "msg_en.h"


error_context::error_context(filestack& p, std::shared_ptr<std::string> ln, unsigned int cpos,
                             error_handler e, warning_handler w)
  : stack(p.clone()),
    line(std::move(ln)),
    character_position(cpos),
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


unsigned int error_context::get_position() const
  {
    if(!this->full_context) throw std::runtime_error(ERROR_NOT_FULL_CONTEXT);
    return(this->character_position);
  }
