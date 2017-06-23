//
// Created by David Seery on 12/03/15.
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


#ifndef CPPTRANSPORT_GINAC_CACHE_H
#define CPPTRANSPORT_GINAC_CACHE_H


#include <unordered_map>

#include "cache_detail/cache_key.h"

#include "timing_instrument.h"

#include "disable_warnings.h"
#include "ginac/ginac.h"


template <typename ItemClass>
class ginac_cache
	{

    // CONSTRUCTOR, DESTRUCTOR

  public:

		//! construct a cache object
		ginac_cache()
			: hits(0),
        misses(0)
			{
				// pause timers
				query_timer.stop();
        insert_timer.stop();
			}

		//! destructor is default
		~ginac_cache() = default;


		// INTERFACE - CACHING FUNCTIONS

  public:

		//! query for an element; returns true if it exists in the cache, in which case its value
		//! is copied to expr
		bool query(ItemClass c, unsigned int i, const cache_tags& t, GiNaC::ex& e);

		//! convenience form without an argument list
		bool query(ItemClass c, unsigned int i, GiNaC::ex& e);

		//! store an element in the cache. No collision checking is performed.
		void store(ItemClass c, unsigned int i, cache_tags t, const GiNaC::ex& e);

		//! convenience form without an argument list
		void store(ItemClass c, unsigned int i, const GiNaC::ex& e);


		// INTERFACE - CACHE STATISTICS

  public:

		//! get number of hits
		unsigned int get_hits() const { return(this->hits); }

		//! get number of misses
		unsigned int get_misses() const { return(this->misses); }

		//! get time spent performing queries
		boost::timer::nanosecond_type get_query_time() const { return(this->query_timer.elapsed().wall); }

    //! get time spent performing insertions
    boost::timer::nanosecond_type get_insert_time() const { return(this->insert_timer.elapsed().wall); }


		// INTERNAL DATA

  private:

		//! hash table representing the cache
    std::unordered_map< cache_key<ItemClass>, GiNaC::ex > cache;

		//! record number of cache hits
		unsigned int hits;

    //! record number of cache misses
		unsigned int misses;

    //! record time spent performing queries
		boost::timer::cpu_timer query_timer;

    //! record time spent performing insertions
    boost::timer::cpu_timer insert_timer;

	};


template <typename ItemClass>
bool ginac_cache<ItemClass>::query(ItemClass c, unsigned int i, const cache_tags& t, GiNaC::ex& e)
  {
    timing_instrument timer(this->query_timer);

    // build key from supplied data
    cache_key<ItemClass> key(c, i, t);

    auto it = this->cache.find(key);
    if(it == this->cache.end())
      {
        ++misses;
        return false;
      }

    // assign recovered cache value to e
    ++hits;
    e = it->second;
    return true;
  }


template <typename ItemClass>
bool ginac_cache<ItemClass>::query(ItemClass c, unsigned int i, GiNaC::ex& e)
	{
    timing_instrument timer(this->query_timer);

    // build key from supplied data using blank tags
    cache_key<ItemClass> key(c, i, cache_tags());

    auto it = this->cache.find(key);
    if(it == this->cache.end())
      {
        ++misses;
        return false;
      }

    // assign recovered cache value to e
    ++hits;
    e = it->second;
    return true;
	}


template <typename ItemClass>
void ginac_cache<ItemClass>::store(ItemClass c, unsigned int i, cache_tags t, const GiNaC::ex& e)
	{
    timing_instrument timer(this->insert_timer);
    auto res = this->cache.emplace(std::make_pair(cache_key<ItemClass>(c, i, t), e));
	}


template <typename ItemClass>
void ginac_cache<ItemClass>::store(ItemClass c, unsigned int i, const GiNaC::ex& e)
	{
    timing_instrument timer(this->insert_timer);
    auto res = this->cache.emplace(std::make_pair(cache_key<ItemClass>(c, i, cache_tags()), e));
	}



#endif //CPPTRANSPORT_GINAC_CACHE_H
