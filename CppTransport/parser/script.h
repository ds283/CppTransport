//
// Created by David Seery on 18/06/2013.
// Copyright (c) 2013-15 University of Sussex. All rights reserved.
//
// To change the template use AppCode | Preferences | File Templates.
//



#ifndef __parse_tree_H_
#define __parse_tree_H_


#include <iostream>
#include <memory>

#include "stepper.h"
#include "indexorder.h"

#include "semantic_data.h"
#include "filestack.h"

#include "symbol_factory.h"
#include "ginac/ginac.h"


// abstract 'declaration' concept

// the filestack* object used here is inherited from the parent lexeme.
// we manage its lifetime with a std::shared_ptr<>

class declaration    // is an abstract class
	{

		// CONSTRUCTOR, DESTRUCTOR

  public:

    declaration(const std::string& n, GiNaC::symbol& s, std::shared_ptr<filestack> p);

		virtual ~declaration() = default;


    // INTERFACE

  public:

    const std::string& get_name() const { return(this->name); }

    const GiNaC::symbol& get_ginac_symbol() const { return(this->symbol); }

    std::shared_ptr<filestack> get_path() const { return(this->path); }

		virtual GiNaC::ex get_expression() const = 0;


		// PRINT TO STANDARD STREAM

  public:

    virtual void print(std::ostream& stream) const = 0;


		// INTERNAL DATA

  protected:

		std::string name;

		GiNaC::symbol symbol;

    std::shared_ptr<filestack> path;

	};


class field_declaration : public declaration
	{

		// CONSTRUCTOR, DESTRUCTOR

  public:

    field_declaration(const std::string& n, GiNaC::symbol& s, std::shared_ptr<filestack> p, attributes* a);

    virtual ~field_declaration() = default;


		// INTERFACE

  public:

    std::string get_latex_name() const;

		virtual GiNaC::ex get_expression() const override { return GiNaC::ex(this->symbol); }


    // PRINT TO STANDARD STREAM

  public:

    void print(std::ostream& stream) const override;


		// INTERNAL DATA

  protected:

		std::shared_ptr<attributes> attrs;

	};


class parameter_declaration : public declaration
	{

		// CONSTRUCTOR, DESTRUCTOR

  public:

    parameter_declaration(const std::string& n, GiNaC::symbol& s, std::shared_ptr<filestack> p, attributes* a);

    ~parameter_declaration() = default;


    // INTERFACE

  public:

    std::string get_latex_name() const;

    virtual GiNaC::ex get_expression() const override { return GiNaC::ex(this->symbol); }


    // PRINT TO STANDARD STREAM

  public:

    void print(std::ostream& stream) const override;


    // INTERNAL DATA

  protected:

    std::shared_ptr<attributes> attrs;

	};


class subexpr_declaration : public declaration
	{

		// CONSTRUCTOR, DESTRUCTOR

  public:

    subexpr_declaration(const std::string& n, GiNaC::symbol& s, std::shared_ptr <filestack> p, subexpr* e);

    ~subexpr_declaration() = default;


    // INTERFACE

  public:

    std::string get_latex_name() const;

		GiNaC::ex get_value() const;

    virtual GiNaC::ex get_expression() const override { return this->get_value(); }


    // PRINT TO STANDARD STREAM

  public:

    void print(std::ostream& stream) const override;


    // INTERNAL DATA

  protected:

    std::shared_ptr<subexpr> sexpr;

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

    ~script() = default;

    // delete copying constructor, to avoid multiple aliasing of the
    // symbol_table<> and deque<declaration*> objects contained within

    script(const script& other) = delete;              // non copy-constructible

    script& operator=(const script& rhs) = delete;     // non copyable


    // INTERFACE

  public:

    void print(std::ostream& stream) const;

		std::shared_ptr<declaration> check_symbol_exists(const std::string& nm) const;


		// MODIFY CONTENT

  public:

    bool add_field(field_declaration d);

    bool add_parameter(parameter_declaration d);

		bool add_subexpr(subexpr_declaration d);

    void set_background_stepper(stepper* s);

    void set_perturbations_stepper(stepper* s);

    const struct stepper& get_background_stepper() const;

    const struct stepper& get_perturbations_stepper() const;

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

    std::string name;
    std::string author;
    std::string tag;
    std::string core;
    std::string implementation;
    std::string model;

    enum indexorder order;

    struct stepper background_stepper;
    struct stepper perturbations_stepper;

    typedef std::map< std::string, std::shared_ptr<field_declaration> >     field_symbol_table;
    typedef std::map< std::string, std::shared_ptr<parameter_declaration> > parameter_symbol_table;
    typedef std::map< std::string, std::shared_ptr<subexpr_declaration> >   subexpr_symbol_table;

    field_symbol_table     fields;
    parameter_symbol_table parameters;
		parameter_symbol_table reserved;
    subexpr_symbol_table   subexprs;

    bool      potential_set;
    GiNaC::ex potential;

    // symbols

		symbol_factory             sym_factory;

    std::vector<GiNaC::symbol> deriv_symbols;

		// reserved symbols

    GiNaC::symbol              M_Planck;

	};


#endif //__parse_tree_H_
