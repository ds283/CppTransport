//
// Created by David Seery on 22/06/2014.
// Copyright (c) 2014 University of Sussex. All rights reserved.
//


#ifndef __zeta_timeseries_compute_H_
#define __zeta_timeseries_compute_H_


#include <vector>
#include <memory>


// need data_manager for datapipe
#include "transport-runtime-api/manager/data_manager.h"

// need 3pf shifter
#include "transport-runtime-api/derived-products/derived-content/threepf_time_shift.h"


namespace transport
  {

    namespace derived_data
      {

        //! zeta_timeseries_compute is a utility class which computes derived lines for the zeta 2- and 3-point functions
        template <typename number>
        class zeta_timeseries_compute
          {

          public:

            class handle
              {

                // CONSTRUCTOR, DESTRUCTOR

              public:

                handle(typename data_manager<number>::datapipe& pipe, integration_task<number>* tk,
                       const std::vector<unsigned int>& tsample, const std::vector<double>& taxis, unsigned int Nf);

                ~handle() = default;

                // INTERNAL DATA

              protected:

                //! datapipe object
                typename data_manager<number>::datapipe& pipe;

                //! model pointer
                model<number>* mdl;

                //! task pointer
                integration_task<number>* tk;

                //! set of time serial numbers for which we are computing
                const std::vector<unsigned int>& time_sample_sns;

                //! cache of time axis
                const std::vector<double>& time_axis;

                //! datapipe handle for this set of serial numbers
                typename data_manager<number>::datapipe::time_data_handle& t_handle;

                //! number of fields
                unsigned int N_fields;

                //! cache background evolution
                std::vector< std::vector<number> > background;

                //! cached gauge transformation coefficients
                std::vector< std::vector<number> > dN;

                friend class zeta_timeseries_compute;

              };

            // CONSTRUCTOR, DESTRUCTOR

          public:

            // constructor is default
            zeta_timeseries_compute() = default;

            // destructor is default
            ~zeta_timeseries_compute() = default;


            // HANDLE

          public:

            //! make a handle
            std::shared_ptr<handle> make_handle(typename data_manager<number>::datapipe& pipe, integration_task<number>* tk,
                                                const std::vector<unsigned int>& tsample, const std::vector<double>& taxis, unsigned int Nf) const;


            // COMPUTE ZETA PRODUCTS

          public:

            //! compute a time series for the zeta two-point function
            void twopf(std::shared_ptr<handle>& h, std::vector<number>& line_data, const typename data_manager<number>::twopf_configuration& k) const;

            //! compute a time series for the zeta three-point function
            void threepf(std::shared_ptr<handle>& h, std::vector<number>& line_data, const typename data_manager<number>::threepf_configuration& k) const;

            //! compute a time series for the zeta reduced bispectrum
            void reduced_bispectrum(std::shared_ptr<handle>& h, std::vector<number>& line_data, const typename data_manager<number>::threepf_configuration& k) const;


            // INTERNAL DATA

          protected:

            threepf_time_shift<number> shifter;

          };


        // HANDLE METHODS


        template <typename number>
        zeta_timeseries_compute<number>::handle::handle(typename data_manager<number>::datapipe& p, integration_task<number>* t,
                                                        const std::vector<unsigned int>& tsample, const std::vector<double>& taxis, unsigned int Nf)
          : pipe(p),
            tk(t),
            time_sample_sns(tsample),
            time_axis(taxis),
            t_handle(p.new_time_data_handle(tsample)),
            N_fields(Nf)
          {
            assert(tk != nullptr);

            mdl = tk->get_model();
            assert(mdl != nullptr);

            // pull background data for the time_sample we are using,
            // and slice it up by time in an array 'background'
            background.clear();
            background.resize(tsample.size());

            for(unsigned int i = 0; i < 2*N_fields; i++)
              {
                typename data_manager<number>::datapipe::background_time_data_tag tag = pipe.new_background_time_data_tag(i);

                // safe to take a reference here and avoid a copy
                const std::vector<number>& bg_line = t_handle.lookup_tag(tag);

                assert(bg_line.size() == background.size());
                for(unsigned int j = 0; j < tsample.size(); j++)
                  {
                    background[j].push_back(bg_line[j]);
                  }
              }

            // cache gauge transformation coefficients
            dN.resize(tsample.size());
            for(unsigned int j = 0; j < tsample.size(); j++)
              {
                mdl->compute_gauge_xfm_1(tk->get_params(), background[j], dN[j]);
//                mdl->compute_deltaN_xfm_1(tk->get_params(), background[j], dN[j]);
              }
          }


        // IMPLEMENTATION


        template <typename number>
        std::shared_ptr<typename zeta_timeseries_compute<number>::handle>
        zeta_timeseries_compute<number>::make_handle(typename data_manager<number>::datapipe& pipe, integration_task<number>* t,
                                                     const std::vector<unsigned int>& tsample, const std::vector<double>& taxis, unsigned int Nf) const
          {
            return std::shared_ptr<handle>(new handle(pipe, t, tsample, taxis, Nf));
          }


        template <typename number>
        void zeta_timeseries_compute<number>::twopf(std::shared_ptr<typename zeta_timeseries_compute<number>::handle>& h,
                                                    std::vector<number>& line_data, const typename data_manager<number>::twopf_configuration& k) const
          {
            unsigned int N_fields = h->N_fields;

            line_data.clear();
            line_data.assign(h->time_sample_sns.size(), 0.0);

            std::vector<number> small;
            std::vector<number> large;
            small.assign(h->time_sample_sns.size(), +DBL_MAX);
            large.assign(h->time_sample_sns.size(), -DBL_MAX);

            for(unsigned int m = 0; m < 2*N_fields; m++)
              {
                for(unsigned int n = 0; n < 2*N_fields; n++)
                  {
                    typename data_manager<number>::datapipe::cf_time_data_tag tag =
                      h->pipe.new_cf_time_data_tag(data_manager<number>::datapipe::cf_twopf_re, h->mdl->flatten(m,n), k.serial);

                    // pull twopf data for this component
                    const std::vector<number>& sigma_line = h->t_handle.lookup_tag(tag);

                    for(unsigned int j = 0; j < h->time_sample_sns.size(); j++)
                      {
                        number component = h->dN[j][m]*h->dN[j][n]*sigma_line[j];

                        if(fabs(component) > large[j]) large[j] = fabs(component);
                        if(fabs(component) < small[j]) small[j] = fabs(component);
                        line_data[j] += component;
                      }
                  }
              }

            number global_small = +DBL_MAX;
            number global_large = -DBL_MAX;
            for(unsigned int j = 0; j < h->time_sample_sns.size(); j++)
              {
                number large_fraction = fabs(large[j]/line_data[j]);
                number small_fraction = fabs(small[j]/line_data[j]);

                if(large_fraction > global_large) global_large = large_fraction;
                if(small_fraction < global_small) global_small = small_fraction;
              }

            std::ostringstream msg;
            msg << std::setprecision(2) << "-- zeta twopf time series: serial " << k.serial << ": smallest intermediate = " << global_small*100.0 << "%, largest intermediate = " << global_large*100.0 << "%";
            BOOST_LOG_SEV(h->pipe.get_log(), data_manager<number>::normal) << msg.str();
//            std::cout << msg.str() << std::endl;
          }


        template <typename number>
        void zeta_timeseries_compute<number>::threepf(std::shared_ptr<typename zeta_timeseries_compute<number>::handle>& h,
                                                      std::vector<number>& line_data, const typename data_manager<number>::threepf_configuration& k) const
          {
            unsigned int N_fields = h->N_fields;

            line_data.clear();
            line_data.assign(h->time_sample_sns.size(), 0.0);

            // cache gauge transformation coefficients
            // these have to be recomputed for each k-configuration, because they are time and shape-dependent
            std::vector< std::vector< std::vector<number> > > ddN123(h->time_sample_sns.size());
            std::vector< std::vector< std::vector<number> > > ddN213(h->time_sample_sns.size());
            std::vector< std::vector< std::vector<number> > > ddN312(h->time_sample_sns.size());
            for(unsigned int j = 0; j < h->time_sample_sns.size(); j++)
              {
              h->mdl->compute_gauge_xfm_2(h->tk->get_params(), h->background[j], k.k1_comoving, k.k2_comoving, k.k3_comoving, h->time_axis[j], ddN123[j]);
              h->mdl->compute_gauge_xfm_2(h->tk->get_params(), h->background[j], k.k2_comoving, k.k1_comoving, k.k3_comoving, h->time_axis[j], ddN213[j]);
              h->mdl->compute_gauge_xfm_2(h->tk->get_params(), h->background[j], k.k3_comoving, k.k1_comoving, k.k2_comoving, h->time_axis[j], ddN312[j]);
//                h->mdl->compute_deltaN_xfm_2(h->tk->get_params(), h->background[j], ddN123[j]);
//                h->mdl->compute_deltaN_xfm_2(h->tk->get_params(), h->background[j], ddN213[j]);
//                h->mdl->compute_deltaN_xfm_2(h->tk->get_params(), h->background[j], ddN312[j]);
              }

            std::vector<number> small;
            std::vector<number> large;
            small.assign(h->time_sample_sns.size(), +DBL_MAX);
            large.assign(h->time_sample_sns.size(), -DBL_MAX);

            // linear component of the gauge transformation
            for(unsigned int l = 0; l < 2*N_fields; l++)
              {
                for(unsigned int m = 0; m < 2*N_fields; m++)
                  {
                    for(unsigned int n = 0; n < 2*N_fields; n++)
                      {
                        // pull threepf data for this component
                        typename data_manager<number>::datapipe::cf_time_data_tag tag = h->pipe.new_cf_time_data_tag(data_manager<number>::datapipe::cf_threepf, h->mdl->flatten(l,m,n), k.serial);

                        // have to take a copy of the data line, rather than use a reference, because shifting the derivative will modify it in place
                        std::vector<number> threepf_line = h->t_handle.lookup_tag(tag);

                        // shift field so it represents a derivative correlation function, not a momentum one
                        this->shifter.shift(h->tk, h->mdl, h->pipe, h->time_sample_sns, threepf_line, h->time_axis, l, m, n, k);

                        for(unsigned int j = 0; j < h->time_sample_sns.size(); j++)
                          {
                            number component = h->dN[j][l]*h->dN[j][m]*h->dN[j][n]*threepf_line[j];

                            if(fabs(component) > large[j])  large[j]  = fabs(component);
                            if(fabs(component) < small[j]) small[j] = fabs(component);
                            line_data[j] += component;
                          }
                      }
                  }
              }

            // quadratic component of the gauge transformation
            for(unsigned int l = 0; l < 2*N_fields; l++)
              {
                for(unsigned int m = 0; m < 2*N_fields; m++)
                  {
                    for(unsigned int p = 0; p < 2*N_fields; p++)
                      {
                        for(unsigned int q = 0; q < 2*N_fields; q++)
                          {
                            // the indices are N_lm, N_p, N_q, so the 2pfs we sum over are
                            // sigma_lp(k2)*sigma_mq(k3) etc.

                            typename data_manager<number>::datapipe::cf_time_data_tag k1_re_lp_tag = h->pipe.new_cf_time_data_tag(data_manager<number>::datapipe::cf_twopf_re, h->mdl->flatten(l,p), k.k1_serial);
                            typename data_manager<number>::datapipe::cf_time_data_tag k1_im_lp_tag = h->pipe.new_cf_time_data_tag(data_manager<number>::datapipe::cf_twopf_im, h->mdl->flatten(l,p), k.k1_serial);

                            typename data_manager<number>::datapipe::cf_time_data_tag k2_re_lp_tag = h->pipe.new_cf_time_data_tag(data_manager<number>::datapipe::cf_twopf_re, h->mdl->flatten(l,p), k.k2_serial);
                            typename data_manager<number>::datapipe::cf_time_data_tag k2_im_lp_tag = h->pipe.new_cf_time_data_tag(data_manager<number>::datapipe::cf_twopf_im, h->mdl->flatten(l,p), k.k2_serial);

                            typename data_manager<number>::datapipe::cf_time_data_tag k2_re_mq_tag = h->pipe.new_cf_time_data_tag(data_manager<number>::datapipe::cf_twopf_re, h->mdl->flatten(m,q), k.k2_serial);
                            typename data_manager<number>::datapipe::cf_time_data_tag k2_im_mq_tag = h->pipe.new_cf_time_data_tag(data_manager<number>::datapipe::cf_twopf_im, h->mdl->flatten(m,q), k.k2_serial);

                            typename data_manager<number>::datapipe::cf_time_data_tag k3_re_mq_tag = h->pipe.new_cf_time_data_tag(data_manager<number>::datapipe::cf_twopf_re, h->mdl->flatten(m,q), k.k3_serial);
                            typename data_manager<number>::datapipe::cf_time_data_tag k3_im_mq_tag = h->pipe.new_cf_time_data_tag(data_manager<number>::datapipe::cf_twopf_im, h->mdl->flatten(m,q), k.k3_serial);

                            const std::vector<number>& k1_re_lp = h->t_handle.lookup_tag(k1_re_lp_tag);
                            const std::vector<number>& k1_im_lp = h->t_handle.lookup_tag(k1_im_lp_tag);
                            const std::vector<number>& k2_re_lp = h->t_handle.lookup_tag(k2_re_lp_tag);
                            const std::vector<number>& k2_im_lp = h->t_handle.lookup_tag(k2_im_lp_tag);
                            const std::vector<number>& k2_re_mq = h->t_handle.lookup_tag(k2_re_mq_tag);
                            const std::vector<number>& k2_im_mq = h->t_handle.lookup_tag(k2_im_mq_tag);
                            const std::vector<number>& k3_re_mq = h->t_handle.lookup_tag(k3_re_mq_tag);
                            const std::vector<number>& k3_im_mq = h->t_handle.lookup_tag(k3_im_mq_tag);

                            for(unsigned int j = 0; j < h->time_sample_sns.size(); j++)
                              {
                                number component1 = ddN123[j][l][m] * h->dN[j][p] * h->dN[j][q] * (k2_re_lp[j]*k3_re_mq[j] - k2_im_lp[j]*k3_im_mq[j]);
                                number component2 = ddN213[j][l][m] * h->dN[j][p] * h->dN[j][q] * (k1_re_lp[j]*k3_re_mq[j] - k1_im_lp[j]*k3_im_mq[j]);
                                number component3 = ddN312[j][l][m] * h->dN[j][p] * h->dN[j][q] * (k1_re_lp[j]*k2_re_mq[j] - k1_im_lp[j]*k2_im_mq[j]);

                                if(fabs(component1) > large[j]) large[j] = fabs(component1);
                                if(fabs(component1) < small[j]) small[j] = fabs(component1);
                                if(fabs(component2) > large[j]) large[j] = fabs(component2);
                                if(fabs(component2) < small[j]) small[j] = fabs(component2);
                                if(fabs(component3) > large[j]) large[j] = fabs(component3);
                                if(fabs(component3) < small[j]) small[j] = fabs(component3);

                                line_data[j] += component1;
                                line_data[j] += component2;
                                line_data[j] += component3;
                              }
                          }
                      }
                  }
              }

            number global_small = +DBL_MAX;
            number global_large = -DBL_MAX;
            for(unsigned int j = 0; j < h->time_sample_sns.size(); j++)
              {
                number large_fraction = fabs(large[j]/line_data[j]);
                number small_fraction = fabs(small[j]/line_data[j]);

                if(large_fraction > global_large) global_large = large_fraction;
                if(small_fraction < global_small) global_small = small_fraction;
              }

            std::ostringstream msg;
            msg << std::setprecision(2) << "-- zeta threepf time series: serial " << k.serial << ": smallest intermediate = " << global_small*100.0 << "%, largest intermediate = " << global_large*100.0 << "%";
            BOOST_LOG_SEV(h->pipe.get_log(), data_manager<number>::normal) << msg.str();
//            std::cout << msg.str() << std::endl;
          }


        template <typename number>
        void zeta_timeseries_compute<number>::reduced_bispectrum(std::shared_ptr<typename zeta_timeseries_compute::handle>& h,
                                                                 std::vector<number>& line_data, const typename data_manager<number>::threepf_configuration& k) const
          {
            line_data.clear();
            line_data.assign(h->time_sample_sns.size(), 0.0);

            // First, obtain the bispectrum
            std::vector<number> threepf_line;
            this->threepf(h, threepf_line, k);

            // Second, compute the three copies of the spectra which we need
            std::vector<number> twopf_k1;
            std::vector<number> twopf_k2;
            std::vector<number> twopf_k3;

            typename data_manager<number>::twopf_configuration k1;
            k1.serial         = k.k1_serial;
            k1.k_comoving     = k.k1_comoving;
            k1.k_conventional = k.k1_conventional;

            typename data_manager<number>::twopf_configuration k2;
            k2.serial         = k.k2_serial;
            k2.k_comoving     = k.k2_comoving;
            k2.k_conventional = k.k2_conventional;

            typename data_manager<number>::twopf_configuration k3;
            k3.serial         = k.k3_serial;
            k3.k_comoving     = k.k3_comoving;
            k3.k_conventional = k.k3_conventional;

            this->twopf(h, twopf_k1, k1);
            this->twopf(h, twopf_k2, k2);
            this->twopf(h, twopf_k3, k3);

            // Third, build the reduced bispectrum
            for(unsigned int j = 0; j < h->time_sample_sns.size(); j++)
              {
                number form_factor = (6.0/5.0) * ( twopf_k1[j]*twopf_k2[j] + twopf_k1[j]*twopf_k3[j] + twopf_k2[j]*twopf_k3[j] );

                line_data[j] = threepf_line[j] / form_factor;
              }
          }


      }   // namespace derived_data

  }   // namespace transport


#endif //__zeta_timeseries_compute_H_
