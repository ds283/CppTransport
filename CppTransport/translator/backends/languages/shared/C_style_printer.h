//
// Created by David Seery on 09/12/2013.
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
// @contributor: Alessandro Maraio <am963@sussex.ac.uk>
// --@@
//


#ifndef CPPTRANSPORT_C_STYLE_PRINTER_H
#define CPPTRANSPORT_C_STYLE_PRINTER_H


#include "language_printer.h"


constexpr auto         DEFAULT_C_STYLE_COMMENT_PREFIX        = "//";
constexpr auto         DEFAULT_C_STYLE_COMMENT_POSTFIX       = "";
constexpr auto         DEFAULT_C_STYLE_COMMENT_PAD           = " ";

constexpr auto         DEFAULT_C_STYLE_FOR_KEYWORD           = "for";
constexpr auto         DEFAULT_C_STYLE_LOOP_TYPE             = "unsigned int";

constexpr auto         DEFAULT_C_STYLE_OPEN_BRACE            = "{";
constexpr auto         DEFAULT_C_STYLE_CLOSE_BRACE           = "}";
constexpr unsigned int DEFAULT_C_STYLE_BRACE_INDENT          = 1;
constexpr unsigned int DEFAULT_C_STYLE_BLOCK_INDENT          = 3;

constexpr auto         DEFAULT_C_STYLE_ARRAY_OPEN            = "[";
constexpr auto         DEFAULT_C_STYLE_ARRAY_CLOSE           = "]";

constexpr auto         DEFAULT_C_STYLE_FUNCTION_OPEN         = "(";
constexpr auto         DEFAULT_C_STYLE_FUNCTION_CLOSE        = ")";
constexpr auto         DEFAULT_C_STYLE_ARGUMENT_SEPARATOR    = ",";

constexpr auto         DEFAULT_C_STYLE_INITIALIZER_OPEN      = "{";
constexpr auto         DEFAULT_C_STYLE_INITIALIZER_CLOSE     = "}";
constexpr auto         DEFAULT_C_STYLE_INITIALIZER_SEPARATOR = ",";

constexpr auto         DEFAULT_C_STYLE_RETURN_KEYWORD        = "return";
constexpr auto         DEFAULT_C_STYLE_IF_KEYWORD            = "if";
constexpr auto         DEFAULT_C_STYLE_ELSE_KEYWORD          = "else";

constexpr auto         DEFAULT_C_STYLE_LAMBDA_CAPTURE        = "[&]";
constexpr auto         DEFAULT_C_STYLE_LAMBDA_ARGUMENT_TYPE  = "unsigned int";
constexpr auto         DEFAULT_C_STYLE_LAMBDA_RETURN_TYPE    = "->";

constexpr auto         DEFAULT_C_STYLE_SEMICOLON             = ";";


//! policy class for output to C-style languages, eg. C++, VexCL, CUDA, OpenCL
class C_style_printer: public language_printer
  {

    // CONSTRUCTOR, DESTRUCTOR

  public:

    //! constructor
    C_style_printer(std::string cpre = DEFAULT_C_STYLE_COMMENT_PREFIX,
                    std::string cpost = DEFAULT_C_STYLE_COMMENT_POSTFIX,
                    std::string cpad = DEFAULT_C_STYLE_COMMENT_PAD,
                    std::string fk = DEFAULT_C_STYLE_FOR_KEYWORD,
                    std::string lt = DEFAULT_C_STYLE_LOOP_TYPE,
                    std::string opb = DEFAULT_C_STYLE_OPEN_BRACE,
                    std::string clb = DEFAULT_C_STYLE_CLOSE_BRACE,
                    unsigned int brc_ind = DEFAULT_C_STYLE_BRACE_INDENT,
                    unsigned int blk_ind = DEFAULT_C_STYLE_BLOCK_INDENT,
                    std::string ao = DEFAULT_C_STYLE_ARRAY_OPEN,
                    std::string ac = DEFAULT_C_STYLE_ARRAY_CLOSE,
                    std::string fo = DEFAULT_C_STYLE_FUNCTION_OPEN,
                    std::string fc = DEFAULT_C_STYLE_FUNCTION_CLOSE,
                    std::string as = DEFAULT_C_STYLE_ARGUMENT_SEPARATOR,
                    std::string io = DEFAULT_C_STYLE_INITIALIZER_OPEN,
                    std::string ic = DEFAULT_C_STYLE_INITIALIZER_CLOSE,
                    std::string is = DEFAULT_C_STYLE_INITIALIZER_SEPARATOR,
                    std::string rk = DEFAULT_C_STYLE_RETURN_KEYWORD,
                    std::string ik = DEFAULT_C_STYLE_IF_KEYWORD,
                    std::string ek = DEFAULT_C_STYLE_ELSE_KEYWORD,
                    std::string lc = DEFAULT_C_STYLE_LAMBDA_CAPTURE,
                    std::string la = DEFAULT_C_STYLE_LAMBDA_ARGUMENT_TYPE,
                    std::string lr = DEFAULT_C_STYLE_LAMBDA_RETURN_TYPE,
                    std::string sc = DEFAULT_C_STYLE_SEMICOLON)
      : comment_prefix(std::move(cpre)),
        comment_postfix(std::move(cpost)),
        comment_pad(std::move(cpad)),
        for_keyword(std::move(fk)),
        loop_type(std::move(lt)),
        open_brace(std::move(opb)),
        close_brace(std::move(clb)),
        brace_indent(brc_ind),
        block_indent(blk_ind),
        array_open(std::move(ao)),
        array_close(std::move(ac)),
        function_open(std::move(fo)),
        function_close(std::move(fc)),
        argument_sep(std::move(as)),
        initializer_open(std::move(io)),
        initializer_close(std::move(ic)),
        initializer_sep(std::move(is)),
        return_keyword(std::move(rk)),
        if_keyword(std::move(ik)),
        else_keyword(std::move(ek)),
        lambda_capture(std::move(lc)),
        lambda_argument_type(std::move(la)),
        lambda_return(std::move(lr)),
        semicolon(std::move(sc))
      {
      }

    //! destructor is default
    virtual ~C_style_printer() = default;


    // INTERFACE -- PRINT TO STRING
    // implements a 'language_printer' interface

  public:

    //! print a GiNaC expression to a string
    virtual std::string ginac(const GiNaC::ex& expr) const override;

    //! print a comment to a string
    virtual std::string comment(const std::string tag) const override;


    // INTERFACE -- FORMATTING

  public:

    //! return delimiter for start-of-block, if used in this language
    virtual boost::optional< std::string > get_start_block_delimiter() const override;

    //! return delimiter for end-of-block, if used in this language
    virtual boost::optional< std::string > get_end_block_delimiter() const override;

    //! return indent-level for start-of-block delimitere
    virtual unsigned int get_block_delimiter_indent() const override;

    //! return indent-level for block
    virtual unsigned int get_block_indent() const override;


    // INTERFACE -- CONTROL STRUCTURES

  public:

    //! generate a 'for' loop
    virtual std::string for_loop(const std::string& loop_variable, unsigned int min, unsigned int max) const override;


    // INTERFACE -- ARRAY SUBSCRIPTING
  
  protected:
    
    //! format an array subscript
    std::string
    format_array_subscript(const std::string& kernel, const std::initializer_list<std::string> args,
                               const boost::optional<std::string>& flatten) const override;


    // INTERFACE -- INITIALIZATION LISTS

  public:

    //! generate initialization list from a set of strings
    virtual std::string initialization_list(const std::vector<std::string>& list, bool quote) const override;

    //! generate CpptSample style list of fields from a set of strings
    virtual std::string cpptsample_fields_list(const std::vector<std::string>& list, bool init) const override;


    // INTERFACE -- LAMBDAS

  public:

    //! format a lambda invokation string, remembering that the indices it is being invoked with (supplied in
    //! 'indices') may be different from the indices it was defined with
    virtual std::string lambda_invokation(const std::string& name, const generic_lambda& lambda,
                                          const abstract_index_database& indices) const override;

    //! format a lambda definition open string
    virtual std::string open_lambda(const abstract_index_database& indices, const std::string& return_type) const override;

    //! format a lambda definition close string
    virtual std::string close_lambda() const override;

    //! format a return statement
    virtual std::string format_return(const GiNaC::ex& expr) const override;
    virtual std::string format_return(const std::string& expr) const override;

    //! format an if statement
    virtual std::string format_if(const std::list<GiNaC::ex>& conditions) const override;

    //! format an else-if statement
    virtual std::string format_elseif(const std::list<GiNaC::ex>& conditions) const override;

    //! format an else statement
    virtual std::string format_else(const GiNaC::ex& expr) const override;

  protected:

    //! format a generic if-type statement
    std::string format_if_stmt(const std::string& kw, const std::list<GiNaC::ex>& conditions) const;


    // INTERNAL DATA

  private:

    // COMMENT DATA

    //! comment prefix string
    std::string comment_prefix;

    //! comment postfix string
    std::string comment_postfix;

    //! comment pad string
    std::string comment_pad;


    // CODE GENERATION DATA

    //! open brace
    std::string open_brace;

    //! close brace
    std::string close_brace;

    //! brace-level indent
    unsigned int brace_indent;

    //! block-level indent
    unsigned int block_indent;

    //! array open subscript
    std::string array_open;

    //! array close subscript
    std::string array_close;

    //! function invokation open
    std::string function_open;

    //! argument list separator
    std::string argument_sep;

    //! function invokation close
    std::string function_close;

    //! keyword used for 'for' loop
    std::string for_keyword;

    //! variable type used for 'for' loop
    std::string loop_type;

    //! initializer list open
    std::string initializer_open;

    //! initializer list close
    std::string initializer_close;

    //! initializer list separator
    std::string initializer_sep;

    //! keyword used for 'return'
    std::string return_keyword;

    //! keyword used for 'if'
    std::string if_keyword;

    //! keyword used for 'else if'
    std::string else_keyword;

    //! lambda-capture header
    std::string lambda_capture;

    //! lambda argument type
    std::string lambda_argument_type;

    //! lambda return type indicator
    std::string lambda_return;

    //! semicolon
    std::string semicolon;

  };


#endif //CPPTRANSPORT_C_STYLE_PRINTER_H
