//
// Created by David Seery on 19/12/2015.
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
            shared(p, mgr, c),
            res(p, mgr, c, shared, compute_timer)       // entitled to assume that mgr, shared, compute_timer have been constructed at this point
          {
          }

        //! destructor is default
        virtual ~tensor_factory() = default;


        // GENERIC TENSOR CLASSES

      public:

        // NOTE we cannot use contravariant return types with managed pointers
        // the u1, u2, u3, etc. classes visible within the 'canonical' namespace are
        // canonical::u1, canonical::u2 etc., whereas the virtual functions being overridden
        // expect ::u1, ::u2, etc.

        //! obtain a Hubble-object
        std::unique_ptr<Hubble> make_Hubble(language_printer& p, cse& cw) override;
    
        //! obtain a parameter object
        std::unique_ptr<parameters> make_parameters(language_printer& p, cse& cw) override;
    
        //! obtain a fields object
        std::unique_ptr<fields> make_fields(language_printer& p, cse& cw) override;
    
        //! obtain a coordinates object
        std::unique_ptr<coordinates> make_coordinates(language_printer& p, cse& cw) override;

        //! obtain a dV-tensor
        std::unique_ptr<dV> make_dV(language_printer& p, cse& cw) override;

        //! obtain a ddV-tensor
        std::unique_ptr<ddV> make_ddV(language_printer& p, cse& cw) override;

        //! obtain a dddV-tensor
        std::unique_ptr<dddV> make_dddV(language_printer& p, cse& cw) override;

        //! obtain an SR_velocity tensor
        std::unique_ptr<SR_velocity> make_SR_velocity(language_printer& p, cse& cw) override;

        //! obtain an A-tensor
        std::unique_ptr<A> make_A(language_printer& p, cse& cw) override;
    
        //! obtain an Atilde-tensor
        std::unique_ptr<Atilde> make_Atilde(language_printer& p, cse& cw) override;

        //! obtain a B-tensor
        std::unique_ptr<B> make_B(language_printer& p, cse& cw) override;

        //! obtain a C-tensor
        std::unique_ptr<C> make_C(language_printer& p, cse& cw) override;

        //! obtain an M-tensor
        std::unique_ptr<M> make_M(language_printer& p, cse& cw) override;

        //! obtain a u1-tensor object
        std::unique_ptr<u1> make_u1(language_printer& p, cse& cw) override;

        //! obtain a u2-tensor object
        std::unique_ptr<u2> make_u2(language_printer& p, cse& cw) override;

        //! obtain a u3-tensor object
        std::unique_ptr<u3> make_u3(language_printer& p, cse& cw) override;

        //! obtain a zeta1-tensor object
        std::unique_ptr<zeta1> make_zeta1(language_printer& p, cse& cw) override;

        //! obtain a zeta2-tensor object
        std::unique_ptr<zeta2> make_zeta2(language_printer& p, cse& cw) override;

        //! obtain a dN1-tensor object
        std::unique_ptr<dN1> make_dN1(language_printer& p, cse& cw) override;

        //! obtain a dN2-tensor object
        std::unique_ptr<dN2> make_dN2(language_printer& p, cse& cw) override;
    
    
        // PROVIDE ACCESS TO RESOURCE MANAGER
  
      public:
    
        //! link to resource manager object
        canonical_resource_manager& get_resource_manager() override { return this->mgr; }


        // INTERNAL DATA

      private:
        
        // RESOURCES
    
        //! canonical resource manager
        canonical_resource_manager mgr;
    
        //! shared resources
        shared_resources shared;

        //! canonical resources
        resources res;

      };

  }


#endif //CPPTRANSPORT_CANONICAL_FACTORY_H
