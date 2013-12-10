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

      bool get_latex  (std::string& l) const;
      void set_latex  (const std::string l);
      void unset_latex();

    private:
      bool        latex_set;
      std::string latex;
  };


class quantity
  {
    public:
      quantity(std::string n, const filestack* p);                    // basic constructor
      quantity(std::string n, attributes& a, const filestack* p);     // constructor with attributes
      quantity(std::string n, attributes& a, GiNaC::symbol& s);       // constructor with GiNaC symbol, but no filestack data
      ~quantity();

      void                                print(std::ostream& stream) const;

      const std::string                   get_name() const;
      const std::string                   get_latex_name() const;

      const GiNaC::symbol*                get_ginac_symbol() const;

      const filestack*                    get_path() const;

    protected:
      std::string                         name;

      const filestack*                    path;

      attributes                          attrs;
      GiNaC::symbol                       ginac_symbol;
  };


#endif //__symbol_H_
