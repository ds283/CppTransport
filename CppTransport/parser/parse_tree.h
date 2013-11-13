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

#define DEFAULT_ABS_ERR   (1E-6)
#define DEFAULT_REL_ERR   (1E-6)
#define DEFAULT_STEP_SIZE (1E-2)
#define DEFAULT_STEPPER   "runge_kutta_dopri5"

struct stepper
  {
    double      abserr;
    double      relerr;
    double      stepsize;
    std::string name;
  };

enum indexorder { indexorder_left, indexorder_right };

#define SYMBOL_TABLE_SIZE (1024)

class script
  {
    public:
      script();
      ~script();

      void                               print                    (std::ostream& stream);

      bool                               add_field                (field_declaration* d);
      bool                               add_parameter            (parameter_declaration* d);

      void                               set_background_stepper   (stepper* s);
      void                               set_perturbations_stepper(stepper* s);

      const struct stepper&              get_background_stepper   ();
      const struct stepper&              get_perturbations_stepper();

      bool                               lookup_symbol            (std::string id, quantity*& s);

      unsigned int                       get_number_fields        ();
      unsigned int                       get_number_params        ();

      std::vector<std::string>           get_field_list           ();
      std::vector<std::string>           get_latex_list           ();
      std::vector<std::string>           get_param_list           ();
      std::vector<std::string>           get_platx_list           ();

      std::vector<GiNaC::symbol>         get_field_symbols        ();
      std::vector<GiNaC::symbol>         get_deriv_symbols        ();
      std::vector<GiNaC::symbol>         get_param_symbols        ();

      const GiNaC::symbol&               get_Mp_symbol            ();

      void                               set_name                 (const std::string n);
      const std::string&                 get_name                 ();

      void                               set_author               (const std::string a);
      const std::string&                 get_author               ();

      void                               set_tag                  (const std::string t);
      const std::string&                 get_tag                  ();

      void                               set_core                 (const std::string c);
      const std::string&                 get_core                 ();

      void                               set_implementation       (const std::string i);
      const std::string&                 get_implementation       ();

      void                               set_model                (const std::string m);
      const std::string&                 get_model                ();

      void                               set_indexorder           (enum indexorder o);
      enum indexorder                    get_indexorder           ();

      void                               set_potential            (GiNaC::ex* V);
      GiNaC::ex                          get_potential            ();
      void                               unset_potential          ();

    private:
      // blank out copying options, to avoid multiple aliasing of the
      // symbol_table<> and deque<declaration*> objects contained within
      script(const script& other);              // non copy-constructible
      script& operator=(const script& rhs);     // non copyable

      std::string                        name;
      std::string                        author;
      std::string                        tag;
      std::string                        core;
      std::string                        implementation;
      std::string                        model;

      enum indexorder                    order;

      struct stepper                     background_stepper;
      struct stepper                     perturbations_stepper;

      std::deque<field_declaration*>     fields;
      std::deque<parameter_declaration*> parameters;

      bool                               potential_set;
      GiNaC::ex*                         potential;

      // reserved symbols
      GiNaC::symbol                      M_Planck;
      std::vector<GiNaC::symbol>         deriv_symbols;

      symbol_table<quantity>*            table;
  };


#endif //__parse_tree_H_
