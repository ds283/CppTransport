//
// Created by David Seery on 09/12/2013.
// Copyright (c) 2013-15 University of Sussex. All rights reserved.
//


#ifndef __C_style_printer_H_
#define __C_style_printer_H_


#include "language_printer.h"


#define DEFAULT_C_STYLE_COMMENT_PREFIX  "//"
#define DEFAULT_C_STYLE_COMMENT_POSTFIX ""
#define DEFAULT_C_STYLE_COMMENT_PAD     " "


class C_style_printer: public language_printer
  {
  public:
    C_style_printer(std::string cpre =DEFAULT_C_STYLE_COMMENT_PREFIX,
                    std::string cpost=DEFAULT_C_STYLE_COMMENT_POSTFIX,
                    std::string cpad =DEFAULT_C_STYLE_COMMENT_PAD)
      : comment_prefix(cpre), comment_postfix(cpost), comment_pad(cpad)
      {
      }

    virtual std::string ginac  (const GiNaC::ex& expr) override;
    virtual std::string comment(const std::string tag) override;

  protected:

    std::string print_operands(const GiNaC::ex& expr, std::string op);

    std::string comment_prefix;
    std::string comment_postfix;
    std::string comment_pad;
  };


#endif //__C_style_printer_H_
