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

#ifndef CPPTRANSPORT_DERIVED_PRODUCT_DECL_H
#define CPPTRANSPORT_DERIVED_PRODUCT_DECL_H


namespace transport
  {

    // DERIVED PRODUCT RECORD


    template <typename number>
    class derived_product_record: public repository_record
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! construct a derived product record
        derived_product_record(const derived_data::derived_product<number>& prod, repository_record::handler_package& pkg);

        //! Override copy constructor to perform a deep copy
        derived_product_record(const derived_product_record& obj);

        //! deserialization constructor
        derived_product_record(Json::Value& reader, task_finder<number>& f, repository_record::handler_package& pkg);

        //! destructor is default
        virtual ~derived_product_record() = default;


        // GET CONTENTS

      public:

        //! Get product
        derived_data::derived_product<number>* get_product() const { return(this->product.get()); }


        // SERIALIZATION -- implements a 'serializable' interface

      public:

        //! serialize this object
        virtual void serialize(Json::Value& writer) const override;


        // CLONE

      public:

        //! clone this object
        virtual derived_product_record<number>* clone() const override { return new derived_product_record<number>(static_cast<const derived_product_record<number>&>(*this)); };


        // INTERNAL DATA

      protected:

        //! Derived product associated with this record
        std::unique_ptr< derived_data::derived_product<number> > product;

      };

  }   // namespace transport


#endif //CPPTRANSPORT_DERIVED_PRODUCT_DECL_H
