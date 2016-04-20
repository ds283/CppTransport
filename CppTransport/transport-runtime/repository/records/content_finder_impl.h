//
// Created by David Seery on 25/01/2016.
// Copyright (c) 2016 University of Sussex. All rights reserved.
//

#ifndef CPPTRANSPORT_CONTENT_FINDER_IMPL_H
#define CPPTRANSPORT_CONTENT_FINDER_IMPL_H

#include "transport-runtime-api/repository/records/content_finder_decl.h"


namespace transport
  {


    template <typename number>
    std::unique_ptr< content_group_record<integration_payload> >
    integration_content_finder<number>::operator()(const std::string& name, const std::list<std::string>& tags)
      {
        return this->repo.find_integration_task_output(name, tags);
      }


    template <typename number>
    std::unique_ptr< content_group_record<postintegration_payload> >
    postintegration_content_finder<number>::operator()(const std::string& name, const std::list<std::string>& tags)
      {
        return this->repo.find_postintegration_task_output(name, tags);
      }


  }   // namespace transport


#endif //CPPTRANSPORT_CONTENT_FINDER_IMPL_H
