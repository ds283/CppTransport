//
// Created by David Seery on 27/06/2013.
// Copyright (c) 2013 University of Sussex. All rights reserved.
//
// To change the template use AppCode | Preferences | File Templates.
//



#ifndef __macro_H_
#define __macro_H_

#include <iostream>
#include <vector>

#include "core.h"
#include "parse_tree.h"
#include "index_assignment.h"


struct replacement_data
  {
    script*     source;           // parse tree corresponding to input script
    std::string source_file;      // name of input script

    std::string output_file;      // output file being written
    std::string template_file;    // template file being used to produce output
  };


typedef std::string (*replacement_function_simple)(struct replacement_data& data);
typedef std::string (*replacement_function_index) (struct replacement_data& data, std::vector<struct index_assignment> indices);


class macro_package
  {
    public:
      macro_package                       (unsigned int N_f, std::string pf, std::string sp, struct replacement_data& d,
        unsigned int N1, const std::string* n1, const replacement_function_simple* f1,
        unsigned int N2, const std::string* n2, const unsigned int* i2, const unsigned int* r2, const replacement_function_index* f2)
      : fields(N_f), prefix(pf), split(sp), data(d),
        N_simple(N1), simple_names(n1), simple_replacements(f1),
        N_index(N2), index_names(n2), index_indices(i2), index_ranges(r2), index_replacements(f2)
      {}

      void apply                          (std::string& line, unsigned int current_line, const std::deque<struct inclusion>& path);

    private:

      void apply_simple                   (std::string& line, unsigned int current_line, const std::deque<struct inclusion>& path);
      void apply_index                    (std::string& line, const std::vector<struct index_abstract>& lhs_indices,
                                           unsigned int current_line, const std::deque<struct inclusion>& path);

      std::vector<struct index_abstract>
            get_lhs_indices               (std::string lhs, unsigned int current_line, const std::deque<struct inclusion>& path);
      void
            assign_lhs_index_types        (std::string rhs, std::vector<struct index_abstract>& lhs_indices,
                                           unsigned int current_line, const std::deque<struct inclusion>& path);

      unsigned int                        fields;
      struct replacement_data&            data;

      const std::string                   prefix;
      const std::string                   split;

      unsigned int                        N_simple;
      const std::string*                  simple_names;
      const replacement_function_simple*  simple_replacements;

      unsigned int                        N_index;
      const std::string*                  index_names;
      const unsigned int*                 index_indices;
      const unsigned int*                 index_ranges;
      const replacement_function_index*   index_replacements;
  };


#endif //__macro_H_
