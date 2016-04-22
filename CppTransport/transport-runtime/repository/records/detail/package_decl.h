//
// Created by David Seery on 25/01/2016.
// Copyright (c) 2016 University of Sussex. All rights reserved.
//

#ifndef CPPTRANSPORT_PACKAGE_DECL_H
#define CPPTRANSPORT_PACKAGE_DECL_H


namespace transport
  {

    // PACKAGE RECORD


    //! Encapsulates metadata for a package record stored in the repository
    template <typename number>
    class package_record: public repository_record
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! construct a package record
        package_record(const initial_conditions<number>& i, repository_record::handler_package& pkg);

        //! deserialization constructor
        package_record(Json::Value& reader, model_manager <number>& f, repository_record::handler_package& pkg);

        virtual ~package_record() = default;


        // GET CONTENTS

      public:

        //! Get initial conditionss
        const initial_conditions<number>& get_ics() const { return(this->ics); }


        // SERIALIZATION -- implements a 'serializable' interface

      public:

        //! serialize this object
        virtual void serialize(Json::Value& writer) const override;


        // CLONE

      public:

        //! clone this object
        virtual package_record<number>* clone() const override { return new package_record<number>(static_cast<const package_record<number>&>(*this)); };


        // INTERNAL DATA

      protected:

        //! Initial conditions data associated with this package
        initial_conditions<number> ics;

      };

  }   // namespace transport


#endif //CPPTRANSPORT_PACKAGE_DECL_H
