//
// Created by David Seery on 26/01/2016.
// --@@
// Copyright (c) 2016 University of Sussex. All rights reserved.
//
// This file is part of the CppTransport platform.
//
// CppTransport is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 2 of the License, or
// (at your option) any later version.
//
// CppTransport is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with CppTransport.  If not, see <http://www.gnu.org/licenses/>.
//
// @license: GPL-2
// @contributor: David Seery <D.Seery@sussex.ac.uk>
// --@@
//

#ifndef CPPTRANSPORT_SQLITE3_CONTAINER_REPLACE_IMPL_H
#define CPPTRANSPORT_SQLITE3_CONTAINER_REPLACE_IMPL_H


#include "transport-runtime/sqlite3/detail/container_replace_decl.h"


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
