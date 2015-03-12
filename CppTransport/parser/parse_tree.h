//
// Created by David Seery on 18/06/2013.
// Copyright (c) 2013 University of Sussex. All rights reserved.
//
// To change the template use AppCode | Preferences | File Templates.
//



#ifndef __parse_tree_H_
#define __parse_tree_H_


#include <iostream>

#include "stepper.h"
#include "indexorder.h"

#include "quantity.h"
#include "symbol_table.h"
#include "filestack.h"

#include "symbol_factory.h"
#include "ginac/ginac.h"


// abstract 'declaration' concept
// the filestack* object used here is inherited from the parent lexeme,
// so we don't have to keep track of its deletion -- it will be deleted
// automatically when the parent lexeme is destroyed

class declaration
	{
  public:
    declaration(const quantity& o, const filestack* p); // constructor needed for const members

    // return pointer to the symbol table object this declaration corresponds to
    quantity* get_quantity() const;

    virtual void print(std::ostream& stream) const = 0;

  protected:
    quantity       * obj;
    const filestack* path;
	};


class field_declaration : public declaration
	{
  public:
    field_declaration(const quantity& o, const filestack* p);

    ~field_declaration();

    void print(std::ostream& stream) const;
	};


class parameter_declaration : public declaration
	{
  public:
    parameter_declaration(const quantity& o, const filestack* p);

    ~parameter_declaration();

    void print(std::ostream& stream) const;
	};


#define DEFAULT_ABS_ERR   (1E-6)
#define DEFAULT_REL_ERR   (1E-6)
#define DEFAULT_STEP_SIZE (1E-2)
#define DEFAULT_STEPPER   "runge_kutta_dopri5"

#define SYMBOL_TABLE_SIZE (1024)


class script
	{

    // CONSTRUCTOR, DESTRUCTOR

  public:

    script(symbol_factory& s);

    ~script();


    // INTERFACE

  public:

    void print(std::ostream& stream) const;

    bool add_field(field_declaration* d);

    bool add_parameter(parameter_declaration* d);

    void set_background_stepper(stepper* s);

    void set_perturbations_stepper(stepper* s);

    const struct stepper& get_background_stepper() const;

    const struct stepper& get_perturbations_stepper() const;

    bool lookup_symbol(std::string id, quantity*& s) const;

    unsigned int get_number_fields() const;

    unsigned int get_number_params() const;

    std::vector<std::string> get_field_list() const;

    std::vector<std::string> get_latex_list() const;

    std::vector<std::string> get_param_list() const;

    std::vector<std::string> get_platx_list() const;

    std::vector<GiNaC::symbol> get_field_symbols() const;

    std::vector<GiNaC::symbol> get_deriv_symbols() const;

    std::vector<GiNaC::symbol> get_param_symbols() const;

    const GiNaC::symbol& get_Mp_symbol() const;

    void set_name(const std::string n);

    const std::string& get_name() const;

    void set_author(const std::string a);

    const std::string& get_author() const;

    void set_tag(const std::string t);

    const std::string& get_tag() const;

    void set_core(const std::string c);

    const std::string& get_core() const;

    void set_implementation(const std::string i);

    const std::string& get_implementation() const;

    void set_model(const std::string m);

    const std::string& get_model() const;

    void set_indexorder(enum indexorder o);

    enum indexorder get_indexorder() const;

    void set_potential(GiNaC::ex V);

    GiNaC::ex get_potential() const;

    void unset_potential();


		// INTERNAL DATA

  private:

    // blank out copying options, to avoid multiple aliasing of the
    // symbol_table<> and deque<declaration*> objects contained within
    script(const script& other);              // non copy-constructible
    script& operator=(const script& rhs);     // non copyable

    std::string name;
    std::string author;
    std::string tag;
    std::string core;
    std::string implementation;
    std::string model;

    enum indexorder order;

    struct stepper background_stepper;
    struct stepper perturbations_stepper;

    std::deque<field_declaration*>     fields;
    std::deque<parameter_declaration*> parameters;

    bool      potential_set;
    GiNaC::ex potential;

    // symbols, reserved quantities

		symbol_factory             sym_factory;

    GiNaC::symbol              M_Planck;
    std::vector<GiNaC::symbol> deriv_symbols;

    symbol_table<quantity>* table;
	};


#endif //__parse_tree_H_
