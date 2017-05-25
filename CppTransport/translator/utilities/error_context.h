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

#ifndef CPPTRANSPORT_ERROR_CONTEXT_H
#define CPPTRANSPORT_ERROR_CONTEXT_H


#include <memory>
#include <functional>

#include "filestack.h"


class error_context
  {

    // HANDLER TYPES

  public:

    typedef std::function<void(const std::string&, const error_context&)> error_handler;
    typedef std::function<void(const std::string&, const error_context&)> warning_handler;


    // CONSTRUCTOR, DESTRUCTOR

  public:

    //! constructor -- with full context
    error_context(filestack& p, std::shared_ptr<std::string> ln, unsigned int sp, unsigned int ep,
                  error_handler ce, warning_handler cw);

    //! constructor -- just filestack context
    error_context(filestack& p, error_handler e, warning_handler w);

    //! destructor is default
    ~error_context() = default;


    // INTERFACE

  public:

    //! does this context have full information, ie. includes line and character position?
    bool has_full_context() const { return(this->full_context); }

    //! get filestack object
    const filestack& get_filestack() const;

    //! get line
    const std::string& get_line() const;

    //! get start position of context area
    size_t get_context_start_position() const;
    
    //! get end position of context area
    size_t get_context_end_position() const;


    // INTERFACE -- RAISE ERRORS

  public:

    //! raise error
    void error(const std::string& msg) const;

    //! raise wraning
    void warn(const std::string& msg) const;


    // INTERNAL DATA

  private:

    //! copy of filestack object (ownership shared with copies of this error context)
    std::shared_ptr<filestack> stack;

    //! shared pointer to source line (ownership shared with copies of this error context, and possibly shared with other error contexts)
    //! (typically this object is initially generated in a lexfile object)
    std::shared_ptr<std::string> line;

    //! start position of context area on source line
    unsigned int start_position;
    
    //! end position of context area on source line
    unsigned int end_position;

    //! type of error context available -- full (including line data and character position) or partial (just a filestack)?
    bool full_context;


    // CONTEXT ERROR HANDLERS

    //! error handler object
    error_handler err_handle;

    //! warning handler object
    warning_handler wrn_handle;

  };


#endif //CPPTRANSPORT_ERROR_CONTEXT_H
