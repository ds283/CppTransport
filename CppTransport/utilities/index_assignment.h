//
// Created by David Seery on 27/06/2013.
// Copyright (c) 2013 University of Sussex. All rights reserved.
//
// To change the template use AppCode | Preferences | File Templates.
//



#ifndef __index_assignment_H_
#define __index_assignment_H_

#include <iostream>
#include <vector>
#include <string>
#include <deque>


#define INDEX_RANGE_UNKNOWN   (0)
#define INDEX_RANGE_PARAMETER (1000)

enum index_trait
  {
    index_field, index_momentum, index_parameter, index_unknown
  };

enum index_order
  {
    index_left_order, index_right_order
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

    bool                    assign;      // should this index be included when generating assignments?
    struct index_assignment assignment;  // if assign = false, use this fixed assignment instead
  };


class assignment_package
  {
    public:
      assignment_package(unsigned int f, unsigned int p, enum index_order o=index_right_order)
      : num_fields(f), num_parameters(p), order(o) {}

      std::vector< std::vector<struct index_assignment> > assign(const std::vector<struct index_abstract>& indices);

    private:
      const unsigned int     num_fields;
      const unsigned int     num_parameters;
      const enum index_order order;
  };


std::string  index_stringize(const struct index_assignment& index);
unsigned int index_numeric  (const struct index_assignment& index);


#endif //__index_assignment_H_
