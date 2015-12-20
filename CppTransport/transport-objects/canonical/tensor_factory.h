//
// Created by David Seery on 19/12/2015.
// Copyright (c) 2015 University of Sussex. All rights reserved.
//

#ifndef CPPTRANSPORT_CANONICAL_FACTORY_H
#define CPPTRANSPORT_CANONICAL_FACTORY_H


#include "concepts/tensor_factory.h"
#include "resources.h"


namespace canonical
  {

    class tensor_factory: public ::tensor_factory
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! constructor
        tensor_factory(translator_data& p, expression_cache& c)
          : ::tensor_factory(p, c),
            cres(p, mgr, c, shared, compute_timer)       // entitled to assume that mgr, shared, compute_timer have been constructed at this point
          {
          }

        //! destructor is default
        virtual ~tensor_factory() = default;


        // INTERFACE

      public:

        // NOTE we cannot use contravariant return types with managed pointers
        // the u1, u2, u3, etc. classes visible within the 'canonical' namespace are
        // canonical::u1, canonical::u2 etc., whereas the virtual functions being overridden
        // expect ::u1, ::u2, etc.

        //! obtain a Hubble-object
        virtual std::unique_ptr<Hubble> make_Hubble(language_printer& p) override;

        //! obtain a dV-tensor
        virtual std::unique_ptr<dV> make_dV(language_printer& p) override;

        //! obtain a ddV-tensor
        virtual std::unique_ptr<ddV> make_ddV(language_printer& p) override;

        //! obtain a dddV-tensor
        virtual std::unique_ptr<dddV> make_dddV(language_printer& p) override;

        //! obtain an SR_velocity tensor
        virtual std::unique_ptr<SR_velocity> make_SR_velocity(language_printer& p) override;

        //! obtain an A-tensor
        virtual std::unique_ptr<A> make_A(language_printer& p) override;

        //! obtain a B-tensor
        virtual std::unique_ptr<B> make_B(language_printer& p) override;

        //! obtain a C-tensor
        virtual std::unique_ptr<C> make_C(language_printer& p) override;

        //! obtain an M-tensor
        virtual std::unique_ptr<M> make_M(language_printer& p) override;

        //! obtain a u1-tensor object
        virtual std::unique_ptr<u1> make_u1(language_printer& p) override;

        //! obtain a u2-tensor object
        virtual std::unique_ptr<u2> make_u2(language_printer& p) override;

        //! obtain a u3-tensor object
        virtual std::unique_ptr<u3> make_u3(language_printer& p) override;

        //! obtain a zeta1-tensor object
        virtual std::unique_ptr<zeta1> make_zeta1(language_printer& p) override;

        //! obtain a zeta2-tensor object
        virtual std::unique_ptr<zeta2> make_zeta2(language_printer& p) override;

        //! obtain a dN1-tensor object
        virtual std::unique_ptr<dN1> make_dN1(language_printer& p) override;

        //! obtain a dN2-tensor object
        virtual std::unique_ptr<dN2> make_dN2(language_printer& p) override;


        // INTERNAL DATA

      private:


        // RESOURCES

        //! canonical resources
        resources cres;     // must be constructed after compute_timer

      };

  }


#endif //CPPTRANSPORT_CANONICAL_FACTORY_H
