//
// Created by David Seery on 09/12/2013.
// Copyright (c) 2013 University of Sussex. All rights reserved.
//


#ifndef __macropackages_replacement_data_H_
#define __macropackages_replacement_data_H_


#include "parse_tree.h"


namespace macro_packages
  {

    class replacement_data
      {
      public:
        script*                      parse_tree;         // parse tree corresponding to input script
        std::string                  script_in;          // name of input script

        // information about the template being processed
        std::string                  template_in;        // template file being read from
        std::string                  file_out;           // output file being written

        std::string                  core_out;           // name of core .h file
        std::string                  implementation_out; // name of implementation .h file

        std::string                  guard;              // tag for #ifndef guard

        // basic information about the model being processed
        unsigned int                 num_fields;         // number of fields
        unsigned int                 num_params;         // number of parameters
        enum indexorder              index_order;        // index ordering

        // information to support helpful error output
        filestack*                   path;
        unsigned int                 current_line;
      };

  } // namespace macro_packages


#endif //__macropackages_replacement_data_H_
