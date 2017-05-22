//
// Created by David Seery on 18/06/2013.
// --@@
// Copyright (c) 2016 University of Sussex. All rights reserved.
//
// This file is part of the CppTransport platform.
//
// CppTransport is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 2 of the License, or
// (at your option) any later version.
//
// CppTransport is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with CppTransport.  If not, see <http://www.gnu.org/licenses/>.
//
// @license: GPL-2
// @contributor: David Seery <D.Seery@sussex.ac.uk>
// --@@
//



#ifndef CPPTRANSPORT_SCRIPT_H
#define CPPTRANSPORT_SCRIPT_H


#include <iostream>
#include <memory>
#include <unordered_map>
#include <map>

#include "stepper.h"
#include "model_settings.h"
#include "symbol_list.h"

#include "semantic_data.h"
#include "filestack.h"
#include "input_stack.h"
#include "error_context.h"
#include "contexted_value.h"

#include "defaults.h"

#include "y_common.h"

#include "symbol_factory.h"
#include "disable_warnings.h"
#include "ginac/ginac.h"

#include "boost/optional.hpp"


// abstract 'declaration' concept

class declaration    // is an abstract class
	{

		// CONSTRUCTOR, DESTRUCTOR

  public:

    //! constructor
    declaration(const std::string& n, GiNaC::symbol& s, const y::lexeme_type& l);

    //! destructor is default
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

    //! constructor
    field_declaration(const std::string& n, GiNaC::symbol& s, const y::lexeme_type& l, attributes* a);

    //! destructor is default
    virtual ~field_declaration() = default;


		// INTERFACE

  public:

    //! get LaTeX name of field
    std::string get_latex_name() const;

    //! get GiNaC expression for field
		virtual GiNaC::ex get_expression() const override { return GiNaC::ex(this->symbol); }


    // PRINT TO STANDARD STREAM

  public:

    //! print details to specified stream
    void print(std::ostream& stream) const override;


		// INTERNAL DATA

  protected:

    //! attributes block
		std::unique_ptr<attributes> attrs;

	};


class parameter_declaration : public declaration
	{

		// CONSTRUCTOR, DESTRUCTOR

  public:

    //! constructor
    parameter_declaration(const std::string& n, GiNaC::symbol& s, const y::lexeme_type& l, attributes* a);

    //! destructor is default
    ~parameter_declaration() = default;


    // INTERFACE

  public:

    //! get LaTeX name of parameter
    std::string get_latex_name() const;

    //! get GiNaC expression for parameter
    virtual GiNaC::ex get_expression() const override { return GiNaC::ex(this->symbol); }


    // PRINT TO STANDARD STREAM

  public:

    //! print details to specified stream
    void print(std::ostream& stream) const override;


    // INTERNAL DATA

  protected:

    //! attributes block
    std::unique_ptr<attributes> attrs;

	};


class subexpr_declaration : public declaration
	{

		// CONSTRUCTOR, DESTRUCTOR

  public:

    //! constructor
    subexpr_declaration(const std::string& n, GiNaC::symbol& s, const y::lexeme_type& l, subexpr* e);

    //! destructor is default
    ~subexpr_declaration() = default;


    // INTERFACE

  public:

    //! get LaTeX name of subexpression
    std::string get_latex_name() const;

    //! get GiNaC expression for subexpression
		GiNaC::ex get_value() const;

    //! redirect inherited virtual function 'get_expression()' to 'get_value()'
    virtual GiNaC::ex get_expression() const override { return this->get_value(); }


    // PRINT TO STANDARD STREAM

  public:

    //! print details to specified stream
    void print(std::ostream& stream) const override;


    // INTERNAL DATA

  protected:

    //! attribute block
    std::unique_ptr<subexpr> sexpr;

	};


class model_descriptor
	{

    // CONSTRUCTOR, DESTRUCTOR

  public:

    //! constructor;
    //! symbol_factory is inherited from parent translation_unit
    //! error_context is passed down from parent translation_unit and is used to construct
    //! fake error contexts for default reserved symbols such as M_Planck
    model_descriptor(symbol_factory& s, error_context err_ctx);

    //! destructor is default
    ~model_descriptor() = default;

    // delete copying constructor, to avoid multiple aliasing of the
    // symbol_table<> and deque<declaration*> objects contained within

    //! not copy-constructible, so delete the copy constructor
    model_descriptor(const model_descriptor& other) = delete;

    //! not copyable, so delete the assignment operator
    model_descriptor& operator=(const model_descriptor& rhs) = delete;


    // INTERFACE

  public:

    //! print details to a specified stream
    void print(std::ostream& stream) const;

    //! detect error condition
    bool failed() const { return(this->errors_encountered); }


		// SYMBOL SERVICES

  public:

    //! add symbol representing a field
    bool add_field(const std::string& n, GiNaC::symbol& s, const y::lexeme_type& p, attributes* a);

    //! add symbol representing a parameter
    bool add_parameter(const std::string& n, GiNaC::symbol& s, const y::lexeme_type& p, attributes* a);

    //! add symbol representing a subexpression
		bool add_subexpr(const std::string& n, GiNaC::symbol& s, const y::lexeme_type& p, subexpr* e);
    
    //! check whether a given symbol name has been declared
    //! if the symbol exists, returns a boost::optional<> containing a reference
    //! to its declaration
    boost::optional<declaration&> check_symbol_exists(const std::string& nm) const;


    // MODEL DATA

  public:

    //! get number of fields in the model
    unsigned int get_number_fields() const;

    //! get number of parameters in the model
    unsigned int get_number_params() const;

    
    // FIELDS AND PARAMETERS
    
  public:

    //! get std::vector<> of field names
    std::vector<std::string> get_field_name_list() const;

    //! get_std::vector<> of LaTeX field names
    std::vector<std::string> get_field_latex_list() const;

    //! get std::vector<> of parameter names
    std::vector<std::string> get_param_name_list() const;

    //! get std::vector<> of LaTeX parameter names
    std::vector<std::string> get_param_latex_list() const;

    //! get list of symbols for fields
    symbol_list get_field_symbols() const;

    //! get list of symbols for field derivatives
    symbol_list get_deriv_symbols() const;

    //! get list of symbols for parameters
    symbol_list get_param_symbols() const;

    //! get symbol for Planck mass M_P
    const GiNaC::symbol& get_Mp_symbol() const;


    // MISCELLANEOUS SETTINGS
    
  public:
    
    //! set index order flag (left-most first or right-most first)
    void set_indexorder(enum index_order o);

    //! get index order flag
    enum index_order get_indexorder() const;

    
    // LAGRANGIAN MANAGEMENT
    
  public:

    //! set potential
    void set_potential(GiNaC::ex V, const y::lexeme_type& l);

    //! get potential as a contexted value
    boost::optional< contexted_value<GiNaC::ex>& > get_potential() const;

    //! unset potential
    void unset_potential();


    // GET AND SET BASIC METADATA

  public:

    //! get model name (a textual string)
    void set_name(const std::string n, const y::lexeme_type& l);

    //! get model name as contexted value
    boost::optional< contexted_value<std::string>& > get_name() const;


    //! add an author
    bool add_author(const std::string& n, const y::lexeme_type& l, author* a);

    //! get author table
    const author_table& get_author() const;


    //! set citation guidance
    void set_citeguide(const std::string t, const y::lexeme_type& l);

    //! get citation guidance as contexted value
    boost::optional< contexted_value<std::string>& > get_citeguide() const;


    //! set model description
    void set_description(const std::string t, const y::lexeme_type& l);

    //! get model description as contexted value
    boost::optional< contexted_value<std::string>& > get_description() const;


    //! set model revision
    void set_revision(int r, const y::lexeme_type& l);

    //! get model revision as contexted value
    boost::optional< contexted_value<unsigned int>& > get_revision() const;


    //! set model license string
    void set_license(const std::string t, const y::lexeme_type& l);

    //! get model license string as contexted value
    boost::optional< contexted_value<std::string>& > get_license() const;


    //! set model reference list
    void set_references(const std::vector< contexted_value<std::string> >& refs);

    //! get model reference list as contexted value
    boost::optional< std::vector< contexted_value<std::string> >& > get_references() const;


    //! set model URL list
    void set_urls(const std::vector< contexted_value<std::string> >& urls);

    //! get model URL list as contexted value
    boost::optional< std::vector< contexted_value<std::string> >& > get_urls() const;


    // TEMPLATE SPECIFICATION
    
  public:
    
    //! set core template name
    void set_core(const std::string c, const y::lexeme_type& l);

    //! get core template name as contexted value
    boost::optional< contexted_value<std::string>& > get_core() const;


    //! set implementation template name
    void set_implementation(const std::string i, const y::lexeme_type& l);

    //! get implementation template name as contexted value
    boost::optional< contexted_value<std::string>& > get_implementation() const;


    //! set model tag (an identifier)
    void set_model(const std::string m, const y::lexeme_type& l);

    //! get model tag as a contexted value
    boost::optional< contexted_value<std::string>& > get_model() const;


    // IMPLEMENTATION DATA

  public:

    //! set background stepper name
    void set_background_stepper(stepper* s, const y::lexeme_type& l);

    //! get background stepper name as contexted value
    boost::optional< contexted_value<stepper>& > get_background_stepper() const;


    //! set perturbations stepper name
    void set_perturbations_stepper(stepper* s, const y::lexeme_type& l);

    //! get perturbations stepper name as contexted value
    boost::optional< contexted_value<stepper>& > get_perturbations_stepper() const;


		// INTERNAL DATA

  private:

    //! flag to indicate errors encountered during processing
    bool errors_encountered;

    //! 'model' is the C++ name
    std::unique_ptr< contexted_value<std::string> >                model;

    //! 'name' is a human-readable name
    std::unique_ptr< contexted_value<std::string> >                name;

    //! citation guidance
    std::unique_ptr< contexted_value<std::string> >                citeguide;
    
    //! model description
    std::unique_ptr< contexted_value<std::string> >                description;

    //! model revision
    std::unique_ptr< contexted_value<unsigned int> >               revision;

    //! model license string
    std::unique_ptr< contexted_value<std::string> >                license;

    //! list of references
    std::unique_ptr< std::vector< contexted_value<std::string> > > references;

    //! list of URLs
    std::unique_ptr< std::vector< contexted_value<std::string> > > urls;

    //! name of core template
    std::unique_ptr< contexted_value<std::string> >                core;
    
    //! name of implementation template
    std::unique_ptr< contexted_value<std::string> >                implementation;

    //! author table
    author_table authors;

    //! index ordering setting (left-most first or right-most first)
    enum index_order order;

    //! specification of background stepper
    std::unique_ptr< contexted_value<stepper> > background_stepper;
    
    //! specification of perturbations stepper
    std::unique_ptr< contexted_value<stepper> > perturbations_stepper;

    //! typedef for field symbol table
    typedef std::unordered_map< std::string, std::unique_ptr<field_declaration> >     field_symbol_table;
    
    //! typedef for parameter symbol table
    typedef std::unordered_map< std::string, std::unique_ptr<parameter_declaration> > parameter_symbol_table;
    
    //! typedef for subexpression symbol table
    typedef std::unordered_map< std::string, std::unique_ptr<subexpr_declaration> >   subexpr_symbol_table;

    //! symbol table: fields
    field_symbol_table     fields;
    
    //! symbol table: parameters
    parameter_symbol_table parameters;
    
    //! symbol table: reserved words
		parameter_symbol_table reserved;
    
    //! symbol table: subexpressions
    subexpr_symbol_table   subexprs;

    //! place-holder filestack for initializing reserved words
    std::unique_ptr<y::lexeme_type> fake_MPlanck_lexeme;

    //! store details of potentials
    std::unique_ptr< contexted_value<GiNaC::ex> > potential;

    
    // SYMBOL SERVICES

    //! symbol factor
    symbol_factory sym_factory;

    //! symbol list: names of field derivatives
    symbol_list deriv_symbols;

		// reserved symbols

    //! symbol representing Planck mass
    GiNaC::symbol M_Planck;

	};


#endif //CPPTRANSPORT_SCRIPT_H
