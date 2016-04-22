//
// Created by David Seery on 10/12/2013.
// Copyright (c) 2013-2016 University of Sussex. All rights reserved.
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

    void set_abserr(double d, y::lexeme_type& l) { this->abserr.reset(); this->abserr = std::make_unique< contexted_value<double> >(std::abs(d), l.get_error_context()); }

    double get_abserr() const { if(this->abserr) return *this->abserr; else return(DEFAULT_ABS_ERR); }

    void set_relerr(double d, y::lexeme_type& l) { this->relerr.reset(); this->relerr = std::make_unique< contexted_value<double> >(std::abs(d), l.get_error_context()); }

    double get_relerr() const { if(this->relerr) return *this->relerr; else return(DEFAULT_REL_ERR); }

    void set_stepsize(double d, y::lexeme_type& l) { this->stepsize.reset(); this->stepsize = std::make_unique< contexted_value<double> >(d, l.get_error_context()); }

    double get_stepsize() const { if(this->stepsize) return *this->stepsize; else return(DEFAULT_STEP_SIZE); }

    void set_name(const std::string& s, y::lexeme_type& l) { this->name.reset(); this->name = std::make_unique< contexted_value<std::string> >(s, l.get_error_context()); }

    const std::string get_name() const { if(this->name) return *this->name; else return(DEFAULT_STEPPER); }


    // INTERNAL DATA

  protected:

    // storage is via shared pointers because we sometimes copy the stepper structure

    std::shared_ptr< contexted_value<double> > abserr;

    std::shared_ptr< contexted_value<double> > relerr;

    std::shared_ptr< contexted_value<double> > stepsize;

    std::shared_ptr< contexted_value<std::string> > name;

  };


#endif //CPPTRANSPORT_STEPPER_H
