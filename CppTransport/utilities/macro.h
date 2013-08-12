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
#include "u_tensor_factory.h"


struct replacement_data
  {
    script*           source;           // parse tree corresponding to input script
    std::string       source_file;      // name of input script
    u_tensor_factory* u_factory;        // manufactured u_tensor factory

    std::string       output_file;      // output file being written
    std::string       core_file;        // name of core .h file (whatever output file is being produced)
    std::string       template_file;    // template file being used to produce output
    std::string       guard;            // tag for #ifndef guard

    unsigned int      unique;           // unique number used to keep track of tags
  };


typedef std::string (*replacement_function_simple)(struct replacement_data& data, const std::vector<std::string>& args);
typedef std::string (*replacement_function_index) (struct replacement_data& data, const std::vector<std::string>& args,
                                                   std::vector<struct index_assignment> indices);


class macro_package
  {
    public:
      macro_package                       (unsigned int N_f, unsigned int N_p,
        std::string pf, std::string sp, struct replacement_data& d,
        unsigned int N1, const std::string* n1, const unsigned int* a1, const replacement_function_simple* f1,
        unsigned int N2, const std::string* n2, const unsigned int* a2, const replacement_function_simple* f2,
        unsigned int N3, const std::string*n3, const unsigned int*i3, const unsigned int*r3,
        const unsigned int*a3, const replacement_function_index*f3)
      : fields(N_f), parameters(N_p),
        prefix(pf), split(sp), data(d),
        N_pre(N1), pre_names(n1), pre_args(a1), pre_replacements(f1),
        N_post(N2), post_names(n2), post_args(a2), post_replacements(f2),
        N_index(N3), index_names(n3), index_indices(i3), index_ranges(r3), index_args(a3), index_replacements(f3)
      {}

      void apply                          (std::string& line, unsigned int current_line, const std::deque<struct inclusion>& path);

    private:

      void apply_pre                      (std::string& line, unsigned int current_line, const std::deque<struct inclusion>& path);
      void apply_post                     (std::string& line, unsigned int current_line, const std::deque<struct inclusion>& path);
      void apply_index                    (std::string& line, const std::vector<struct index_abstract>& lhs_indices,
                                           unsigned int current_line, const std::deque<struct inclusion>& path);

      std::vector<struct index_abstract>
            get_lhs_indices               (std::string lhs, unsigned int current_line, const std::deque<struct inclusion>& path);
      void
            assign_lhs_index_types        (std::string rhs, std::vector<struct index_abstract>& lhs_indices,
                                           unsigned int current_line, const std::deque<struct inclusion>& path);
		  void  assign_index_defaults				(std::vector<struct index_abstract>& lhs_indices);

      unsigned int                        fields;
      unsigned int                        parameters;
      struct replacement_data&            data;

      const std::string                   prefix;
      const std::string                   split;

      unsigned int                        N_pre;
      const std::string*                  pre_names;
      const unsigned int*                 pre_args;
      const replacement_function_simple*  pre_replacements;

      unsigned int                        N_post;
      const std::string*                  post_names;
      const unsigned int*                 post_args;
      const replacement_function_simple*  post_replacements;

      unsigned int                        N_index;
      const std::string*                  index_names;
      const unsigned int*                 index_indices;
      const unsigned int*                 index_ranges;
      const unsigned int*                 index_args;
      const replacement_function_index*   index_replacements;
  };


#endif //__macro_H_
