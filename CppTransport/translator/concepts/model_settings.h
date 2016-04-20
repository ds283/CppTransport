//
// Created by David Seery on 10/12/2013.
// Copyright (c) 2013-2016 University of Sussex. All rights reserved.
//


#ifndef CPPTRANSPORT_MODEL_SETTINGS_H
#define CPPTRANSPORT_MODEL_SETTINGS_H


#include <map>

#include "y_common.h"
#include "semantic_data.h"


// author declarations are slightly different; they do not inherit from the base
// declaration class since they are not really symbols in the normal sense
class author_declaration
  {

    // CONSTRUCTOR, DESTRUCTOR

  public:

    author_declaration(const std::string& n, const y::lexeme_type& l, author* a);

    ~author_declaration() = default;


    // INTERFACE

  public:

    //! return name
    std::string get_name() const;

    //! return email
    std::string get_email() const;

    //! get institute
    std::string get_institute() const;

    //! return lexeme representing declaration point
    const y::lexeme_type& get_declaration_point() const { return(this->declaration_point); }


    // PRINT TO STANDARD STREAM

  public:

    void print(std::ostream& stream) const;


    // INTERNAL DATA

  protected:

    //! link to author record
    std::unique_ptr<author> auth;

    //! reference to declaration lexeme
    const y::lexeme_type& declaration_point;

  };


enum class index_order
  {
    left, right
  };


enum class model_type
  {
    canonical, nontrivial_field_space
  };


typedef std::map< std::string, std::unique_ptr<author_declaration> > author_table;


#endif //CPPTRANSPORT_MODEL_SETTINGS_H
