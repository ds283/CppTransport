//
// Created by David Seery on 27/06/2013.
// Copyright (c) 2013 University of Sussex. All rights reserved.
//
// To change the template use AppCode | Preferences | File Templates.
//


#include <assert.h>
#include <sstream>
#include <math.h>

#include "index_assignment.h"


#define FIELD_STRING    "f"
#define MOMENTUM_STRING "p"


static std::vector<unsigned int>            make_assignment      (unsigned int max, unsigned int indices, unsigned int i);
static std::vector<struct index_assignment> make_index_assignment(unsigned int max, const std::vector<unsigned int>& assignment, const std::vector<char>& labels);
static bool                                 is_ordered           (std::vector<unsigned int>& a);


// **************************************************************************************


std::vector< std::vector<struct index_assignment> > assignment_package::fields()
  {
    std::vector< std::vector<struct index_assignment> > rval;

    for(int i = 0; i < (int)pow(this->num_fields, this->labels.size()); i++)
      {
        std::vector<unsigned int> assignment = make_assignment(this->num_fields, this->labels.size(), i);
        assert(assignment.size() == this->labels.size());

        if(is_ordered(assignment))
          {
            rval.push_back(make_index_assignment(this->num_fields, assignment, this->labels));
          }
      }

    assert(rval.size() > 0);

    return(rval);
  }


std::vector< std::vector<struct index_assignment> > assignment_package::all()
  {
    std::vector< std::vector<struct index_assignment> > rval;

    for(int i = 0; i < (int)pow(2*this->num_fields, this->labels.size()); i++)
      {
        std::vector<unsigned int> assignment = make_assignment(2*this->num_fields, this->labels.size(), i);
        assert(assignment.size() == this->labels.size());

        if(is_ordered(assignment))
          {
            rval.push_back(make_index_assignment(this->num_fields, assignment, this->labels));
          }
      }

    assert(rval.size() > 0);

    return(rval);
  }


std::string index_stringize(struct index_assignment& index)
  {
    std::ostringstream rval;

    switch(index.trait)
      {
        case index_field:
          rval << FIELD_STRING << index.species;
          break;

        case index_momentum:
          rval << MOMENTUM_STRING << index.species;
          break;

        default:
          assert(false);
      }

    return(rval.str());
  }

unsigned int index_numeric(struct index_assignment& index)
  {
    unsigned int rval = 0;

    switch(index.trait)
      {
        case index_field:
          rval = index.species;
          break;

        case index_momentum:
          rval = index.num_fields + index.species;
          break;

        default:
          assert(false);
      }

    return(rval);
  }


// / **************************************************************************************


static std::vector<unsigned int> make_assignment(unsigned int max, unsigned int indices, unsigned int i)
  {
    std::vector<unsigned int> rval;

    for(int j = 0; j < indices; j++)
      {
        unsigned int this_index = i % max;     // max is maximum value assumed by each index
        i                       = i / max;

        rval.push_back(this_index);
      }

    return(rval);
  }

static std::vector<struct index_assignment> make_index_assignment(unsigned int max, const std::vector<unsigned int>& assignment, const std::vector<char>& labels)
  {
    std::vector<struct index_assignment> rval;

    assert(assignment.size() == labels.size());

    for(int i = 0; i < assignment.size(); i++)
      {
        struct index_assignment index;

        index.species = assignment[i];
        if(index.species >= max)               // this time, max is the total number of fields!
          {
            index.trait   = index_momentum;
            index.species -= max;
          }
        else
          {
            index.trait = index_field;
          }
        index.number     = i;
        index.label      = labels[i];
        index.num_fields = max;

        rval.push_back(index);
      }

    return(rval);
  }

static bool is_ordered (std::vector<unsigned int>& a)
  {
    bool rval = true;

    for(int i = 0; i < a.size()-1; i++)
      {
        if(a[i] < a[i+1])
          {
            rval = false;
            break;
          }
      }

    return(rval);
  }
