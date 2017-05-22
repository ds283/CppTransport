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

    //! constructor
    stepper() = default;

    //! destructor
    ~stepper() = default;


    // INTERFACE

  public:

    //! set absolute error used by stepper
    bool set_abserr(double d, y::lexeme_type& l);

    //! get absolute error used by stepper; returns default if no value has been set
    double get_abserr() const { if(this->abserr) return *this->abserr; else return(DEFAULT_ABS_ERR); }

    //! set relative error used by stepper
    bool set_relerr(double d, y::lexeme_type& l);

    //! get relative error used by stepper; returns default if no value has been set
    double get_relerr() const { if(this->relerr) return *this->relerr; else return(DEFAULT_REL_ERR); }

    //! set stepsize to be used by stepper
    bool set_stepsize(double d, y::lexeme_type& l);

    //! get stepsize to be used by stepper; returns default if no value has been set
    double get_stepsize() const { if(this->stepsize) return *this->stepsize; else return(DEFAULT_STEP_SIZE); }

    //! set name of stepper
    bool set_name(const std::string& s, y::lexeme_type& l);

    //! get name of stepper; returns default if no value has been set
    const std::string get_name() const { if(this->name) return *this->name; else return(DEFAULT_STEPPER); }


    // INTERNAL DATA

  protected:

    // storage is via shared pointers because we sometimes copy the stepper structure

    //! contexted value representing absolute error
    std::shared_ptr< contexted_value<double> > abserr;

    //! contexted value representing relative error
    std::shared_ptr< contexted_value<double> > relerr;

    //! contexted value representing stepsize
    std::shared_ptr< contexted_value<double> > stepsize;

    //! contexted value representing stepper name
    std::shared_ptr< contexted_value<std::string> > name;

  };


inline bool stepper::set_name(const std::string& s, y::lexeme_type& l)
  {
    return SetContextedValue(this->name, s, l, ERROR_STEPPER_REDECLARATION);
  }


inline bool stepper::set_stepsize(double d, y::lexeme_type& l)
  {
    return SetContextedValue(this->stepsize, d, l, ERROR_STEPSIZE_REDECLARATION);
  }


inline bool stepper::set_relerr(double d, y::lexeme_type& l)
  {
    return SetContextedValue(this->relerr, d, l, ERROR_RELERR_REDECLARATION);
  }


inline bool stepper::set_abserr(double d, y::lexeme_type& l)
  {
    return SetContextedValue(this->abserr, d, l, ERROR_ABSERR_REDECLARATION);
  }


#endif //CPPTRANSPORT_STEPPER_H
