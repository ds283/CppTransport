//
// Created by David Seery on 19/12/2015.
// Copyright (c) 2015 University of Sussex. All rights reserved.
//


#include "resources.h"


namespace canonical
  {

    resources::resources(translator_data& p, resource_manager& m, expression_cache& c, shared_resources& s, boost::timer::cpu_timer& t)
      : mgr(m),
        cache(c),
        payload(p),
        share(s),
        compute_timer(t),
        sym_factory(p.get_symbol_factory()),
        V(p.get_potential()),
        field_list(p.get_field_symbols()),
        deriv_list(p.get_deriv_symbols()),
        param_list(p.get_parameter_symbols()),
        num_params(p.get_number_parameters()),
        num_fields(p.get_number_fields()),
        fl(p.get_number_parameters(), p.get_number_fields())
      {
        compute_timer.stop();
      }


    std::unique_ptr<ginac_cache_args> resources::generate_arguments(language_printer& printer) const
      {
        std::unique_ptr<ginac_cache_args> args = std::make_unique<ginac_cache_args>();

        // query resource manager for parameter and coordinate labels
        const boost::optional< contexted_value<std::string> >& param_resource = this->mgr.parameters();
        const boost::optional< contexted_value<std::string> >& coord_resource = this->mgr.coordinates();
        const boost::optional< contexted_value<std::string> >& flatten = this->mgr.phase_flatten();

        if(param_resource)
          {
            GiNaC::symbol sym = this->sym_factory.get_symbol(*param_resource);
            args->push_back(sym);
          }

        if(coord_resource && flatten)
          {
            GiNaC::symbol sym = this->sym_factory.get_symbol(*coord_resource);
            args->push_back(sym);
          }

        return(args);
      }


    GiNaC::ex resources::V_resource(language_printer& printer)
      {
        std::unique_ptr<ginac_cache_args> args = this->generate_arguments(printer);

        // if no substitutions, then nothing to do, so exit immediately
        if(args->size() == 0) return(this->V);

        GiNaC::ex subs_V;

        // check whether this set of substitutions has already been done; if so, there
        // is no need to do any work
        if(!this->cache.query(expression_item_types::V_item, 0, *args, subs_V))
          {
            timing_instrument timer(this->compute_timer);

            // we didn't find an expression for V with this set of substitutions in the cache, so
            // query resource manager for parameter and coordinate labels, then build a substitution map
            const boost::optional< contexted_value<std::string> >& param_resource = this->mgr.parameters();
            const boost::optional< contexted_value<std::string> >& coord_resource = this->mgr.coordinates();
            const boost::optional< contexted_value<std::string> >& flatten = this->mgr.phase_flatten();

            // build substitution map
            GiNaC::exmap subs_map;

            if(param_resource)
              {
                std::unique_ptr<symbol_list> p_list = this->share.generate_parameters(printer);

                // copy parameter symbols into substitution list
                for(param_index i = param_index(0); i < this->num_params; ++i)
                  {
                    subs_map[this->param_list[this->fl.flatten(i)]] = (*p_list)[this->fl.flatten(i)];
                  }
              }

            if(coord_resource && flatten)
              {
                std::unique_ptr<symbol_list> f_list = this->share.generate_fields(printer);

                // copy field-label symbols into substitution list
                for(field_index i = field_index(0); i < this->num_fields; ++i)
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


    GiNaC::ex resources::eps_resource(language_printer& printer)
      {
        std::unique_ptr<ginac_cache_args> args = this->generate_arguments(printer);

        GiNaC::ex eps;

        if(!this->cache.query(expression_item_types::epsilon_item, 0, *args, eps))
          {
            timing_instrument timer(this->compute_timer);

            std::unique_ptr<symbol_list> derivs = this->share.generate_derivs(printer);
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


    GiNaC::ex resources::Hsq_resource(language_printer& printer)
      {
        std::unique_ptr<ginac_cache_args> args = this->generate_arguments(printer);

        GiNaC::ex Hsq;

        if(!this->cache.query(expression_item_types::Hubble2_item, 0, *args, Hsq))
          {
            timing_instrument timer(this->compute_timer);

            GiNaC::ex V = this->V_resource(printer);
            GiNaC::ex eps = this->eps_resource(printer);
            GiNaC::symbol Mp = this->share.generate_Mp();

            Hsq = V / ((3-eps)*Mp*Mp);
            this->cache.store(expression_item_types::Hubble2_item, 0, *args, Hsq);
          }

        return(Hsq);
      }


    std::unique_ptr<flattened_tensor> resources::dV_resource(language_printer& printer)
      {
        std::unique_ptr<flattened_tensor> list = std::make_unique<flattened_tensor>(this->fl.get_flattened_size<field_index>(1));

        const boost::optional< contexted_value<std::string> >& resource = this->mgr.dV();
        const boost::optional< contexted_value<std::string> >& flatten = this->mgr.field_flatten();

        if(resource && flatten)     // dV is available
          {
            for(field_index i = field_index(0); i < this->num_fields; ++i)
              {
                unsigned int index = this->fl.flatten(i);

                std::string variable = printer.array_subscript(*resource, *flatten, this->fl.flatten(i));

                (*list)[index] = this->sym_factory.get_symbol(variable);
              }
          }
        else                        // have to construct dV ourselves
          {
            // build argument list
            std::unique_ptr<ginac_cache_args> args = this->generate_arguments(printer);

            // don't generate other objects unless we need them
            GiNaC::ex subs_V;
            std::unique_ptr<symbol_list> f_list;
            bool cached = false;

            for(field_index i = field_index(0); i < this->num_fields; ++i)
              {
                GiNaC::ex dV;
                unsigned int index = this->fl.flatten(i);

                if(!this->cache.query(expression_item_types::dV_item, index, *args, dV))
                  {
                    timing_instrument timer(this->compute_timer);

                    // generate necessary items if they have not been done
                    if(!cached)
                      {
                        subs_V = this->V_resource(printer);
                        f_list = this->share.generate_fields(printer);
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


    std::unique_ptr<flattened_tensor> resources::ddV_resource(language_printer& printer)
      {
        std::unique_ptr<flattened_tensor> list = std::make_unique<flattened_tensor>(this->fl.get_flattened_size<field_index>(2));

        const boost::optional< contexted_value<std::string> >& resource = this->mgr.ddV();
        const boost::optional< contexted_value<std::string> >& flatten = this->mgr.field_flatten();

        if(resource && flatten)     // ddV is available
          {
            for(field_index i = field_index(0); i < this->num_fields; ++i)
              {
                for(field_index j = field_index(0); j < this->num_fields; ++j)
                  {
                    unsigned int index = this->fl.flatten(i,j);

                    std::string variable = printer.array_subscript(*resource, *flatten, this->fl.flatten(i), this->fl.flatten(j));

                    (*list)[index] = this->sym_factory.get_symbol(variable);
                  }
              }
          }
        else                        // have to construct ddV ourselves
          {
            // build argument list
            std::unique_ptr<ginac_cache_args> args = this->generate_arguments(printer);

            // don't generate other objects unless we need them
            GiNaC::ex subs_V;
            std::unique_ptr<symbol_list> f_list;
            bool cached = false;

            for(field_index i = field_index(0); i < this->num_fields; ++i)
              {
                for(field_index j = field_index(0); j < this->num_fields; ++j)
                  {
                    GiNaC::ex ddV;
                    unsigned int index = this->fl.flatten(i, j);

                    if(!this->cache.query(expression_item_types::ddV_item, index, *args, ddV))
                      {
                        timing_instrument timer(this->compute_timer);

                        // generate necessary items if they have not been done
                        if(!cached)
                          {
                            subs_V = this->V_resource(printer);
                            f_list = this->share.generate_fields(printer);
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


    std::unique_ptr<flattened_tensor> resources::dddV_resource(language_printer& printer)
      {
        std::unique_ptr<flattened_tensor> list = std::make_unique<flattened_tensor>(this->fl.get_flattened_size<field_index>(3));

        const boost::optional< contexted_value<std::string> >& resource = this->mgr.dddV();
        const boost::optional< contexted_value<std::string> >& flatten = this->mgr.field_flatten();

        if(resource && flatten)     // dddV is available
          {
            for(field_index i = field_index(0); i < this->num_fields; ++i)
              {
                for(field_index j = field_index(0); j < this->num_fields; ++j)
                  {
                    for(field_index k = field_index(0); k < this->num_fields; ++k)
                      {
                        unsigned int index = this->fl.flatten(i,j,k);

                        std::string variable = printer.array_subscript(*resource, *flatten, this->fl.flatten(i), this->fl.flatten(j), this->fl.flatten(k));

                        (*list)[index] = this->sym_factory.get_symbol(variable);
                      }
                  }
              }
          }
        else                        // have to construct dddV ourselves
          {
            // build argument list
            std::unique_ptr<ginac_cache_args> args = this->generate_arguments(printer);

            // don't generate other objects unless we need them
            GiNaC::ex subs_V;
            std::unique_ptr<symbol_list> f_list;
            bool cached = false;

            for(field_index i = field_index(0); i < this->num_fields; ++i)
              {
                for(field_index j = field_index(0); j < this->num_fields; ++j)
                  {
                    for(field_index k = field_index(0); k < this->num_fields; ++k)
                      {
                        GiNaC::ex dddV;
                        unsigned int index = this->fl.flatten(i, j, k);

                        if(!this->cache.query(expression_item_types::dddV_item, index, *args, dddV))
                          {
                            timing_instrument timer(this->compute_timer);

                            // generate necessary items if they have not been done
                            if(!cached)
                              {
                                subs_V = this->V_resource(printer);
                                f_list = this->share.generate_fields(printer);
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


    std::unique_ptr<ginac_cache_args> resources::generate_arguments(unsigned int flags, language_printer& printer) const
      {
        // first, generate arguments from param/coordinates if they exist
        std::unique_ptr<ginac_cache_args> args = this->generate_arguments(printer);
        const boost::optional< contexted_value<std::string> >& flatten = this->mgr.field_flatten();

        if(flatten && (flags & use_dV_argument))
          {
            const boost::optional< contexted_value<std::string> >& dV_resource = this->mgr.dV();
            if(dV_resource)   // no need to push arguments if no resource available
              {
                GiNaC::symbol sym = this->sym_factory.get_symbol(*dV_resource);
                args->push_back(sym);
              }
          }

        if(flatten && (flags & use_ddV_argument))
          {
            const boost::optional< contexted_value<std::string> >& ddV_resource = this->mgr.ddV();
            if(ddV_resource)
              {
                GiNaC::symbol sym = this->sym_factory.get_symbol(*ddV_resource);
                args->push_back(sym);
              }
          }

        if(flatten && (flags & use_dddV_argument))
          {
            const boost::optional< contexted_value<std::string> >& dddV_resource = this->mgr.dddV();
            if(dddV_resource)
              {
                GiNaC::symbol sym = this->sym_factory.get_symbol(*dddV_resource);
                args->push_back(sym);
              }
          }

        return(args);
      }

  }   // namespace canonical

