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


enum field_trait { index_field, index_momentum };

struct index_assignment
  {
    enum field_trait trait;       // index value - field or momentum?
    unsigned int     species;     // index value - species

    char             label;       // index identifier - alphanumeric label
    unsigned int     number;      // index identifier - serial number

    unsigned int     num_fields;  // number of fields
  };


class assignment_package
  {
    public:
      assignment_package(unsigned int f, const std::vector<char>& l) : num_fields(f), labels(l) {}

      std::vector< std::vector<struct index_assignment> > fields   ();
      std::vector< std::vector<struct index_assignment> > all      ();

    private:
      const unsigned int      num_fields;
      const std::vector<char> labels;
  };


std::string  index_stringize(struct index_assignment& index);
unsigned int index_numeric  (struct index_assignment& index);


#endif //__index_assignment_H_
