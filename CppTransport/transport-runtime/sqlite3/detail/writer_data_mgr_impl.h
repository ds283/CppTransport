//
// Created by David Seery on 28/01/2016.
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

#ifndef CPPTRANSPORT_SQLITE3_WRITER_DATA_MGR_IMPL_H
#define CPPTRANSPORT_SQLITE3_WRITER_DATA_MGR_IMPL_H


namespace transport
  {

    template <typename number>
    bool sqlite3_twopf_writer_aggregate<number>::operator()(integration_writer<number>& writer, const std::string& product)
      {
        return this->mgr.aggregate_twopf_batch(writer, product);
      }


    template <typename number>
    void sqlite3_twopf_writer_integrity<number>::operator()(integration_writer<number>& writer, integration_task<number>* task)
      {
        return this->mgr.check_twopf_integrity_handler(writer, task);
      }


    template <typename number>
    bool sqlite3_threepf_writer_aggregate<number>::operator()(integration_writer<number>& writer, const std::string& product)
      {
        return this->mgr.aggregate_threepf_batch(writer, product);
      }


    template <typename number>
    void sqlite3_threepf_writer_integrity<number>::operator()(integration_writer<number>& writer, integration_task<number>* task)
      {
        return this->mgr.check_threepf_integrity_handler(writer, task);
      }


    template <typename number>
    bool sqlite3_zeta_twopf_writer_aggregate<number>::operator()(postintegration_writer<number>& writer, const std::string& product)
      {
        return this->mgr.aggregate_zeta_twopf_batch(writer, product);
      }


    template <typename number>
    void sqlite3_zeta_twopf_writer_integrity<number>::operator()(postintegration_writer<number>& writer, postintegration_task<number>* task)
      {
        return this->mgr.check_zeta_twopf_integrity_handler(writer, task);
      }


    template <typename number>
    bool sqlite3_zeta_threepf_writer_aggregate<number>::operator()(postintegration_writer<number>& writer, const std::string& product)
      {
        return this->mgr.aggregate_zeta_threepf_batch(writer, product);
      }


    template <typename number>
    void sqlite3_zeta_threepf_writer_integrity<number>::operator()(postintegration_writer<number>& writer, postintegration_task<number>* task)
      {
        return this->mgr.check_zeta_threepf_integrity_handler(writer, task);
      }


    template <typename number>
    bool sqlite3_fNL_writer_aggregate<number>::operator()(postintegration_writer<number>& writer, const std::string& product)
      {
        return this->mgr.aggregate_fNL_batch(writer, product, this->type);
      }


    template <typename number>
    void sqlite3_fNL_writer_integrity<number>::operator()(postintegration_writer<number>& writer, postintegration_task<number>* task)
      {
        return this->mgr.check_fNL_integrity_handler(writer, task);
      }


    template <typename number>
    bool sqlite3_derived_content_writer_aggregate<number>::operator()(derived_content_writer<number>& writer, const std::string& product, const std::list<std::string>& used_groups)
      {
        return this->mgr.aggregate_derived_product(writer, product, used_groups);
      }

  }   // namespace transport


#endif //CPPTRANSPORT_SQLITE3_WRITER_DATA_MGR_IMPL_H
