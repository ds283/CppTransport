//
// Created by David Seery on 15/04/15.
// Copyright (c) 2015 University of Sussex. All rights reserved.
//


#ifndef __range_aggregation_H_
#define __range_aggregation_H_


#include <limits>

#include "transport-runtime-api/concepts/range_detail/common.h"
#include "transport-runtime-api/concepts/range_detail/abstract.h"

#define CPPTRANSPORT_NODE_SUBRANGE_ARRAY "subranges"


namespace transport
	{

    template <typename value> class aggregation_range;

    template <typename value>
    std::ostream& operator<<(std::ostream& out, aggregation_range<value>& obj);

		template <typename value>
		aggregation_range<value> operator+(const aggregation_range<value>& lhs, const range<value>& rhs);

		template <typename value>
		aggregation_range<value> operator+(const range<value>& lhs, const range<value>& rhs);

		// forward-declare helper
		namespace range_helper
			{
				template <typename value> range<value>* deserialize(Json::Value& reader);
			}

		template <typename value>
		class aggregation_range: public range<value>
			{

				// CONSTRUCTOR, DESTRUCTOR

		  public:

				//! Construct an aggregation range
				aggregation_range();

				//! Construct an aggregation range from a given subrange
				aggregation_range(const range<value>& a);

				//! Construct an aggregation range from a pair of subranges
				aggregation_range(const range<value>& a, const range<value>& b);

				//! Deserialization constructor
				aggregation_range(Json::Value& reader);

        //! Provide explicit copy constructor to handle deep move on std::unique_ptr
        aggregation_range(const aggregation_range<value>& obj);


				// OVERLOAD ARITHMETIC OPERATORS FOR CONVENIENCE

      public:

				//! assignment
				aggregation_range<value>& operator=(const range<value>& rhs) { this->subrange_list.clear(); this->add_subrange(rhs); return(*this); }

				//! compound addition
				aggregation_range<value>& operator+=(const range<value>& rhs) { this->add_subrange(rhs); return(*this); }

				//! addition
				friend aggregation_range<value> operator+ <>(const aggregation_range<value>& lhs, const range<value>& rhs);
				friend aggregation_range<value> operator+ <>(const range<value>& lhs, const range<value>& rhs);


				// INTERFACE

      public:

		    //! Get minimum entry
		    virtual value get_min()                      override       { if(this->dirty) this->populate_grid(); return(this->min); }

		    //! Get maximum entry
		    virtual value get_max()                      override       { if(this->dirty) this->populate_grid(); return(this->max); }

		    //! Get number of steps
		    virtual unsigned int get_steps()             override       { if(this->dirty) this->populate_grid(); return(this->steps); }

		    //! Get number of entries
		    virtual unsigned int size()                  override       { if(this->dirty) this->populate_grid(); return(this->grid.size()); }

		    //! Is a simple, linear range?
		    virtual bool is_simple_linear()              const override { return(false); }

		    //! Get grid of entries
		    virtual const std::vector<value>& get_grid() override       { if(this->dirty) this->populate_grid(); return(this->grid); }

		    value operator[](unsigned int d)             override;


				// ADD RANGES TO THE AGGREGATION

		  public:

				//! add a subrange
				void add_subrange(const range<value>& s);


		    // POPULATE GRID

		  protected:

		    //! fill out the grid entries
		    void populate_grid(void);


		    // CLONE -- implements a 'range<>' interface

		  public:

		    virtual aggregation_range<value>* clone() const override { return new aggregation_range<value>(dynamic_cast<const aggregation_range<value>&>(*this)); }


		    // SERIALIZATION INTERFACE -- implements a 'serializable' interface

		  public:

		    //! Serialize this object
		    virtual void serialize(Json::Value& writer) const override;

        friend std::ostream& operator<< <>(std::ostream& out, aggregation_range<value>& obj);


				// INTERNAL DATA

		  protected:

				//! dirty flag - if set, grid requires recalculation
				bool dirty;

				//! Minimium value
				value min;

		    //! Maximum value
		    value max;

		    //! Number of steps
		    unsigned int steps;

		    //! Grid of values
		    std::vector<value> grid;

        //! set up typename alias for list of pointers to constituent objects
        typedef typename std::list< std::unique_ptr< range<value> > > subrange_list_type;

				//! List of subranges; use std::unique_ptr to manage lifetime of instances
				subrange_list_type subrange_list;

			};


		template <typename value>
		aggregation_range<value> operator+(const aggregation_range<value>& lhs, const range<value>& rhs)
			{
		    return(aggregation_range<value>(lhs) += rhs);
			}


		template <typename value>
		aggregation_range<value> operator+(const range<value>& lhs, const range<value>& rhs)
			{
				return aggregation_range<value>(lhs, rhs);
			}


		template <typename value>
		aggregation_range<value>::aggregation_range()
			: min(std::numeric_limits<double>::max()),
        max(-std::numeric_limits<double>::max()),
        steps(0),
				dirty(true)
			{
			}


		template <typename value>
		aggregation_range<value>::aggregation_range(const range<value>& a)
			: aggregation_range<value>()
			{
		    this->subrange_list.emplace_back(a.clone());
			}


    template <typename value>
    aggregation_range<value>::aggregation_range(const range<value>& a, const range<value>& b)
	    : aggregation_range<value>()
	    {
        this->subrange_list.emplace_back(a.clone());
        this->subrange_list.emplace_back(b.clone());
	    }


		template <typename value>
		aggregation_range<value>::aggregation_range(Json::Value& reader)
			: aggregation_range<value>()
			{
		    Json::Value array = reader[CPPTRANSPORT_NODE_SUBRANGE_ARRAY];
				assert(array.isArray());

				for(Json::Value::iterator t = array.begin(); t != array.end(); ++t)
					{
						this->subrange_list.emplace_back(range_helper::deserialize<value>(*t));
					}

				this->populate_grid();
			}


    template <typename value>
    aggregation_range<value>::aggregation_range(const aggregation_range<value>& obj)
      : dirty(true),
        min(obj.min),
        max(obj.max),
        steps(obj.steps)
      {
        for(typename subrange_list_type::const_iterator t = obj.subrange_list.begin(); t != obj.subrange_list.end(); ++t)
          {
            if(*t)
              {
                subrange_list.emplace_back((*t)->clone());
              }
          }
      }


		template <typename value>
		void aggregation_range<value>::serialize(Json::Value& writer) const
			{
				writer[CPPTRANSPORT_NODE_RANGE_TYPE] = std::string(CPPTRANSPORT_NODE_RANGE_AGGREGATE);

		    Json::Value array(Json::arrayValue);

				for(typename subrange_list_type::const_iterator t = this->subrange_list.begin(); t != this->subrange_list.end(); ++t)
			    {
				    Json::Value obj(Json::objectValue);
				    (*t)->serialize(obj);
				    array.append(obj);
			    }

				writer[CPPTRANSPORT_NODE_SUBRANGE_ARRAY] = array;
			}


		template <typename value>
		void aggregation_range<value>::add_subrange(const range<value>& s)
			{
				this->subrange_list.emplace_back(s.clone());
				this->dirty = true;
			}


    template <typename value>
    value aggregation_range<value>::operator[](unsigned int d)
	    {
		    if(this->dirty) this->populate_grid();

        assert(d < this->grid.size());
        if(d < this->grid.size())
	        {
            return (this->grid[d]);
	        }
        else
	        {
            throw std::out_of_range(CPPTRANSPORT_RANGE_RANGE);
	        }
	    }


		template <typename value>
		void aggregation_range<value>::populate_grid()
			{
		    this->grid.clear();
		    this->min   = std::numeric_limits<double>::max();
		    this->max   = -std::numeric_limits<double>::max();
		    this->steps = 0;

				// splice all subranges together
				for(typename std::list< std::unique_ptr< range<value> > >::const_iterator t = this->subrange_list.begin(); t != this->subrange_list.end(); ++t)
			    {
				    if((*t)->get_max() > this->max) this->max = (*t)->get_max();
				    if((*t)->get_min() < this->min) this->min = (*t)->get_min();

				    std::vector<value> temp = (*t)->get_grid();
				    this->grid.reserve(this->grid.size() + temp.size());
				    this->grid.insert(this->grid.end(), temp.begin(), temp.end());
			    }

				// sort resulting grid into order and remove duplicates
		    std::sort(this->grid.begin(), this->grid.end());
        auto last = std::unique(this->grid.begin(), this->grid.end(), aggregation_range_impl::DuplicateRemovalPredicate<value>(1E-10));
		    this->grid.erase(last, this->grid.end());

				this->steps = this->grid.size()+1;
				this->dirty = false;
			}


    template <typename value>
    std::ostream& operator<<(std::ostream& out, aggregation_range<value>& obj)
      {
		    if(obj.dirty) obj.populate_grid();

        out << CPPTRANSPORT_AGGREGATION_RANGE_A << obj.subrange_list.size() << " ";
        out << CPPTRANSPORT_AGGREGATION_RANGE_B << '\n';

        out << CPPTRANSPORT_AGGREGATION_RANGE_C << '\n';
        for(unsigned int i = 0; i < obj.grid.size(); ++i)
          {
            out << i << ". " << obj.grid[i] << '\n';
          }

        return(out);
      }

	}


#endif //__range_aggregation_H_
