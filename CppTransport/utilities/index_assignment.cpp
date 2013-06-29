//
// Created by David Seery on 27/06/2013.
// Copyright (c) 2013 University of Sussex. All rights reserved.
//
// To change the template use AppCode | Preferences | File Templates.
//


#include <assert.h>
#include <sstream>

#include "index_assignment.h"


#define FIELD_STRING    "f"
#define MOMENTUM_STRING "p"


static std::vector<unsigned int>            make_assignment(unsigned int max,
  const std::vector<struct index_abstract>& indices, unsigned int i);

static std::vector<struct index_assignment> make_index_assignment(unsigned int max,
  const std::vector<unsigned int>& assignment, const std::vector<struct index_abstract>& indices);

static bool                                 is_ordered(std::vector<unsigned int>& a,
  const std::vector<struct index_abstract>& indices);


// **************************************************************************************


std::vector< std::vector<struct index_assignment> > assignment_package::assign(const std::vector<struct index_abstract>& indices)
  {
    std::vector< std::vector<struct index_assignment> > rval;

    unsigned int limit = 1;
    for(int i = 0; i < indices.size(); i++)
      {
        limit *= this->num_fields * indices[i].range;
      }

    for(int i = 0; i < limit; i++)
      {
        std::vector<unsigned int> assignment = make_assignment(this->num_fields, indices, i);
        assert(assignment.size() == indices.size());

        if(is_ordered(assignment, indices))
          {
            rval.push_back(make_index_assignment(this->num_fields, assignment, indices));
          }
      }

    assert(rval.size() > 0);

    return(rval);
  }


std::string index_stringize(const struct index_assignment& index)
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

unsigned int index_numeric(const struct index_assignment& index)
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


static std::vector<unsigned int> make_assignment(unsigned int max,
  const std::vector<struct index_abstract>& indices, unsigned int i)
  {
    std::vector<unsigned int> rval;

    for(int j = 0; j < indices.size(); j++)
      {
        unsigned int this_index = i % (indices[j].range * max);
        i                       = i / (indices[j].range * max);

        rval.push_back(this_index);
      }

    return(rval);
  }


static std::vector<struct index_assignment> make_index_assignment(unsigned int max,
  const std::vector<unsigned int>& assignment, const std::vector<struct index_abstract>& indices)
  {
    std::vector<struct index_assignment> rval;

    assert(assignment.size() == indices.size());

    for(int i = 0; i < assignment.size(); i++)
      {
        struct index_assignment index;

        index.species = assignment[i];
        if(index.species >= max)
          {
            index.trait   = index_momentum;
            index.species -= max;
          }
        else
          {
            index.trait = index_field;
          }
        index.number     = i;
        index.label      = indices[i].label;
        index.num_fields = max;

        rval.push_back(index);
      }

    return(rval);
  }

static bool is_ordered (std::vector<unsigned int>& a,
  const std::vector<struct index_abstract>& indices)
  {
    bool rval = true;

    assert(a.size() == indices.size());

    for(int i = 0; i < a.size(); i++)
      {
        if(indices[i].assign)
          {
            if(i < a.size()-1)
              {
                if(a[i] < a[i+1])
                  {
                    rval = false;
                    break;
                  }
              }
          }
        else
          {
            unsigned int count = 0;

            switch(indices[i].assignment.trait)
              {
                case index_field:
                  count = indices[i].assignment.species;
                  break;

                case index_momentum:
                  count = indices[i].assignment.species + indices[i].assignment.num_fields;
                  break;

                case index_unknown:
                default:
                  assert(false);
              }

            if(a[i] != count)
              {
                rval = false;
              }
          }
      }

    return(rval);
  }
