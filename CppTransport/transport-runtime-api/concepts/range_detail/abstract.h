//
// Created by David Seery on 15/04/15.
// Copyright (c) 2015 University of Sussex. All rights reserved.
//


#ifndef __range_abstract_H_
#define __range_abstract_H_


#include "transport-runtime-api/concepts/range_detail/common.h"


namespace transport
	{

    template <typename value>
    class range: public serializable
	    {

        // INTERFACE

      public:

        //! Get minimum entry
        virtual value get_min() const = 0;

        //! Get maximum entry
        virtual value get_max() const = 0;

        //! Get number of steps
        virtual unsigned int get_steps() const = 0;

        //! Get number of entries
        virtual unsigned int size() const = 0;

        //! Is a simple, linear range?
        virtual bool is_simple_linear() const = 0;

        //! Get grid of entries
        virtual const std::vector<value>& get_grid() const = 0;

        virtual value operator[](unsigned int d) const = 0;


		    // CLONE

      public:

		    virtual range<value>* clone() const = 0;

	    };


	}   // namespace transport


#endif //__range_abstract_H_
