//
// Created by David Seery on 25/12/2015.
// Copyright (c) 2015 University of Sussex. All rights reserved.
//

#include "lambda_manager.h"

#include "timing_instrument.h"


lambda_manager::lambda_manager(unsigned int s, language_printer& p, translator_data& pd, std::string k)
  : serial_number(s),
    symbol_counter(0),
    temporary_name_kernel(k),
    printer(p),
    data_payload(pd)
  {
    timer.stop();
  }


std::string lambda_manager::cache(std::unique_ptr<atomic_lambda> lambda)
  {
    timing_instrument(this->timer);

    atomic_cache_type::const_iterator t = this->find<atomic_lambda_record>(this->atomic_cache.cbegin(), this->atomic_cache.cend(), *lambda);

    if(t == this->atomic_cache.end())
      {
        this->atomic_cache.emplace_front(std::make_unique<atomic_lambda_record>(this->make_name(), std::move(lambda), this->data_payload.get_number_fields()));
        t = this->atomic_cache.begin();
      }

    return this->printer.lambda_invokation((*t)->get_name(), (*t)->get_lambda());
  }


std::string lambda_manager::cache(std::unique_ptr<map_lambda> lambda)
  {
    timing_instrument(this->timer);

    map_cache_type::const_iterator t = this->find<map_lambda_record>(this->map_cache.cbegin(), this->map_cache.cend(), *lambda);

    if(t == this->map_cache.end())
      {
        this->map_cache.emplace_front(std::make_unique<map_lambda_record>(this->make_name(), std::move(lambda), this->data_payload.get_number_fields()));
        t = this->map_cache.begin();
      }

    return this->printer.lambda_invokation((*t)->get_name(), (*t)->get_lambda());
  }


namespace lambda_manager_impl
  {

    template <typename RecordType>
    class LambdaRecordComparator
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        LambdaRecordComparator(const typename RecordType::lambda_type& a)
          : rhs(a)
          {
          }


        // INTERFACE

      public:

        bool operator()(const std::unique_ptr<RecordType>& b)
          {
            const typename RecordType::lambda_type& lhs = b->get_lambda();

            if(lhs.get_type() != rhs.get_type()) return false;

            auto lhs_tags = lhs.get_tags();
            auto rhs_tags = rhs.get_tags();

            if(lhs_tags.size() != rhs_tags.size()) return false;

            auto lhs_it = lhs_tags.begin();
            auto rhs_it = rhs_tags.begin();

            while(lhs_it != lhs_tags.end() && rhs_it != rhs_tags.end())
              {
                if(*lhs_it != *rhs_it) return false;
                ++lhs_it;
                ++rhs_it;
              }

            return(true);
          }


        // INTERNAL DATA

      protected:

        const typename RecordType::lambda_type& rhs;

      };

  }   // namespace lambda_manager_impl


template <typename RecordType>
typename std::list< std::unique_ptr<RecordType> >::const_iterator lambda_manager::find(typename std::list< std::unique_ptr<RecordType> >::const_iterator begin,
                                                                                       typename std::list< std::unique_ptr<RecordType> >::const_iterator end,
                                                                                       const typename RecordType::lambda_type& lambda) const
  {
    return std::find_if(begin, end, lambda_manager_impl::LambdaRecordComparator<RecordType>(lambda));
  }


std::string lambda_manager::make_name()
  {
    std::ostringstream name;
    name << this->temporary_name_kernel << "_" << this->serial_number << "_" << this->symbol_counter;

    ++this->symbol_counter;

    return name.str();
  }


void lambda_manager::clear()
  {
    // destroying the contents of the caches will destroy the std::unique_ptrs<>, and
    // therefore the lambda objects
    this->atomic_cache.clear();
    this->map_cache.clear();

    ++this->serial_number;
    this->symbol_counter = 0;
  }


std::unique_ptr< std::list<std::string> > lambda_manager::temporaries(const std::string& left, const std::string& mid, const std::string& right) const
  {
    std::unique_ptr< std::list<std::string> > rval = std::make_unique< std::list<std::string> >();

    for(const std::unique_ptr<atomic_lambda_record>& rec : this->atomic_cache)
      {
        rval->push_back(rec->make_temporary(left, mid, right, this->printer));
      }

    for(const std::unique_ptr<map_lambda_record>& rec : this->map_cache)
      {
        rval->push_back(rec->make_temporary(left, mid, right, this->printer));
      }

    return(rval);
  }
