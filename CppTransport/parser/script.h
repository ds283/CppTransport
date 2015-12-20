//
// Created by David Seery on 18/06/2013.
// Copyright (c) 2013-15 University of Sussex. All rights reserved.
//



#ifndef CPPTRANSPORT_SCRIPT_H
#define CPPTRANSPORT_SCRIPT_H


#include <iostream>
#include <memory>
#include <unordered_map>

#include "stepper.h"
#include "model_settings.h"
#include "symbol_list.h"

#include "semantic_data.h"
#include "filestack.h"
#include "input_stack.h"
#include "error_context.h"
#include "contexted_value.h"

#include "y_common.h"

#include "symbol_factory.h"
#include "ginac/ginac.h"

#include "boost/optional.hpp"


// abstract 'declaration' concept

class declaration    // is an abstract class
	{

		// CONSTRUCTOR, DESTRUCTOR

  public:

    declaration(const std::string& n, GiNaC::symbol& s, const y::lexeme_type& l);

		virtual ~declaration() = default;


    // INTERFACE

  public:

		//! get text name of declaration/symbol
    const std::string& get_name() const { return(this->name); }

		//! get GiNaC symbol association with declaration/symbol
    const GiNaC::symbol& get_ginac_symbol() const { return(this->symbol); }

    //! return GiNaC expression to be substituted when this declaration is used;
		//! often this will just be the GiNaC symbol, but may be more complex
		//! eg. for a subexpression declaration
		virtual GiNaC::ex get_expression() const = 0;

		//! return unique identifier representing order of declarations
		unsigned int get_unique_id() const { return(this->my_id); }

    //! return lexeme representing declaration point
    const y::lexeme_type& get_declaration_point() const { return(this->declaration_point); }


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

		//! reference to declaration lexeme
    const y::lexeme_type& declaration_point;

		//! class id; used to record the order in which declarations have been made
		unsigned int my_id;

		//! global id counter; initialized in script.cpp
		static unsigned int current_id;

	};


class field_declaration : public declaration
	{

		// CONSTRUCTOR, DESTRUCTOR

  public:

    field_declaration(const std::string& n, GiNaC::symbol& s, const y::lexeme_type& l, attributes* a);

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

    parameter_declaration(const std::string& n, GiNaC::symbol& s, const y::lexeme_type& l, attributes* a);

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

    subexpr_declaration(const std::string& n, GiNaC::symbol& s, const y::lexeme_type& l, subexpr* e);

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


constexpr double DEFAULT_ABS_ERR   = 1E-6;
constexpr double DEFAULT_REL_ERR   = 1E-6;
constexpr double DEFAULT_STEP_SIZE = 1E-2;
constexpr auto   DEFAULT_STEPPER   = "runge_kutta_dopri5";

constexpr unsigned int SYMBOL_TABLE_SIZE = 1024;


class script
	{

    // CONSTRUCTOR, DESTRUCTOR

  public:

    //! constructor;
    //! symbol_factory is inherited from parent translation_unit
    //! error_context is passed down from parent translation_unit and is used to construct
    //! fake error contexts for default reserved symbols such as M_Planck
    script(symbol_factory& s, error_context err_ctx);

    //! destructor is default
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


		// POPULATE SYMBOLS

  public:

    //! add symbol representing a field
    bool add_field(const std::string& n, GiNaC::symbol& s, const y::lexeme_type& p, attributes* a);

    //! add symbol representing a parameter
    bool add_parameter(const std::string& n, GiNaC::symbol& s, const y::lexeme_type& p, attributes* a);

    //! add symbol representing a subexpression
		bool add_subexpr(const std::string& n, GiNaC::symbol& s, const y::lexeme_type& p, subexpr* e);


    // MODEL DATA

  public:

    unsigned int get_number_fields() const;

    unsigned int get_number_params() const;


    std::vector<std::string> get_field_list() const;

    std::vector<std::string> get_latex_list() const;

    std::vector<std::string> get_param_list() const;

    std::vector<std::string> get_platx_list() const;

    symbol_list get_field_symbols() const;

    symbol_list get_deriv_symbols() const;

    symbol_list get_param_symbols() const;

    const GiNaC::symbol& get_Mp_symbol() const;


    void set_indexorder(enum index_order o);

    enum index_order get_indexorder() const;


    void set_potential(GiNaC::ex V);

    GiNaC::ex get_potential() const;

    void unset_potential();


    // BASIC METADATA

  public:

    void set_name(const std::string n, const y::lexeme_type& l);

    boost::optional< contexted_value<std::string>& > get_name() const;


    void set_author(const std::string a, const y::lexeme_type& l);

    boost::optional< contexted_value<std::string>& > get_author() const;


    void set_tag(const std::string t, const y::lexeme_type& l);

    boost::optional< contexted_value<std::string>& > get_tag() const;


    void set_core(const std::string c, const y::lexeme_type& l);

    boost::optional< contexted_value<std::string>& > get_core() const;


    void set_implementation(const std::string i, const y::lexeme_type& l);

    boost::optional< contexted_value<std::string>& > get_implementation() const;


    void set_model(const std::string m, const y::lexeme_type& l);

    boost::optional< contexted_value<std::string>& > get_model() const;


    // IMPLEMENTATION DATA

  public:

    void set_background_stepper(stepper* s);

    void set_perturbations_stepper(stepper* s);

    const struct stepper& get_background_stepper() const;

    const struct stepper& get_perturbations_stepper() const;


		// INTERNAL DATA

  private:

    //! flag to indicate errors encountered during processing
    bool errors_encountered;

    std::unique_ptr< contexted_value<std::string> > name;
    std::unique_ptr< contexted_value<std::string> > author;
    std::unique_ptr< contexted_value<std::string> > tag;
    std::unique_ptr< contexted_value<std::string> > core;
    std::unique_ptr< contexted_value<std::string> > implementation;
    std::unique_ptr< contexted_value<std::string> > model;

    enum index_order order;

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
    std::unique_ptr<y::lexeme_type> fake_MPlanck_lexeme;

    //! store details of potentials
    bool      potential_set;
    GiNaC::ex potential;

    // symbols

    symbol_factory sym_factory;

    symbol_list deriv_symbols;

		// reserved symbols

    GiNaC::symbol M_Planck;

	};


#endif //CPPTRANSPORT_SCRIPT_H
