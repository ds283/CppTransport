//
// Created by David Seery on 25/01/2016.
// Copyright (c) 2016 University of Sussex. All rights reserved.
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

        virtual ~derived_product_record();


        // GET CONTENTS

      public:

        //! Get product
        derived_data::derived_product<number>* get_product() const { return(this->product); }


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
        derived_data::derived_product<number>* product;

      };

  }   // namespace transport


#endif //CPPTRANSPORT_DERIVED_PRODUCT_DECL_H
