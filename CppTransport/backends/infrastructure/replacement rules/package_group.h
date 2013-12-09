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

    virtual void                                         set_buffer         (buffer* b) = 0;
    virtual void                                         set_macros         (macro_package* m) = 0;

    inline void                                          error              (std::string const msg) { ::error(msg, this->data.current_line, this->data.path); }
    inline void                                          warn               (std::string const msg)  { ::warn(msg, this->data.current_line, this->data.path); }

    // return references to our internal ruleset caches
    // TODO find some way to prevent them being changed explicitly - they can change *indirectly* by rebuilding the cache, so is it ok to use const?
    std::vector<macro_packages::simple_rule>&            get_pre_ruleset    ();
    std::vector<macro_packages::simple_rule>&            get_post_ruleset   ();
    std::vector<macro_packages::index_rule>&             get_index_ruleset  ();

  protected:
    void                                                 push_back          (macro_packages::replacement_rule_package* package);

    void                                                 build_pre_ruleset  ();
    void                                                 build_post_ruleset ();
    void                                                 build_index_ruleset();

    macro_packages::replacement_data&                    data;
    u_tensor_factory*                                    u_factory;
    cse*                                                 cse_worker;  // should be set by implementations
    flattener*                                           fl;

    std::list<macro_packages::replacement_rule_package*> packages;

    std::vector<macro_packages::simple_rule>             pre_ruleset;
    std::vector<macro_packages::simple_rule>             post_ruleset;
    std::vector<macro_packages::index_rule>              index_ruleset;
  };


#endif //__package_group_H_
