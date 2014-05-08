//
// Created by David Seery on 08/05/2014.
// Copyright (c) 2014 University of Sussex. All rights reserved.
//


#ifndef __named_list_H_
#define __named_list_H_


#include <string>
#include <algorithm>
#include <map>
#include <vector>

#include "transport-runtime-api/localizations/messages_en.h"
#include "transport-runtime-api/exceptions.h"


namespace
	{

    namespace named_list
	    {
        typedef std::map<std::string, unsigned int> ordering;

        inline ordering make_ordering(const std::vector<std::string>& order)
	        {
            ordering order_map;

            for(unsigned int i = 0; i < order.size(); i++)
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

                if(t_a == this->order_map.end() || t_b == this->order_map.end()) throw transport::runtime_exception(transport::runtime_exception::RUNTIME_ERROR, __CPP_TRANSPORT_NAMED_LIST_FAIL);

                return((*t_a).second < (*t_b).second);
	            }

          protected:
            const ordering& order_map;
	        };

	    } // namespace named_list


	}   // unnamed namespace


#endif //__named_list_H_
