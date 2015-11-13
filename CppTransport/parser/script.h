//
// Created by David Seery on 18/06/2013.
// Copyright (c) 2013-15 University of Sussex. All rights reserved.
//
// To change the template use AppCode | Preferences | File Templates.
//



#ifndef __script_H_
#define __script_H_


#include <iostream>
#include <memory>
#include <unordered_map>

#include "stepper.h"
#include "indexorder.h"

#include "semantic_data.h"
#include "filestack.h"
#include "input_stack.h"

#include "symbol_factory.h"
#include "ginac/ginac.h"

#include "boost/optional.hpp"


// abstract 'declaration' concept

class declaration    // is an abstract class
	{

		// CONSTRUCTOR, DESTRUCTOR

  public:

    declaration(const std::string& n, GiNaC::symbol& s, const filestack& p);

		virtual ~declaration() = default;


    // INTERFACE

  public:

		//! get text name of declaration/symbol
    const std::string& get_name() const { return(this->name); }

		//! get GiNaC symbol association with declaration/symbol
    const GiNaC::symbol& get_ginac_symbol() const { return(this->symbol); }

		//! get filestack object representing definition point
    const filestack& get_path() const { return(this->path); }

		//! return GiNaC expression to be substituted when this declaration is used;
		//! often this will just be the GiNaC symbol, but may be more complex
		//! eg. for a subexpression declaration
		virtual GiNaC::ex get_expression() const = 0;

		//! return unique identifier representing order of declarations
		unsigned int get_unique_id() const { return(this->my_id); }


		// PRINT TO STANDARD STREAM

  public:

		//! write self-details to standard output
    virtual void print(std::ostream& stream) const = 0;


		// INTERNAL DATA

  protected:

		//! text name of declaration
		std::string name;

		//! GiNaC symbol for declaration
		GiNaC::symbol symbol;

		//! filestack representing definition point
    const filestack& path;

		//! class id; used to record the order in which declarations have been made
		unsigned int my_id;

		//! global id counter; initialized in script.cpp
		static unsigned int current_id;

	};


class field_declaration : public declaration
	{

		// CONSTRUCTOR, DESTRUCTOR

  public:

    field_declaration(const std::string& n, GiNaC::symbol& s, const filestack& p, attributes* a);

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

		std::unique_ptr<attributes> attrs;

	};


class parameter_declaration : public declaration
	{

		// CONSTRUCTOR, DESTRUCTOR

  public:

    parameter_declaration(const std::string& n, GiNaC::symbol& s, const filestack& p, attributes* a);

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

    std::unique_ptr<attributes> attrs;

	};


class subexpr_declaration : public declaration
	{

		// CONSTRUCTOR, DESTRUCTOR

  public:

    subexpr_declaration(const std::string& n, GiNaC::symbol& s, const filestack& p, subexpr* e);

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

    std::unique_ptr<subexpr> sexpr;

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

		boost::optional<declaration&> check_symbol_exists(const std::string& nm) const;

    //! detect error condition
    bool failed() const { return(this->errors_encountered); }


		// MODIFY CONTENT

  public:

    bool add_field(const std::string& n, GiNaC::symbol& s, const filestack& p, attributes* a);

    bool add_parameter(const std::string& n, GiNaC::symbol& s, const filestack& p, attributes* a);

		bool add_subexpr(const std::string& n, GiNaC::symbol& s, const filestack& p, subexpr* e);

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

    //! flag to indicate errors encountered during processing
    bool errors_encountered;

    std::string name;
    std::string author;
    std::string tag;
    std::string core;
    std::string implementation;
    std::string model;

    enum indexorder order;

    struct stepper background_stepper;
    struct stepper perturbations_stepper;

    //! symbol tables
    typedef std::unordered_map< std::string, std::unique_ptr<field_declaration> >     field_symbol_table;
    typedef std::unordered_map< std::string, std::unique_ptr<parameter_declaration> > parameter_symbol_table;
    typedef std::unordered_map< std::string, std::unique_ptr<subexpr_declaration> >   subexpr_symbol_table;

    field_symbol_table     fields;
    parameter_symbol_table parameters;
		parameter_symbol_table reserved;
    subexpr_symbol_table   subexprs;

    //! place-holder filestack for initializing reserved words
    input_stack placeholder_filestack;

    //! store details of potentials
    bool      potential_set;
    GiNaC::ex potential;

    // symbols

		symbol_factory             sym_factory;

    std::vector<GiNaC::symbol> deriv_symbols;

		// reserved symbols

    GiNaC::symbol              M_Planck;

	};


#endif //__script_H_
