//
// Created by David Seery on 18/06/2013.
// Copyright (c) 2013 University of Sussex. All rights reserved.
//
// To change the template use AppCode | Preferences | File Templates.
//



#ifndef __parse_tree_H_
#define __parse_tree_H_

#include <iostream>
#include <deque>

#include "quantity.h"
#include "symbol_table.h"

#include "ginac/ginac.h"


class declaration
  {
    public:
      quantity*                          get_quantity();
      virtual void                       print       (std::ostream& stream) = 0;

    protected:
      declaration
        (const quantity& o, unsigned int l, const std::deque<struct inclusion>& p); // constructor needed for const members

      quantity*                          obj;
      const unsigned int                 line;
      const std::deque<struct inclusion> path;
  };

class field_declaration: public declaration
  {
    public:
      field_declaration(const quantity& o, unsigned int l, const std::deque<struct inclusion>& p);
      ~field_declaration();

      void print(std::ostream& stream);
  };

class parameter_declaration: public declaration
  {
    public:
      parameter_declaration(const quantity& o, unsigned int l, const std::deque<struct inclusion>& p);
      ~parameter_declaration();

      void print(std::ostream& stream);
  };

#define SYMBOL_TABLE_SIZE (1024)

class script
  {
    public:
      script();
      ~script();

      void              print          (std::ostream& stream);

      bool              add_declaration(declaration* d);

      bool              lookup_symbol  (std::string id, quantity*& s);

      void              set_name       (const std::string n);
      const std::string get_name       ();

      void              set_author     (const std::string a);
      const std::string get_author     ();

      void              set_tag        (const std::string t);
      const std::string get_tag        ();

      void              set_potential  (GiNaC::ex* V);
      bool              get_potential  (GiNaC::ex*& V);
      void              unset_potential();

    private:
      // blank out copying options, to avoid multiple aliasing of the
      // symbol_table<> and deque<declaration*> objects contained within
      script(const script& other);              // non copy-constructible
      script& operator=(const script& rhs);     // non copyable

      std::string                 name;
      std::string                 author;
      std::string                 tag;

      std::deque<declaration*>    decls;

      bool                        potential_set;
      GiNaC::ex*                  potential;

      symbol_table<quantity>*     table;
  };


#endif //__parse_tree_H_
