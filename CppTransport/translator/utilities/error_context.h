//
// Created by David Seery on 01/12/2015.
// Copyright (c) 2013-2016 University of Sussex. All rights reserved.
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
    error_context(filestack& p, std::shared_ptr<std::string> ln, unsigned int cpos,
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

    //! get character position
    unsigned int get_position() const;


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

    //! character position
    unsigned int character_position;

    //! type of error context available -- full (including line data and character position) or partial (just a filestack)?
    bool full_context;


    // CONTEXT ERROR HANDLERS

    //! error handler object
    error_handler err_handle;

    //! warning handler object
    warning_handler wrn_handle;

  };


#endif //CPPTRANSPORT_ERROR_CONTEXT_H
