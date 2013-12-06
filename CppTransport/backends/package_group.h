//
// Created by David Seery on 05/12/2013.
// Copyright (c) 2013 University of Sussex. All rights reserved.
//



#ifndef __package_group_H_
#define __package_group_H_


#include "macro.h"
#include "replacement_rule_package.h"
#include "buffer.h"
#include "cse.h"
#include "error.h"


class package_group
  {
  public:
    package_group(macro_packages::replacement_data& d);
    ~package_group();

    virtual void                             set_buffer       (buffer* b) = 0;
    virtual void                             set_macros       (macro_package* m) = 0;

    virtual std::string                      get_name         () = 0;

    inline void                              error            (std::string const msg) { ::error(msg, this->data.current_line, this->data.path); }
    inline void                              warn             (std::string const msg)  { ::warn(msg, this->data.current_line, this->data.path); }

    std::vector<macro_packages::simple_rule> get_pre_ruleset  ();
    std::vector<macro_packages::simple_rule> get_post_ruleset ();
    std::vector<macro_packages::index_rule>  get_index_ruleset();

  protected:
    void push_back(macro_packages::replacement_rule_package* package);

    macro_packages::replacement_data&        data;
    u_tensor_factory*                        u_factory;
    cse*                                     cse_worker;  // should be set by implementations
    flattener*                               fl;

    std::list<macro_packages::replacement_rule_package*> packages;
  };


#endif //__package_group_H_
