//
// Created by David Seery on 26/03/15.
// Copyright (c) 2015 University of Sussex. All rights reserved.
//


#if !defined(__initial_conditions_forward_declare_H_) && !defined(__initial_conditions_H_)
#define __initial_conditions_forward_declare_H_


#include <iostream>


namespace transport
	{

    template <typename number> class initial_conditions;

    template <typename number>
    std::ostream& operator<<(std::ostream& out, const initial_conditions<number>& obj);

	}   // namespace transport


#endif //__initial_conditions_forward_declare_H_
