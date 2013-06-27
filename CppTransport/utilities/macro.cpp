//
// Created by David Seery on 27/06/2013.
// Copyright (c) 2013 University of Sussex. All rights reserved.
//
// To change the template use AppCode | Preferences | File Templates.
//


#include <assert.h>

#include "macro.h"

static std::vector<char> get_index_set(std::string line, size_t pos, std::string name, unsigned int args);

void macro_package::apply(std::string& line)
  {
    std::string line_prefix = "";

    // first, apply all simple macros
    this->apply_simple(line);

    // is there a line split?
    size_t split_point;
    if((split_point = line.find(this->split)) != std::string::npos)
      {
        line_prefix = line.substr(0, split_point);
        line.replace(0, split_point + this->split.size(), "");
      }

    // now apply all iterated macros
    // these functions are practically exact duplicates; it's a pity that they duplicate code
    // in future, it would be nice to find a way to avoid this duplication
    this->apply_iterative_fields(line);
    this->apply_iterative_all(line);

    if(line_prefix != "")
      {
        line = line_prefix + "\n" + line;
      }
  }

void macro_package::apply_simple(std::string& line)
  {
    while(line.find(this->prefix) != std::string::npos)
      {
        bool fail = true;

        for(int i = 0; i < this->sz1; i++)
          {
            size_t pos;

            if((pos = line.find(this->prefix + this->nm1[i])) != std::string::npos)
              {
                // found a simple macro; replace it with its value
                line.replace(pos, this->prefix.size() + this->nm1[i].size(), (*(this->fn1[i]))(this->data));
                fail = false;
              }
          }

        if(fail)    // no more simple macros to replace
          {
            break;
          }
      }
  }

void macro_package::apply_iterative_fields(std::string& line)
  {
    std::string temp_line;
    std::string new_line = "";

    while(line.find(this->prefix) != std::string::npos)
      {
        bool fail = true;

        for(int i = 0; i < this->sz2; i++)
          {
            size_t pos;

            if((pos = line.find(this->prefix + this->nm2[i])) != std::string::npos)
              {
                std::string temp_line;
                std::string new_line = "";

                // found a macro -- strip out the index set
                std::vector<char> indices = get_index_set(line, pos + this->prefix.size() + this->nm2[i].size(), this->nm2[i], this->ag2[i]);

                assignment_package assign(this->fields, indices);

                // obtain a list of index assignments
                std::vector< std::vector<struct index_assignment> > assgn = assign.fields();

                // for each index assignment, write out a replaced version
                std::vector< std::vector<struct index_assignment> >::iterator ptr;
                for(ptr = assgn.begin(); ptr != assgn.end(); ptr++)
                  {
                    assert((*ptr).size() == indices.size());    // make sure we have the correct number of of indices

                    // replace macro
                    temp_line = line;
                    temp_line.replace(pos, this->prefix.size() + this->nm2[i].size() + 2 + this->ag2[i],
                      (*(this->fn2[i]))(this->data, *ptr));

                    // replace indices associated with the macro
                    for(int k = 0; k < indices.size(); k++)
                      {
                        size_t index_pos;

                        while((index_pos = temp_line.find(this->prefix + indices[k])) != std::string::npos)
                          {
                            temp_line.replace(index_pos, this->prefix.size() + 1, index_stringize((*ptr)[k]));
                          }
                      }

                    // add this line
                    if(ptr != assgn.begin())
                      {
                        new_line += "\n";
                      }
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
  }


void macro_package::apply_iterative_all(std::string& line)
  {
    std::string temp_line;
    std::string new_line = "";

    while(line.find(this->prefix) != std::string::npos)
      {
        bool fail = true;

        for(int i = 0; i < this->sz3; i++)
          {
            size_t pos;

            if((pos = line.find(this->prefix + this->nm3[i])) != std::string::npos)
              {
                std::string temp_line;
                std::string new_line = "";

                // found a macro -- strip out the index set
                std::vector<char> indices = get_index_set(line, pos + this->prefix.size() + this->nm3[i].size(), this->nm3[i], this->ag3[i]);

                assignment_package assign(this->fields, indices);

                // obtain a list of index assignments
                std::vector< std::vector<struct index_assignment> > assgn = assign.all();

                // for each index assignment, write out a replaced version
                std::vector< std::vector<struct index_assignment> >::iterator ptr;
                for(ptr = assgn.begin(); ptr != assgn.end(); ptr++)
                  {
                    assert((*ptr).size() == indices.size());    // make sure we have the correct number of of indices

                    // replace macro
                    temp_line = line;
                    temp_line.replace(pos, this->prefix.size() + this->nm3[i].size() + 2 + this->ag3[i],
                      (*(this->fn3[i]))(this->data, *ptr));

                    // replace indices associated with the macro
                    for(int k = 0; k < indices.size(); k++)
                      {
                        size_t index_pos;

                        while((index_pos = temp_line.find(this->prefix + indices[k])) != std::string::npos)
                          {
                            temp_line.replace(index_pos, this->prefix.size() + 1, index_stringize((*ptr)[k]));
                          }
                      }

                    // add this line
                    if(ptr != assgn.begin())
                      {
                        new_line += "\n";
                      }
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
  }

static std::vector<char> get_index_set(std::string line, size_t pos, std::string name, unsigned int args)
  {
    std::vector<char> indices;

    if(line.size() < pos + 2 + args + 1)
      {
        std::ostringstream msg;
        msg << ERROR_TOO_FEW_INDICES << " '" << name << "'";
        error(msg.str());
      }
    if(line[pos] != '[')
      {
        std::ostringstream msg;
        msg << ERROR_EXPECTED_OPEN_ARGL << " '" << name << "'";
        error(msg.str());
      }
    else
      {
        pos++;
      }

    for(int i = 0; i < args; i++)
      {
        if(line[pos] == ']')
          {
            std::ostringstream msg;
            msg << ERROR_TOO_FEW_INDICES << " '" << name << "'";
            error(msg.str());
            break;
          }
        else
          {
            indices.push_back(line[pos]);
            pos++;
          }
      }
    if(line[pos] != ']')
      {
        std::ostringstream msg;
        msg << ERROR_EXPECTED_CLOSE_ARGL << " '" << name << "'";
        error(msg.str());
      }

    return(indices);
  }
