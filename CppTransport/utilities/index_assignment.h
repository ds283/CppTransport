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


#define INDEX_RANGE_UNKNOWN (0)

enum index_trait
  {
    index_field, index_momentum, index_unknown
  };

struct index_assignment
  {
    enum index_trait trait;       // index value - field or momentum?
    unsigned int     species;     // index value - species

    char             label;       // index identifier - alphanumeric label
    unsigned int     number;      // index identifier - serial number

    unsigned int     num_fields;  // number of fields
  };

struct index_abstract
  {
    char                    label;       // index label
    unsigned int            range;       // how many values does this index assume? this is a multiplier for the total number of fields

    bool                    assign;      // should this index be included when generating assignments?
    struct index_assignment assignment;  // if assign = false, use this fixed assignment instead
  };


class assignment_package
  {
    public:
      assignment_package(unsigned int f) : num_fields(f) {}

      std::vector< std::vector<struct index_assignment> > assign(const std::vector<struct index_abstract>& indices);

    private:
      const unsigned int      num_fields;
  };


std::string  index_stringize(const struct index_assignment& index);
unsigned int index_numeric  (const struct index_assignment& index);


#endif //__index_assignment_H_
