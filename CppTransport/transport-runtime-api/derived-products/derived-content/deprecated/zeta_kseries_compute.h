//
// Created by David Seery on 22/06/2014.
// Copyright (c) 2014-15 University of Sussex. All rights reserved.
//


#ifndef __zeta_kseries_compute_H_
#define __zeta_kseries_compute_H_


#include <vector>
#include <memory>


// need data_manager for datapipe
#include "transport-runtime-api/data/data_manager.h"

// need 3pf shifter
#include "transport-runtime-api/derived-products/derived-content/correlation-functions/compute-gadgets/threepf_kconfig_shift.h"


namespace transport
  {

    namespace derived_data
      {

        //! zeta_kseries_compute is a utility class which computes derived lines for the zeta 2- and 3-point functions
        template <typename number>
        class zeta_kseries_compute
          {

          public:

            class handle
              {

                // CONSTRUCTOR, DESTRUCTOR

              public:

                handle(datapipe<number>& pipe, integration_task<number>* tk,
                       const std::vector<unsigned int>& ksample, const std::vector<unsigned int>& tsample,
                       const std::vector<double>& tv, unsigned int Nf);

                ~handle() = default;

                // INTERNAL DATA

              protected:

                //! datapipe object
                datapipe<number>& pipe;

                //! model pointer
                model<number>* mdl;

                //! task pointer
                integration_task<number>* tk;

                //! set of time serial numbers for which we are computing
                const std::vector<unsigned int>& time_sample_sns;

                //! corresponding time values
                const std::vector<double>& time_values;

                //! datapipe handle for this set of time serial numbers
                typename datapipe<number>::time_data_handle& t_handle;

                //! set of kconfig serial numbers for which we are computing
                const std::vector<unsigned int>& kconfig_sample_sns;

                //! datapipe handle for this set of kconfig serial numbers
                typename datapipe<number>::kconfig_data_handle& k_handle;

                //! number of fields
                unsigned int N_fields;

                //! cache background field configuration
                std::vector< std::vector<number> > background;

                friend class zeta_kseries_compute;

              };

            // CONSTRUCTOR, DESTRUCTOR

          public:

            // constructor is default
            zeta_kseries_compute() = default;

            // destructor is default
            ~zeta_kseries_compute() = default;


            // HANDLE

          public:

            //! make a handle
            std::shared_ptr<handle> make_handle(datapipe<number>& pipe, integration_task<number>* tk,
                                                const std::vector<unsigned int>& ksample, const std::vector<unsigned int>& tsample,
                                                const std::vector<double>& tv, unsigned int Nf) const;


            // COMPUTE ZETA PRODUCTS

          public:

            //! compute a time series for the zeta two-point functton
            void twopf(std::shared_ptr<handle>& h, std::vector<number>& line_data, unsigned int tindex) const;

            //! compute a time series for the zeta three-pount function
            void threepf(std::shared_ptr<handle>& h, std::vector<number>& line_data, unsigned int tindex) const;

            //! compute a time series for the zeta reduced bispectrum
            void reduced_bispectrum(std::shared_ptr<handle>& h, std::vector<number>& line_data, unsigned int tindex) const;


            // INTERNAL DATA

          protected:

            threepf_kconfig_shift<number> shifter;

          };


        // HANDLE METHODS


        template <typename number>
        zeta_kseries_compute<number>::handle::handle(datapipe<number>& p, integration_task<number>* t,
                                                     const std::vector<unsigned int>& ksample, const std::vector<unsigned int>& tsample,
                                                     const std::vector<double>& tv, unsigned int Nf)
          : pipe(p),
            tk(t),
            kconfig_sample_sns(ksample),
            time_sample_sns(tsample),
            time_values(tv),
            t_handle(p.new_time_data_handle(tsample)),
            k_handle(p.new_kconfig_data_handle(ksample)),
            N_fields(Nf)
          {
            assert(tk != nullptr);

            mdl = tk->get_model();
            assert(mdl != nullptr);

            // pull the background field configuration for each time sample point
            background.clear();
            background.resize(tsample.size());

            for(unsigned int i = 0; i < 2*N_fields; ++i)
              {
                background_time_data_tag<number> tag = pipe.new_background_time_data_tag(i);
                // safe to take a reference here and avoid a copy
                const std::vector<number>& bg_line = t_handle.lookup_tag(tag);
                assert(bg_line.size() == tsample.size());

                for(unsigned int j = 0; j < tsample.size(); ++j)
                  {
                    background[j].push_back(bg_line[j]);
                  }
              }
          }


        // IMPLEMENTATION


        template <typename number>
        std::shared_ptr<typename zeta_kseries_compute<number>::handle>
        zeta_kseries_compute<number>::make_handle(datapipe<number>& pipe, integration_task<number>* t,
                                                  const std::vector<unsigned int>& ksample, const std::vector<unsigned int>& tsample,
                                                  const std::vector<double>& tv, unsigned int Nf) const
          {
            return std::shared_ptr<handle>(new handle(pipe, t, ksample, tsample, tv, Nf));
          }


        template <typename number>
        void zeta_kseries_compute<number>::twopf(std::shared_ptr<typename zeta_kseries_compute<number>::handle>& h,
                                                 std::vector<number>& line_data, unsigned int tindex) const
          {
            unsigned int N_fields = h->N_fields;

            line_data.clear();
            line_data.assign(h->kconfig_sample_sns.size(), 0.0);

            // cache gauge transformation matrices for this time sample
            std::vector<number> dN;
            h->mdl->compute_gauge_xfm_1(h->tk, h->background[tindex], dN);

//            std::vector<number> small;
//            std::vector<number> large;
//            small.assign(h->kconfig_sample_sns.size(), +std::numeric_limits<double>::max());
//            large.assign(h->kconfig_sample_sns.size(), -std::numeric_limits<double>::max());

            for(unsigned int m = 0; m < 2*N_fields; ++m)
              {
                for(unsigned int n = 0; n < 2*N_fields; ++n)
                  {
                    cf_kconfig_data_tag<number> tag =
                      h->pipe.new_cf_kconfig_data_tag(data_tag<number>::cf_twopf_re, h->mdl->flatten(m,n), h->time_sample_sns[tindex]);

                    const std::vector<number>& sigma_line = h->k_handle.lookup_tag(tag);

                    for(unsigned int j = 0; j < h->kconfig_sample_sns.size(); ++j)
                      {
                        number component = dN[m]*dN[n]*sigma_line[j];

//                        if(fabs(component) > large[j]) large[j] = fabs(component);
//                        if(fabs(component) < small[j]) small[j] = fabs(component);
                        line_data[j] += component;
                      }
                  }
              }

//            number global_small = +std::numeric_limits<double>::max();
//            number global_large = -std::numeric_limits<double>::max();
//            for(unsigned int j = 0; j < h->kconfig_sample_sns.size(); ++j)
//              {
//                number large_fraction = fabs(large[j]/line_data[j]);
//                number small_fraction = fabs(small[j]/line_data[j]);
//
//                if(large_fraction > global_large) global_large = large_fraction;
//                if(small_fraction < global_small) global_small = small_fraction;
//              }

//            std::ostringstream msg;
//            msg << std::setprecision(2) << "-- zeta twopf wavenumber series: serial " << h->time_sample_sns[tindex] << ": smallest intermediate = " << global_small*100.0 << "%, largest intermediate = " << global_large*100.0 << "%";
//            BOOST_LOG_SEV(h->pipe.get_log(), datapipe<number>::normal) << msg.str();
//            std::cout << msg.str() << std::endl;
          }


        template <typename number>
        void zeta_kseries_compute<number>::threepf(std::shared_ptr<typename zeta_kseries_compute<number>::handle>& h,
                                                   std::vector<number>& line_data, unsigned int tindex) const
          {
            unsigned int N_fields = h->N_fields;

            line_data.clear();
            line_data.assign(h->kconfig_sample_sns.size(), 0.0);

            // cache gauge transformation matrices for this time sample
            std::vector<number> dN;
            h->mdl->compute_gauge_xfm_1(h->tk, h->background[tindex], dN);

            // set up cache handle for 3pf configuration data
            typename datapipe<number>::threepf_kconfig_handle& kc_handle = h->pipe.new_threepf_kconfig_handle(h->kconfig_sample_sns);

            // extract k-configuration data
            threepf_kconfig_tag<number> k_tag = h->pipe.new_threepf_kconfig_tag();
            std::vector< threepf_kconfig > configs = kc_handle.lookup_tag(k_tag);

            // zip lists of serial numbers for each of the k1, k2, k3 configurations
            std::vector<unsigned int> k1_serials;
            std::vector<unsigned int> k2_serials;
            std::vector<unsigned int> k3_serials;

            for(typename std::vector< threepf_kconfig >::const_iterator t = configs.begin(); t != configs.end(); ++t)
              {
                k1_serials.push_back((*t).k1_serial);
                k2_serials.push_back((*t).k2_serial);
                k3_serials.push_back((*t).k3_serial);
              }

            // set up cache handles for each of these serial-number lists
            typename datapipe<number>::kconfig_data_handle& k1_handle = h->pipe.new_kconfig_data_handle(k1_serials);
            typename datapipe<number>::kconfig_data_handle& k2_handle = h->pipe.new_kconfig_data_handle(k2_serials);
            typename datapipe<number>::kconfig_data_handle& k3_handle = h->pipe.new_kconfig_data_handle(k3_serials);

            // cache gauge transformation coefficients
            // these have to be recomputed for each k-configuration, because they are time and shape-dependent
            std::vector< std::vector< std::vector<number> > > ddN123(h->kconfig_sample_sns.size());
            std::vector< std::vector< std::vector<number> > > ddN213(h->kconfig_sample_sns.size());
            std::vector< std::vector< std::vector<number> > > ddN312(h->kconfig_sample_sns.size());

            for(unsigned int j = 0; j < h->kconfig_sample_sns.size(); ++j)
              {
                h->mdl->compute_gauge_xfm_2(h->tk, h->background[tindex], configs[j].k1_comoving, configs[j].k2_comoving, configs[j].k3_comoving, h->time_values[tindex], ddN123[j]);
                h->mdl->compute_gauge_xfm_2(h->tk, h->background[tindex], configs[j].k2_comoving, configs[j].k1_comoving, configs[j].k3_comoving, h->time_values[tindex], ddN213[j]);
                h->mdl->compute_gauge_xfm_2(h->tk, h->background[tindex], configs[j].k3_comoving, configs[j].k1_comoving, configs[j].k2_comoving, h->time_values[tindex], ddN312[j]);
              }

//            std::vector<number> small;
//            std::vector<number> large;
//            small.assign(h->kconfig_sample_sns.size(), +std::numeric_limits<double>::max());
//            large.assign(h->kconfig_sample_sns.size(), -std::numeric_limits<double>::max());

            // linear part of the gauge transformation
            for(unsigned int l = 0; l < 2*N_fields; ++l)
              {
                for(unsigned int m = 0; m < 2*N_fields; ++m)
                  {
                    for(unsigned int n = 0; n < 2*N_fields; ++n)
                      {
                        cf_kconfig_data_tag<number> tag = h->pipe.new_cf_kconfig_data_tag(data_tag<number>::cf_threepf, h->mdl->flatten(l,m,n), h->time_sample_sns[tindex]);

                        std::vector<number> threepf_line = h->k_handle.lookup_tag(tag);

                        // shift field so it represents a derivative correlation function, not a momentum one
                        this->shifter.shift(h->tk, h->mdl, h->pipe, h->background[tindex], configs, threepf_line, l, m, n, h->time_sample_sns[tindex], h->time_values[tindex]);

                        for(unsigned int j = 0; j < h->kconfig_sample_sns.size(); ++j)
                          {
                            number component = dN[l]*dN[m]*dN[n]*threepf_line[j];

//                            if(fabs(component) > large[j]) large[j] = fabs(component);
//                            if(fabs(component) < small[j]) small[j] = fabs(component);
                            line_data[j] += component;
                          }
                      }
                  }
              }

            // quadratic component of the gauge transformation
            for(unsigned int l = 0; l < 2*N_fields; ++l)
              {
                for(unsigned int m = 0; m < 2*N_fields; ++m)
                  {
                    for(unsigned int p = 0; p < 2*N_fields; ++p)
                      {
                        for(unsigned int q = 0; q < 2*N_fields; ++q)
                          {
                            // the indices are N_lm, N_p, N_q so the 2pfs we sum over are
                            // sigma_lp(k2)*sigma_mq(k3) etc.

                            cf_kconfig_data_tag<number> k1_re_lp_tag = h->pipe.new_cf_kconfig_data_tag(data_tag<number>::cf_twopf_re, h->mdl->flatten(l,p), h->time_sample_sns[tindex]);
                            cf_kconfig_data_tag<number> k1_im_lp_tag = h->pipe.new_cf_kconfig_data_tag(data_tag<number>::cf_twopf_im, h->mdl->flatten(l,p), h->time_sample_sns[tindex]);

                            cf_kconfig_data_tag<number> k2_re_lp_tag = h->pipe.new_cf_kconfig_data_tag(data_tag<number>::cf_twopf_re, h->mdl->flatten(l,p), h->time_sample_sns[tindex]);
                            cf_kconfig_data_tag<number> k2_im_lp_tag = h->pipe.new_cf_kconfig_data_tag(data_tag<number>::cf_twopf_im, h->mdl->flatten(l,p), h->time_sample_sns[tindex]);

                            cf_kconfig_data_tag<number> k2_re_mq_tag = h->pipe.new_cf_kconfig_data_tag(data_tag<number>::cf_twopf_re, h->mdl->flatten(m,q), h->time_sample_sns[tindex]);
                            cf_kconfig_data_tag<number> k2_im_mq_tag = h->pipe.new_cf_kconfig_data_tag(data_tag<number>::cf_twopf_im, h->mdl->flatten(m,q), h->time_sample_sns[tindex]);

                            cf_kconfig_data_tag<number> k3_re_mq_tag = h->pipe.new_cf_kconfig_data_tag(data_tag<number>::cf_twopf_re, h->mdl->flatten(m,q), h->time_sample_sns[tindex]);
                            cf_kconfig_data_tag<number> k3_im_mq_tag = h->pipe.new_cf_kconfig_data_tag(data_tag<number>::cf_twopf_im, h->mdl->flatten(m,q), h->time_sample_sns[tindex]);

                            const std::vector<number> k1_re_lp = k1_handle.lookup_tag(k1_re_lp_tag);
                            const std::vector<number> k1_im_lp = k1_handle.lookup_tag(k1_im_lp_tag);
                            const std::vector<number> k2_re_lp = k2_handle.lookup_tag(k2_re_lp_tag);
                            const std::vector<number> k2_im_lp = k2_handle.lookup_tag(k2_im_lp_tag);
                            const std::vector<number> k2_re_mq = k2_handle.lookup_tag(k2_re_mq_tag);
                            const std::vector<number> k2_im_mq = k2_handle.lookup_tag(k2_im_mq_tag);
                            const std::vector<number> k3_re_mq = k3_handle.lookup_tag(k3_re_mq_tag);
                            const std::vector<number> k3_im_mq = k3_handle.lookup_tag(k3_im_mq_tag);

                            for(unsigned int j = 0; j < h->kconfig_sample_sns.size(); ++j)
                              {
                                number component1 = ddN123[j][l][m] * dN[p] * dN[q] * (k2_re_lp[j]*k3_re_mq[j] - k2_im_lp[j]*k3_im_mq[j]);
                                number component2 = ddN213[j][l][m] * dN[p] * dN[q] * (k1_re_lp[j]*k3_re_mq[j] - k1_im_lp[j]*k3_im_mq[j]);
                                number component3 = ddN312[j][l][m] * dN[p] * dN[q] * (k1_re_lp[j]*k2_re_mq[j] - k1_im_lp[j]*k2_im_mq[j]);

//                                if(fabs(component1) > large[j]) large[j] = fabs(component1);
//                                if(fabs(component1) < small[j]) small[j] = fabs(component1);
//                                if(fabs(component2) > large[j]) large[j] = fabs(component2);
//                                if(fabs(component2) < small[j]) small[j] = fabs(component2);
//                                if(fabs(component3) > large[j]) large[j] = fabs(component3);
//                                if(fabs(component3) < small[j]) small[j] = fabs(component3);

                                line_data[j] += component1;
                                line_data[j] += component2;
                                line_data[j] += component3;
                              }
                          }
                      }
                  }
              }

//            number global_small = +std::numeric_limits<double>::max();
//            number global_large = -std::numeric_limits<double>::max();
//            for(unsigned int j = 0; j < h->kconfig_sample_sns.size(); ++j)
//              {
//                number large_fraction = fabs(large[j]/line_data[j]);
//                number small_fraction = fabs(small[j]/line_data[j]);
//
//                if(large_fraction > global_large) global_large = large_fraction;
//                if(small_fraction < global_small) global_small = small_fraction;
//              }

//            std::ostringstream msg;
//            msg << std::setprecision(2) << "-- zeta threepf wavenumber series: serial " << h->time_sample_sns[tindex] << ": smallest intermediate = " << global_small*100.0 << "%, largest intermediate = " << global_large*100.0 << "%";
//            BOOST_LOG_SEV(h->pipe.get_log(), datapipe<number>::normal) << msg.str();
//            std::cout << msg.str() << std::endl;
          }


        template <typename number>
        void zeta_kseries_compute<number>::reduced_bispectrum(std::shared_ptr<typename zeta_kseries_compute<number>::handle>& h,
                                                              std::vector<number>& line_data, unsigned int tindex) const
          {
            line_data.clear();
            line_data.assign(h->kconfig_sample_sns.size(), 0.0);

            // First, obtain the bispectrum
            std::vector<number> threepf_line;
            this->threepf(h, threepf_line, tindex);

            // Second, compute the three copies of the spectra which we need.
            // TODO: this isn't as efficient as the timeseries implementation. Maybe worth revisiting.

            // set up cache handle for 3pf configuration data
            typename datapipe<number>::threepf_kconfig_handle& kc_handle = h->pipe.new_threepf_kconfig_handle(h->kconfig_sample_sns);

            // extract k-configuration data
            threepf_kconfig_tag<number> k_tag = h->pipe.new_threepf_kconfig_tag();
            // safe to take a reference here and avoid a copy
            const std::vector< threepf_kconfig >& configs = kc_handle.lookup_tag(k_tag);

            // zip lists of serial numbers for each of the k1, k2, k3 configurations
            std::vector<unsigned int> k1_serials;
            std::vector<unsigned int> k2_serials;
            std::vector<unsigned int> k3_serials;

            for(typename std::vector< threepf_kconfig >::const_iterator t = configs.begin(); t != configs.end(); ++t)
              {
                k1_serials.push_back((*t).k1_serial);
                k2_serials.push_back((*t).k2_serial);
                k3_serials.push_back((*t).k3_serial);
              }

            // TODO: could consider pulling only the time sample we need, but that will inevitably mean going out to the database when reading the background. This way we use the cache...
            std::shared_ptr<handle> k1_handle = this->make_handle(h->pipe, h->tk, k1_serials, h->time_sample_sns, h->time_values, h->N_fields);
            std::shared_ptr<handle> k2_handle = this->make_handle(h->pipe, h->tk, k2_serials, h->time_sample_sns, h->time_values, h->N_fields);
            std::shared_ptr<handle> k3_handle = this->make_handle(h->pipe, h->tk, k3_serials, h->time_sample_sns, h->time_values, h->N_fields);

            std::vector<number> twopf_k1;
            std::vector<number> twopf_k2;
            std::vector<number> twopf_k3;

            this->twopf(k1_handle, twopf_k1, tindex);
            this->twopf(k2_handle, twopf_k2, tindex);
            this->twopf(k3_handle, twopf_k3, tindex);

            // Third, construct the reduced bispectrum
            for(unsigned int j = 0; j < h->kconfig_sample_sns.size(); ++j)
              {
                number form_factor = (6.0/5.0) * ( twopf_k1[j]*twopf_k2[j] + twopf_k1[j]*twopf_k3[j] + twopf_k2[j]*twopf_k3[j] );

                line_data[j] = threepf_line[j] / form_factor;
              }
          }


      }   // namespace derived_data

  }   // namespace transport


#endif //__zeta_kseries_compute_H_
