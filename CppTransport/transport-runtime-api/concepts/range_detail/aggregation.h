//
// Created by David Seery on 15/04/15.
// Copyright (c) 2015 University of Sussex. All rights reserved.
//


#ifndef __range_aggregation_H_
#define __range_aggregation_H_


#include "transport-runtime-api/concepts/range_detail/common.h"
#include "transport-runtime-api/concepts/range_detail/abstract.h"

#define __CPP_TRANSPORT_NODE_SUBRANGE_ARRAY "subranges"


namespace transport
	{

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

				//! Deserialization constructor
				aggregation_range(Json::Value& reader);


				// INTERFACE

		    //! Get minimum entry
		    virtual value get_min()                      const override { return(this->min); }

		    //! Get maximum entry
		    virtual value get_max()                      const override { return(this->max); }

		    //! Get number of steps
		    virtual unsigned int get_steps()             const override { return(this->steps); }

		    //! Get number of entries
		    virtual unsigned int size()                  const override { return(this->grid.size()); }

		    //! Is a simple, linear range?
		    virtual bool is_simple_linear()              const override { return(false); }

		    //! Get grid of entries
		    virtual const std::vector<value>& get_grid() const override { return(this->grid); }

		    value operator[](unsigned int d)             const override;


				// ADD RANGES TO THE AGGREGATION

		  public:

				//! add a subrange
				void add_subrange(range<value>& s);


		    // POPULATE GRID

		  protected:

		    //! fill out the grid entries
		    void populate_grid(void);


		    // CLONE -- implements a 'range<>' interface

		  public:

		    virtual range<value>* clone() const override { return new aggregation_range<value>(dynamic_cast<const aggregation_range<value>&>(*this)); }


		    // SERIALIZATION INTERFACE -- implements a 'serializable' interface

		  public:

		    //! Serialize this object
		    virtual void serialize(Json::Value& writer) const override;


				// INTERNAL DATA

		  protected:

				//! Minimium value
				value min;

		    //! Maximum value
		    value max;

		    //! Number of steps
		    unsigned int steps;

		    //! Grid of values
		    std::vector<value> grid;

				//! List of subranges; use std::shared_ptr to manage lifetime of instances
				typename std::list< std::shared_ptr< range<value> > > subrange_list;

			};


		template <typename value>
		aggregation_range<value>::aggregation_range()
			: min(DBL_MAX),
        max(-DBL_MAX),
        steps(0)
			{
			}


		template <typename value>
		aggregation_range<value>::aggregation_range(Json::Value& reader)
			{
		    Json::Value array = reader[__CPP_TRANSPORT_NODE_SUBRANGE_ARRAY];
				assert(array.isArray());

				for(Json::Value::iterator t = array.begin(); t != array.end(); t++)
					{
				    std::shared_ptr< range<value> > element(range_helper::deserialize<value>(*t));
						this->subrange_list.push_back(element);
					}

				this->populate_grid();
			}


		template <typename value>
		void aggregation_range<value>::serialize(Json::Value& writer) const
			{
				writer[__CPP_TRANSPORT_NODE_RANGE_TYPE] = std::string(__CPP_TRANSPORT_NODE_RANGE_AGGREGATE);

		    Json::Value array(Json::arrayValue);

				for(typename std::list< std::shared_ptr< range<value> > >::const_iterator t = this->subrange_list.begin(); t != this->subrange_list.end(); t++)
			    {
				    Json::Value obj(Json::objectValue);
				    (*t)->serialize(obj);
				    array.append(obj);
			    }

				writer[__CPP_TRANSPORT_NODE_SUBRANGE_ARRAY] = array;
			}


		template <typename value>
		void aggregation_range<value>::add_subrange(range<value>& s)
			{
		    std::shared_ptr< range<value> > element(s.clone());
				this->subrange_list.push_back(element);

				this->populate_grid();
			}


    template <typename value>
    value aggregation_range<value>::operator[](unsigned int d) const
	    {
        assert(d < this->grid.size());
        if(d < this->grid.size())
	        {
            return (this->grid[d]);
	        }
        else
	        {
            throw std::out_of_range(__CPP_TRANSPORT_RANGE_RANGE);
	        }
	    }


		template <typename value>
		void aggregation_range<value>::populate_grid()
			{
		    this->grid.clear();
		    this->min   = DBL_MAX;
		    this->max   = -DBL_MAX;
		    this->steps = 0;

				// splice all subranges together
				for(typename std::list< std::shared_ptr< range<value> > >::const_iterator t = this->subrange_list.begin(); t != this->subrange_list.end(); t++)
			    {
				    if((*t)->get_max() > this->max) this->max = (*t)->get_max();
				    if((*t)->get_min() < this->min) this->min = (*t)->get_min();

				    std::vector<value> temp = (*t)->get_grid();
				    this->grid.reserve(this->grid.size() + temp.size());
				    this->grid.insert(this->grid.end(), temp.begin(), temp.end());
			    }

				// sort resulting grid into order and remove duplicates
		    std::sort(this->grid.begin(), this->grid.end());
		    std::unique(this->grid.begin(), this->grid.end());

				this->steps = this->grid.size()+1;
			}

	}


#endif //__range_aggregation_H_
