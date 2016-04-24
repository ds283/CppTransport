//
// Created by David Seery on 25/01/2016.
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

#ifndef CPPTRANSPORT_DERIVED_PRODUCT_IMPL_H
#define CPPTRANSPORT_DERIVED_PRODUCT_IMPL_H


namespace transport
  {

    // DERIVED PRODUCT RECORD


    template <typename number>
    derived_product_record<number>::derived_product_record(const derived_data::derived_product<number>& prod, repository_record::handler_package& pkg)
      : repository_record(prod.get_name(), pkg),
        product(prod.clone())
      {
        assert(product);
      }


    template <typename number>
    derived_product_record<number>::derived_product_record(const derived_product_record<number>& obj)
      : repository_record(obj),
        product(obj.product->clone())
      {
        assert(product);
      }


    template <typename number>
    derived_product_record<number>::derived_product_record(Json::Value& reader, task_finder<number>& f, repository_record::handler_package& pkg)
      : repository_record(reader, pkg),
        product(derived_data::derived_product_helper::deserialize<number>(this->name, reader, f))
      {
        assert(product != nullptr);

        if(product == nullptr) throw runtime_exception(exception_type::SERIALIZATION_ERROR, CPPTRANSPORT_REPO_PRODUCT_DESERIALIZE_FAIL);
      }


    template <typename number>
    void derived_product_record<number>::serialize(Json::Value& writer) const
      {
        writer[CPPTRANSPORT_NODE_RECORD_TYPE] = std::string(CPPTRANSPORT_NODE_RECORD_DERIVED_PRODUCT);
        this->product->serialize(writer);
        this->repository_record::serialize(writer);
      }

  }   // namespace transport


#endif //CPPTRANSPORT_DERIVED_PRODUCT_IMPL_H
