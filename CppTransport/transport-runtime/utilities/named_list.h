//
// Created by David Seery on 08/05/2014.
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


#ifndef __named_list_H_
#define __named_list_H_


#include <string>
#include <algorithm>
#include <map>
#include <vector>

#include "transport-runtime/localizations/messages_en.h"
#include "transport-runtime/exceptions.h"


namespace
	{

    namespace named_list
	    {
        typedef std::map<std::string, unsigned int> ordering;

        inline ordering make_ordering(const std::vector<std::string>& order)
	        {
            ordering order_map;

            for(unsigned int i = 0; i < order.size(); ++i)
	            {
                order_map.insert(std::make_pair(order[i], i));
	            }

            return(order_map);
	        }

        template <typename number> class comparator;

        template <typename number>
        class element
	        {
          public:
            element(std::string n, number p)
	            : value(p), name(n)
	            {
	            }

            number get_value() const { return(this->value); }
            const std::string& get_name() const { return(this->name); }

            friend class comparator<number>;

          protected:
            number value;
            std::string name;
	        };

        template <typename number>
        class comparator
	        {
          public:
            comparator(const ordering& o)
	            : order_map(o)
	            {
	            }

            bool operator()(const element<number>& a, const element<number>& b)
	            {
                auto t_a = this->order_map.find(a.name);
                auto t_b = this->order_map.find(b.name);

                if(t_a == this->order_map.end() || t_b == this->order_map.end()) throw transport::runtime_exception(transport::exception_type::RUNTIME_ERROR, CPPTRANSPORT_NAMED_LIST_FAIL);

                return((*t_a).second < (*t_b).second);
	            }

          protected:
            const ordering& order_map;
	        };

	    } // namespace named_list


	}   // unnamed namespace


#endif //__named_list_H_
