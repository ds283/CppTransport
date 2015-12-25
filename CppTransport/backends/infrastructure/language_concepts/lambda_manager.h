//
// Created by David Seery on 25/12/2015.
// Copyright (c) 2015 University of Sussex. All rights reserved.
//

#ifndef CPPTRANSPORT_LAMBDA_MANAGER_H
#define CPPTRANSPORT_LAMBDA_MANAGER_H


#include "lambdas.h"
#include "language_printer.h"
#include "translator_data.h"

#include "boost/timer/timer.hpp"


//! lambda manager deals with storage and deposition of any lambdas
//! required by rolling up index sets into for-loops
class lambda_manager
  {

    // CONSTRUCTOR, DESTRUCTOR

  public:

    //! constructor
    lambda_manager(unsigned int s, language_printer& p, translator_data& pd, std::string k= OUTPUT_DEFAULT_CSE_TEMPORARY_NAME);

    //! destructor is default
    ~lambda_manager() = default;


    // INTERFACE

  public:

    //! cache an atomic lambda and return an invokation string suitable for use in macro replacement;
    //! takes ownership of the std::unique_ptr<> to the lambda object
    std::string cache(std::unique_ptr<atomic_lambda> lambda);

    //! cache a 'map' lambda and return an invokation string suitable for use in macro replacement;
    //! takes ownership of the std::unique_ptr<> to the lambda object
    std::string cache(std::unique_ptr<map_lambda> lambda);


    // INTERNAL DATA

  private:

    //! supplied language printer
    language_printer& printer;

    //! supplied translator data block
    translator_data& data_payload;

    //! serial number used to identify the current scope; incremented when a new
    //! TEMP_POOL macro is encountered
    unsigned int serial_number;

    //! serial number used to identify lambdas within the current scope
    unsigned int symbol_counter;

    //! kernel of lambda identifiers
    std::string temporary_name_kernel;

    // timer
    boost::timer::cpu_timer timer;

  };


#endif //CPPTRANSPORT_LAMBDA_MANAGER_H
