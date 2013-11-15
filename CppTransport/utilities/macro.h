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
#include <list>
#include <string>

#include "core.h"
#include "parse_tree.h"
#include "index_assignment.h"
#include "u_tensor_factory.h"
#include "cse.h"
#include "flatten.h"


class macro_package;

class replacement_data
  {
  public:
    replacement_data(std::list<std::string>& b, cse& t, flattener& f) : buffer(b), temp_factory(t), fl(f)
      {}

    script*           source;               // parse tree corresponding to input script
    std::string       source_file;          // name of input script

    u_tensor_factory* u_factory;            // manufactured u_tensor factory
    cse&              temp_factory;         // gadget for performing common sub-expression elimination
    flattener&        fl;                   // flattening rule

    macro_package*    ms;                   // macro package containing replacement rules

    std::string       output_file;          // output file being written
    std::string       core_file;            // name of core .h file (whatever output file is being produced)
    std::string       template_file;        // template file being used to produce output
    std::string       guard;                // tag for #ifndef guard

    unsigned int      unique;               // unique number used to keep track of tags

    unsigned int      num_fields;           // number of fields
    bool              do_cse;               // whether to perform CSE on output exprs, generating temporaries
    std::list<std::string>&
                      buffer;               // output buffer
    std::list<std::string>::iterator
                      pool;                 // current insertion point for temporary pool
    std::string       pool_template;        // template for generating temporaries

    std::deque<struct inclusion> path;
    unsigned int      current_line;
  };


typedef std::string (*replacement_function_simple)(struct replacement_data& data, const std::vector<std::string>& args);
typedef std::string (*replacement_function_index) (struct replacement_data& data, const std::vector<std::string>& args,
                                                   std::vector<struct index_assignment> indices, void* state);

typedef void*       (*replacement_function_pre)   (struct replacement_data& data, const std::vector<std::string>& args);
typedef void        (*replacement_function_post)  (void* state);

class macro_package
  {
    public:
      macro_package                       (unsigned int N_f, unsigned int N_p, enum indexorder o,
                                           std::string pf, std::string sp, replacement_data& d,
                                           unsigned int N1, const std::string* n1, const unsigned int* a1, const replacement_function_simple* f1,
                                           unsigned int N2, const std::string* n2, const unsigned int* a2, const replacement_function_simple* f2,
                                           unsigned int N3, const std::string* n3, const unsigned int* i3, const unsigned int*r3,
                                           const unsigned int* a3, const replacement_function_index* f3,
                                           const replacement_function_pre* pr3, const replacement_function_post* po3)
      : fields(N_f), parameters(N_p), order(o),
        prefix(pf), split(sp), data(d),
        N_pre(N1), pre_names(n1), pre_args(a1), pre_replacements(f1),
        N_post(N2), post_names(n2), post_args(a2), post_replacements(f2),
        N_index(N3), index_names(n3), index_indices(i3), index_ranges(r3), index_args(a3),
        index_replacements(f3), index_pre(pr3), index_post(po3)
      {}

      void apply                          (std::string& line);

    private:

      void apply_pre                      (std::string& line);
      void apply_post                     (std::string& line);
      void blank_post                     (std::string& line);
      void apply_index                    (std::string& line, const std::vector<struct index_abstract>& lhs_indices,
                                           const bool semicolon, const bool comma, const bool lhs_present);

      std::vector<struct index_abstract> get_lhs_indices(std::string lhs);
      void                               assign_lhs_index_types(std::string rhs, std::vector<struct index_abstract>& lhs_indices);
		  void                               assign_index_defaults(std::vector<struct index_abstract>& lhs_indices);

      std::vector<std::string>           get_argument_list(std::string& line, size_t pos, unsigned int num_args, std::string macro_name);

      std::vector<struct index_abstract> get_index_set(std::string line, size_t pos, std::string name, unsigned int indices, unsigned int range);

      void                               map_indices (std::string& line, std::string prefx, const std::vector<struct index_assignment>& assignment);


      unsigned int                        fields;
      unsigned int                        parameters;
      enum indexorder                     order;
      replacement_data&                   data;

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
      const replacement_function_pre*     index_pre;
      const replacement_function_post*    index_post;
  };


#endif //__macro_H_
