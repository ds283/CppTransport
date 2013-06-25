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
#include <deque>

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
      quantity(std::string n, unsigned int l, std::deque<struct inclusion>& p);                 // basic constructor
      quantity(std::string n, attributes& a, unsigned int l, std::deque<struct inclusion>& p);  // constructor with attributes
      ~quantity();

      void                          print(std::ostream& stream);

      std::string                   get_name();
      GiNaC::symbol*                get_ginac_symbol();

      unsigned int                  get_line();
      std::deque<struct inclusion>& get_path();

    protected:
      std::string                   name;

      unsigned int                  line;
      std::deque<struct inclusion>  path;

      attributes                    attrs;
      GiNaC::symbol                 ginac_symbol;
  };


#endif //__symbol_H_
