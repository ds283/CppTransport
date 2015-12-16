//
// Created by David Seery on 09/12/2013.
// Copyright (c) 2013-15 University of Sussex. All rights reserved.
//


#ifndef CPPTRANSPORT_C_STYLE_PRINTER_H
#define CPPTRANSPORT_C_STYLE_PRINTER_H


#include "language_printer.h"


constexpr auto         DEFAULT_C_STYLE_COMMENT_PREFIX   = "//";
constexpr auto         DEFAULT_C_STYLE_COMMENT_POSTFIX  = "";
constexpr auto         DEFAULT_C_STYLE_COMMENT_PAD      = " ";

constexpr auto         DEFAULT_C_STYLE_FOR_KEYWORD      = "for";
constexpr auto         DEFAULT_C_STYLE_LOOP_TYPE        = "unsigned int";

constexpr auto         DEFAULT_C_STYLE_OPEN_BRACE       = "{";
constexpr auto         DEFAULT_C_STYLE_CLOSE_BRACE      = "}";
constexpr unsigned int DEFAULT_C_STYLE_BRACE_INDENT     = 1;
constexpr unsigned int DEFAULT_C_STYLE_BLOCK_INDENT     = 3;


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
                    unsigned int blk_ind = DEFAULT_C_STYLE_BLOCK_INDENT)
      : comment_prefix(std::move(cpre)),
        comment_postfix(std::move(cpost)),
        comment_pad(std::move(cpad)),
        for_keyword(std::move(fk)),
        loop_type(std::move(lt)),
        open_brace(std::move(opb)),
        close_brace(std::move(clb)),
        brace_indent(brc_ind),
        block_indent(blk_ind)
      {
      }

    //! destructor is default
    virtual ~C_style_printer() = default;


    // INTERFACE -- PRINT TO STRING
    // implements a 'language_printer' interface

  public:

    //! print a GiNaC expression ot a string
    virtual std::string ginac  (const GiNaC::ex& expr) const override;

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


    // INTERFACE -- CODE PLANTING

  public:

    //! plant a 'for' loop appropriate for this backend; should be supplied by a concrete class
    virtual std::string plant_for_loop(const std::string& loop_variable, unsigned int min, unsigned int max) const override;


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

    //! keyword used for 'for' loop
    std::string for_keyword;

    //! variable type used for 'for' loop
    std::string loop_type;

  };


#endif //CPPTRANSPORT_C_STYLE_PRINTER_H
