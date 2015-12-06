//
// Created by David Seery on 27/06/2013.
// Copyright (c) 2013-15 University of Sussex. All rights reserved.
//


#include <assert.h>
#include <sstream>
#include <stdexcept>
#include <ctype.h>

#include "index_assignment.h"


#define FIELD_STRING     "f"
#define MOMENTUM_STRING  "p"
#define PARAMETER_STRING "param"


static void make_assignment(unsigned int fields, unsigned int parameters,
                            const std::vector<index_abstract>& indices,
                            unsigned int i, enum indexorder order,
                            std::deque<unsigned int>& assignment);

static void make_index_assignment(unsigned int fields, unsigned int parameters,
                                  const std::deque<unsigned int>& assignment,
                                  const std::vector<index_abstract>& indices,
                                  std::vector<index_assignment>& index_assignment);

static bool is_ordered(std::deque<unsigned int>& a, const std::vector<index_abstract>& indices);


// **************************************************************************************


std::vector< std::vector<index_assignment> > assignment_package::assign(const std::vector<index_abstract>& indices)
  {
    std::vector< std::vector<index_assignment> > rval;

    // generate the size of the set of all index assignments
    unsigned int limit = 1;
    for(int i = 0; i < indices.size(); ++i)
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
    // keeping track of which ones are correctly ordered (if necessary)
    std::deque<unsigned int> assignment;
    std::vector<index_assignment> index_assignment;
    for(unsigned int i = 0; i < limit; ++i)
      {
		    assignment.clear();
        make_assignment(this->num_fields, this->num_parameters, indices, i, this->order, assignment);

        if(is_ordered(assignment, indices))
          {
		        index_assignment.clear();
            make_index_assignment(this->num_fields, this->num_parameters, assignment, indices, index_assignment);

            rval.push_back(index_assignment);
          }
      }

    return(rval);
  }


std::string index_stringize(const index_assignment& index)
  {
    std::ostringstream rval;

    switch(index.trait)
      {
        case index_trait::field:
          {
            rval << FIELD_STRING << index.species;
            break;
          }

        case index_trait::momentum:
          {
            rval << MOMENTUM_STRING << index.species;
            break;
          }

        case index_trait::parameter:
          {
            rval << PARAMETER_STRING << index.species;
            break;
          }

        case index_trait::unknown:
          {
            assert(false);
            break;
          }
      }

    return(rval.str());
  }

unsigned int index_numeric(const index_assignment& index)
  {
    unsigned int rval = 0;

    switch(index.trait)
      {
        case index_trait::field:
          {
            rval = index.species;
            break;
          }

        case index_trait::momentum:
          {
            rval = index.num_fields + index.species;
            break;
          }

        case index_trait::parameter:
          {
            rval = index.species;
            break;
          }

        case index_trait::unknown:
          {
            assert(false);
            break;
          }
      }

    return(rval);
  }

unsigned int identify_index(char label)
	{
		unsigned int range = INDEX_RANGE_UNKNOWN;

		if(islower(label))        // lower-case indices label fields
			{
				range = INDEX_RANGE_FIELDS;
			}
		else if(isupper(label))   // upper-case indices label fields+momenta
			{
				range = INDEX_RANGE_ALL;
			}
		else if(isdigit(label))  // numeric indices label parameters
			{
				range = INDEX_RANGE_PARAMETER;
			}

		return(range);
	}


// / **************************************************************************************


static void make_assignment(unsigned int fields, unsigned int parameters,
                            const std::vector<index_abstract>& indices, unsigned int i,
                            enum indexorder order, std::deque<unsigned int>& assignment)
	{
    for(int j = 0; j < indices.size(); ++j)
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
        i = i / size;

        // order indicates whether we want to assign values to the indices
        // beginning from the left or from the right
        switch(order)
	        {
            case indexorder::left:
              {
                assignment.push_back(this_index);
                break;
              }

            case indexorder::right:
              {
                assignment.push_front(this_index);
                break;
              }
	        }
	    }
	}


static void make_index_assignment(unsigned int fields, unsigned int parameters,
                                  const std::deque<unsigned int>& assigned_numbers, const std::vector<index_abstract>& indices,
                                  std::vector<index_assignment>& assignment)
	{
    assert(assigned_numbers.size() == indices.size());
		assert(assignment.size() == 0);

    for(unsigned int i = 0; i < assigned_numbers.size(); ++i)
	    {
         index_assignment index;

        index.species = assigned_numbers[i];
        if(indices[i].range == INDEX_RANGE_PARAMETER)
	        {
            index.trait = index_trait::parameter;
            assert(index.species < parameters);
	        }
        else if(indices[i].range == INDEX_RANGE_ALL)
	        {
            if(index.species >= fields)
	            {
                index.trait = index_trait::momentum;
                index.species -= fields;
	            }
            else
	            {
                index.trait = index_trait::field;
	            }

            assert(index.species < fields);
	        }
        else if(indices[i].range == INDEX_RANGE_FIELDS)
	        {
            index.trait = index_trait::field;
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

        assignment.push_back(index);
	    }
	}

static bool is_ordered(std::deque<unsigned int>& a, const std::vector<index_abstract>& indices)
	{
    bool rval = true;

    assert(a.size() == indices.size());

    for(int i = 0; i < a.size(); ++i)
	    {
	      // UNCOMMENT TO ENFORCE ONLY ORDERED INDEX PAIRS
	      // (that's not usually what we want; we want full summation convention expressions)

//        if(i < a.size()-1)
//          {
//            if(a[i] < a[i+1])
//              {
//                rval = false;
//                break;
//              }
//          }
	    }

    return (rval);
	}


unsigned int assignment_package::value(index_assignment& v)
	{
    unsigned int rval = 0;

    switch(v.trait)
	    {
        case index_trait::parameter:
          {
            rval = v.species;
            break;
          }

        case index_trait::field:
          {
            rval = v.species;
            break;
          }

        case index_trait::momentum:
          {
            rval = v.species + v.num_fields;
            break;
          }

        case index_trait::unknown:
          {
            assert(false);
            break;
          }
	    }

    return (rval);
	}


std::vector<index_assignment> assignment_package::merge(const std::vector<index_assignment>& l, const std::vector<index_assignment>& r)
	{
    std::vector<index_assignment> merged = l;

		for(const index_assignment& ri : r)
			{
				bool found = false;

				for(const index_assignment& li : l)
					{
						if(ri.label == li.label)
							{
								found = true;
                break;
							}
					}

				if(found)
					{
						throw std::runtime_error("Duplicate index assignment while merging!");
					}

				merged.push_back(ri);
			}

		return(merged);
	}


std::vector<index_abstract> assignment_package::difference(const std::vector<index_abstract>& l, const std::vector<index_abstract>& r)
	{
    std::vector<index_abstract> diffed;

		for(const index_abstract& li : l)
			{
				bool found = false;

				for(const index_abstract& ri : r)
					{
						if(ri.label == li.label)
							{
								found = true;
                break;
							}
					}

				if(!found)
					{
						diffed.push_back(li);
					}
			}

		return(diffed);
	}
