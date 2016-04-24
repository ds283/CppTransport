//
// Created by David Seery on 25/12/2015.
// Copyright (c) 2016 University of Sussex. All rights reserved.
//

#ifndef CPPTRANSPORT_LAMBDA_MANAGER_H
#define CPPTRANSPORT_LAMBDA_MANAGER_H


#include "lambdas.h"
#include "language_printer.h"
#include "translator_data.h"

#include "boost/timer/timer.hpp"


template <typename LambdaItem>
class lambda_record
  {

  public:

    //! support lambda type introspection
    typedef LambdaItem lambda_type;


    // CONSTRUCTOR, DESTRUCTOR

  public:

    lambda_record(std::string n, std::unique_ptr<LambdaItem> l, unsigned int nf)
      : name(std::move(n)),
        lambda(std::move(l)),
        num_fields(nf)
      {
      }


    // INTERFACE

  public:

    //! get name
    const std::string& get_name() const { return(this->name); }

    //! get lambda objects
    const lambda_type& get_lambda() const { return(*this->lambda); }

    //! make temporary object for deposition into a pool
    std::string make_temporary(const std::string& left, const std::string& mid, const std::string& right, language_printer& printer) const;


    // INTERNAL DATA

  protected:

    //! owning pointer to lambda object
    std::unique_ptr<LambdaItem> lambda;

    //! name
    std::string name;

    //! cache number of fields for use when constructing relations (if needed)
    unsigned int num_fields;

  };


template <typename LambdaItem>
std::string lambda_record<LambdaItem>::make_temporary(const std::string& left, const std::string& mid, const std::string& right, language_printer& printer) const
  {
    std::ostringstream out;

    out << left << this->name << mid << lambda->make_temporary(printer, this->num_fields) << right << '\n';

    return(out.str());
  }


typedef lambda_record<atomic_lambda> atomic_lambda_record;
typedef lambda_record<map_lambda> map_lambda_record;


//! lambda manager deals with storage and deposition of any lambdas
//! required by rolling up index sets into for-loops
class lambda_manager
  {

    // CONSTRUCTOR, DESTRUCTOR

  public:

    //! constructor
    lambda_manager(unsigned int s, language_printer& p, translator_data& pd, std::string k= OUTPUT_DEFAULT_LAMBDA_TEMPORARY_NAME);

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

    //! compute list of temporaries which should be deposited in the current pool
    std::unique_ptr< std::list<std::string> > temporaries(const std::string& left, const std::string& mid, const std::string& right) const;

    //! reset lambda manager on replacement of a temporary pool
    void clear();


    // INTERNAL API

  protected:

    //! search for a lambda record
    template <typename RecordType>
    typename std::list<std::unique_ptr<RecordType> >::const_iterator find(typename std::list<std::unique_ptr<RecordType> >::const_iterator begin,
                                                                          typename std::list<std::unique_ptr<RecordType> >::const_iterator end,
                                                                          const typename RecordType::lambda_type& lambda) const;

    //! make a lambda name
    std::string make_name();

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


    // CACHES

    typedef std::list< std::unique_ptr<atomic_lambda_record> > atomic_cache_type;
    typedef std::list< std::unique_ptr<map_lambda_record> > map_cache_type;

    //! atomic lambda cache
    atomic_cache_type atomic_cache;

    //! map lambda cache
    map_cache_type map_cache;

  };


#endif //CPPTRANSPORT_LAMBDA_MANAGER_H
