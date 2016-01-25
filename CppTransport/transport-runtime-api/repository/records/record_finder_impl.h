//
// Created by David Seery on 25/01/2016.
// Copyright (c) 2016 University of Sussex. All rights reserved.
//

#ifndef CPPTRANSPORT_RECORD_FINDER_IMPL_H
#define CPPTRANSPORT_RECORD_FINDER_IMPL_H


#include "transport-runtime-api/repository/records/record_finder_decl.h"


namespace transport
  {


    template <typename number>
    package_record<number>* package_finder<number>::operator()(const std::string& name)
      {
        return this->repo.query_package(name);
      }


    template <typename number>
    task_record<number>* task_finder<number>::operator()(const std::string& name)
      {
        return this->repo.query_task(name);
      }


    template <typename number>
    derived_product_record<number>* derived_product_finder<number>::operator()(const std::string& name)
      {
        return this->repo.query_derived_product(name);
      }

  }


#endif //CPPTRANSPORT_RECORD_FINDER_IMPL_H
