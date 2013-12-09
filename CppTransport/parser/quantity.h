//
// Created by David Seery on 21/06/2013.
// Copyright (c) 2013 University of Sussex. All rights reserved.
//
// To change the template use AppCode | Preferences | File Templates.
//



#ifndef __symbol_H_
#define __symbol_H_

#include <iostream>
#include <string>
#include "filestack.h"

#include "lexeme.h"

#include "ginac/ginac.h"

class attributes
  {
    public:
      attributes();
      ~attributes();

      bool get_latex  (std::string& l);
      void set_latex  (const std::string l);
      void unset_latex();

    private:
      bool        latex_set;
      std::string latex;
  };


class quantity
  {
    public:
      quantity(std::string n, unsigned int l, filestack* p);                 // basic constructor
      quantity(std::string n, attributes& a, unsigned int l, filestack* p);  // constructor with attributes
      quantity(std::string n, attributes& a, GiNaC::symbol& s);              // constructor with GiNaC symbol
      ~quantity();

      void                                print(std::ostream& stream);

      const std::string                   get_name();
      const std::string                   get_latex_name();

      const GiNaC::symbol*                get_ginac_symbol();

      unsigned int                        get_line();
      filestack*                          get_path();

    protected:
      std::string                         name;

      const unsigned int                  line;
      filestack*                          path;

      attributes                          attrs;
      GiNaC::symbol                       ginac_symbol;
  };


#endif //__symbol_H_
