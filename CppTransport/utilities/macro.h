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

#include "parse_tree.h"
#include "index_assignment.h"


struct replacement_data
  {
    script*     source;
    std::string hname;
    std::string source_file;
  };


typedef std::string (*replacement_function)     (struct replacement_data& data);
typedef std::string (*replacement_function_iter)(struct replacement_data& data, std::vector<struct index_assignment> indices);


class macro_package
  {
    public:
      macro_package                    (unsigned int N_f, std::string pf, std::string sp, struct replacement_data& d,
        unsigned int N1, const std::string* n1, const replacement_function* f1,
        unsigned int N2, const std::string* n2, const unsigned int* a2, const replacement_function_iter* f2,
        unsigned int N3, const std::string* n3, const unsigned int* a3, const replacement_function_iter* f3)
      : fields(N_f), prefix(pf), split(sp), data(d),
        sz1(N1), nm1(n1), fn1(f1),
        sz2(N2), nm2(n2), ag2(a2), fn2(f2),
        sz3(N3), nm3(n3), ag3(a3), fn3(f3)
      {}

      void apply                       (std::string& line);

    private:

      void apply_simple                (std::string& line);
      void apply_iterative_fields      (std::string& line);
      void apply_iterative_all         (std::string& line);

      unsigned int                     fields;
      struct replacement_data&         data;

      unsigned int                     sz1;
      const std::string*               nm1;
      const replacement_function*      fn1;

      unsigned int                     sz2;
      const std::string*               nm2;
      const unsigned int*              ag2;
      const replacement_function_iter* fn2;

      unsigned int                     sz3;
      const std::string*               nm3;
      const unsigned int*              ag3;
      const replacement_function_iter* fn3;

      const std::string                prefix;
      const std::string                split;
  };


#endif //__macro_H_
