//
// Created by David Seery on 27/06/2013.
// Copyright (c) 2013 University of Sussex. All rights reserved.
//
// To change the template use AppCode | Preferences | File Templates.
//


#include <assert.h>
#include <sstream>

#include "index_assignment.h"


#define FIELD_STRING     "f"
#define MOMENTUM_STRING  "p"
#define PARAMETER_STRING "param"


static void
  make_assignment(unsigned int fields, unsigned int parameters,
  const std::vector<struct index_abstract>& indices, unsigned int i, enum indexorder order, std::deque<unsigned int>& assignment);

static void
  make_index_assignment(unsigned int fields, unsigned int parameters,
  const std::deque<unsigned int>& assignment, const std::vector<struct index_abstract>& indices,
  std::vector<struct index_assignment>& index_assignment);

static bool
  is_ordered(std::deque<unsigned int>& a, const std::vector<struct index_abstract>& indices);


// **************************************************************************************


std::vector< std::vector<struct index_assignment> > assignment_package::assign(const std::vector<struct index_abstract>& indices)
  {
    std::vector< std::vector<struct index_assignment> > rval;

    // generate the size of the set of all index assignments
    unsigned int limit = 1;
    for(int i = 0; i < indices.size(); i++)
      {
        if(indices[i].range == INDEX_RANGE_PARAMETER)
          {
            limit *= this->num_parameters;
          }
        else
          {
            limit *= this->num_fields * indices[i].range;
          }
      }

    // now work through the space of all index assignments, in the correct order,
    // keeping track of which ones are correctly ordered.
    std::deque<unsigned int> assignment;
    std::vector<struct index_assignment> index_assignment;
    for(unsigned int i = 0; i < limit; i++)
      {
        make_assignment(this->num_fields, this->num_parameters, indices, i, this->order, assignment);
        assert(assignment.size() == indices.size());

        if(is_ordered(assignment, indices))
          {
            make_index_assignment(this->num_fields, this->num_parameters, assignment, indices, index_assignment);
            rval.push_back(index_assignment);
          }
      }

//    assert(rval.size() > 0);

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

        case index_parameter:
          rval << PARAMETER_STRING << index.species;
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

        case index_parameter:
          rval = index.species;
          break;

        default:
          assert(false);
      }

    return(rval);
  }


// / **************************************************************************************


static void make_assignment(unsigned int fields, unsigned int parameters,
  const std::vector<struct index_abstract>& indices, unsigned int i,
  enum indexorder order, std::deque<unsigned int>& assignment)
  {
    assignment.clear();

    for(int j = 0; j < indices.size(); j++)
      {
        unsigned int size = 1;

        if(indices[j].range == INDEX_RANGE_PARAMETER)
          {
            size = parameters;
          }
        else
          {
            size = indices[j].range * fields;
          }

        unsigned int this_index = i % size;
        i                       = i / size;

        // order indicates whether we want to assign values to the indices
        // beginning from the left or from the right
        switch(order)
          {
            case indexorder_left:
              assignment.push_back(this_index);
              break;
            case indexorder_right:
            default:
              assignment.push_front(this_index);
          }
      }
  }


static void make_index_assignment(unsigned int fields, unsigned int parameters,
  const std::deque<unsigned int>& assignment, const std::vector<struct index_abstract>& indices,
  std::vector<struct index_assignment>& index_assignment)
  {
    assert(assignment.size() == indices.size());

    index_assignment.clear();

    for(unsigned int i = 0; i < assignment.size(); i++)
      {
        struct index_assignment index;

        index.species = assignment[i];
        if (indices[i].range == INDEX_RANGE_PARAMETER)
          {
            index.trait = index_parameter;
            assert(index.species < parameters);
          }
        else if(indices[i].range == 2)
          {
            if(index.species >= fields)
              {
                index.trait = index_momentum;
                index.species -= fields;
              }
            else
              {
                index.trait = index_field;
              }

            assert(index.species < fields);
          }
        else if (indices[i].range == 1)
          {
            index.trait = index_field;
            assert(index.species < fields);
          }
        else
          {
            assert(false);
          }

        index.number     = i;
        index.label      = indices[i].label;
        index.num_fields = fields;
        index.num_params = parameters;

        index_assignment.push_back(index);
      }
  }

static bool is_ordered (std::deque<unsigned int>& a, const std::vector<struct index_abstract>& indices)
  {
    bool rval = true;

    assert(a.size() == indices.size());

    for(int i = 0; i < a.size(); i++)
      {
        if(indices[i].assign)
          {
            // UNCOMMENT TO ENFORCE ONLY ORDERED INDEX PAIRS
            // (that's not usually what we want; we want full summation convention expressions)
            //
//            if(i < a.size()-1)
//              {
//                if(a[i] < a[i+1])
//                  {
//                    rval = false;
//                    break;
//                  }
//              }
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

                case index_parameter:
                  count = indices[i].assignment.species;
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


unsigned int assignment_package::value(struct index_assignment& v)
  {
    unsigned int rval = 0;

    switch(v.trait)
      {
        case index_parameter:
          rval = v.species;
          break;

        case index_field:
          rval = v.species;
          break;

        case index_momentum:
          rval = v.species + v.num_fields;
          break;

        case index_unknown:
        default:
          assert(false);
      }

    return(rval);
  }