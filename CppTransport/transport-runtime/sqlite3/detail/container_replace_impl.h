//
// Created by David Seery on 26/01/2016.
// Copyright (c) 2016 University of Sussex. All rights reserved.
//

#ifndef CPPTRANSPORT_SQLITE3_CONTAINER_REPLACE_IMPL_H
#define CPPTRANSPORT_SQLITE3_CONTAINER_REPLACE_IMPL_H


#include "transport-runtime-api/sqlite3/detail/container_replace_decl.h"


namespace transport
  {


    template <typename number>
    void sqlite3_container_replace_twopf<number>::operator()(generic_batcher& batcher, replacement_action action)
      {
        this->data_mgr.replace_temp_twopf_container(this->tempdir, this->worker, this->mdl, this->ics, batcher, action);
      }


    template <typename number>
    void sqlite3_container_replace_threepf<number>::operator()(generic_batcher& batcher, replacement_action action)
      {
        this->data_mgr.replace_temp_threepf_container(this->tempdir, this->worker, this->mdl, this->ics, batcher, action);
      }


    template <typename number>
    void sqlite3_container_replace_zeta_twopf<number>::operator()(generic_batcher& batcher, replacement_action action)
      {
        this->data_mgr.replace_temp_zeta_twopf_container(this->tempdir, this->worker, this->mdl, batcher, action);
      }


    template <typename number>
    void sqlite3_container_replace_zeta_threepf<number>::operator()(generic_batcher& batcher, replacement_action action)
      {
        this->data_mgr.replace_temp_zeta_threepf_container(this->tempdir, this->worker, this->mdl, batcher, action);
      }


    template <typename number>
    void sqlite3_container_replace_fNL<number>::operator()(generic_batcher& batcher, replacement_action action)
      {
        this->data_mgr.replace_temp_fNL_container(this->tempdir, this->worker, this->type, batcher, action);
      }


  }   // namespace transport


#endif //CPPTRANSPORT_SQLITE3_CONTAINER_REPLACE_IMPL_H
