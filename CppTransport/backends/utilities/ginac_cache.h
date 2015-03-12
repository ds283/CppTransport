//
// Created by David Seery on 12/03/15.
// Copyright (c) 2015 University of Sussex. All rights reserved.
//


#ifndef __ginac_cache_H_
#define __ginac_cache_H_


#include <array>

#include "ginac/ginac.h"


#define DEFAULT_GINAC_CACHE_SIZE 2048


template <typename ExpressionType, unsigned int HashSize>
class ginac_cache
	{

  protected:

		class cache_element
			{

		  public:

				// constructor
				cache_element(ExpressionType t, unsigned int i, const std::vector<GiNaC::ex>& a, const GiNaC::ex& e);

				cache_element(ExpressionType t, unsigned int i, const GiNaC::ex& e);

				// destructor
				~cache_element() = default;

				// INTERFACE

		  public:

				bool compare(ExpressionType t, unsigned int i, const std::vector<GiNaC::ex>& a, GiNaC::ex& e);

				bool compare(ExpressionType t, unsigned int i, GiNaC::ex& e);

				// INTERNAL DATA

		  protected:

		    ExpressionType type;

				unsigned int index;
				std::vector<GiNaC::ex> args;

				GiNaC::ex expr;

			};

  public:

		//! construct a cache object
		ginac_cache()
			: hits(0), misses(0)
			{
			}

		//! destroy a cache object
		~ginac_cache() = default;


		// INTERFACE

  public:

		//! query for an element; returns true if it exists in the cache, in which case its value
		//! is copied to expr
		bool query(ExpressionType t, unsigned int i, const std::vector<GiNaC::ex>& a, GiNaC::ex& e);

		//! convenience form without an argument list
		bool query(ExpressionType t, unsigned int i, GiNaC::ex& e);

		//! store an element in the cache. No collision checking is performed.
		void store(ExpressionType t, unsigned int i, const std::vector<GiNaC::ex>& a, const GiNaC::ex& e);

		//! convenience form without an argument list
		void store(ExpressionType t, unsigned int i, const GiNaC::ex& e);

		//! get number of hits
		unsigned int get_hits() const { return(this->hits); }

		//! get number of misses
		unsigned int get_misses() const { return(this->misses); }


		// INTERNAL DATA

  private:

		std::array< std::vector<cache_element>, HashSize> cache;

		unsigned int hits;
		unsigned int misses;

	};


template <typename ExpressionType, unsigned int HashSize>
ginac_cache<ExpressionType, HashSize>::cache_element::cache_element(ExpressionType t, unsigned int i, const std::vector<GiNaC::ex>& a, const GiNaC::ex& e)
	: type(t),
		index(i),
		args(a),
		expr(e)
	{
	}


template <typename ExpressionType, unsigned int HashSize>
ginac_cache<ExpressionType, HashSize>::cache_element::cache_element(ExpressionType t, unsigned int i, const GiNaC::ex& e)
	: type(t),
	  index(i),
	  expr(e)
	{
	}


template <typename ExpressionType, unsigned int HashSize>
bool ginac_cache<ExpressionType, HashSize>::cache_element::compare(ExpressionType t, unsigned int i, const std::vector<GiNaC::ex>& a, GiNaC::ex& e)
	{
		if(this->type != t) return(false);
		if(this->index != i) return(false);
		if(this->args.size() != a.size()) return(false);

    std::vector<GiNaC::ex>::const_iterator m;
    std::vector<GiNaC::ex>::const_iterator n;
		for(m = this->args.begin(), n = a.begin(); m != this->args.end() && n != a.end(); m++, n++)
			{
		    GiNaC::ex test = ((*m) - (*n)).eval();
				bool equal = static_cast<bool>((test == 0));
				bool not_equal = static_cast<bool>((test != 0));
				if(!equal) return(false);
			}

		if(m != this->args.end() || n != a.end()) return(false);

		e = this->expr;
		return(true);
	}


template <typename ExpressionType, unsigned int HashSize>
bool ginac_cache<ExpressionType, HashSize>::cache_element::compare(ExpressionType t, unsigned int i, GiNaC::ex& e)
	{
    if(this->type != t) return(false);
    if(this->index != i) return(false);
    if(this->args.size() != 0) return(false);

    e = this->expr;
    return(true);
	}


template <typename ExpressionType, unsigned int HashSize>
bool ginac_cache<ExpressionType, HashSize>::query(ExpressionType t, unsigned int i, const std::vector<GiNaC::ex>& a, GiNaC::ex& e)
	{
		unsigned int hash = i % HashSize;

		for(typename std::vector<cache_element>::iterator u = this->cache[hash].begin(); u != this->cache[hash].end(); u++)
			{
				if((*u).compare(t, i, a, e))
					{
				    hits++;
				    return(true);
					}
			}

		misses++;
		return(false);
	}


template <typename ExpressionType, unsigned int HashSize>
bool ginac_cache<ExpressionType, HashSize>::query(ExpressionType t, unsigned int i, GiNaC::ex& e)
	{
    unsigned int hash = i % HashSize;

    for(typename std::vector<cache_element>::iterator u = this->cache[hash].begin(); u != this->cache[hash].end(); u++)
	    {
        if((*u).compare(t, i, e))
	        {
		        hits++;
            return(true);
	        }
	    }

		misses++;
    return(false);
	}


template <typename ExpressionType, unsigned int HashSize>
void ginac_cache<ExpressionType, HashSize>::store(ExpressionType t, unsigned int i, const std::vector<GiNaC::ex>& a, const GiNaC::ex& e)
	{
		unsigned int hash = i % HashSize;

		this->cache[hash].push_back(cache_element(t, i, a, e));
	}


template <typename ExpressionType, unsigned int HashSize>
void ginac_cache<ExpressionType, HashSize>::store(ExpressionType t, unsigned int i, const GiNaC::ex& e)
	{
    unsigned int hash = i % HashSize;

    this->cache[hash].push_back(cache_element(t, i, e));
	}



#endif //__ginac_cache_H_
