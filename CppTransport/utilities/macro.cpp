//
// Created by David Seery on 27/06/2013.
// Copyright (c) 2013 University of Sussex. All rights reserved.
//
// To change the template use AppCode | Preferences | File Templates.
//


#include <assert.h>
#include <ctype.h>

#include <boost/algorithm/string.hpp>

#include "macro.h"


static std::vector<std::string>           get_argument_list (std::string& line, size_t pos,
  unsigned int current_line, const std::deque<struct inclusion>& path,
  unsigned int num_args, std::string macro_name);

static std::vector<struct index_abstract> get_index_set     (std::string line, size_t pos, std::string name, unsigned int indices,
  unsigned int range, unsigned int current_line, const std::deque<struct inclusion>& path);

static void                               map_indices       (std::string& line, std::string prefx,
  const std::vector<struct index_assignment>& assignment);

// **************************************************************************************


void macro_package::apply(std::string& line, unsigned int current_line, const std::deque<struct inclusion>& path)
  {
    std::string line_prefix = "";
    std::string new_line = "";

    // apply all pre- macros
    this->apply_pre(line, current_line, path);

    // first, find a split point if there is one
    size_t split_point;
    if((split_point = line.find(this->split)) != std::string::npos)
      {
        line_prefix = line.substr(0, split_point);
        line.replace(0, split_point + this->split.size(), "");
      }

    // trim trailing white space on 'line'
    boost::algorithm::trim_right(line);

    // check if the last component is a semicolon
    // note std:string::back() and std::string::pop_back() require C++11
    bool semicolon = false;
    if(line.back() == ';')
      {
        semicolon = true;
        line.pop_back();
      }

    // extract a set of indices on the LHS, represented by here by the prefix 'line_prefix'
    // these 'lvalue' indices are excluded from the summation convention *within* each line
    // (of course, we eventually want to write out the whole system of equations;
    // we will have one equation for each assignment of indices on the LHS)
    std::vector<struct index_abstract> lhs_indices = this->get_lhs_indices(line_prefix, current_line, path);

    // work out the type of each lvalue index
    this->assign_lhs_index_types(line, lhs_indices, current_line, path);

    // generate an assignment for each lvalue index
    assignment_package lvalue_assigner(this->fields, this->parameters, this->order == indexorder_left ? index_left_order : index_right_order);
    std::vector< std::vector<struct index_assignment> > lvalue_assignments = lvalue_assigner.assign(lhs_indices);

    // for each index assignment, carry out replacement in 'line_prefix' and then
    // apply summation convention to the rest of the line
    for(int i = 0; i < lvalue_assignments.size(); i++)
      {
        assert(lvalue_assignments[i].size() == lhs_indices.size());

        std::string cur_line_prefix = line_prefix;
        std::string cur_line        = line;

        map_indices(cur_line_prefix, this->prefix, lvalue_assignments[i]);
        for(int j = 0; j < lhs_indices.size(); j++)
          {
            lhs_indices[j].assignment = (lvalue_assignments[i])[j];
          }

        this->apply_index(cur_line, lhs_indices, current_line, path, semicolon, line_prefix != "");

        if(i > 0)
          {
            new_line += NEWLINE_CHAR;
          }
//        new_line += cur_line_prefix + (cur_line_prefix != "" ? NEWLINE_CHAR : "") + cur_line;
        new_line += cur_line_prefix + cur_line;
      }

    line = new_line;

    this->apply_post(line, current_line, path);
  }

void macro_package::apply_pre(std::string& line, unsigned int current_line, const std::deque<struct inclusion>& path)
  {
    while(line.find(this->prefix) != std::string::npos)
      {
        bool fail = true;

        for(int i = 0; i < this->N_pre; i++)
          {
            size_t pos;

            if((pos = line.find(this->prefix + this->pre_names[i])) != std::string::npos)
              {
                std::vector<std::string> arg_list = get_argument_list(line, pos + this->prefix.size() + this->pre_names[i].size(),
                                                                      current_line, path, this->pre_args[i], this->pre_names[i]);

                // found a simple macro; replace it with its value
                line.replace(pos, this->prefix.size() + this->pre_names[i].size(), (*(this->pre_replacements[i]))(this->data, arg_list));
                fail = false;
              }
          }

        if(fail)    // no more simple macros to replace
          {
            break;
          }
      }
  }

void macro_package::apply_post(std::string& line, unsigned int current_line, const std::deque<struct inclusion>& path)
  {
    while(line.find(this->prefix) != std::string::npos)
      {
        bool fail = true;

        for(int i = 0; i < this->N_post; i++)
          {
            size_t pos;

            if((pos = line.find(this->prefix + this->post_names[i])) != std::string::npos)
              {
                std::vector<std::string> arg_list = get_argument_list(line, pos + this->prefix.size() + this->post_names[i].size(),
                  current_line, path, this->post_args[i], this->post_names[i]);

                // found a simple macro; replace it with its value
                line.replace(pos, this->prefix.size() + this->post_names[i].size(), (*(this->post_replacements[i]))(this->data, arg_list));
                fail = false;
              }
          }

        if(fail)    // no more simple macros to replace
          {
            break;
          }
      }
  }

void macro_package::blank_post(std::string& line, unsigned int current_line, const std::deque<struct inclusion>& path)
  {
    while(line.find(this->prefix) != std::string::npos)
      {
        bool fail = true;

        for(int i = 0; i < this->N_post; i++)
          {
            size_t pos;

            if((pos = line.find(this->prefix + this->post_names[i])) != std::string::npos)
              {
                std::vector<std::string> arg_list = get_argument_list(line, pos + this->prefix.size() + this->post_names[i].size(),
                  current_line, path, this->post_args[i], this->post_names[i]);

                // found a simple macro; replace it with its value
                line.replace(pos, this->prefix.size() + this->post_names[i].size(), "");
                fail = false;
              }
          }

        if(fail)    // no more simple macros to replace
          {
            break;
          }
      }
  }

// TODO: this macro replacement implementation is fairly kludgy - would be nice to improve it
void macro_package::apply_index(std::string& line, const std::vector<struct index_abstract>& lhs_indices,
  unsigned int current_line, const std::deque<struct inclusion>& path, const bool semicolon, const bool lhs_present)
  {
    std::string temp_line;
    std::string new_line = "";

    bool replaced = false;    // have we made any macro replacements? if not, might need to add a trailing semicolon later

    // loop while there are still macros to replace, indicated by the presence of 'this->prefix'
    while(line.find(this->prefix) != std::string::npos)
      {
        bool fail = true;   // flag to break out of the loop, if instances of 'this->prefix' still exist but can't be rewritten

        // loop over all macros, testing whether we can make a replacement
        for(int i = 0; i < this->N_index; i++)
          {
            size_t pos;

            if((pos = line.find(this->prefix + this->index_names[i])) != std::string::npos)
              {
                replaced = true;

                std::string temp_line;
                std::string new_line = "";

                // found a macro -- strip out the index set associated with it
                std::vector<struct index_abstract> indices = get_index_set(line, pos + this->prefix.size() + this->index_names[i].size(),
                  this->index_names[i], this->index_indices[i], this->index_ranges[i], current_line, path);

                // work through the index set, matching to lhs_indices if possible
                // these won't be replaced; instead, we use the current value provided to us
                // NOTE - this need not replace *all* instances of LHS indices (some might not appear
                // in association with this macro), so we cannot rely on that later
                for(int j = 0; j < indices.size(); j++)
                  {
                    for(int k = 0; k < lhs_indices.size(); k++)
                      {
                        if(indices[j].label == lhs_indices[k].label)
                          {
                            indices[j].assign     = false;
                            indices[j].assignment = lhs_indices[k].assignment;
                            break;
                          }
                      }
                  }

                // obtain a list of suitable index assignments
                assignment_package assign(this->fields, this->parameters, (this->order == indexorder_left) ? index_left_order : index_right_order);
                std::vector< std::vector<struct index_assignment> > assgn = assign.assign(indices);

                // for each index assignment, write out a replaced version
                for(int j = 0; j < assgn.size(); j++)
                  {
                    assert(assgn[j].size() == indices.size());    // make sure we have the correct number of of indices

                    // replace macro
                    temp_line = line;

                    std::ostringstream macro_instance;
                    macro_instance << this->prefix << this->index_names[i] << "[";
                    for(int k = 0; k < indices.size(); k++)
                      {
                        macro_instance << indices[k].label;
                      }
                    macro_instance << "]";

                    size_t r_pos;
                    while((r_pos = temp_line.find(macro_instance.str())) != std::string::npos)
                      {
                        std::vector<std::string> arg_list = get_argument_list(temp_line,
                          r_pos + macro_instance.str().size(),
                          current_line, path, this->index_args[i], this->index_names[i]);

                        temp_line.replace(r_pos, macro_instance.str().size(),
                          (*(this->index_replacements[i]))(this->data, arg_list, assgn[j]));
                      }

                    // map indices associated with this macro
                    map_indices(temp_line, this->prefix, assgn[j]);

                    // check whether there are any unresolved replacements after rewriting any remaining instances of LHS indices
                    // and removing any instances of post-macros

                    // if there are none, we want to consider adding a terminal semicolon if that has been asked for
                    // we only do this if there is no LHS present, because only in that case is the result of
                    // each individual index replacement a complete statement;
                    // if a LHS is present, then it's the *sum* of all index replacements which is a complete statement

                    // if there are still indices to be replaced then we do nothing

                    std::string temp_temp_line = temp_line;
                    std::vector<struct index_assignment> lhs_assignments;
                    for(int k = 0; k < lhs_indices.size(); k++)
                      {
                        lhs_assignments.push_back(lhs_indices[k].assignment);
                      }
                    map_indices(temp_temp_line, this->prefix, lhs_assignments);
                    this->blank_post(temp_temp_line, current_line, path);

                    if(temp_temp_line.find(this->prefix) == std::string::npos)
                      {
                        if(!lhs_present)
                          {
                            if(semicolon)
                              {
                                temp_line += ';';
                              }
                            temp_line += NEWLINE_CHAR;
                          }
                      }

                    // add this line
                    new_line += temp_line;
                  }

                fail = false;
                line = new_line;
              }
          }

        if(fail)
          {
            break;
          }
      }

    // finally, rewrite any instances of LHS indices which have not yet been replaced
    // eg. they may not appear explicitly in any RHS macros
    std::vector<struct index_assignment> lhs_assignments;
    for(int i = 0; i < lhs_indices.size(); i++)
      {
        lhs_assignments.push_back(lhs_indices[i].assignment);
      }
    map_indices(line, this->prefix, lhs_assignments);

    if((lhs_present || !replaced) && semicolon)
      {
        line += ";";
      }
  }


// **************************************************************************************


std::vector<struct index_abstract> macro_package::get_lhs_indices(std::string lhs,
  unsigned int current_line, const std::deque<struct inclusion>& path)
  {
    std::vector<struct index_abstract> rval;
    size_t                             pos;

    while((pos = lhs.find(this->prefix)) != std::string::npos)
      {
        if(pos + this->prefix.size() > lhs.size() + 1)
          {
            std::ostringstream msg;
            msg << ERROR_EXPECTED_INDEX << " '" << this->prefix << "'";
            error(msg.str(), current_line, path);
            lhs.replace(pos, this->prefix.size()+1, "");
          }
        else
          {
            struct index_abstract index;
            index.label  = lhs[pos + this->prefix.size()];
            index.range  = INDEX_RANGE_UNKNOWN;            // this will be assigned later, when we know which object this index belongs to
            index.assign = true;                           // mark for inclusion when generating assignments
            rval.push_back(index);

            while((pos = lhs.find(this->prefix + index.label)) != std::string::npos)
              {
                lhs.replace(pos, this->prefix.size()+1, "");
              }
          }
      }

    return(rval);
  }


void macro_package::assign_lhs_index_types(std::string rhs, std::vector<struct index_abstract>& lhs_indices,
  unsigned int current_line, const std::deque<struct inclusion>& path)
  {
    while(rhs.find(this->prefix) != std::string::npos)
      {
        bool fail = true;     // flag to break out of the loop, if instances of 'this->prefix' still exist but can't be recognized

        for(int i = 0; i < this->N_index; i++)
          {
            size_t pos;

            if((pos = rhs.find(this->prefix + this->index_names[i])) != std::string::npos)
              {
                // found a macro -- strip out the index set
                std::vector<struct index_abstract> indices = get_index_set(rhs, pos + this->prefix.size() + this->index_names[i].size(),
                  this->index_names[i], this->index_indices[i], this->index_ranges[i], current_line, path);

                // now try to match this index set with elements of lhs_indices
                for(int j = 0; j < indices.size(); j++)
                  {
                    for(int k = 0; k < lhs_indices.size(); k++)
                      {
                        if(indices[j].label == lhs_indices[k].label)
                          {
                            if(lhs_indices[k].range != INDEX_RANGE_UNKNOWN) // must be a duplicate
                              {
                                std::ostringstream msg;
                                msg << ERROR_DUPLICATE_INDEX << " '" << lhs_indices[k].label << "'";
                                error(msg.str(), current_line, path);
                              }
                            lhs_indices[k].range = this->index_ranges[i];

                            break;
                          }
                      }
                  }

                // remove this macro
                rhs.replace(pos, this->prefix.size() + this->index_names[i].size() + 2 + this->index_indices[i], "");
                fail = false;
              }
          }

        if(fail)
          {
            break;
          }
      }

    // pick sensible defaults for any LHS indices which remain unassigned
    this->assign_index_defaults(lhs_indices);
  }


void macro_package::assign_index_defaults(std::vector<struct index_abstract>& lhs_indices)
  {
    for(int i = 0; i < lhs_indices.size(); i++)
      {
        if(lhs_indices[i].range == INDEX_RANGE_UNKNOWN)
          {
            if(islower(lhs_indices[i].label))         // assume ranges only over fields if lower case Latin letter, otherwise fields+momenta
              {
                lhs_indices[i].range = 1;
              }
            else if (isdigit(lhs_indices[i].label))  // assume numeric labels range over parameters
              {
                lhs_indices[i].range = INDEX_RANGE_PARAMETER;
              }
            else
              {
                lhs_indices[i].range = 2;
              }
          }
      }
  }


// **************************************************************************************


static std::vector<std::string> get_argument_list(std::string& line, size_t pos,
  unsigned int current_line, const std::deque<struct inclusion>& path,
  unsigned int num_args, std::string macro_name)
  {
    std::vector<std::string> rval;
    size_t                   length = 0;

    if(line[pos] == '{')   // an argument list follows
      {
        length++;
        std::string arg;

        while(pos + length < line.size() && line[pos + length] != '}')
          {
            if(line[pos + length] == ',')
              {
                rval.push_back(arg);
                arg = "";
              }
            else
              {
                if(!isspace(line[pos + length]))
                  {
                    arg += line[pos + length];
                  }
              }
            length++;
          }
        if(pos + length >= line.size())
          {
            error(ERROR_EXPECTED_CLOSE_ARGL, current_line, path);
          }
        else
          {
            length++;
          }

        if(arg != "")
          {
            rval.push_back(arg);
          }
      }

    if(rval.size() != num_args)
      {
        std::ostringstream msg;
        msg << ERROR_WRONG_ARG_NUM << " '" << macro_name << "' (" << ERROR_WRONG_ARG_NUM_EXPECT << " " << num_args << ")";
        error(msg.str(), current_line, path);
      }

    line.replace(pos, length, "");
    return(rval);
  }

static std::vector<struct index_abstract> get_index_set(std::string line, size_t pos, std::string name, unsigned int indices,
  unsigned int range, unsigned int current_line, const std::deque<struct inclusion>& path)
  {
    std::vector<struct index_abstract> rhs_indices;

    if(line.size() < pos + 2 + indices)
      {
        std::ostringstream msg;
        msg << ERROR_TOO_FEW_INDICES << " '" << name << "'";
        error(msg.str(), current_line, path);
      }
    if(line[pos] != '[')
      {
        std::ostringstream msg;
        msg << ERROR_EXPECTED_OPEN_IDXL << " '" << name << "'";
        error(msg.str(), current_line, path);
      }
    else
      {
        pos++;
      }

    for(int i = 0; i < indices; i++)
      {
        if(line[pos] == ']')
          {
            std::ostringstream msg;
            msg << ERROR_TOO_FEW_INDICES << " '" << name << "'";
            error(msg.str(), current_line, path);
            break;
          }
        else
          {
            struct index_abstract index;

            index.label  = line[pos];
            index.range  = range;             // index_assignment has the potential for different index types per object, but not currently used
            index.assign = true;              // default to assign; may be switched later if this turns out to be a LHS index

            rhs_indices.push_back(index);
            pos++;
          }
      }
    if(line[pos] != ']')
      {
        std::ostringstream msg;
        msg << ERROR_EXPECTED_CLOSE_IDXL << " '" << name << "'";
        error(msg.str(), current_line, path);
      }

    return(rhs_indices);
  }


static void map_indices(std::string& line, std::string prefix, const std::vector<struct index_assignment>& assignment)
  {
    for(int k = 0; k < assignment.size(); k++)
      {
        size_t index_pos;

        while((index_pos = line.find(prefix + assignment[k].label)) != std::string::npos)
          {
            std::ostringstream replacement;
            replacement << index_numeric(assignment[k]);
            line.replace(index_pos, prefix.size() + 1, replacement.str());
          }
      }
  }
