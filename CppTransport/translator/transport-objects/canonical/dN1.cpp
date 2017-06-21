//
// Created by David Seery on 20/12/2015.
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

#include "dN1.h"


namespace canonical
  {

    std::unique_ptr<flattened_tensor> canonical_dN1::compute(const index_literal_list& indices)
      {
        if(indices.size() != DN1_TENSOR_INDICES) throw tensor_exception("dN1 indices");

        auto result = std::make_unique<flattened_tensor>(this->fl.get_flattened_size<phase_index>(DN1_TENSOR_INDICES));

        const phase_index max_i = this->shared.get_max_phase_index(indices[0]->get_variance());

        // set up a TensorJanitor to manage use of cache
        TensorJanitor J(*this, indices);

        for(phase_index i = phase_index(0, indices[0]->get_variance()); i < max_i; ++i)
          {
            (*result)[this->fl.flatten(i)] = this->compute_component(i);
          }

        return(result);
      }


    GiNaC::ex canonical_dN1::compute_component(phase_index i)
      {
        if(!this->cached) throw tensor_exception("dN1 cache not ready");

        unsigned int index = this->fl.flatten(i);
        std::unique_ptr<cache_tags> args = this->res.generate_cache_arguments(0, this->printer);

        GiNaC::ex result;

        if(!this->cache.query(expression_item_types::dN1_item, index, *args, result))
          {
            timing_instrument timer(this->compute_timer);

            field_index i_species = this->traits.to_species(i);
            result = -(1/(2*dotH)) * diff(Hsq, (*fields)[this->fl.flatten(i_species)]);

            this->cache.store(expression_item_types::dN1_item, index, *args, result);
          }

        return(result);
      }


    unroll_behaviour canonical_dN1::get_unroll()
      {
        return unroll_behaviour::force;   // currently can't roll-up delta-N expressions
      }


    void canonical_dN1::pre_explicit(const index_literal_list& indices)
      {
        if(cached) throw tensor_exception("dN1 already cached");

        Hsq = this->res.raw_Hsq_resource(this->printer);
        eps = this->res.raw_eps_resource(this->printer);
        dotH = -eps*Hsq;
        fields = this->shared.generate_field_symbols(this->printer);

        this->cached = true;
      }


    void canonical_dN1::post()
      {
        if(!this->cached) throw tensor_exception("dN1 not cached");

        // invalidate cache
        this->cached = false;
      }


    canonical_dN1::canonical_dN1(language_printer& p, cse& cw, expression_cache& c, resources& r, shared_resources& s,
                                 boost::timer::cpu_timer& tm, index_flatten& f, index_traits& t)
      : ::dN1(),
        printer(p),
        cse_worker(cw),
        cache(c),
        res(r),
        shared(s),
        fl(f),
        traits(t),
        compute_timer(tm),
        cached(false)
      {
      }

  }   // namespace canonical
