//
// Created by David Seery on 25/12/2015.
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

#ifndef CPPTRANSPORT_LAMBDA_MANAGER_H
#define CPPTRANSPORT_LAMBDA_MANAGER_H


#include "lambdas.h"
#include "language_printer.h"
#include "translator_data.h"
#include "cse.h"

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
    std::unique_ptr<std::list<std::string>>
    make_temporary(const std::string& left, const std::string& mid, const std::string& right,
                   language_printer& printer, cse& cse_worker) const;


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
std::unique_ptr<std::list<std::string> >
lambda_record<LambdaItem>::make_temporary(const std::string& left, const std::string& mid, const std::string& right,
                                          language_printer& printer, cse& cse_worker) const
  {
    std::ostringstream out;

    auto rval = std::make_unique< std::list<std::string> >();

    // ask this lambda to format itself for deposition into the temporary pool
    auto v = lambda->make_temporary(left, mid, right, printer, cse_worker, this->num_fields);

    if(!v || v->size() == 0) return rval;

    if(v->size() == 1)
      {
        std::ostringstream out;
        const std::string& l = v->front();

        out << left << this->name << mid << l << right;
        rval->push_back(out.str());
      }
    else
      {
        std::ostringstream first;
        const std::string& l = v->front();

        first << left << this->name << mid << l;
        rval->push_back(first.str());

        auto t = v->begin();
        ++t;                    // points at second element in list, which is guaranteed to exist
        auto u = t;
        ++u;                    // points at third element in list, which may be end()

        while(u != v->end())
          {
            rval->push_back(*t);
            ++t;
            ++u;
          }

        std::ostringstream last;
        last << *t << right;
        rval->push_back(last.str());
      }

    return rval;
  }


typedef lambda_record<atomic_lambda> atomic_lambda_record;
typedef lambda_record<map_lambda> map_lambda_record;


//! lambda manager deals with storage and deposition of any lambdas
//! required by rolling up index sets into for-loops
class lambda_manager
  {

    // TYPES

  protected:

    //! cache for atomic-type lambda records
    using atomic_cache_type = std::list< std::unique_ptr<atomic_lambda_record> >;

    //! cache for map-type lambda records
    using map_cache_type = std::list< std::unique_ptr<map_lambda_record> >;

    // CONSTRUCTOR, DESTRUCTOR

  public:

    //! constructor
    lambda_manager(unsigned int s, language_printer& p, translator_data& pd, std::unique_ptr<cse> cw,
                   std::string k = OUTPUT_DEFAULT_LAMBDA_TEMPORARY_NAME,
                   std::string lt = OUTPUT_DEFAULT_LAMBDA_CSE_NAME);

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
    std::unique_ptr< std::list<std::string> >
    temporaries(const std::string& left, const std::string& mid, const std::string& right) const;

    //! get number of temporary definitions
    size_t number_temporaries() const { return this->atomic_cache.size() + this->map_cache.size(); }

    //! reset lambda manager on replacement of a temporary pool
    void clear();


    // STATISTICS

  public:

    //! get number of hits
    unsigned int get_hits() const { return(this->hits); }

    //! get number of misses
    unsigned int get_misses() const { return(this->misses); }

    //! get time spent performing queries
    boost::timer::nanosecond_type get_query_time() const { return(this->query_timer.elapsed().wall); }

    //! get time spent performing insertions
    boost::timer::nanosecond_type get_insert_time() const { return(this->insert_timer.elapsed().wall); }


    // INTERNAL API

  protected:

    //! search for a lambda record
    template <typename Iterator>
    Iterator find(Iterator begin, Iterator end, const typename Iterator::value_type::element_type::lambda_type& lambda);

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


    // STATISTICS

    //! record number of cache hits
    unsigned int hits;

    //! record number of cache misses
    unsigned int misses;

    // query timer
    boost::timer::cpu_timer query_timer;

    // insert timer
    boost::timer::cpu_timer insert_timer;


    // CACHES

    //! atomic lambda cache
    atomic_cache_type atomic_cache;

    //! map lambda cache
    map_cache_type map_cache;


    // SERVICE OBJECTS

    // CSE worker
    std::unique_ptr<cse> cse_worker;

  };


#endif //CPPTRANSPORT_LAMBDA_MANAGER_H
