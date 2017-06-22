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


#include "resources.h"


namespace canonical
  {
    
    resources::resources(translator_data& p, resource_manager& m, expression_cache& c,
                         shared_resources& s, boost::timer::cpu_timer& t)
      : mgr(m),
        cache(c),
        payload(p),
        share(s),
        compute_timer(t),
        sym_factory(p.get_symbol_factory()),
        field_list(p.model.get_field_symbols()),
        deriv_list(p.model.get_deriv_symbols()),
        param_list(p.model.get_param_symbols()),
        fl(p.model.get_number_params(), p.model.get_number_fields())
      {
        boost::optional< contexted_value< std::shared_ptr<GiNaC::ex> > > pot = p.model.get_potential();
        if(pot) V = **(pot.get()); else V = GiNaC::ex(0);
        compute_timer.stop();
      }


    std::unique_ptr<cache_tags>
    resources::generate_cache_arguments(const language_printer& printer) const
      {
        std::unique_ptr<cache_tags> args = std::make_unique<cache_tags>();

        // query resource manager for parameter and coordinate labels
        const auto& param_resource = this->mgr.parameters();
        const auto coord_resource = this->mgr.coordinates();
        const auto& flatten = this->mgr.phase_flatten();

        if(param_resource)
          {
            GiNaC::symbol sym = this->sym_factory.get_symbol(*param_resource);
            args->push_back(sym);
          }

        if(coord_resource && flatten)
          {
            GiNaC::symbol sym = this->sym_factory.get_symbol(coord_resource.get().second);
            args->push_back(sym);
          }

        return(args);
      }


    std::unique_ptr<flattened_tensor>
    resources::generate_field_vector(const language_printer& printer) const
      {
        // no distinction between co- and contravariant indices in a flat model, so we can always return the
        // raw variable labels (which notionally are the contravariant components)

        auto X = std::make_unique<flattened_tensor>();

        const auto Y = this->share.generate_field_symbols(printer);
        for(const auto& label : *Y)
          {
            X->push_back(label);
          }

        return X;
      }


    std::unique_ptr<flattened_tensor> resources::generate_deriv_vector(const language_printer& printer) const
      {
        // no distinction between co- and contravariant indices in a flat model, so we can always return the
        // raw variable labels (which notionally are the contravariant components)

        auto X = std::make_unique<flattened_tensor>();

        const auto Y = this->share.generate_deriv_symbols(printer);
        for(const auto& label : *Y)
          {
            X->push_back(label);
          }

        return X;
      }


    GiNaC::ex resources::generate_field_vector(const abstract_index& idx, const language_printer& printer) const
      {
        // no distinction between co- and contravariant indices in a flat model, so we can always return the
        // raw variable labels (which notionally are the contravariant components)

        const auto resource = this->mgr.coordinates();
        const auto& flatten = this->mgr.phase_flatten();

        if(!resource || !flatten) throw resource_failure(idx.get_loop_variable());

        std::string variable = printer.array_subscript(resource.get().second, idx, *flatten);
        return this->sym_factory.get_symbol(variable);
      }


    GiNaC::ex resources::generate_deriv_vector(const abstract_index& idx, const language_printer& printer) const
      {
        // no distinction between co- and contravariant indices in a flat model, so we can always return the
        // raw variable labels (which notionally are the contravariant components)

        const auto resource = this->mgr.coordinates();
        const auto& flatten = this->mgr.phase_flatten();

        if(!resource || !flatten) throw resource_failure(idx.get_loop_variable());

        std::string variable = printer.array_subscript(resource.get().second, idx, *flatten, this->payload.model.get_number_fields());
        return this->sym_factory.get_symbol(variable);
      }


    GiNaC::ex resources::V_resource(cse& cse_worker, const language_printer& printer)
      {
        // behaviour differs depending whether CSE is in use or not
        // If not, we want to return the raw expression; if it is, we want to assign an internal name to a collection
        // of CSE temporaries, and then return a *different* symbolic expression representing this collection
        if(this->payload.do_cse())
          {
            GiNaC::symbol V = this->share.generate_V();
            GiNaC::ex raw_V = this->raw_V_resource(printer);

            // parse raw expression, assigning result to correct symbolic name
            cse_worker.parse(raw_V, V.get_name());

            // return symbol
            return V;
          }
    
        return this->raw_V_resource(printer);
      }


    GiNaC::ex resources::raw_V_resource(const language_printer& printer)
      {
        auto args = this->generate_cache_arguments(printer);

        // if no substitutions, then nothing to do, so exit immediately
        if(args->size() == 0) return(this->V);

        GiNaC::ex subs_V;

        // check whether this set of substitutions has already been done; if so, there is no need to do any work
        if(!this->cache.query(expression_item_types::V_item, 0, *args, subs_V))
          {
            timing_instrument timer(this->compute_timer);

            // we didn't find an expression for V with this set of substitutions in the cache, so
            // query resource manager for parameter and coordinate labels, then build a substitution map
            const auto& param_resource = this->mgr.parameters();
            const auto coord_resource = this->mgr.coordinates();
            const auto& flatten = this->mgr.phase_flatten();

            // build substitution map
            GiNaC::exmap subs_map;

            if(param_resource)
              {
                std::unique_ptr<symbol_list> p_list = this->share.generate_parameter_symbols(printer);

                // copy parameter symbols into substitution list
                const param_index max_i = this->share.get_max_param_index();
                for(param_index i = param_index(0); i < max_i; ++i)
                  {
                    subs_map[this->param_list[this->fl.flatten(i)]] = (*p_list)[this->fl.flatten(i)];
                  }
              }

            if(coord_resource && flatten)
              {
                std::unique_ptr<symbol_list> f_list = this->share.generate_field_symbols(printer);

                // copy field-label symbols into substitution list
                const field_index max_i = this->share.get_max_field_index(variance::none);
                for(field_index i = field_index(0); i < max_i; ++i)
                  {
                    subs_map[this->field_list[this->fl.flatten(i)]] = (*f_list)[this->fl.flatten(i)];
                  }
              }

            // apply substitution and cache the result
            subs_V = this->V.subs(subs_map, GiNaC::subs_options::no_pattern);
            this->cache.store(expression_item_types::V_item, 0, *args, subs_V);
          }

        return(subs_V);
      }


    GiNaC::ex resources::eps_resource(cse& cse_worker, const language_printer& printer)
      {
        if(this->payload.do_cse())
          {
            GiNaC::symbol eps = this->share.generate_eps();
            GiNaC::ex raw_eps = this->raw_eps_resource(printer);

            // parse raw expression, assigning result to the correct symbolic name
            cse_worker.parse(raw_eps, eps.get_name());

            // return symbol
            return eps;
          }
    
        return this->raw_eps_resource(printer);
      }


    GiNaC::ex resources::raw_eps_resource(const language_printer& printer)
      {
        auto args = this->generate_cache_arguments(printer);

        GiNaC::ex eps;

        if(!this->cache.query(expression_item_types::epsilon_item, 0, *args, eps))
          {
            timing_instrument timer(this->compute_timer);

            std::unique_ptr<symbol_list> derivs = this->share.generate_deriv_symbols(printer);
            GiNaC::symbol Mp = this->share.generate_Mp();

            eps = 0;
            for(GiNaC::symbol& dv: *derivs)
              {
                eps += dv*dv;
              }

            eps = eps/(2*Mp*Mp);
            this->cache.store(expression_item_types::epsilon_item, 0, *args, eps);
          }

        return(eps);
      }


    GiNaC::ex resources::Hsq_resource(cse& cse_worker, const language_printer& printer)
      {
        if(this->payload.do_cse())
          {
            GiNaC::symbol Hsq = this->share.generate_Hsq();
            GiNaC::ex raw_Hsq = this->raw_Hsq_resource(printer);

            // parse raw expression, assigning result to the correct symbolic name
            cse_worker.parse(raw_Hsq, Hsq.get_name());

            // return symbol (no deposition into temporary pool yet guaranteed -- needs to be explicitly printed)
            return Hsq;
          }
    
        return this->raw_Hsq_resource(printer);
      }


    GiNaC::ex resources::raw_Hsq_resource(const language_printer& printer)
      {
        auto args = this->generate_cache_arguments(printer);

        GiNaC::ex Hsq;

        if(!this->cache.query(expression_item_types::Hubble2_item, 0, *args, Hsq))
          {
            timing_instrument timer(this->compute_timer);

            GiNaC::ex V = this->raw_V_resource(printer);
            GiNaC::ex eps = this->raw_eps_resource(printer);
            GiNaC::symbol Mp = this->share.generate_Mp();

            Hsq = V / ((3-eps)*Mp*Mp);
            this->cache.store(expression_item_types::Hubble2_item, 0, *args, Hsq);
          }

        return(Hsq);
      }


    std::unique_ptr<flattened_tensor>
    resources::dV_resource(const language_printer& printer)
      {
        auto list = std::make_unique<flattened_tensor>(this->fl.get_flattened_size<field_index>(1));

        const auto resource = this->mgr.dV();
        const auto& flatten = this->mgr.field_flatten();
    
        const field_index max_i = this->share.get_max_field_index(variance::none);

        if(resource && flatten)     // dV is available
          {
            for(field_index i = field_index(0); i < max_i; ++i)
              {
                unsigned int index = this->fl.flatten(i);

                std::string variable = printer.array_subscript(resource.get().second, this->fl.flatten(i), *flatten);

                (*list)[index] = this->sym_factory.get_symbol(variable);
              }
          }
        else                        // have to construct dV ourselves
          {
            // build argument list
            std::unique_ptr<cache_tags> args = this->generate_cache_arguments(printer);

            // don't generate other objects unless we need them
            GiNaC::ex subs_V;
            std::unique_ptr<symbol_list> f_list;
            bool cached = false;

            for(field_index i = field_index(0); i < max_i; ++i)
              {
                GiNaC::ex dV;
                unsigned int index = this->fl.flatten(i);

                if(!this->cache.query(expression_item_types::dV_item, index, *args, dV))
                  {
                    timing_instrument timer(this->compute_timer);

                    // generate necessary items if they have not been done
                    if(!cached)
                      {
                        subs_V = this->raw_V_resource(printer);
                        f_list = this->share.generate_field_symbols(printer);
                        cached = true;
                      }

                    GiNaC::symbol x1 = (*f_list)[this->fl.flatten(i)];
                    dV = GiNaC::diff(subs_V, x1);

                    this->cache.store(expression_item_types::dV_item, index, *args, dV);
                  }

                (*list)[index] = dV;
              }
          }

        return(list);
      }


    std::unique_ptr<flattened_tensor>
    resources::ddV_resource(const language_printer& printer)
      {
        auto list = std::make_unique<flattened_tensor>(this->fl.get_flattened_size<field_index>(2));

        const auto resource = this->mgr.ddV();
        const auto& flatten = this->mgr.field_flatten();
    
        const field_index max_i = this->share.get_max_field_index(variance::none);
        const field_index max_j = this->share.get_max_field_index(variance::none);

        if(resource && flatten)     // ddV is available
          {
            for(field_index i = field_index(0); i < max_i; ++i)
              {
                for(field_index j = field_index(0); j < max_j; ++j)
                  {
                    unsigned int index = this->fl.flatten(i,j);

                    std::string variable = printer.array_subscript(resource.get().second, this->fl.flatten(i), this->fl.flatten(j), *flatten);

                    (*list)[index] = this->sym_factory.get_symbol(variable);
                  }
              }
          }
        else                        // have to construct ddV ourselves
          {
            // build argument list
            std::unique_ptr<cache_tags> args = this->generate_cache_arguments(printer);

            // don't generate other objects unless we need them
            GiNaC::ex subs_V;
            std::unique_ptr<symbol_list> f_list;
            bool cached = false;

            for(field_index i = field_index(0); i < max_i; ++i)
              {
                for(field_index j = field_index(0); j < max_j; ++j)
                  {
                    GiNaC::ex ddV;
                    unsigned int index = this->fl.flatten(i,j);

                    if(!this->cache.query(expression_item_types::ddV_item, index, *args, ddV))
                      {
                        timing_instrument timer(this->compute_timer);

                        // generate necessary items if they have not been done
                        if(!cached)
                          {
                            subs_V = this->raw_V_resource(printer);
                            f_list = this->share.generate_field_symbols(printer);
                            cached = true;
                          }

                        GiNaC::symbol x1 = (*f_list)[this->fl.flatten(i)];
                        GiNaC::symbol x2 = (*f_list)[this->fl.flatten(j)];
                        ddV = GiNaC::diff(GiNaC::diff(subs_V, x1), x2);

                        this->cache.store(expression_item_types::ddV_item, index, *args, ddV);
                      }

                    (*list)[index] = ddV;
                  }
              }
          }

        return(list);
      }


    std::unique_ptr<flattened_tensor>
    resources::dddV_resource(const language_printer& printer)
      {
        auto list = std::make_unique<flattened_tensor>(this->fl.get_flattened_size<field_index>(3));

        const auto resource = this->mgr.dddV();
        const auto& flatten = this->mgr.field_flatten();

        const field_index max_i = this->share.get_max_field_index(variance::none);
        const field_index max_j = this->share.get_max_field_index(variance::none);
        const field_index max_k = this->share.get_max_field_index(variance::none);
        
        if(resource && flatten)     // dddV is available
          {
            for(field_index i = field_index(0); i < max_i; ++i)
              {
                for(field_index j = field_index(0); j < max_j; ++j)
                  {
                    for(field_index k = field_index(0); k < max_k; ++k)
                      {
                        unsigned int index = this->fl.flatten(i,j,k);

                        std::string variable = printer.array_subscript(resource.get().second, this->fl.flatten(i), this->fl.flatten(j), this->fl.flatten(k), *flatten);

                        (*list)[index] = this->sym_factory.get_symbol(variable);
                      }
                  }
              }
          }
        else                        // have to construct dddV ourselves
          {
            // build argument list
            std::unique_ptr<cache_tags> args = this->generate_cache_arguments(printer);

            // don't generate other objects unless we need them
            GiNaC::ex subs_V;
            std::unique_ptr<symbol_list> f_list;
            bool cached = false;

            for(field_index i = field_index(0); i < max_i; ++i)
              {
                for(field_index j = field_index(0); j < max_j; ++j)
                  {
                    for(field_index k = field_index(0); k < max_k; ++k)
                      {
                        GiNaC::ex dddV;
                        unsigned int index = this->fl.flatten(i,j,k);

                        if(!this->cache.query(expression_item_types::dddV_item, index, *args, dddV))
                          {
                            timing_instrument timer(this->compute_timer);

                            // generate necessary items if they have not been done
                            if(!cached)
                              {
                                subs_V = this->raw_V_resource(printer);
                                f_list = this->share.generate_field_symbols(printer);
                                cached = true;
                              }

                            GiNaC::symbol x1 = (*f_list)[this->fl.flatten(i)];
                            GiNaC::symbol x2 = (*f_list)[this->fl.flatten(j)];
                            GiNaC::symbol x3 = (*f_list)[this->fl.flatten(k)];
                            dddV = GiNaC::diff(GiNaC::diff(GiNaC::diff(subs_V, x1), x2), x3);

                            this->cache.store(expression_item_types::dddV_item, index, *args, dddV);
                          }

                        (*list)[index] = dddV;
                      }
                  }
              }
          }

        return(list);
      }


    std::unique_ptr<cache_tags>
    resources::generate_cache_arguments(unsigned int flags, const language_printer& printer) const
      {
        // first, generate arguments from param/coordinates if they exist
        auto args = this->generate_cache_arguments(printer);
        const auto& flatten = this->mgr.field_flatten();

        if(flatten && (flags & use_dV_argument))
          {
            const auto dV_resource = this->mgr.dV();
            if(dV_resource)   // no need to push arguments if no resource available
              {
                GiNaC::symbol sym = this->sym_factory.get_symbol(dV_resource.get().second);
                args->push_back(sym);
              }
          }

        if(flatten && (flags & use_ddV_argument))
          {
            const auto ddV_resource = this->mgr.ddV();
            if(ddV_resource)
              {
                GiNaC::symbol sym = this->sym_factory.get_symbol(ddV_resource.get().second);
                args->push_back(sym);
              }
          }

        if(flatten && (flags & use_dddV_argument))
          {
            const auto dddV_resource = this->mgr.dddV();
            if(dddV_resource)
              {
                GiNaC::symbol sym = this->sym_factory.get_symbol(dddV_resource.get().second);
                args->push_back(sym);
              }
          }

        return(args);
      }


    bool resources::can_roll_dV() const
      {
        const auto resource = this->mgr.dV();
        const auto& flatten = this->mgr.field_flatten();
        const auto& working_type = this->mgr.working_type();

        return(resource && flatten && working_type);
      }


    bool resources::can_roll_ddV() const
      {
        const auto resource = this->mgr.ddV();
        const auto& flatten = this->mgr.field_flatten();
        const auto& working_type = this->mgr.working_type();

        return(resource && flatten && working_type);
      }


    bool resources::can_roll_dddV() const
      {
        const auto resource = this->mgr.dddV();
        const auto& flatten = this->mgr.field_flatten();
        const auto& working_type = this->mgr.working_type();

        return(resource && flatten && working_type);
      }


    GiNaC::ex resources::dV_resource(const index_literal& a, const language_printer& printer)
      {
        const auto resource = this->mgr.dV();
        const auto& flatten = this->mgr.field_flatten();
        
        const auto& a_idx = a.get();

        if(!resource || !flatten) throw resource_failure(a_idx.get_loop_variable());

        std::string variable = printer.array_subscript(resource.get().second, a_idx, *flatten);
        return this->sym_factory.get_symbol(variable);
      }


    GiNaC::ex resources::ddV_resource(const index_literal& a, const index_literal& b, const language_printer& printer)
      {
        const auto resource = this->mgr.ddV();
        const auto& flatten = this->mgr.field_flatten();
    
        const auto& a_idx = a.get();
        const auto& b_idx = b.get();
    
        if(!resource || !flatten) throw resource_failure(a_idx.get_loop_variable() + ", " + b_idx.get_loop_variable());

        std::string variable = printer.array_subscript(resource.get().second, a_idx, b_idx, *flatten);
        return this->sym_factory.get_symbol(variable);
      }


    GiNaC::ex resources::dddV_resource(const index_literal& a, const index_literal& b, const index_literal& c,
                                       const language_printer& printer)
      {
        const auto resource = this->mgr.dddV();
        const auto& flatten = this->mgr.field_flatten();
    
        const auto& a_idx = a.get();
        const auto& b_idx = b.get();
        const auto& c_idx = c.get();

        if(!resource || !flatten) throw resource_failure(a_idx.get_loop_variable() + ", " + b_idx.get_loop_variable() + ", " + c_idx.get_loop_variable());

        std::string variable = printer.array_subscript(resource.get().second, a_idx, b_idx, c_idx, *flatten);
        return this->sym_factory.get_symbol(variable);
      }


  }   // namespace canonical

