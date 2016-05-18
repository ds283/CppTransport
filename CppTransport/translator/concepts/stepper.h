//
// Created by David Seery on 10/12/2013.
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


#ifndef CPPTRANSPORT_STEPPER_H
#define CPPTRANSPORT_STEPPER_H


#include <string>

#include "contexted_value.h"
#include "y_common.h"
#include "defaults.h"


class stepper
  {

  public:

    stepper() = default;

    ~stepper() = default;


    // INTERFACE

  public:

    void set_abserr(double d, y::lexeme_type& l);

    double get_abserr() const { if(this->abserr) return *this->abserr; else return(DEFAULT_ABS_ERR); }

    void set_relerr(double d, y::lexeme_type& l);

    double get_relerr() const { if(this->relerr) return *this->relerr; else return(DEFAULT_REL_ERR); }

    void set_stepsize(double d, y::lexeme_type& l);

    double get_stepsize() const { if(this->stepsize) return *this->stepsize; else return(DEFAULT_STEP_SIZE); }

    void set_name(const std::string& s, y::lexeme_type& l);

    const std::string get_name() const { if(this->name) return *this->name; else return(DEFAULT_STEPPER); }


    // INTERNAL DATA

  protected:

    // storage is via shared pointers because we sometimes copy the stepper structure

    std::shared_ptr< contexted_value<double> > abserr;

    std::shared_ptr< contexted_value<double> > relerr;

    std::shared_ptr< contexted_value<double> > stepsize;

    std::shared_ptr< contexted_value<std::string> > name;

  };


inline void stepper::set_name(const std::string& s, y::lexeme_type& l)
  {
    if(this->name)
      {
        l.error(ERROR_STEPPER_REDECLARATION);
        this->name->get_declaration_point().warn(NOTIFY_DUPLICATION_DECLARATION_WAS);
      }
    else
      {
        this->name = std::make_unique<contexted_value<std::string> >(s, l.get_error_context());
      }
  }


inline void stepper::set_stepsize(double d, y::lexeme_type& l)
  {
    if(this->stepsize)
      {
        l.error(ERROR_STEPSIZE_REDECLARATION);
        this->stepsize->get_declaration_point().warn(NOTIFY_DUPLICATION_DECLARATION_WAS);
      }
    else
      {
        this->stepsize = std::make_unique<contexted_value<double> >(d, l.get_error_context());
      }
  }


inline void stepper::set_relerr(double d, y::lexeme_type& l)
  {
    if(this->relerr)
      {
        l.error(ERROR_RELERR_REDECLARATION);
        this->relerr->get_declaration_point().warn(NOTIFY_DUPLICATION_DECLARATION_WAS);
      }
    else
      {
        this->relerr = std::make_unique<contexted_value<double> >(d, l.get_error_context());
      }
  }


inline void stepper::set_abserr(double d, y::lexeme_type& l)
  {
    if(this->abserr)
      {
        l.error(ERROR_ABSERR_REDECLARATION);
        this->abserr->get_declaration_point().warn(NOTIFY_DUPLICATION_DECLARATION_WAS);
      }
    else
      {
        this->abserr = std::make_unique<contexted_value<double> >(d, l.get_error_context());
      }
  }


#endif //CPPTRANSPORT_STEPPER_H
