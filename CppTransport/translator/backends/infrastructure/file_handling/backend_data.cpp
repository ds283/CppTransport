//
// Created by David Seery on 18/12/2015.
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


#include <vector>
#include <sstream>

#include "backend_data.h"
#include "msg_en.h"

#include "lexeme.h"
#include "lexstream_data.h"
#include "lexstream.h"

#include "boost/algorithm/string.hpp"
#include "boost/lexical_cast.hpp"
#include "translator_data.h"


constexpr auto BACKEND_TOKEN = "backend";
constexpr auto MINVER_TOKEN  = "minver";


enum class backend_keywords
  {
    backend, minver, lagrangian
  };

enum class backend_characters
  {
    comma, semicolon, colon, foreslash, equals
  };


typedef lexeme::lexeme<backend_keywords, backend_characters> b_lexeme;

typedef lexstream<backend_keywords, backend_characters> b_lexstream;

const b_lexeme::keyword_map keywords =
  {
    { "backend", backend_keywords::backend },
    { "minver", backend_keywords::minver },
    { "lagrangian", backend_keywords::lagrangian }
  };

const b_lexeme::character_map symbols =
  {
    { ",", { backend_characters::comma, true } },
    { ";", { backend_characters::semicolon, true } },
    { ":", { backend_characters::colon, true } },
    { "/", { backend_characters::foreslash, true } },
    { "=", { backend_characters::equals, true } }
  };


template <typename LexemeType, typename StreamType>
bool check_next_lexeme(LexemeType& lex, StreamType& stream, b_lexeme::type T, std::string err_msg)
  {
    // advance stream and read next value
    ++stream;
    lex = *stream;
    
    if(lex->get_type() == T) return true;
    
    lex->error(err_msg);
    return false;
  };


template <typename LexemeType, typename StreamType>
bool check_next_symbol(LexemeType& lex, StreamType& stream, backend_characters C, std::string err_msg)
  {
    // advance stream and read next value
    ++stream;
    lex = *stream;
    
    if(lex->get_type() == b_lexeme::type::character && lex->get_symbol() == C) return true;
    
    lex->error(err_msg);
    return false;
  };


backend_data::backend_data(const boost::filesystem::path& in, filestack& s, error_context::error_handler e,
                           error_context::warning_handler w, finder& f, argument_cache& a)
  : input(in)
  {
    // set up a lexstream to tokenize just one line from the top of the input file
    lexstream_data data(in, e, w, f, a, 1);
    b_lexstream instream(data, keywords, symbols);
    
    // loop over tokens; at the top level, skip over anything that isn't a keyword
    instream.reset();
    for(std::shared_ptr< b_lexeme > lex = *instream; lex; ++instream, lex = *instream)
      {
        if(lex->get_type() != b_lexeme::type::keyword) continue;
        
        const auto& keyw = lex->get_keyword();
        if(!keyw) continue;
        
        switch(*keyw)
          {
            case backend_keywords::backend:
              {
                if(!check_next_symbol(lex, instream, backend_characters::equals, ERROR_EXPECTED_EQUALS)) break;
                if(!check_next_lexeme(lex, instream, b_lexeme::type::identifier, ERROR_EXPECTED_BACKEND_IDENTIFIER)) break;
    
                try
                  {
                    auto id = lex->get_identifier();
                    SetContextedValue(this->name, *id, *lex, WARNING_DUPLICATE_TEMPLATE_BACKEND);
                  }
                catch(parse_error& xe)
                  {
                  }
                
                break;
              }

            case backend_keywords::minver:
              {
                if(!check_next_symbol(lex, instream, backend_characters::equals, ERROR_EXPECTED_EQUALS)) break;
                if(!check_next_lexeme(lex, instream, b_lexeme::type::integer, ERROR_EXPECTED_CPPTRANSPORT_VERSION)) break;
    
                try
                  {
                    auto ver = lex->get_integer();
                    SetContextedValue(this->minver, static_cast<unsigned int>(std::abs(*ver)), *lex, WARNING_DUPLICATE_TEMPLATE_MINVER);
                  }
                catch(parse_error& xe)
                  {
                  }
              
                break;
              }

            case backend_keywords::lagrangian:
              {
                if(!check_next_symbol(lex, instream, backend_characters::equals, ERROR_EXPECTED_EQUALS)) break;
                if(!check_next_lexeme(lex, instream, b_lexeme::type::identifier, ERROR_EXPECTED_TEMPLATE_TYPE)) break;
    
                try
                  {
                    auto id = lex->get_identifier();
                    
                    if(*id == "canonical")              SetContextedValue(this->type, model_type::canonical, *lex, WARNING_DUPLICATE_TEMPLATE_TYPE);
                    else if(*id == "nontrivial_metric") SetContextedValue(this->type, model_type::nontrivial_metric, *lex, WARNING_DUPLICATE_TEMPLATE_TYPE);
                  }
                catch(parse_error& xe)
                  {
                  }
    
                break;
              }
          }
      }
    
    if(!(*this))  // validate ourselves
      {
        std::ostringstream msg;
        msg << ERROR_IMPROPER_TEMPLATE_HEADER << " " << in;
    
        error_context err_context(s, e, w);
        err_context.error(msg.str());
      }
  }


bool backend_data::validate(translator_data& payload) const
  {
    unsigned int minver = this->get_min_version();

    if(minver > CPPTRANSPORT_NUMERIC_VERSION)
      {
        // we can't handle this template -- the platform version required is too new
        std::ostringstream msg;
        msg << ERROR_TEMPLATE_TOO_RECENT_A
            << " " << minver/100 << "." << minver%100 << ", " << ERROR_TEMPLATE_TOO_RECENT_B << " ="
            << " " << CPPTRANSPORT_VERSION << ")";
        
        this->minver->get_declaration_point().error(msg.str());
        return false;
      }
    
    model_type T = this->get_model_type();
    
    if(T != payload.model.get_lagrangian_type())
      {
        // template type doesn't match the type required by the model
        std::ostringstream msg;
        msg << ERROR_TEMPLATE_LAGRANGIAN_A << " '" << format(T) << "', "
            << ERROR_TEMPLATE_LAGRANGIAN_B << " '" << format(payload.model.get_lagrangian_type()) << "'";
        
        if(this->type)
          {
            this->type->get_declaration_point().error(msg.str());
          }
        else
          {
            error_context err_ctx = payload.make_error_context();
            err_ctx.error(msg.str());
          }
    
        return false;
      }
    
    return true;
  }
