//
// Created by David Seery on 25/05/2017.
// --@@
// Copyright (c) 2017 University of Sussex. All rights reserved.
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


#ifndef CPPTRANSPORT_DECLARATIONS_H
#define CPPTRANSPORT_DECLARATIONS_H


#include "contexted_value.h"
#include "model_settings.h"
#include "y_common.h"

#include "ginac/ginac.h"


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
    
    //! text name of object (parameter, field, etc.)
    std::string name;
    
    //! GiNaC symbol for object
    GiNaC::symbol symbol;
    
    //! reference to lexeme specifying declaration point, used for
    //! reporting context-aware errors
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
    field_declaration(const std::string& n, GiNaC::symbol& s, const y::lexeme_type& l, attributes& a);
    
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
    parameter_declaration(const std::string& n, GiNaC::symbol& s, const y::lexeme_type& l, attributes& a);
    
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
    subexpr_declaration(const std::string& n, GiNaC::symbol& s, const y::lexeme_type& l, subexpr& e);
    
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


#endif //CPPTRANSPORT_DECLARATIONS_H
