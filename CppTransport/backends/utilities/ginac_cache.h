//
// Created by David Seery on 12/03/15.
// Copyright (c) 2015 University of Sussex. All rights reserved.
//


#ifndef __ginac_cache_H_
#define __ginac_cache_H_


#include <array>

#include "ginac/ginac.h"

#include <boost/timer/timer.hpp>


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

		    bool compare(ExpressionType t, unsigned int i, const std::vector<GiNaC::ex>& a, GiNaC::ex& e,
		                 GiNaC::ex& expected, bool& matched);

				bool compare(ExpressionType t, unsigned int i, GiNaC::ex& e);


		    // WRITE SELF DETAILS TO STREAM

		  public:

		    //! write to stream
		    void write(std::ostream& out);


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
				// pause query timer
				query_timer.stop();
			}

		//! destroy a cache object
		~ginac_cache() = default;


		// INTERFACE - CACHING FUNCTIONS

  public:

		//! query for an element; returns true if it exists in the cache, in which case its value
		//! is copied to expr
		bool query(ExpressionType t, unsigned int i, const std::vector<GiNaC::ex>& a, GiNaC::ex& e);

		//! convenience form without an argument list
		bool query(ExpressionType t, unsigned int i, GiNaC::ex& e);

    //! query for an element; returns true if it exists in the cache, in which case its value
    //! is copied to expr. This is a debugging version which checks that the
		//! returned value matches the expected expression
    bool query(ExpressionType t, unsigned int i, const std::vector<GiNaC::ex>& a, GiNaC::ex& e, GiNaC::ex& expected);

		//! store an element in the cache. No collision checking is performed.
		void store(ExpressionType t, unsigned int i, const std::vector<GiNaC::ex>& a, const GiNaC::ex& e);

		//! convenience form without an argument list
		void store(ExpressionType t, unsigned int i, const GiNaC::ex& e);


		// INTERFACE - CACHE STATISTICS

  public:

		//! get number of hits
		unsigned int get_hits() const { return(this->hits); }

		//! get number of misses
		unsigned int get_misses() const { return(this->misses); }

		//! get time spend performing queryies
		boost::timer::nanosecond_type get_query_time() const { return(this->query_timer.elapsed().wall); }


		// INTERNAL API

  protected:

		//! hash
		unsigned int hash(unsigned int a, unsigned int b, unsigned int c) { return((a + b*101 + c*463) % HashSize); }


		// INTERNAL DATA

  private:

		// hash table representing the cache
		std::array< std::vector<cache_element>, HashSize> cache;

		// cache statistics
		unsigned int hits;
		unsigned int misses;

		boost::timer::cpu_timer query_timer;

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
		for(m = this->args.begin(), n = a.begin(); m != this->args.end() && n != a.end(); ++m, ++n)
			{
		    GiNaC::ex test = ((*m) - (*n)).eval();
				bool equal = static_cast<bool>(test == 0);
				if(!equal) return(false);
			}

		if(m != this->args.end() || n != a.end()) return(false);

		e = this->expr;
		return(true);
	}


template <typename ExpressionType, unsigned int HashSize>
bool ginac_cache<ExpressionType, HashSize>::cache_element::compare(ExpressionType t, unsigned int i, const std::vector<GiNaC::ex>& a, GiNaC::ex& e,
                                                                   GiNaC::ex& expected, bool& matched)
	{
    if(this->type != t) return(false);
    if(this->index != i) return(false);
    if(this->args.size() != a.size()) return(false);

    std::vector<GiNaC::ex>::const_iterator m;
    std::vector<GiNaC::ex>::const_iterator n;
    for(m = this->args.begin(), n = a.begin(); m != this->args.end() && n != a.end(); ++m, ++n)
	    {
        GiNaC::ex test = ((*m) - (*n)).eval();
        bool equal = static_cast<bool>(test == 0);
        if(!equal) return(false);
	    }

    if(m != this->args.end() || n != a.end()) return(false);

    e = this->expr;


    GiNaC::ex diff = (expected - e).eval();
    bool equal = static_cast<bool>(diff == 0);
    if(!equal)
	    {
        std::cout << "Detected mismatching expression: type = " << t << ", index = " << index << std::endl;
        std::cout << "  Lookup expression = " << e << std::endl;
        std::cout << "  Expected expression = " << expected << std::endl;
        std::cout << "  Difference = " << diff << std::endl;

        unsigned int count = 0;
        std::cout << "  Lookup argument list:" << std::endl;
        for(std::vector<GiNaC::ex>::const_iterator u = this->args.begin(); u != this->args.end(); ++u, ++count)
	        {
            std::cout << "  argument " << count << " = " << (*u) << std::endl;
	        }

        count = 0;
        std::cout << "  Expected argument list:" << std::endl;
        for(std::vector<GiNaC::ex>::const_iterator u = a.begin(); u != a.end(); ++u, ++count)
	        {
            std::cout << "  argument " << count << " = " << (*u) << std::endl;
	        }

		    matched = false;
	    }
		else
	    {
		    matched = true;
	    }

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
void ginac_cache<ExpressionType, HashSize>::cache_element::write(std::ostream& out)
	{
		out << "== Cache element" << std::endl;
		out << "   Type = " << this->type << std::endl;
		out << "   Index = " << this->index << std::endl;
		out << "   Number of arguments = " << this->args.size() << std::endl;

		unsigned int count = 0;
		for(std::vector<GiNaC::ex>::const_iterator t = this->args.begin(); t != this->args.end(); ++t, ++count)
			{
				out << "   Argument " << count << " = " << (*t) << std::endl;
			}
		out << "   Expression = " << this->expr << std::endl;
	}


template <typename ExpressionType, unsigned int HashSize>
bool ginac_cache<ExpressionType, HashSize>::query(ExpressionType t, unsigned int i, const std::vector<GiNaC::ex>& a, GiNaC::ex& e)
	{
		this->query_timer.resume();

		unsigned int hash = this->hash(t, i, a.size());

		for(typename std::vector<cache_element>::iterator u = this->cache[hash].begin(); u != this->cache[hash].end(); ++u)
			{
				if((*u).compare(t, i, a, e))
					{
				    hits++;
						this->query_timer.stop();
				    return(true);
					}
			}

		misses++;
		this->query_timer.stop();
		return(false);
	}


template <typename ExpressionType, unsigned int HashSize>
bool ginac_cache<ExpressionType, HashSize>::query(ExpressionType t, unsigned int i, const std::vector<GiNaC::ex>& a, GiNaC::ex& e, GiNaC::ex& expected)
	{
		this->query_timer.resume();

    unsigned int hash = this->hash(t, i, a.size());

    for(typename std::vector<cache_element>::iterator u = this->cache[hash].begin(); u != this->cache[hash].end(); ++u)
	    {
		    bool matched = true;

        if((*u).compare(t, i, a, e, expected, matched))
	        {
		        if(!matched)
			        {
		            std::cout << std::endl;
		            std::cout << "Detected matching failure -- dumping diagnostics" << std::endl;
		            std::cout << "  index = " << i << ", hash = " << hash << ", number of entries in hash table at this value = " << this->cache[hash].size() << std::endl;
		            for(typename std::vector<cache_element>::iterator v = this->cache[hash].begin(); v != this->cache[hash].end(); ++v)
			            {
		                std::cout << std::endl;
		                (*v).write(std::cout);
		                std::cout << std::endl;
		                std::cout << "Match to this cache element = " << (*v).compare(t, i, a, e) << std::endl;
			            }
		            std::cout << std::endl;
			        }

            hits++;
		        this->query_timer.stop();
            return(true);
	        }
	    }

    misses++;
		this->query_timer.stop();
    return(false);
	}


template <typename ExpressionType, unsigned int HashSize>
bool ginac_cache<ExpressionType, HashSize>::query(ExpressionType t, unsigned int i, GiNaC::ex& e)
	{
		this->query_timer.resume();

    unsigned int hash = this->hash(t, i, 0);

    for(typename std::vector<cache_element>::iterator u = this->cache[hash].begin(); u != this->cache[hash].end(); ++u)
	    {
        if((*u).compare(t, i, e))
	        {
		        hits++;
		        this->query_timer.stop();
            return(true);
	        }
	    }

		misses++;
		this->query_timer.stop();
    return(false);
	}


template <typename ExpressionType, unsigned int HashSize>
void ginac_cache<ExpressionType, HashSize>::store(ExpressionType t, unsigned int i, const std::vector<GiNaC::ex>& a, const GiNaC::ex& e)
	{
    unsigned int hash = this->hash(t, i, a.size());

		this->cache[hash].push_back(cache_element(t, i, a, e));
	}


template <typename ExpressionType, unsigned int HashSize>
void ginac_cache<ExpressionType, HashSize>::store(ExpressionType t, unsigned int i, const GiNaC::ex& e)
	{
    unsigned int hash = this->hash(t, i, 0);

    this->cache[hash].push_back(cache_element(t, i, e));
	}



#endif //__ginac_cache_H_
