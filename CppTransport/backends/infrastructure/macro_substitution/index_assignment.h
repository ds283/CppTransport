//
// Created by David Seery on 27/06/2013.
// Copyright (c) 2013-15 University of Sussex. All rights reserved.
//
// To change the template use AppCode | Preferences | File Templates.
//



#ifndef __index_assignment_H_
#define __index_assignment_H_

#include <iostream>
#include <vector>
#include <string>
#include <deque>

#include "indexorder.h"


#define INDEX_RANGE_FIELDS    (1)
#define INDEX_RANGE_ALL       (2)

#define INDEX_RANGE_UNKNOWN   (0)
#define INDEX_RANGE_PARAMETER (1000)


enum class index_trait
  {
    field, momentum, parameter, unknown
  };

struct index_assignment
  {
    enum index_trait trait;       // index value - field or momentum?
    unsigned int     species;     // index value - species

    char             label;       // index identifier - alphanumeric label
    unsigned int     number;      // index identifier - serial number

    unsigned int     num_fields;  // total number of fields
    unsigned int     num_params;  // total number of parameters
  };

struct index_abstract
  {
    char                    label;       // index label
    unsigned int            range;       // how many values does this index assume? this is a multiplier for the total number of fields
                                         // UNLESS it is INDEX_RANGE_PARAMETER, in which case this is a parameter index
  };


class assignment_package
  {

    // CONSTRUCTOR, DESTRUCTOR

  public:

    //! constructor
    assignment_package(unsigned int f, unsigned int p, enum indexorder o = indexorder::right)
	    : num_fields(f),
        num_parameters(p),
        order(o)
	    {
	    }

    //! destructor is default
    ~assignment_package() = default;


    // INTERFACE

  public:

    std::vector<std::vector<index_assignment> > assign(const std::vector<index_abstract>& indices);

    std::vector<index_assignment> merge(const std::vector<index_assignment>& l, const std::vector<index_assignment>& r);

    std::vector<index_abstract> difference(const std::vector<index_abstract>& l, const std::vector<index_abstract>& r);

    unsigned int value(struct index_assignment& v);


    // INTERNAL DATA

  private:

    const unsigned int    num_fields;
    const unsigned int    num_parameters;
    const enum indexorder order;

  };


std::string  index_stringize(const struct index_assignment& index);
unsigned int index_numeric  (const struct index_assignment& index);

unsigned int identify_index (char label);


#endif //__index_assignment_H_
