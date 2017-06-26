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
#include "curvature_classes.h"
#include "concepts/tensor_exception.h"

#include "ginac/ginac.h"


namespace nontrivial_metric
  {

    class ResourceCache
      {

        // TYPE

      public:

        using value_type = std::pair< std::reference_wrapper<const GiNaC::ex>, std::reference_wrapper<const symbol_list> >;

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! constructor captures resource manager and shared resource manager
        ResourceCache(resources& r, shared_resources& s, const language_printer& p)
          : res(r),
            share(s),
            printer(p)
          {
          }

        //! destructor is default
        ~ResourceCache() = default;


        // INTERFACE

      public:

        value_type get()
          {
            // acquire resources if we do not already have them;
            // these resources are provided with all necessary substitutions applied, so there is no need
            // to do further substitutions in the resource manager

            if(!this->subs_V) this->subs_V = this->res.raw_V_resource(printer);
            if(!this->f_list) this->f_list = this->share.generate_field_symbols(printer);

            // return references
            return std::make_pair(std::cref(*subs_V), std::cref(**f_list));
          }


        // INTERNAL DATA

      private:

        //! resource manager
        resources& res;

        //! shared resources
        shared_resources& share;

        //! language printer
        const language_printer& printer;


        // CACHE

        //! raw V expressions, after substitution with the current resource labels
        boost::optional< GiNaC::ex > subs_V;

        //! symbol list
        boost::optional< std::unique_ptr<symbol_list> > f_list;


      };


    class SubstitutionMapCache
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! constructor captures resource manager
        SubstitutionMapCache(resources& r, const language_printer& p)
          : res(r),
            printer(p)
          {
          }

        //! destructor is default
        ~SubstitutionMapCache() = default;


        // INTERFACE

      public:

        GiNaC::exmap& get()
          {
            // acquire substitution map if we don't already have it
            if(!this->map) this->map = this->res.make_substitution_map(printer);

            return *map;
          }


        // INTERNAL DATA

      private:

        //! resource manager
        resources& res;

        //! language printer
        const language_printer& printer;


        // CACHE

        //! substitition map
        boost::optional< GiNaC::exmap > map;

      };


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
        // get potential stored by the model descriptor, if one is available
        auto pot = p.model.get_potential();
        
        // if no potential was set, fail gracefully; errors should have been emitted before this point
        if(pot) V = **(pot.get()); else V = GiNaC::ex(0);

        // get number of fields in the current model
        auto N = payload.model.get_number_fields();

        // populate GiNaC matrices representing the field-space metric and its inverse
        this->G = std::make_unique<GiNaC::matrix>(N, N);
        this->Ginv = std::make_unique<GiNaC::matrix>(N, N);

        // get metric stored by model descriptor, if one exists
        auto metric = p.model.get_metric();

        if(metric)    // expected case that a metric has been provided
          {
            // we now need to copy the user-defined metric into a GiNaC::matrix that will
            // represent it during analytic calculations
            auto& G = **(metric.get());

            // GiNaC matrices are indexed by numbers; here, the row/column numbering order
            // matches the declaration order of the fields
            for(int i = 0; i < N; ++i)
              {
                for(int j = 0; j < N; ++j)
                  {
                    field_metric::index_type idx = std::make_pair(field_list[i].get_name(), field_list[j].get_name());
                    this->G->set(i, j, G(idx));
                  }
              }

            // construct inverse metric
            *this->Ginv = GiNaC::ex_to<GiNaC::matrix>(this->G->inverse());
          }
        else    // unexpected case that no metric has been provided; attempt to recover gracefully (errors should have been emitted before this stage)
          {
            *(this->G) = GiNaC::ex_to<GiNaC::matrix>(GiNaC::unit_matrix(N));
            *(this->Ginv) = GiNaC::ex_to<GiNaC::matrix>(GiNaC::unit_matrix(N));
          }

        // construct curvature tensors based on this metric
        this->Crstfl = std::make_unique<Christoffel>(*this->G, *this->Ginv, field_list);
        this->Rie_T = std::make_unique<Riemann_T>(*this->Crstfl);

        // switch off compute timer (it will be restarted if needed during subsequent computations)
        compute_timer.stop();
      }


    cache_tags
    resources::generate_cache_arguments(const language_printer& printer) const
      {
        cache_tags args;

        // query resource manager for parameter and coordinate labels
        const auto& param_resource = this->mgr.parameters();
        const auto coord_resource = this->mgr.coordinates();
        const auto& flatten = this->mgr.phase_flatten();

        // push all parameter labels onto argument list
        if(param_resource)
          {
            GiNaC::symbol sym = this->sym_factory.get_symbol(*param_resource);
            args += sym;
          }

        // if a coordinate resource is being used, push its label onto the argument list
        if(coord_resource && flatten)
          {
            GiNaC::symbol sym = this->sym_factory.get_symbol(*coord_resource);
            args += sym;
          }

        return args;
      }


    std::unique_ptr<flattened_tensor>
    resources::generate_field_vector(const language_printer& printer) const
      {
        // for the coordinates themselves, there is no notion of co- or contravariant components;
        // the coordinate labels are just scalar fields on the manifold.
        // share.generate_field_symbols() gives us these labels directly, so all we need do is return them.

        auto X = std::make_unique<flattened_tensor>();

        const auto Y = this->share.generate_field_symbols(printer);
        for(const auto& label : *Y)
          {
            X->push_back(label);
          }

        return X;
      }


    std::unique_ptr<flattened_tensor>
    resources::generate_deriv_vector(variance var, const language_printer& printer) const
      {
        // the derivatives are field-space *vectors* and therefore *do* have a notion of co- or contravariance.
        // what we get back from share.generate_deriv_symbols() are the contravariant components.

        auto X = std::make_unique<flattened_tensor>();

        const auto Y = this->share.generate_deriv_symbols(printer);
        for(const auto& label : *Y)
          {
            X->push_back(label);
          }
        
        // if the covariant form is required, pull down the index
        if(var == variance::covariant)
          {
            // need to substitute for any parameters or fields that appear in G
            GiNaC::exmap subs_map = this->make_substitution_map(printer);

            auto X_cov = std::make_unique<flattened_tensor>();
            unsigned int max = this->payload.model.get_number_fields();
            
            for(unsigned int i = 0; i < max; ++i)
              {
                GiNaC::ex cmp = 0;
                for(unsigned int j = 0; j < max; ++j)
                  {
                    cmp += (*this->G)(i,j) * (*X)[j];
                  }
                auto cmp_subs = cmp.subs(subs_map, GiNaC::subs_options::no_pattern);
                X_cov->push_back(cmp_subs);
              }
            
            // replace original contravariant vector by the covariant one
            X.swap(X_cov);
          }

        return X;
      }


    GiNaC::ex resources::generate_field_vector(const index_literal& idx, const language_printer& printer) const
      {
        // for the coordinates themselves, there is no notion of co- or contravariant components;
        // the coordinate labels are just scalar fields on the manifold.
        // share.generate_field_symbols() gives us these labels directly, so all we need do is return them.

        const auto resource = this->mgr.coordinates();
        const auto& flatten = this->mgr.phase_flatten();

        if(!resource || !flatten) throw resource_failure(idx.get().get_loop_variable());

        std::string variable = printer.array_subscript(*resource, idx, **flatten);
        return this->sym_factory.get_symbol(variable);
      }


    GiNaC::ex resources::generate_deriv_vector(const index_literal& idx, const language_printer& printer) const
      {
        // the field derivatives transform tensorially and so here there is a notion of co- and contravariant
        // components.
        // Depending what resources are available we may need to perform index raising or lowering, which is handled
        // by position_indices()

        const auto resource = this->mgr.coordinates();
        const auto& flatten = this->mgr.phase_flatten();

        if(!resource || !flatten) throw resource_failure(idx.get().get_loop_variable());
        
        abstract_index idx_abstract_offset = idx;
        std::ostringstream modifier;
        modifier << "-" << this->payload.model.get_number_fields();
        idx_abstract_offset.push_post_modifier(modifier.str());
        
        index_literal idx_offset = idx;
        idx_offset.reassign(idx_abstract_offset);

        return this->position_indices<1, index_literal>({ variance::contravariant }, { std::cref(idx_offset) }, *resource, **flatten, printer);
      }
    
    
    template <size_t Indices, typename IndexType>
    GiNaC::ex
    resources::position_indices(const std::array<variance, Indices> avail,
                                const std::array<std::reference_wrapper<const IndexType>, Indices> reqd,
                                const contexted_value<std::string>& label, const std::string& flatten,
                                const language_printer& printer) const
      {
        const unsigned int N = this->payload.model.get_number_fields();
        
        const auto G = this->mgr.metric();
        const auto Ginv = this->mgr.metric_inverse();
        
        // compute size of sum involving all factors of the metric or inverse metric
        unsigned int size = 1;
        unsigned int repositioned = 0;
        
        for(unsigned int i = 0; i < Indices; ++i)
          {
            variance a = avail[i];
            variance r = reqd[i].get().get_variance();
            
            // if required and available indices don't match, we need a sum over N fields
            if(r != a)
              {
                size *= N;
                ++repositioned;
              }
          }
        
        if(size > 1 && this->payload.get_argument_cache().report_developer_warnings())
          {
            const error_context& ctx = label.get_declaration_point();
            std::ostringstream msg;
            
            msg << NOTIFY_REPOSITIONING_INDICES_A << " " << repositioned << " "
                << (repositioned > 1 ? NOTIFY_REPOSITIONING_INDICES_B1 : NOTIFY_REPOSITIONING_INDICES_B2)
                << " '" << *label << "'";
            ctx.warn(msg.str());
          }
        
        GiNaC::ex res = 0;

        // loop through all terms in the sum, constructing them appropriately
        for(unsigned int i = 0; i < size; ++i)
          {
            std::array< std::string, Indices > index_values;
            unsigned int index_pos = i;
            GiNaC::ex term = 1;
            
            // construct the metric/inverse metric combinations needed for this term
            for(unsigned int j = 0; j < Indices; ++j)
              {
                variance a = avail[i];
                variance r = reqd[j].get().get_variance();
                
                if(r != a)
                  {
                    unsigned int v = index_pos % N;
                    index_pos = index_pos / N;
                    
                    if(r == variance::contravariant)
                      {
                        if(!Ginv) throw resource_failure(to_index_string(reqd[j].get()));
                        auto factor = printer.array_subscript(*Ginv, reqd[j].get(), v, flatten);
                        auto sym = this->sym_factory.get_symbol(factor);
                        index_values[j] = to_index_string(v);
                        term *= sym;
                      }
                    else if(r == variance::covariant)
                      {
                        if(!G) throw resource_failure(to_index_string(reqd[j].get()));
                        auto factor = printer.array_subscript(*G, reqd[j].get(), v, flatten);
                        index_values[j] = to_index_string(v);
                        auto sym = this->sym_factory.get_symbol(factor);
                        term *= sym;
                      }
                    else
                      {
                        throw tensor_exception(ERROR_INDEX_MISSING_VARIANCE_DATA);
                      }
                  }
                else
                  {
                    index_values[j] = to_index_string(reqd[j].get());
                  }
    
                auto factor = this->position_indices_label(label, index_values, std::make_index_sequence<Indices>{}, flatten, printer);
                auto sym = this->sym_factory.get_symbol(factor);
                term *= sym;
              }
            
            res += term;
          }
        
        return res;
      }
    
    
    template <size_t Indices, size_t... Is>
    std::string
    resources::position_indices_label(const std::string& kernel, const std::array<std::string, Indices>& indices,
                                      std::index_sequence<Is...>, const std::string& flatten,
                                      const language_printer& printer) const
      {
        return printer.array_subscript(kernel, indices[Is]..., flatten);
      };


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
        if(args.empty()) return(this->V);

        GiNaC::ex subs_V;

        // check whether this set of substitutions has already been done; if so, there is no need to do any work
        if(!this->cache.query(expression_item_types::V_item, 0, args, subs_V))
          {
            timing_instrument timer(this->compute_timer);

            // we didn't find an expression for V with this set of substitutions in the cache, so
            // we need to construct one by hand. First, generate a substitution map given the
            // currently available set of resources
            GiNaC::exmap subs_map = this->make_substitution_map(printer);

            // apply substitution and cache the result
            subs_V = this->V.subs(subs_map, GiNaC::subs_options::no_pattern);
            this->cache.store(expression_item_types::V_item, 0, args, subs_V);
          }

        return(subs_V);
      }


    GiNaC::exmap resources::make_substitution_map(const language_printer& printer) const
      {
        const auto& param_resource = mgr.parameters();
        const auto coord_resource = mgr.coordinates();
        const auto& flatten = mgr.phase_flatten();

        // build substitution map
        GiNaC::exmap subs_map;

        if(param_resource)
          {
            auto p_list = share.generate_parameter_symbols(printer);

            // copy parameter symbols into substitution list
            const param_index max_i = share.get_max_param_index();
            for(param_index i = param_index(0); i < max_i; ++i)
              {
                subs_map[param_list[fl.flatten(i)]] = (*p_list)[fl.flatten(i)];
              }
          }

        if(coord_resource && flatten)
          {
            auto f_list = share.generate_field_symbols(printer);

            // copy field-label symbols into substitution list
            const field_index max_i = share.get_max_field_index(variance::none);
            for(field_index i = field_index(0); i < max_i; ++i)
              {
                subs_map[field_list[fl.flatten(i)]] = (*f_list)[fl.flatten(i)];
              }
          }

        return subs_map;
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

        if(!this->cache.query(expression_item_types::epsilon_item, 0, args, eps))
          {
            timing_instrument timer(this->compute_timer);

            std::unique_ptr<symbol_list> derivs = this->share.generate_deriv_symbols(printer);
            GiNaC::symbol Mp = this->share.generate_Mp();

            eps = 0;
            
            if(derivs->size() != this->G->rows()) throw std::runtime_error(ERROR_METRIC_DIMENSION);
            if(derivs->size() != this->G->cols()) throw std::runtime_error(ERROR_METRIC_DIMENSION);
            const unsigned int max = derivs->size();
            
            for(unsigned int i = 0; i < max; ++i)
              {
                for(unsigned int j = 0; j < max; ++j)
                  {
                    eps += (*this->G)(i,j) * (*derivs)[i] * (*derivs)[j];
                  }
              }
            
            eps = eps/(2*Mp*Mp);
            
            // potentially need to replace raw parameters in the metric components
            GiNaC::exmap subs_map = this->make_substitution_map(printer);
    
            // apply substitution and cache the result
            auto subs_eps = eps.subs(subs_map, GiNaC::subs_options::no_pattern);
            eps.swap(subs_eps);
            
            this->cache.store(expression_item_types::epsilon_item, 0, args, eps);
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

        if(!this->cache.query(expression_item_types::Hubble2_item, 0, args, Hsq))
          {
            timing_instrument timer(this->compute_timer);

            GiNaC::ex V = this->raw_V_resource(printer);
            GiNaC::ex eps = this->raw_eps_resource(printer);
            GiNaC::symbol Mp = this->share.generate_Mp();

            // no need to perform parameter/coordinate substitution here, since V and eps will already have been
            // substituted individually
            Hsq = V / ((3-eps)*Mp*Mp);
            
            this->cache.store(expression_item_types::Hubble2_item, 0, args, Hsq);
          }

        return(Hsq);
      }


    std::unique_ptr<flattened_tensor> resources::raw_G_resource(const language_printer& printer)
      {
        auto args = this->generate_cache_arguments(printer);

        auto G = std::make_unique<flattened_tensor>(this->fl.get_flattened_size<field_index>(2));

        const auto max = this->share.get_max_field_index(variance::covariant);

        SubstitutionMapCache subs_cache(*this, printer);

        for(field_index i = field_index(0, variance::covariant); i < max; ++i)
          {
            for(field_index j = field_index(0, variance::covariant); j <= i; ++j)
              {
                unsigned int index_ij = this->fl.flatten(i,j);

                // if no substitutions to be done, avoid checking cache
                if(args.empty())
                  {
                    (*G)[index_ij] =  (*this->G)(static_cast<unsigned int>(i), static_cast<unsigned int>(j));
                  }
                else
                  {
                    GiNaC::ex subs_G;

                    if(!this->cache.query(expression_item_types::metric_item, index_ij, args, subs_G))
                      {
                        timing_instrument timer(this->compute_timer);

                        // get substitution map
                        GiNaC::exmap& subs_map = subs_cache.get();

                        //! apply substitution to G component and cache the result
                        subs_G = (*this->G)(static_cast<unsigned int>(i), static_cast<unsigned int>(j)).subs(subs_map, GiNaC::subs_options::no_pattern);
                        this->cache.store(expression_item_types::metric_item, index_ij, args, subs_G);
                      }

                    (*G)[index_ij] = subs_G;
                  }

                if(i != j)
                  {
                    unsigned int index_ji = this->fl.flatten(j,i);
                    (*G)[index_ji] = (*G)[index_ij];
                  }
              }
          }

        return G;
      }


    std::unique_ptr<flattened_tensor> resources::raw_Ginv_resource(const language_printer& printer)
      {
        auto args = this->generate_cache_arguments(printer);

        auto Ginv = std::make_unique<flattened_tensor>(this->fl.get_flattened_size<field_index>(2));

        const auto max = this->share.get_max_field_index(variance::contravariant);

        SubstitutionMapCache subs_cache(*this, printer);

        for(field_index i = field_index(0, variance::contravariant); i < max; ++i)
          {
            for(field_index j = field_index(0, variance::contravariant); j <= i; ++j)
              {
                unsigned int index_ij = this->fl.flatten(i,j);

                // if no substitutions to be done, avoid checking cache
                if(args.empty())
                  {
                    (*Ginv)[index_ij] =  (*this->Ginv)(static_cast<unsigned int>(i), static_cast<unsigned int>(j));
                  }
                else
                  {
                    GiNaC::ex subs_Ginv;

                    if(!this->cache.query(expression_item_types::metric_inverse_item, index_ij, args, subs_Ginv))
                      {
                        timing_instrument timer(this->compute_timer);

                        // get substitution map
                        GiNaC::exmap& subs_map = subs_cache.get();

                        //! apply substitution to G-inverse component and cache the result
                        subs_Ginv = (*this->Ginv)(static_cast<unsigned int>(i), static_cast<unsigned int>(j)).subs(subs_map, GiNaC::subs_options::no_pattern);
                        this->cache.store(expression_item_types::metric_inverse_item, index_ij, args, subs_Ginv);
                      }

                    (*Ginv)[index_ij] = subs_Ginv;
                  }

                if(i != j)
                  {
                    unsigned int index_ji = this->fl.flatten(j,i);
                    (*Ginv)[index_ji] = (*Ginv)[index_ij];
                  }
              }
          }

        return Ginv;
      }


    std::unique_ptr<flattened_tensor> resources::raw_connexion_resource(const language_printer& printer)
      {
        auto args = this->generate_cache_arguments(printer);

        auto Gamma = std::make_unique<flattened_tensor>(this->fl.get_flattened_size<field_index>(3));

        const auto max_i = this->share.get_max_field_index(variance::contravariant);
        const auto max_jk = this->share.get_max_field_index(variance::covariant);

        SubstitutionMapCache subs_cache(*this, printer);

        for(field_index i = field_index(0, variance::contravariant); i < max_i; ++i)
          {
            for(field_index j = field_index(0, variance::covariant); j < max_jk; ++j)
              {
                for(field_index k = field_index(0, variance::covariant); k <= j; ++k)
                  {
                    unsigned int index_ijk = this->fl.flatten(i,j,k);

                    // if no substitutions to be done, avoid checking cache
                    if(args.empty())
                      {
                        (*Gamma)[index_ijk] =  (*this->Crstfl)(static_cast<unsigned int>(i), static_cast<unsigned int>(j), static_cast<unsigned int>(k));
                      }
                    else
                      {
                        GiNaC::ex subs_Gamma;

                        if(!this->cache.query(expression_item_types::connexion_item, index_ijk, args, subs_Gamma))
                          {
                            timing_instrument timer(this->compute_timer);

                            // get substitution map
                            GiNaC::exmap& subs_map = subs_cache.get();

                            //! apply substitution to connexion component and cache the result
                            subs_Gamma = (*this->Crstfl)(static_cast<unsigned int>(i), static_cast<unsigned int>(j), static_cast<unsigned int>(k)).subs(subs_map, GiNaC::subs_options::no_pattern);
                            this->cache.store(expression_item_types::connexion_item, index_ijk, args, subs_Gamma);
                          }

                        (*Gamma)[index_ijk] = subs_Gamma;
                      }

                    if(j != k)
                      {
                        unsigned int index_ikj = this->fl.flatten(i,k,j);
                        (*Gamma)[index_ikj] = (*Gamma)[index_ijk];
                      }
                  }
              }
          }

        return Gamma;
      }


    std::unique_ptr<flattened_tensor>
    resources::dV_resource(variance v, const language_printer& printer)
      {
        auto list = std::make_unique<flattened_tensor>(this->fl.get_flattened_size<field_index>(1));

        const auto resource = this->mgr.dV({ v }, false);
        const auto& flatten = this->mgr.field_flatten();

        if(resource && flatten)     // resources appear to be available
          {
            try
              {
                this->dV_resource_label(v, *list, resource.get().first, resource.get().second, *flatten, printer);
                return list;
              }
            catch(resource_failure& xe)
              {
                const error_context& ctx = resource.get().second.get_declaration_point();
                ctx.warn(WARNING_INDEX_POSITIONING_FAILURE);
              }
          }
        
        // either resources were not available, or a resource failure occurred when raising and lowering indices
        this->dV_resource_expr(v, *list, printer);

        return(list);
      }
    
    
    void resources::dV_resource_label(variance v, flattened_tensor& list,
                                      const std::array<variance, RESOURCE_INDICES::DV_INDICES>& avail,
                                      const contexted_value<std::string>& resource,
                                      const contexted_value<std::string>& flatten,
                                      const language_printer& printer)
      {
        const field_index max_i = this->share.get_max_field_index(v);

        for(field_index i = field_index(0, v); i < max_i; ++i)
          {
            unsigned int index = this->fl.flatten(i);

            list[index] = this->position_indices<1, field_index>(avail, { i }, resource, *flatten, printer);
          }
      }


    GiNaC::ex resources::dV_resource(const index_literal& a, const language_printer& printer)
      {
        const auto resource = this->mgr.dV();
        const auto& flatten = this->mgr.field_flatten();

        if(!resource || !flatten) throw resource_failure(a.get().get_loop_variable());

        return this->position_indices<1, index_literal>(resource.get().first, { a }, resource.get().second, *flatten, printer);
      }


    void resources::dV_resource_expr(variance v, flattened_tensor& list, const language_printer& printer)
      {
        const field_index max = this->share.get_max_field_index(v);

        // build argument list
        auto args = this->generate_cache_arguments(printer);

        // obtain a resource cache
        ResourceCache cache(*this, this->share, printer);

        for(field_index i = field_index(0, v); i < max; ++i)
          {
            GiNaC::ex dV;
            unsigned int index = this->fl.flatten(i);

            if(!this->cache.query(expression_item_types::dV_item, index, args, dV))
              {
                timing_instrument timer(this->compute_timer);

                auto data = cache.get();
                const GiNaC::ex& V = data.first.get();
                const symbol_list& f_list = data.second.get();

                GiNaC::symbol x1 = f_list[this->fl.flatten(i)];
                dV = GiNaC::diff(V, x1);

                this->cache.store(expression_item_types::dV_item, index, args, dV);
              }

            list[index] = dV;
          }

        // TODO: reposition indices if contravariant form is required
      }


    std::unique_ptr<flattened_tensor>
    resources::ddV_resource(const language_printer& printer)
      {
        auto list = std::make_unique<flattened_tensor>(this->fl.get_flattened_size<field_index>(2));

        const auto resource = this->mgr.ddV();
        const auto& flatten = this->mgr.field_flatten();

        if(resource && flatten)     // ddV is available
          {
            this->ddV_resource_label(printer, *list, resource.get().second, *flatten);
          }
        else                        // have to construct ddV ourselves
          {
            this->ddV_resource_expr(printer, *list);
          }

        return(list);
      }


    GiNaC::ex resources::ddV_resource(const index_literal& a, const index_literal& b, const language_printer& printer)
      {
        const auto resource = this->mgr.ddV();
        const auto& flatten = this->mgr.field_flatten();

        const auto& a_idx = a.get();
        const auto& b_idx = b.get();

        if(!resource || !flatten) throw resource_failure(a_idx.get_loop_variable() + ", " + b_idx.get_loop_variable());

        std::string variable = printer.array_subscript(resource.get().second, a_idx, b_idx, **flatten);
        return this->sym_factory.get_symbol(variable);
      }


    void resources::ddV_resource_label(const language_printer& printer, flattened_tensor& list,
                                       const contexted_value<std::string>& resource,
                                       const contexted_value<std::string>& flatten)
      {
        const field_index max_i = this->share.get_max_field_index(variance::none);
        const field_index max_j = this->share.get_max_field_index(variance::none);

        for(field_index i = field_index(0); i < max_i; ++i)
          {
            for(field_index j = field_index(0); j < max_j; ++j)
              {
                unsigned int index = this->fl.flatten(i,j);

                std::string variable = printer.array_subscript(resource, this->fl.flatten(i), this->fl.flatten(j), *flatten);

                list[index] = this->sym_factory.get_symbol(variable);
              }
          }
    }


    void resources::ddV_resource_expr(const language_printer& printer, flattened_tensor& list)
      {
        const field_index max_i = this->share.get_max_field_index(variance::none);
        const field_index max_j = this->share.get_max_field_index(variance::none);

        // build argument list
        auto args = this->generate_cache_arguments(printer);

        // obtain a resource cache
        ResourceCache cache(*this, this->share, printer);

        for(field_index i = field_index(0); i < max_i; ++i)
          {
            for(field_index j = field_index(0); j < max_j; ++j)
              {
                GiNaC::ex ddV;
                unsigned int index = this->fl.flatten(i,j);

                if(!this->cache.query(expression_item_types::ddV_item, index, args, ddV))
                  {
                    timing_instrument timer(this->compute_timer);

                    auto data = cache.get();
                    const GiNaC::ex& V = data.first.get();
                    const symbol_list& f_list = data.second.get();

                    GiNaC::symbol x1 = f_list[this->fl.flatten(i)];
                    GiNaC::symbol x2 = f_list[this->fl.flatten(j)];
                    ddV = GiNaC::diff(GiNaC::diff(V, x1), x2);

                    this->cache.store(expression_item_types::ddV_item, index, args, ddV);
                  }

                list[index] = ddV;
              }
          }
      }


    std::unique_ptr<flattened_tensor>
    resources::dddV_resource(const language_printer& printer)
      {
        auto list = std::make_unique<flattened_tensor>(this->fl.get_flattened_size<field_index>(3));

        const auto resource = this->mgr.dddV();
        const auto& flatten = this->mgr.field_flatten();

        if(resource && flatten)     // dddV is available
          {
            this->dddV_resource_label(printer, *list, resource.get().second, *flatten);
          }
        else                        // have to construct dddV ourselves
          {
            this->dddV_resource_expr(printer, *list);
          }

        return(list);
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

        std::string variable = printer.array_subscript(resource.get().second, a_idx, b_idx, c_idx, **flatten);
        return this->sym_factory.get_symbol(variable);
      }


    void resources::dddV_resource_label(const language_printer& printer, flattened_tensor& list,
                                        const contexted_value<std::string>& resource,
                                        const contexted_value<std::string>& flatten)
      {
        const field_index max_i = this->share.get_max_field_index(variance::none);
        const field_index max_j = this->share.get_max_field_index(variance::none);
        const field_index max_k = this->share.get_max_field_index(variance::none);

        for(field_index i = field_index(0); i < max_i; ++i)
          {
            for(field_index j = field_index(0); j < max_j; ++j)
              {
                for(field_index k = field_index(0); k < max_k; ++k)
                  {
                    unsigned int index = this->fl.flatten(i,j,k);

                    std::string variable = printer.array_subscript(resource, this->fl.flatten(i), this->fl.flatten(j), this->fl.flatten(k), *flatten);

                    list[index] = this->sym_factory.get_symbol(variable);
                  }
              }
          }
      }


    void resources::dddV_resource_expr(const language_printer& printer, flattened_tensor& list)
      {
        const field_index max_i = this->share.get_max_field_index(variance::none);
        const field_index max_j = this->share.get_max_field_index(variance::none);
        const field_index max_k = this->share.get_max_field_index(variance::none);

        // build argument list
        auto args = this->generate_cache_arguments(printer);

        // obtain a resource cache
        ResourceCache cache(*this, this->share, printer);

        for(field_index i = field_index(0); i < max_i; ++i)
          {
            for(field_index j = field_index(0); j < max_j; ++j)
              {
                for(field_index k = field_index(0); k < max_k; ++k)
                  {
                    GiNaC::ex dddV;
                    unsigned int index = this->fl.flatten(i,j,k);

                    if(!this->cache.query(expression_item_types::dddV_item, index, args, dddV))
                      {
                        timing_instrument timer(this->compute_timer);

                        auto data = cache.get();
                        const GiNaC::ex& V = data.first.get();
                        const symbol_list& f_list = data.second.get();

                        GiNaC::symbol x1 = f_list[this->fl.flatten(i)];
                        GiNaC::symbol x2 = f_list[this->fl.flatten(j)];
                        GiNaC::symbol x3 = f_list[this->fl.flatten(k)];
                        dddV = GiNaC::diff(GiNaC::diff(GiNaC::diff(V, x1), x2), x3);

                        this->cache.store(expression_item_types::dddV_item, index, args, dddV);
                      }

                    list[index] = dddV;
                  }
              }
          }
      }


    std::unique_ptr<flattened_tensor> resources::metric_resource(const language_printer& printer)
      {
        std::unique_ptr<flattened_tensor> list;

        const auto resource = this->mgr.metric();
        const auto& flatten = this->mgr.field_flatten();

        if(resource && flatten)
          {
            list = std::make_unique<flattened_tensor>(this->fl.get_flattened_size<field_index>(2));
            this->metric_resource_label(printer, *list, *resource, *flatten);
          }
        else
          {
            list = this->raw_G_resource(printer);
          }

        return list;
      }


    GiNaC::ex
    resources::metric_resource(const index_literal& a, const index_literal& b, const language_printer& printer)
      {
        const auto resource = this->mgr.metric();
        const auto& flatten = this->mgr.field_flatten();

        if(!resource || !flatten)
          throw resource_failure(a.get().get_loop_variable() + ", " + b.get().get_loop_variable());

        std::string variable = printer.array_subscript(*resource, a, b, **flatten);
        return this->sym_factory.get_symbol(variable);
      }


    void resources::metric_resource_label(const language_printer& printer, flattened_tensor& list,
                                          const contexted_value<std::string>& resource,
                                          const contexted_value<std::string>& flatten)
      {
        const field_index max = this->share.get_max_field_index(variance::covariant);

        for(field_index i = field_index(0, variance::covariant); i < max; ++i)
          {
            for(field_index j = field_index(0, variance::covariant); j < max; ++j)
              {
                unsigned int index = this->fl.flatten(i,j);

                std::string variable = printer.array_subscript(resource, this->fl.flatten(i), this->fl.flatten(j), *flatten);

                list[index] = this->sym_factory.get_symbol(variable);
              }
          }
      }


    std::unique_ptr<flattened_tensor> resources::metric_inverse_resource(const language_printer& printer)
      {
        std::unique_ptr<flattened_tensor> list;

        const auto resource = this->mgr.metric_inverse();
        const auto& flatten = this->mgr.field_flatten();

        if(resource && flatten)
          {
            list = std::make_unique<flattened_tensor>(this->fl.get_flattened_size<field_index>(2));
            this->metric_inverse_resource_label(printer, *list, *resource, *flatten);
          }
        else
          {
            list = this->raw_Ginv_resource(printer);
          }

        return list;
      }


    GiNaC::ex
    resources::metric_inverse_resource(const index_literal& a, const index_literal& b, const language_printer& printer)
      {
        const auto resource = this->mgr.metric_inverse();
        const auto& flatten = this->mgr.field_flatten();

        if(!resource || !flatten)
          throw resource_failure(a.get().get_loop_variable() + ", " + b.get().get_loop_variable());

        std::string variable = printer.array_subscript(*resource, a, b, **flatten);
        return this->sym_factory.get_symbol(variable);
      }


    void resources::metric_inverse_resource_label(const language_printer& printer, flattened_tensor& list,
                                                  const contexted_value<std::string>& resource,
                                                  const contexted_value<std::string>& flatten)
      {
        const field_index max = this->share.get_max_field_index(variance::contravariant);

        for(field_index i = field_index(0, variance::contravariant); i < max; ++i)
          {
            for(field_index j = field_index(0, variance::contravariant); j < max; ++j)
              {
                unsigned int index = this->fl.flatten(i,j);

                std::string variable = printer.array_subscript(resource, this->fl.flatten(i), this->fl.flatten(j), *flatten);

                list[index] = this->sym_factory.get_symbol(variable);
              }
          }
      }


    std::unique_ptr<flattened_tensor> resources::connexion_resource(const language_printer& printer)
      {
        std::unique_ptr<flattened_tensor> list;

        const auto resource = this->mgr.connexion();
        const auto& flatten = this->mgr.field_flatten();

        if(resource && flatten)
          {
            list = std::make_unique<flattened_tensor>(this->fl.get_flattened_size<field_index>(3));
            this->connexion_resource_label(printer, *list, *resource, *flatten);
          }
        else
          {
            list = this->raw_connexion_resource(printer);
          }

        return list;
      }


    GiNaC::ex resources::connexion_resource(const index_literal& a, const index_literal& b, const index_literal& c,
                                            const language_printer& printer)
      {
        const auto resource = this->mgr.connexion();
        const auto& flatten = this->mgr.field_flatten();

        if(!resource || !flatten)
          throw resource_failure(
            a.get().get_loop_variable() + ", " + b.get().get_loop_variable() + ", " + c.get().get_loop_variable());

        std::string variable = printer.array_subscript(*resource, a, b, c, **flatten);
        return this->sym_factory.get_symbol(variable);
      }


    void resources::connexion_resource_label(const language_printer& printer, flattened_tensor& list,
                                             const contexted_value<std::string>& resource,
                                             const contexted_value<std::string>& flatten)
      {
        const field_index max_i = this->share.get_max_field_index(variance::contravariant);
        const field_index max_jk  = this->share.get_max_field_index(variance::covariant);

        for(field_index i = field_index(0, variance::contravariant); i < max_i; ++i)
          {
            for(field_index j = field_index(0, variance::covariant); j < max_jk; ++j)
              {
                for(field_index k = field_index(0, variance::covariant); k < max_jk; ++k)
                  {
                    unsigned int index = this->fl.flatten(i,j,k);

                    std::string variable = printer.array_subscript(resource, this->fl.flatten(i), this->fl.flatten(j),
                                                                   this->fl.flatten(k), *flatten);

                    list[index] = this->sym_factory.get_symbol(variable);
                  }
              }
          }
      }


    cache_tags
    resources::generate_cache_arguments(unsigned int flags, const language_printer& printer) const
      {
        // first, generate arguments from param/coordinates if they exist
        auto args = this->generate_cache_arguments(printer);
        const auto& flatten = this->mgr.field_flatten();

        if(flatten && ((flags & use_dV) != 0))
          {
            const auto dV_resource = this->mgr.dV();
            if(dV_resource)   // no need to push arguments if no resource available
              {
                GiNaC::symbol sym = this->sym_factory.get_symbol(dV_resource.get().second);
                args += sym;
              }
          }

        if(flatten && ((flags &  use_ddV) != 0))
          {
            const auto ddV_resource = this->mgr.ddV();
            if(ddV_resource)
              {
                GiNaC::symbol sym = this->sym_factory.get_symbol(ddV_resource.get().second);
                args += sym;
              }
          }

        if(flatten && ((flags & use_dddV) != 0))
          {
            const auto dddV_resource = this->mgr.dddV();
            if(dddV_resource)
              {
                GiNaC::symbol sym = this->sym_factory.get_symbol(dddV_resource.get().second);
                args += sym;
              }
          }

        return args;
      }


    bool resources::can_roll_dV(const std::array< variance, RESOURCE_INDICES::DV_INDICES >& vars) const
      {
        const auto resource = this->mgr.dV(vars, false);
        const auto& flatten = this->mgr.field_flatten();
        const auto& working_type = this->mgr.working_type();
        
        bool G = resource ? this->get_roll_metric_requirements(resource.get().first, vars) : false;

        return(resource && G && flatten && working_type);
      }


    bool resources::can_roll_ddV(const std::array< variance, RESOURCE_INDICES::DDV_INDICES >& vars) const
      {
        const auto resource = this->mgr.ddV(vars, false);
        const auto& flatten = this->mgr.field_flatten();
        const auto& working_type = this->mgr.working_type();
    
        bool G = resource ? this->get_roll_metric_requirements(resource.get().first, vars) : false;

        return(resource && G && flatten && working_type);
      }


    bool resources::can_roll_dddV(const std::array< variance, RESOURCE_INDICES::DDDV_INDICES >& vars) const
      {
        const auto resource = this->mgr.dddV(vars, false);
        const auto& flatten = this->mgr.field_flatten();
        const auto& working_type = this->mgr.working_type();
    
        bool G = resource ? this->get_roll_metric_requirements(resource.get().first, vars) : false;

        return(resource && G && flatten && working_type);
      }
    
    
    bool resources::can_roll_connexion() const
      {
        const auto resource = this->mgr.connexion();
        const auto& flatten = this->mgr.field_flatten();
        const auto& working_type = this->mgr.working_type();
    
        return(resource && G && flatten && working_type);
      }
    
    
    bool resources::can_roll_metric() const
      {
        const auto resource = this->mgr.metric();
        const auto& flatten = this->mgr.field_flatten();
        const auto& working_type = this->mgr.working_type();
    
        return(resource && flatten && working_type);
      }
    
    
    bool resources::can_roll_metric_inverse() const
      {
        const auto resource = this->mgr.metric_inverse();
        const auto& flatten = this->mgr.field_flatten();
        const auto& working_type = this->mgr.working_type();
    
        return(resource && flatten && working_type);
      }
    
    
    bool resources::can_roll_Riemann_A2(const std::array< variance, RESOURCE_INDICES::RIEMANN_A2_INDICES >& vars) const
      {
        const auto resource = this->mgr.Riemann_A2(vars, false);
        const auto& flatten = this->mgr.field_flatten();
        const auto& working_type = this->mgr.working_type();
    
        bool G = resource ? this->get_roll_metric_requirements(resource.get().first, vars) : false;
        
        return(resource && G && flatten && working_type);
      }
    
    
    bool resources::can_roll_Riemann_A3(const std::array< variance, RESOURCE_INDICES::RIEMANN_A3_INDICES >& vars) const
      {
        const auto resource = this->mgr.Riemann_A3(vars, false);
        const auto& flatten = this->mgr.field_flatten();
        const auto& working_type = this->mgr.working_type();
    
        bool G = resource ? this->get_roll_metric_requirements(resource.get().first, vars) : false;
    
        return(resource && G && flatten && working_type);
      }
    
    
    bool resources::can_roll_Riemann_B3(const std::array< variance, RESOURCE_INDICES::RIEMANN_B3_INDICES >& vars) const
      {
        const auto resource = this->mgr.Riemann_A3(vars, false);
        const auto& flatten = this->mgr.field_flatten();
        const auto& working_type = this->mgr.working_type();
    
        bool G = resource ? this->get_roll_metric_requirements(resource.get().first, vars) : false;
    
        return(resource && G && flatten && working_type);
      }
    
    
    template <unsigned long Indices>
    bool resources::get_roll_metric_requirements(const std::array<variance, Indices>& avail,
                                                 const std::array<variance, Indices>& reqd) const
      {
        bool require_G = false;
        bool require_Ginv = false;
        
        for(unsigned long i = 0; i < Indices; ++i)
          {
            variance r = reqd[i];
            variance a = avail[i];
            
            if(r == variance::covariant && a == variance::contravariant) require_G = true;
            if(r == variance::contravariant && a == variance::covariant) require_Ginv = true;
          }
        
        bool ok = true;

        if(require_G)
          {
            auto resource = this->mgr.metric();
            if(!resource) ok = false;
          }
        
        if(require_Ginv)
          {
            auto resource = this->mgr.metric_inverse();
            if(!resource) ok  = false;
          }
        
        return ok;
      }


  }   // namespace nontrivial_metric
