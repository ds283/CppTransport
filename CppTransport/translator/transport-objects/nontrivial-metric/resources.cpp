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

#include "concepts/tensor_exception.h"

#include "resources_detail/common.h"
#include "resources_detail/connexion_cache.h"
#include "resources_detail/covariant_dV_cache.h"
#include "resources_detail/covariant_ddV_cache.h"
#include "resources_detail/covariant_dddV_cache.h"
#include "resources_detail/covariant_RiemannA2_cache.h"
#include "resources_detail/covariant_RiemannA3_cache.h"
#include "resources_detail/covariant_RiemannB3_cache.h"
#include "resources_detail/derivative_symbols.h"
#include "resources_detail/potential_cache.h"
#include "resources_detail/substitution_map_cache.h"

#include "ginac/ginac.h"


namespace nontrivial_metric
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
            for(unsigned int i = 0; i < N; ++i)
              {
                for(unsigned int j = 0; j < N; ++j)
                  {
                    field_metric::index_type idx = std::make_pair(field_list[i].get_name(), field_list[j].get_name());
                    this->G->set(i, j, G(idx));
                  }
              }

            // construct inverse metric by hand
            GiNaC::ex Ginv00 = ((std::power(M,2) + (std::power(chi1,2) + std::power(chi2,2) + std::power(chi3,2) + std::power(phi,2))*xi)*(std::power(M,2) + xi*(std::power(chi1,2) + std::power(chi2,2) + std::power(chi3,2) + std::power(phi,2) + 6*(std::power(chi1,4) + std::power(chi2,4) + std::power(chi3,4))*xi)))/
                               (std::power(M,2)*(std::power(M,2) + xi*(std::power(chi1,2) + std::power(chi2,2) + std::power(chi3,2) + std::power(phi,2) + 6*(std::power(chi1,4) + std::power(chi2,4) + std::power(chi3,4) + std::power(phi,4))*xi)));
            GiNaC::ex Ginv01 = (-6*std::power(chi1,2)*std::power(phi,2)*std::power(xi,2)*(std::power(M,2) + (std::power(chi1,2) + std::power(chi2,2) + std::power(chi3,2) + std::power(phi,2))*xi))/
                               (std::power(M,2)*(std::power(M,2) + xi*(std::power(chi1,2) + std::power(chi2,2) + std::power(chi3,2) + std::power(phi,2) + 6*(std::power(chi1,4) + std::power(chi2,4) + std::power(chi3,4) + std::power(phi,4))*xi)));
            GiNaC::ex Ginv02 = (-6*std::power(chi2,2)*std::power(phi,2)*std::power(xi,2)*(std::power(M,2) + (std::power(chi1,2) + std::power(chi2,2) + std::power(chi3,2) + std::power(phi,2))*xi))/
                               (std::power(M,2)*(std::power(M,2) + xi*(std::power(chi1,2) + std::power(chi2,2) + std::power(chi3,2) + std::power(phi,2) + 6*(std::power(chi1,4) + std::power(chi2,4) + std::power(chi3,4) + std::power(phi,4))*xi)));
            GiNaC::ex Ginv03 = (-6*std::power(chi3,2)*std::power(phi,2)*std::power(xi,2)*(std::power(M,2) + (std::power(chi1,2) + std::power(chi2,2) + std::power(chi3,2) + std::power(phi,2))*xi))/
                               (std::power(M,2)*(std::power(M,2) + xi*(std::power(chi1,2) + std::power(chi2,2) + std::power(chi3,2) + std::power(phi,2) + 6*(std::power(chi1,4) + std::power(chi2,4) + std::power(chi3,4) + std::power(phi,4))*xi)));
            GiNaC::ex Ginv11 = ((std::power(M,2) + (std::power(chi1,2) + std::power(chi2,2) + std::power(chi3,2) + std::power(phi,2))*xi)*(std::power(M,2) + xi*(std::power(chi1,2) + std::power(chi2,2) + std::power(chi3,2) + std::power(phi,2) + 6*(std::power(chi2,4) + std::power(chi3,4) + std::power(phi,4))*xi)))/
                               (std::power(M,2)*(std::power(M,2) + xi*(std::power(chi1,2) + std::power(chi2,2) + std::power(chi3,2) + std::power(phi,2) + 6*(std::power(chi1,4) + std::power(chi2,4) + std::power(chi3,4) + std::power(phi,4))*xi)));
            GiNaC::ex Ginv12 = (-6*std::power(chi1,2)*std::power(chi2,2)*std::power(xi,2)*(std::power(M,2) + (std::power(chi1,2) + std::power(chi2,2) + std::power(chi3,2) + std::power(phi,2))*xi))/
                               (std::power(M,2)*(std::power(M,2) + xi*(std::power(chi1,2) + std::power(chi2,2) + std::power(chi3,2) + std::power(phi,2) + 6*(std::power(chi1,4) + std::power(chi2,4) + std::power(chi3,4) + std::power(phi,4))*xi)));
            GiNaC::ex Ginv13 = (-6*std::power(chi1,2)*std::power(chi3,2)*std::power(xi,2)*(std::power(M,2) + (std::power(chi1,2) + std::power(chi2,2) + std::power(chi3,2) + std::power(phi,2))*xi))/
                               (std::power(M,2)*(std::power(M,2) + xi*(std::power(chi1,2) + std::power(chi2,2) + std::power(chi3,2) + std::power(phi,2) + 6*(std::power(chi1,4) + std::power(chi2,4) + std::power(chi3,4) + std::power(phi,4))*xi)));
            GiNaC::ex Ginv22 = ((std::power(M,2) + (std::power(chi1,2) + std::power(chi2,2) + std::power(chi3,2) + std::power(phi,2))*xi)*(std::power(M,2) + xi*(std::power(chi1,2) + std::power(chi2,2) + std::power(chi3,2) + std::power(phi,2) + 6*(std::power(chi1,4) + std::power(chi3,4) + std::power(phi,4))*xi)))/
                               (std::power(M,2)*(std::power(M,2) + xi*(std::power(chi1,2) + std::power(chi2,2) + std::power(chi3,2) + std::power(phi,2) + 6*(std::power(chi1,4) + std::power(chi2,4) + std::power(chi3,4) + std::power(phi,4))*xi)));
            GiNaC::ex Ginv23 = (-6*std::power(chi2,2)*std::power(chi3,2)*std::power(xi,2)*(std::power(M,2) + (std::power(chi1,2) + std::power(chi2,2) + std::power(chi3,2) + std::power(phi,2))*xi))/
                               (std::power(M,2)*(std::power(M,2) + xi*(std::power(chi1,2) + std::power(chi2,2) + std::power(chi3,2) + std::power(phi,2) + 6*(std::power(chi1,4) + std::power(chi2,4) + std::power(chi3,4) + std::power(phi,4))*xi)));
            GiNaC::ex Ginv33 = ((std::power(M,2) + (std::power(chi1,2) + std::power(chi2,2) + std::power(chi3,2) + std::power(phi,2))*xi)*(std::power(M,2) + xi*(std::power(chi1,2) + std::power(chi2,2) + std::power(chi3,2) + std::power(phi,2) + 6*(std::power(chi1,4) + std::power(chi2,4) + std::power(phi,4))*xi)))/
                               (std::power(M,2)*(std::power(M,2) + xi*(std::power(chi1,2) + std::power(chi2,2) + std::power(chi3,2) + std::power(phi,2) + 6*(std::power(chi1,4) + std::power(chi2,4) + std::power(chi3,4) + std::power(phi,4))*xi)));

            this->Ginv->set(0, 0, Ginv00);
            this->Ginv->set(0, 1, Ginv01);
            this->Ginv->set(0, 2, Ginv02);
            this->Ginv->set(0, 3, Ginv03);
            this->Ginv->set(1, 0, Ginv01);
            this->Ginv->set(1, 1, Ginv11);
            this->Ginv->set(1, 2, Ginv12);
            this->Ginv->set(1, 3, Ginv13);
            this->Ginv->set(2, 0, Ginv02);
            this->Ginv->set(2, 1, Ginv12);
            this->Ginv->set(2, 2, Ginv22);
            this->Ginv->set(2, 3, Ginv23);
            this->Ginv->set(3, 0, Ginv03);
            this->Ginv->set(3, 1, Ginv13);
            this->Ginv->set(3, 2, Ginv23);
            this->Ginv->set(3, 3, Ginv33);
          }
        else    // unexpected case that no metric has been provided; attempt to recover gracefully (errors should have been emitted before this stage)
          {
            *(this->G) = GiNaC::ex_to<GiNaC::matrix>(GiNaC::unit_matrix(N));
            *(this->Ginv) = GiNaC::ex_to<GiNaC::matrix>(GiNaC::unit_matrix(N));
          }

        // construct curvature tensors based on this metric
        this->Crstfl = std::make_unique<Christoffel>(*this->G, *this->Ginv, field_list);
        this->Rie_T = std::make_unique<Riemann_T>(*this->Crstfl);
        this->DRie_T = std::make_unique<DRiemann_T>(*this->Rie_T);

        // switch off compute timer (it will be restarted if needed during subsequent computations)
        compute_timer.stop();
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

        auto X = std::make_unique<flattened_tensor>(this->fl.get_flattened_size<field_index>(1));

        const auto Xcov_syms = this->share.generate_deriv_symbols(printer);
        flattened_tensor Xcov;
        std::copy(Xcov_syms->begin(), Xcov_syms->end(), std::back_inserter(Xcov));
        
        auto max_i = this->share.get_max_field_index(var);
        
        for(field_index i = field_index(0, var); i < max_i; ++i)
          {
            unsigned int index = this->fl.flatten(i);
            (*X)[index] = this->position_indices<1>({variance::contravariant}, {var}, {i}, Xcov, printer);
          }

        return X;
      }


    GiNaC::ex resources::generate_field_vector(const index_literal& idx, const language_printer& printer) const
      {
        if(idx.get_class() != index_class::field_only) throw tensor_exception("generate_field_vector index class");

        // for the coordinates themselves, there is no notion of co- or contravariant components;
        // the coordinate labels are just scalar fields on the manifold.
        // share.generate_field_symbols() gives us these labels directly, so all we need do is return them.

        const auto& resource = this->mgr.coordinates();
        const auto& flatten = this->mgr.phase_flatten();

        if(!resource || !flatten) throw resource_failure("coordinate vector");

        std::string variable = printer.array_subscript(*resource, idx, **flatten);
        return this->sym_factory.get_real_symbol(variable);
      }


    GiNaC::ex resources::generate_deriv_vector(const index_literal& idx, const language_printer& printer) const
      {
        if(idx.get_class() != index_class::field_only) throw tensor_exception("generate_deriv_vector index class");

        // the field derivatives transform tensorially and so here there is a notion of co- and contravariant
        // components.
        // Depending what resources are available we may need to perform index raising or lowering, which is handled
        // by position_indices()

        const auto& resource = this->mgr.coordinates();
        const auto& flatten = this->mgr.phase_flatten();

        if(!resource || !flatten) throw resource_failure("coordinate vector");
        
        // idx is a field vector, so to pick out the derivatives from the full phase-space coordinates
        // it will need to be shifted by +number_of_fields
        // However, we can't use the built in offsetting API from abstract_index, because if we need to raise
        // or lower an index then this offset value would end up appearing in the metric G, which is wrong --
        // it's only values used to index the *label* that we want to offset.
        // to accommodate this we need the extra 'offsets' argument of position_indices()
        return this->position_indices<1, index_literal>({ variance::contravariant }, { std::cref(idx) }, *resource, **flatten, printer, {+1});
      }


    //! expand an array of index labels into a call to printer.array_subscript()
    template <size_t Indices, size_t... Is>
    std::string
    position_indices_label(const std::string& kernel, const std::array<std::string, Indices>& indices,
                           std::index_sequence<Is...>, const std::string& flatten, const language_printer& printer)
      {
        return printer.array_subscript(kernel, indices[Is]..., flatten);
      };


    template <size_t Indices, typename IndexType>
    GiNaC::ex
    resources::position_indices(const std::array<variance, Indices> avail,
                                const std::array<std::reference_wrapper<const IndexType>, Indices> reqd,
                                const contexted_value<std::string>& label, const std::string& flatten,
                                const language_printer& printer,
                                const std::array<int, Indices> offsets) const
      {
        const unsigned int N = this->payload.model.get_number_fields();
        
        const auto G = this->mgr.metric();
        const auto Ginv = this->mgr.metric_inverse();
        
        const auto field_flatten = this->mgr.field_flatten();
        if(!field_flatten) throw resource_failure("fields flatten");
        
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

        this->warn_resource_index_reposition(label, size, repositioned);

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
                variance a = avail[j];
                variance r = reqd[j].get().get_variance();
                
                if(r != a)
                  {
                    unsigned int v = index_pos % N;
                    index_pos = index_pos / N;
                    
                    if(r == variance::contravariant)
                      {
                        if(!Ginv) throw resource_failure("metric inverse");
                        auto factor = printer.array_subscript(*Ginv, reqd[j].get(), v, **field_flatten);
                        auto sym = this->sym_factory.get_real_symbol(factor);
                        
                        // apply offset to label index, if one is present
                        index_values[j] = to_index_string(v + static_cast<int>(N)*offsets[j]);
                        term *= sym;
                      }
                    else if(r == variance::covariant)
                      {
                        if(!G) throw resource_failure("metric");
                        auto factor = printer.array_subscript(*G, reqd[j].get(), v, **field_flatten);
                        
                        // apply offset to label index, if one is present
                        index_values[j] = to_index_string(v + static_cast<int>(N)*offsets[j]);
                        auto sym = this->sym_factory.get_real_symbol(factor);
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
                    
                    // apply offset to label index, if one is present
                    if(offsets[j] > 0)
                      {
                        index_values[j].append("+");
                        index_values[j].append(std::to_string(static_cast<int>(N)*offsets[j]));
                      }
                    if(offsets[j] < 0)
                      {
                        index_values[j].append(std::to_string(static_cast<int>(N)*offsets[j]));
                      }
                  }
              }

            // finally, include copy of resource label with appropriate indices
            auto factor = position_indices_label(label, index_values, std::make_index_sequence<Indices>{}, flatten, printer);
            auto sym = this->sym_factory.get_real_symbol(factor);
            term *= sym;

            res += term;
          }
        
        return res;
      }


    void
    resources::warn_resource_index_reposition(const contexted_value<std::string>& label, unsigned int size,
                                              unsigned int repositioned) const
      {
        if(size > 1 && this->payload.get_argument_cache().report_reposition_warnings())
          {
            const error_context& ctx = label.get_declaration_point();
            std::ostringstream msg;

            msg << NOTIFY_REPOSITIONING_INDICES_A << " " << repositioned << " "
                << (repositioned > 1 ? NOTIFY_REPOSITIONING_INDICES_B1 : NOTIFY_REPOSITIONING_INDICES_B2)
                << " '" << *label << "'";
            ctx.warn(msg.str());
          }
      }


    template <size_t Indices, size_t... Is>
    GiNaC::ex
    unpack_flattened_tensor(const flattened_tensor& tensor, const std::array<field_index, Indices>& indices,
                            std::index_sequence<Is...>, const index_flatten& fl)
      {
        return tensor[fl.flatten(indices[Is]...)];
      }
    
    
    namespace resource_impl
      {

        //! generate a null field_index instance with assigned variance; used when expanding a parameter
        //! pack pattern
        field_index initialize_index_helper(variance v)
          {
            return { 0, v };
          }
        
      }   // namespace resource_impl
    
    
    using resource_impl::initialize_index_helper;
    
    
    template <size_t Indices, size_t... Is>
    std::array<field_index, Indices>
    initialize_index_values(const std::array<variance, Indices>& init, std::index_sequence<Is...>)
      {
        return { initialize_index_helper(init[Is])... };
      };


    template <size_t Indices>
    GiNaC::ex
    resources::position_indices(const std::array<variance, Indices> avail, const std::array<variance, Indices> reqd,
                                const std::array<field_index, Indices> indices, const flattened_tensor& tensor,
                                const language_printer& printer) const
      {
        timing_instrument(this->compute_timer);

        const unsigned int N = this->payload.model.get_number_fields();

        const auto G = this->metric_resource(printer);
        const auto Ginv = this->metric_inverse_resource(printer);

        // compute size of the sum required to produce the desired index combination
        unsigned int size = 1;

        for(unsigned int i = 0; i < Indices; ++i)
          {
            variance a = avail[i];
            variance r = reqd[i];

            // if required and available indices don't match, we need a sum over N fields
            if(r != a) size *= N;
          }

        GiNaC::ex res = 0;

        // loop through all terms in the sum, constructing them appropriately
        for(unsigned int i = 0; i < size; ++i)
          {
            std::array<field_index, Indices> index_values = initialize_index_values(reqd, std::make_index_sequence<Indices>{});
            unsigned int index_pos = i;
            GiNaC::ex term = 1;

            for(unsigned int j = 0; j < Indices; ++j)
              {
                variance a = avail[j];
                variance r = reqd[j];

                if(r != a)
                  {
                    field_index v_r(index_pos % N, r);
                    field_index v_a(index_pos % N, a);
                    index_pos = index_pos / N;

                    if(r == variance::contravariant)
                      {
                        term *= (*Ginv)[this->fl.flatten(indices[j], v_r)];
                        index_values[j] = v_a;
                      }
                    else if(r == variance::covariant)
                      {
                        term *= (*G)[this->fl.flatten(indices[j], v_r)];
                        index_values[j] = v_a;
                      }
                    else
                      {
                        throw tensor_exception(ERROR_INDEX_MISSING_VARIANCE_DATA);
                      }
                  }
                else
                  {
                    index_values[j] = indices[j];
                  }
              }

            // finally, include copy of resource label with appropriate indices
            auto factor = unpack_flattened_tensor(tensor, index_values, std::make_index_sequence<Indices>{}, this->fl);
            term *= factor;

            res += term;
          }

        return res;
      }


    GiNaC::ex resources::V_resource(cse& cse_worker, const language_printer& printer) const
      {
        // behaviour differs depending whether CSE is in use or not
        // If not, we want to return the raw expression; if it is, we want to assign an internal name to a collection
        // of CSE temporaries, and then return a *different* symbolic expression representing this collection
        if(this->payload.do_cse())
          {
            auto V = this->share.generate_V();
            auto raw_V = this->raw_V_resource(printer);

            // parse raw expression, assigning result to correct symbolic name
            cse_worker.parse(raw_V, V.get_name());

            // return symbol
            return V;
          }
    
        return this->raw_V_resource(printer);
      }


    GiNaC::ex resources::raw_V_resource(const language_printer& printer) const
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


    GiNaC::ex resources::eps_resource(cse& cse_worker, const language_printer& printer) const
      {
        if(this->payload.do_cse())
          {
            auto eps = this->share.generate_eps();
            auto raw_eps = this->raw_eps_resource(printer);

            // parse raw expression, assigning result to the correct symbolic name
            cse_worker.parse(raw_eps, eps.get_name());

            // return symbol
            return eps;
          }
    
        return this->raw_eps_resource(printer);
      }


    GiNaC::ex resources::raw_eps_resource(const language_printer& printer) const
      {
        auto args = this->generate_cache_arguments(printer);

        GiNaC::ex eps;

        if(!this->cache.query(expression_item_types::epsilon_item, 0, args, eps))
          {
            timing_instrument timer(this->compute_timer);

            auto derivs = this->share.generate_deriv_symbols(printer);
            auto Mp = this->share.generate_Mp();

            eps = 0;
            
            if(derivs->size() != this->G->rows()) throw std::runtime_error(ERROR_METRIC_DIMENSION);
            if(derivs->size() != this->G->cols()) throw std::runtime_error(ERROR_METRIC_DIMENSION);

            field_index max = this->share.get_max_field_index(variance::none);

            for(field_index i = field_index(0, variance::none); i < max; ++i)
              {
                for(field_index j = field_index(0, variance::none); j < max; ++j)
                  {
                    auto iv = this->fl.flatten(i);
                    auto jv = this->fl.flatten(j);

                    eps += (*this->G)(iv,jv) * (*derivs)[iv] * (*derivs)[jv];
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


    GiNaC::ex resources::eta_resource(cse& cse_worker, const language_printer& printer) const
      {
        auto eps = this->eps_resource(cse_worker, printer);
        auto Hsq = this->Hsq_resource(cse_worker, printer);

        if(this->payload.do_cse())
          {
            auto eta = this->share.generate_eta();
            auto raw_eta = this->raw_eta_resource(eps, Hsq, printer);

            // parse raw expression, assigning result to the correct symbolic name
            cse_worker.parse(raw_eta, eta.get_name());

            // return symbol
            return eta;
          }

        return this->raw_eta_resource(eps, Hsq, printer);
      }


    GiNaC::ex resources::raw_eta_resource(GiNaC::ex eps, GiNaC::ex Hsq, const language_printer& printer) const
      {
        auto args = this->generate_cache_arguments(printer);

        GiNaC::ex eta;

        if(!this->cache.query(expression_item_types::eta_item, 0, args, eta))
          {
            timing_instrument timer(this->compute_timer);

            auto derivs = this->share.generate_deriv_symbols(printer);
            auto dV = this->dV_resource(variance::covariant, printer);
            auto Mp = this->share.generate_Mp();

            if(derivs->size() != this->G->rows()) throw std::runtime_error(ERROR_METRIC_DIMENSION);
            if(derivs->size() != this->G->cols()) throw std::runtime_error(ERROR_METRIC_DIMENSION);

            if(dV->size() != this->G->rows()) throw std::runtime_error(ERROR_METRIC_DIMENSION);
            if(dV->size() != this->G->cols()) throw std::runtime_error(ERROR_METRIC_DIMENSION);

            GiNaC::ex depsdN = 2*eps*(eps-3);

            field_index max_i = this->share.get_max_field_index(variance::none);

            for(field_index i = field_index(0, variance::none); i < max_i; ++i)
              {
                // dV is covariant and derivative is contravariant, so this is a proper invariant
                depsdN -= (*derivs)[this->fl.flatten(i)] * (*dV)[this->fl.flatten(i)] / (Hsq*Mp*Mp);
              }

            // no need to perform parameter/coordinate substitution here, since eps and H will already have been
            // substituted individually
            eta = depsdN / eps;

            this->cache.store(expression_item_types::eta_item, 0, args, eta);
          }

        return eta;
      }


    GiNaC::ex resources::Hsq_resource(cse& cse_worker, const language_printer& printer) const
      {
        if(this->payload.do_cse())
          {
            auto Hsq = this->share.generate_Hsq();
            auto raw_Hsq = this->raw_Hsq_resource(printer);

            // parse raw expression, assigning result to the correct symbolic name
            cse_worker.parse(raw_Hsq, Hsq.get_name());

            // return symbol (no deposition into temporary pool yet guaranteed -- needs to be explicitly printed)
            return Hsq;
          }
    
        return this->raw_Hsq_resource(printer);
      }


    GiNaC::ex resources::raw_Hsq_resource(const language_printer& printer) const
      {
        auto args = this->generate_cache_arguments(printer);

        GiNaC::ex Hsq;

        if(!this->cache.query(expression_item_types::Hubble2_item, 0, args, Hsq))
          {
            timing_instrument timer(this->compute_timer);

            auto V = this->raw_V_resource(printer);
            auto eps = this->raw_eps_resource(printer);
            auto Mp = this->share.generate_Mp();

            // no need to perform parameter/coordinate substitution here, since V and eps will already have been
            // substituted individually
            Hsq = V / ((3-eps)*Mp*Mp);
            
            this->cache.store(expression_item_types::Hubble2_item, 0, args, Hsq);
          }

        return(Hsq);
      }


    std::unique_ptr<flattened_tensor>
    resources::raw_G_resource(const language_printer& printer) const
      {
        auto G = std::make_unique<flattened_tensor>(this->fl.get_flattened_size<field_index>(2));
    
        auto args = this->generate_cache_arguments(printer);
    
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
                    (*G)[index_ij] = (*this->G)(static_cast<unsigned int>(i), static_cast<unsigned int>(j));
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
    
    
    std::unique_ptr<flattened_tensor>
    resources::raw_Ginv_resource(const language_printer& printer) const
      {
        auto Ginv = std::make_unique<flattened_tensor>(this->fl.get_flattened_size<field_index>(2));
    
        auto args = this->generate_cache_arguments(printer);
    
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


    std::unique_ptr<flattened_tensor>
    resources::dV_resource(variance v, const language_printer& printer) const
      {
        auto list = std::make_unique<flattened_tensor>(this->fl.get_flattened_size<field_index>(RESOURCE_INDICES::DV_INDICES));

        const auto resource = this->mgr.dV({ v }, false);
        const auto& flatten = this->mgr.field_flatten();

        if(resource && flatten)     // resources appear to be available
          {
            try
              {
                this->tensor_resource_label(*list, resource.get().first, {v}, resource.get().second, *flatten, printer);
                return list;
              }
            catch(resource_failure& xe)
              {
                const error_context& ctx = resource.get().second.get_declaration_point();
                ctx.warn(WARNING_INDEX_POSITIONING_FAILURE);
              }
          }
        
        // either resources were not available, or a resource failure occurred when raising and lowering indices
        CovariantdVCache cache{*this, printer};
        this->tensor_resource_expr(*list, std::array<variance,1 >{v}, expression_item_types::dV_item, printer, cache);

        return list;
      }

    
    GiNaC::ex resources::dV_resource(const index_literal& a, const language_printer& printer) const
      {
        const auto resource = this->mgr.dV({ a.get_variance() }, false);
        const auto& flatten = this->mgr.field_flatten();
        
        if(!resource || !flatten) throw resource_failure("dV");
        
        return this->position_indices<1, index_literal>(resource.get().first, { a }, resource.get().second, *flatten, printer);
      }


    std::unique_ptr<flattened_tensor>
    resources::ddV_resource(variance vi, variance vj, const language_printer& printer) const
      {
        auto list = std::make_unique<flattened_tensor>(this->fl.get_flattened_size<field_index>(RESOURCE_INDICES::DDV_INDICES));

        const auto resource = this->mgr.ddV({ vi, vj }, false);
        const auto& flatten = this->mgr.field_flatten();

        if(resource && flatten)     // ddV is available
          {
            try
              {
                this->tensor_resource_label(*list, resource.get().first, { vi, vj }, resource.get().second, *flatten, printer);
                return list;
              }
            catch(resource_failure& xe)
              {
                const error_context& ctx = resource.get().second.get_declaration_point();
                ctx.warn(WARNING_INDEX_POSITIONING_FAILURE);
              }
          }

        // either resources were not available, or a resource failure occurred when raising and lowering indices
        CovariantddVCache cache{*this, printer};
        this->tensor_resource_expr(*list, std::array<variance, 2>{ vi, vj }, expression_item_types::ddV_item, printer, cache);

        return list;
      }

    
    GiNaC::ex resources::ddV_resource(const index_literal& a, const index_literal& b, const language_printer& printer) const
      {
        const auto resource = this->mgr.ddV({ a.get_variance(), b.get_variance() }, false);
        const auto& flatten = this->mgr.field_flatten();

        if(!resource || !flatten) throw resource_failure("ddV");

        return this->position_indices<2, index_literal>(resource.get().first, { a, b }, resource.get().second, *flatten, printer);
      }


    std::unique_ptr<flattened_tensor>
    resources::dddV_resource(variance vi, variance vj, variance vk, const language_printer& printer) const
      {
        auto list = std::make_unique<flattened_tensor>(this->fl.get_flattened_size<field_index>(RESOURCE_INDICES::DDDV_INDICES));

        const auto resource = this->mgr.dddV({ vi, vj, vk }, false);
        const auto& flatten = this->mgr.field_flatten();

        if(resource && flatten)     // dddV is available
          {
            try
              {
                this->tensor_resource_label(*list, resource.get().first, { vi, vj, vj }, resource.get().second, *flatten, printer);
              }
            catch(resource_failure& xe)
              {
                const error_context& ctx = resource.get().second.get_declaration_point();
                ctx.warn(WARNING_INDEX_POSITIONING_FAILURE);
              }
          }

        // either resources were not available, or a resource failure occurred when raising and lowering indices
        CovariantdddVCache cache{*this, printer};
        this->tensor_resource_expr(*list, std::array<variance, 3>{ vi, vj, vk }, expression_item_types::dddV_item, printer, cache);

        return(list);
      }


    GiNaC::ex resources::dddV_resource(const index_literal& a, const index_literal& b, const index_literal& c,
                                       const language_printer& printer) const
      {
        const auto resource = this->mgr.dddV({ a.get_variance(), b.get_variance(), c.get_variance() }, false);
        const auto& flatten = this->mgr.field_flatten();

        if(!resource || !flatten) throw resource_failure("dddV");

        return this->position_indices<3, index_literal>(resource.get().first, { a, b, c }, resource.get().second, *flatten, printer);
      }


    std::unique_ptr<flattened_tensor>
    resources::Riemann_A2_resource(variance vi, variance vj, const language_printer& printer) const
      {
        auto list = std::make_unique<flattened_tensor>(this->fl.get_flattened_size<field_index>(RESOURCE_INDICES::RIEMANN_A2_INDICES));

        const auto resource = this->mgr.Riemann_A2({ vi, vj }, false);
        const auto& flatten = this->mgr.field_flatten();

        if(resource && flatten)      // label is apparently available, but may require index repositioning ...
          {
            try
              {
                this->tensor_resource_label(*list, resource.get().first, { vi, vj }, resource.get().second, *flatten, printer);
                return list;
              }
            catch(resource_failure& xe)
              {
                const error_context& ctx = resource.get().second.get_declaration_point();
                ctx.warn(WARNING_INDEX_POSITIONING_FAILURE);
              }
          }

        // either resources were not available, or a resource failure occurred when raising and lowering indices
        CovariantRiemannA2Cache cache{*this, printer};
        this->tensor_resource_expr(*list, std::array<variance, 2>{ vi, vj }, expression_item_types::Riemann_A2_item, printer, cache);

        return list;
      }


    GiNaC::ex
    resources::Riemann_A2_resource(const index_literal& a, const index_literal& b, const language_printer& printer) const
      {
        const auto resource = this->mgr.Riemann_A2({ a.get_variance(), b.get_variance() }, false);
        const auto& flatten = this->mgr.field_flatten();

        if(!resource || !flatten) throw resource_failure("Riemann_A2");

        return this->position_indices<2, index_literal>(resource.get().first, { a, b }, resource.get().second, *flatten, printer);
      }


    std::unique_ptr<flattened_tensor>
    resources::Riemann_A3_resource(variance vi, variance vj, variance vk, const language_printer& printer) const
      {
        auto list = std::make_unique<flattened_tensor>(this->fl.get_flattened_size<field_index>(RESOURCE_INDICES::RIEMANN_A3_INDICES));

        const auto resource = this->mgr.Riemann_A3({ vi, vj, vk }, false);
        const auto& flatten = this->mgr.field_flatten();

        if(resource && flatten)      // label is apparently available, but may require index repositioning ...
          {
            try
              {
                this->tensor_resource_label(*list, resource.get().first, { vi, vj, vk }, resource.get().second, *flatten, printer);
                return list;
              }
            catch(resource_failure& xe)
              {
                const error_context& ctx = resource.get().second.get_declaration_point();
                ctx.warn(WARNING_INDEX_POSITIONING_FAILURE);
              }
          }

        // either resources were not available, or a resource failure occurred when raising and lowering indices
        CovariantRiemannA3Cache cache{*this, printer};
        this->tensor_resource_expr(*list, std::array<variance, 3>{ vi, vj, vk }, expression_item_types::Riemann_A3_item, printer, cache);

        return list;
      }


    GiNaC::ex resources::Riemann_A3_resource(const index_literal& a, const index_literal& b, const index_literal& c,
                                             const language_printer& printer) const
      {
        const auto resource = this->mgr.Riemann_A3({ a.get_variance(), b.get_variance(), c.get_variance() }, false);
        const auto& flatten = this->mgr.field_flatten();

        if(!resource || !flatten) throw resource_failure("Riemann_A3");

        return this->position_indices<3, index_literal>(resource.get().first, { a, b, c }, resource.get().second, *flatten, printer);
      }


    std::unique_ptr<flattened_tensor>
    resources::Riemann_B3_resource(variance vi, variance vj, variance vk, const language_printer& printer) const
      {
        auto list = std::make_unique<flattened_tensor>(this->fl.get_flattened_size<field_index>(RESOURCE_INDICES::RIEMANN_B3_INDICES));

        const auto resource = this->mgr.Riemann_B3({ vi, vj, vk }, false);
        const auto& flatten = this->mgr.field_flatten();

        if(resource && flatten)      // label is apparently available, but may require index repositioning ...
          {
            try
              {
                this->tensor_resource_label(*list, resource.get().first, { vi, vj, vk }, resource.get().second, *flatten, printer);
                return list;
              }
            catch(resource_failure& xe)
              {
                const error_context& ctx = resource.get().second.get_declaration_point();
                ctx.warn(WARNING_INDEX_POSITIONING_FAILURE);
              }
          }

        // either resources were not available, or a resource failure occurred when raising and lowering indices
        CovariantRiemannB3Cache cache{*this, printer};
        this->tensor_resource_expr(*list, { vi, vj, vk }, expression_item_types::Riemann_B3_item, printer, cache);

        return list;
      }


    GiNaC::ex resources::Riemann_B3_resource(const index_literal& a, const index_literal& b, const index_literal& c,
                                             const language_printer& printer) const
      {
        const auto resource = this->mgr.Riemann_B3({ a.get_variance(), b.get_variance(), c.get_variance() }, false);
        const auto& flatten = this->mgr.field_flatten();

        if(!resource || !flatten) throw resource_failure("Riemann_B3");

        return this->position_indices<3, index_literal>(resource.get().first, { a, b, c }, resource.get().second, *flatten, printer);
      }


    void resources::tensor_resource_label(flattened_tensor& list,
                                          const std::array<variance, 1>& avail,
                                          const std::array<variance, 1> reqd,
                                          const contexted_value<std::string>& resource,
                                          const contexted_value<std::string>& flatten,
                                          const language_printer& printer) const
      {
        const field_index max_i = this->share.get_max_field_index(reqd[0]);

        for(field_index i = field_index(0, reqd[0]); i < max_i; ++i)
          {
            unsigned int index = this->fl.flatten(i);

            list[index] = this->position_indices<1, field_index>(avail, {i}, resource, *flatten, printer);
          }
      }


    void resources::tensor_resource_label(flattened_tensor& list,
                                          const std::array<variance, 2>& avail,
                                          const std::array<variance, 2> reqd,
                                          const contexted_value<std::string>& resource,
                                          const contexted_value<std::string>& flatten,
                                          const language_printer& printer) const
      {
        const field_index max_i = this->share.get_max_field_index(reqd[0]);
        const field_index max_j = this->share.get_max_field_index(reqd[1]);

        for(field_index i = field_index(0, reqd[0]); i < max_i; ++i)
          {
            for(field_index j = field_index(0, reqd[1]); j < max_j; ++j)
              {
                unsigned int index = this->fl.flatten(i,j);

                list[index] = this->position_indices<2, field_index>(avail, { i, j }, resource, *flatten, printer);
              }
          }
      }


    void resources::tensor_resource_label(flattened_tensor& list,
                                          const std::array<variance, 3>& avail,
                                          const std::array<variance, 3> reqd,
                                          const contexted_value<std::string>& resource,
                                          const contexted_value<std::string>& flatten,
                                          const language_printer& printer) const
      {
        const field_index max_i = this->share.get_max_field_index(reqd[0]);
        const field_index max_j = this->share.get_max_field_index(reqd[1]);
        const field_index max_k = this->share.get_max_field_index(reqd[2]);

        for(field_index i = field_index(0, reqd[0]); i < max_i; ++i)
          {
            for(field_index j = field_index(0, reqd[1]); j < max_j; ++j)
              {
                for(field_index k = field_index(0, reqd[2]); k < max_k; ++k)
                  {
                    unsigned int index = this->fl.flatten(i,j,k);

                    list[index] = this->position_indices<3, field_index>(avail, { i, j, k }, resource, *flatten, printer);
                  }
              }
          }
      }


    template <typename CacheObject>
    void
    resources::tensor_resource_expr(flattened_tensor& list, const std::array<variance, 1> reqd,
                                    expression_item_types type, const language_printer& printer, CacheObject& cache) const
      {
        const field_index max = this->share.get_max_field_index(reqd[0]);

        // build argument list and tag with index variance information
        auto args = this->generate_cache_arguments(printer);

        GiNaC::varidx idx_i(this->sym_factory.get_real_symbol(I_INDEX_NAME), static_cast<unsigned int>(max), reqd[0] == variance::covariant);
        args += idx_i;

        for(field_index i = field_index(0, reqd[0]); i < max; ++i)
          {
            GiNaC::ex expr;
            unsigned int index = this->fl.flatten(i);

            if(!this->cache.query(type, index, args, expr))
              {
                timing_instrument timer(this->compute_timer);

                // get covariant components from covariant cache
                const auto& covariant_expr = cache.get();

                // reposition indices if needed
                expr = this->position_indices<1>({variance::covariant}, reqd, {i}, covariant_expr, printer);

                this->cache.store(type, index, args, expr);
              }

            list[index] = expr;
          }
      }


    template <typename CacheObject>
    void
    resources::tensor_resource_expr(flattened_tensor& list, const std::array<variance, 2> reqd,
                                    expression_item_types type, const language_printer& printer, CacheObject& cache) const
      {
        const field_index max_i = this->share.get_max_field_index(reqd[0]);
        const field_index max_j = this->share.get_max_field_index(reqd[1]);

        // build argument list and tag with index variance information
        auto args = this->generate_cache_arguments(printer);

        GiNaC::varidx idx_i(this->sym_factory.get_real_symbol(I_INDEX_NAME), static_cast<unsigned int>(max_i), reqd[0] == variance::covariant);
        GiNaC::varidx idx_j(this->sym_factory.get_real_symbol(J_INDEX_NAME), static_cast<unsigned int>(max_j), reqd[1] == variance::covariant);
        args += { idx_i, idx_j };

        for(field_index i = field_index(0, reqd[0]); i < max_i; ++i)
          {
            for(field_index j = field_index(0, reqd[1]); j < max_j; ++j)
              {
                GiNaC::ex expr;
                unsigned int index = this->fl.flatten(i,j);

                if(!this->cache.query(type, index, args, expr))
                  {
                    timing_instrument timer(this->compute_timer);

                    // get covariant components from covariant cache
                    const auto& covariant_expr = cache.get();

                    // reposition indices if needed
                    expr = this->position_indices<2>({variance::covariant, variance::covariant}, reqd, {i, j}, covariant_expr, printer);

                    this->cache.store(type, index, args, expr);
                  }

                list[index] = expr;
              }
          }
      }


    template <typename CacheObject>
    void
    resources::tensor_resource_expr(flattened_tensor& list, const std::array<variance, 3> reqd,
                                    expression_item_types type, const language_printer& printer, CacheObject& cache) const
      {
        const field_index max_i = this->share.get_max_field_index(reqd[0]);
        const field_index max_j = this->share.get_max_field_index(reqd[1]);
        const field_index max_k = this->share.get_max_field_index(reqd[2]);

        // build argument list and tag with index variance information
        auto args = this->generate_cache_arguments(printer);

        GiNaC::varidx idx_i(this->sym_factory.get_real_symbol(I_INDEX_NAME), static_cast<unsigned int>(max_i), reqd[0] == variance::covariant);
        GiNaC::varidx idx_j(this->sym_factory.get_real_symbol(J_INDEX_NAME), static_cast<unsigned int>(max_j), reqd[1] == variance::covariant);
        GiNaC::varidx idx_k(this->sym_factory.get_real_symbol(K_INDEX_NAME), static_cast<unsigned int>(max_j), reqd[2] == variance::covariant);
        args += { idx_i, idx_j, idx_k };

        for(field_index i = field_index(0, reqd[0]); i < max_i; ++i)
          {
            for(field_index j = field_index(0, reqd[1]); j < max_j; ++j)
              {
                for(field_index k = field_index(0, reqd[2]); k < max_k; ++k)
                  {
                    GiNaC::ex expr;
                    unsigned int index = this->fl.flatten(i,j,k);

                    if(!this->cache.query(type, index, args, expr))
                      {
                        timing_instrument timer(this->compute_timer);

                        // get covariant components from covariant cache
                        const auto& covariant_expr = cache.get();

                        // reposition indices if needed
                        expr = this->position_indices<3>(
                          {variance::covariant, variance::covariant, variance::covariant}, reqd, {i, j, k},
                          covariant_expr, printer);

                        this->cache.store(type, index, args, expr);
                      }

                    list[index] = expr;
                  }
              }
          }
      }


    std::unique_ptr<flattened_tensor> resources::metric_resource(const language_printer& printer) const
      {
        std::unique_ptr<flattened_tensor> list;

        const auto resource = this->mgr.metric();
        const auto& flatten = this->mgr.field_flatten();

        if(resource && flatten)
          {
            list = std::make_unique<flattened_tensor>(this->fl.get_flattened_size<field_index>(2));
            this->tensor_resource_label(*list, std::array<variance, 2>{variance::covariant, variance::covariant},
                                        *resource, *flatten, printer);
          }
        else
          {
            list = this->raw_G_resource(printer);
          }

        return list;
      }


    GiNaC::ex
    resources::metric_resource(const index_literal& a, const index_literal& b, const language_printer& printer) const
      {
        const auto resource = this->mgr.metric();
        const auto& flatten = this->mgr.field_flatten();

        if(!resource || !flatten) throw resource_failure("metric");

        std::string variable = printer.array_subscript(*resource, a, b, **flatten);
        return this->sym_factory.get_real_symbol(variable);
      }


    std::unique_ptr<flattened_tensor> resources::metric_inverse_resource(const language_printer& printer) const
      {
        std::unique_ptr<flattened_tensor> list;

        const auto resource = this->mgr.metric_inverse();
        const auto& flatten = this->mgr.field_flatten();

        if(resource && flatten)
          {
            list = std::make_unique<flattened_tensor>(this->fl.get_flattened_size<field_index>(2));
            this->tensor_resource_label(*list, std::array<variance, 2>{variance::contravariant, variance::contravariant},
                                        *resource, *flatten, printer);
          }
        else
          {
            list = this->raw_Ginv_resource(printer);
          }

        return list;
      }


    GiNaC::ex
    resources::metric_inverse_resource(const index_literal& a, const index_literal& b, const language_printer& printer) const
      {
        const auto resource = this->mgr.metric_inverse();
        const auto& flatten = this->mgr.field_flatten();

        if(!resource || !flatten) throw resource_failure("metric inverse");

        std::string variable = printer.array_subscript(*resource, a, b, **flatten);
        return this->sym_factory.get_real_symbol(variable);
      }


    std::unique_ptr<flattened_tensor> resources::connexion_resource(const language_printer& printer) const
      {
        std::unique_ptr<flattened_tensor> list;

        const auto resource = this->mgr.connexion();
        const auto& flatten = this->mgr.field_flatten();

        if(resource && flatten)
          {
            list = std::make_unique<flattened_tensor>(this->fl.get_flattened_size<field_index>(3));
            this->tensor_resource_label(*list, std::array<variance, 3>{variance::contravariant, variance::covariant, variance::covariant},
                                        *resource, *flatten, printer);
          }
        else
          {
            ConnexionCache cache(*this, printer);
            list = std::make_unique<flattened_tensor>(cache.get());
          }

        return list;
      }


    GiNaC::ex resources::connexion_resource(const index_literal& a, const index_literal& b, const index_literal& c,
                                            const language_printer& printer) const
      {
        const auto resource = this->mgr.connexion();
        const auto& flatten = this->mgr.field_flatten();

        if(!resource || !flatten) throw resource_failure("connexion");

        std::string variable = printer.array_subscript(*resource, a, b, c, **flatten);
        return this->sym_factory.get_real_symbol(variable);
      }


    void resources::tensor_resource_label(flattened_tensor& list, const std::array<variance, 2> reqd,
                                          const contexted_value<std::string>& resource,
                                          const contexted_value<std::string>& flatten, const language_printer& printer) const
      {
        const field_index max_i = this->share.get_max_field_index(reqd[0]);
        const field_index max_j = this->share.get_max_field_index(reqd[1]);

        for(field_index i = field_index(0, reqd[0]); i < max_i; ++i)
          {
            for(field_index j = field_index(0, reqd[1]); j < max_j; ++j)
              {
                unsigned int index = this->fl.flatten(i,j);
                std::string variable = printer.array_subscript(resource, this->fl.flatten(i), this->fl.flatten(j),
                                                               *flatten);
                list[index] = this->sym_factory.get_real_symbol(variable);
              }
          }
      }


    void resources::tensor_resource_label(flattened_tensor& list, const std::array<variance, 3> reqd,
                                          const contexted_value<std::string>& resource,
                                          const contexted_value<std::string>& flatten, const language_printer& printer) const
      {
        const field_index max_i = this->share.get_max_field_index(reqd[0]);
        const field_index max_j = this->share.get_max_field_index(reqd[1]);
        const field_index max_k = this->share.get_max_field_index(reqd[2]);

        for(field_index i = field_index(0, reqd[0]); i < max_i; ++i)
          {
            for(field_index j = field_index(0, reqd[1]); j < max_j; ++j)
              {
                for(field_index k = field_index(0, reqd[2]); k < max_k; ++k)
                  {
                    unsigned int index = this->fl.flatten(i,j,k);
                    std::string variable = printer.array_subscript(resource, this->fl.flatten(i), this->fl.flatten(j),
                                                                   this->fl.flatten(k), *flatten);
                    list[index] = this->sym_factory.get_real_symbol(variable);
                  }
              }
          }
      }


    cache_tags
    resources::generate_cache_arguments(const language_printer& printer) const
      {
        cache_tags args;
        const auto& flatten = this->mgr.phase_flatten();

        // push all parameter labels onto argument list
        this->push_resource_tag(args, this->mgr.parameters());

        // if a coordinate resource is being used, push its label onto the argument list
        if(flatten) this->push_resource_tag(args, this->mgr.coordinates());

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
        const auto resource = this->mgr.Riemann_B3(vars, false);
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
