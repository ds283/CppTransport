//
// Created by David Seery on 15/04/15.
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


#ifndef CPPTRANSPORT_RANGE_ABSTRACT_H
#define CPPTRANSPORT_RANGE_ABSTRACT_H


#include "transport-runtime/concepts/range_detail/common.h"


namespace transport
	{

    template <typename value>
    class range: public serializable
	    {

        // INTERFACE

      public:

        //! Get minimum entry
        virtual value get_min() = 0;

        //! Get maximum entry
        virtual value get_max() = 0;

        //! Get number of steps
        virtual unsigned int get_steps() = 0;

        //! Get number of entries
        virtual unsigned int size() = 0;

        //! Is a simple, linear range?
        virtual bool is_simple_linear() const = 0;

        //! Get grid of entries
        virtual const std::vector<value>& get_grid() = 0;

        virtual value operator[](unsigned int d) = 0;


		    // CLONE

      public:

		    virtual range<value>* clone() const = 0;

	    };


	}   // namespace transport


#endif //CPPTRANSPORT_RANGE_ABSTRACT_H
