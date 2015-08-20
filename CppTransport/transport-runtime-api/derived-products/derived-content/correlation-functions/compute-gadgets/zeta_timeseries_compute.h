//
// Created by David Seery on 22/06/2014.
// Copyright (c) 2014-15 University of Sussex. All rights reserved.
//


#ifndef __zeta_timeseries_compute_H_
#define __zeta_timeseries_compute_H_


#include <vector>
#include <memory>


// need data_manager for datapipe
#include "transport-runtime-api/data/data_manager.h"

// need 3pf shifter
#include "transport-runtime-api/derived-products/derived-content/correlation-functions/compute-gadgets/threepf_time_shift.h"


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

		            //! constructor
                handle(datapipe<number>& pipe, twopf_list_task<number>* tk, const SQL_time_config_query& tq, unsigned int Nf);

		            //! destructor is default
                ~handle() = default;

                // INTERNAL DATA

              protected:

                //! datapipe object
                datapipe<number>& pipe;

                //! model pointer
                model<number>* mdl;

                //! task pointer
                twopf_list_task<number>* tk;

		            //! database SQL query for time axis
		            const SQL_time_config_query tquery;

                //! datapipe handle for this set of serial numbers
                typename datapipe<number>::time_data_handle& t_handle;

                //! time configuration data corresponding to this SQL query, pulled from the datapipe
                std::vector<time_config> t_axis;

                //! number of fields
                const unsigned int N_fields;

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
            std::shared_ptr<handle> make_handle(datapipe<number>& pipe, twopf_list_task<number>* tk, const SQL_time_config_query& tq, unsigned int Nf) const;


            // COMPUTE ZETA PRODUCTS

          public:

            //! compute a time series for the zeta two-point function
            void twopf(std::shared_ptr<handle>& h, std::vector<number>& zeta_twopf, std::vector< std::vector<number> >& gauge_xfm1, const twopf_kconfig& k) const;

            //! compute a time series for the zeta three-point function
            void threepf(std::shared_ptr<handle>& h, std::vector<number>& zeta_threepf, std::vector<number>& redbsp,
                         std::vector< std::vector<number> >& gauge_xfm2_123, std::vector< std::vector<number> >& gauge_xfm_213,
                         std::vector< std::vector<number> >& gauge_xfm2_312, const threepf_kconfig& k) const;

          protected:

            //! compute a time series for the zeta two-point function (don't copy gauge xfms)
            void twopf(std::shared_ptr<handle>& h, std::vector<number>& zeta_twopf, const twopf_kconfig& k) const;


            // INTERNAL DATA

          protected:

            threepf_time_shift<number> shifter;

          };


        // HANDLE METHODS


        template <typename number>
        zeta_timeseries_compute<number>::handle::handle(datapipe<number>& p, twopf_list_task<number>* t, const SQL_time_config_query& tq, unsigned int Nf)
          : pipe(p),
            tk(t),
            tquery(tq),
            t_handle(p.new_time_data_handle(tq)),
            N_fields(Nf)
          {
            assert(tk != nullptr);

            mdl = tk->get_model();
            assert(mdl != nullptr);

		        // look up timeline
		        typename datapipe<number>::time_config_handle& tc_handle = pipe.new_time_config_handle(tq);
            time_config_tag<number> t_tag = p.new_time_config_tag();
            t_axis = tc_handle.lookup_tag(t_tag);

            // pull background data for the time_sample we are using,
            // and slice it up by time in an array 'background'
            background.clear();
            background.resize(t_axis.size());

            for(unsigned int i = 0; i < 2*N_fields; ++i)
              {
                background_time_data_tag<number> tag = pipe.new_background_time_data_tag(i);

                // safe to take a reference here and avoid a copy
                const std::vector<number>& bg_line = t_handle.lookup_tag(tag);
                assert(bg_line.size() == background.size());

		            for(unsigned int j = 0; j < background.size(); ++j)
			            {
				            background[j].push_back(bg_line[j]);
			            }
              }

            // cache gauge transformation coefficients
            dN.resize(t_axis.size());
		        for(unsigned int j = 0; j < background.size(); ++j)
              {
                dN[j].resize(2*N_fields);
                mdl->compute_gauge_xfm_1(tk, background[j], dN[j]);
              }
          }


        // IMPLEMENTATION


        template <typename number>
        std::shared_ptr<typename zeta_timeseries_compute<number>::handle>
        zeta_timeseries_compute<number>::make_handle(datapipe<number>& pipe, twopf_list_task<number>* t, const SQL_time_config_query& tq, unsigned int Nf) const
          {
            return std::make_shared<handle>(pipe, t, tq, Nf);
          }


        template <typename number>
        void zeta_timeseries_compute<number>::twopf(std::shared_ptr<typename zeta_timeseries_compute<number>::handle>& h,
                                                    std::vector<number>& zeta_twopf, const twopf_kconfig& k) const
          {
            unsigned int N_fields = h->N_fields;

            zeta_twopf.clear();
            zeta_twopf.assign(h->t_axis.size(), 0.0);

            // compute zeta twopf
            for(unsigned int m = 0; m < 2*N_fields; ++m)
              {
                for(unsigned int n = 0; n < 2*N_fields; ++n)
                  {
                    cf_time_data_tag<number> tag =
                                               h->pipe.new_cf_time_data_tag(cf_data_type::cf_twopf_re, h->mdl->flatten(m,n), k.serial);

                    // pull twopf data for this component; can use a reference to avoid copying
                    const std::vector<number>& sigma_line = h->t_handle.lookup_tag(tag);

                    for(unsigned int j = 0; j < h->t_axis.size(); ++j)
                      {
                        number component = h->dN[j][m]*h->dN[j][n]*sigma_line[j];
                        zeta_twopf[j] += component;
                      }
                  }
              }
          }


        template <typename number>
        void zeta_timeseries_compute<number>::twopf(std::shared_ptr<typename zeta_timeseries_compute<number>::handle>& h,
                                                    std::vector<number>& zeta_twopf, std::vector< std::vector<number> >& gauge_xfm1, const twopf_kconfig& k) const
          {
            this->twopf(h, zeta_twopf, k);

            // copy gauge xfm into return list
            gauge_xfm1 = h->dN;
          }


        template <typename number>
        void zeta_timeseries_compute<number>::threepf(std::shared_ptr<typename zeta_timeseries_compute<number>::handle>& h,
                                                      std::vector<number>& zeta_threepf, std::vector<number>& redbsp,
                                                      std::vector< std::vector<number> >& gauge_xfm2_123, std::vector< std::vector<number> >& gauge_xfm2_213,
                                                      std::vector< std::vector<number> >& gauge_xfm2_312, const threepf_kconfig& k) const
          {
            unsigned int N_fields = h->N_fields;

            zeta_threepf.clear();
            zeta_threepf.assign(h->t_axis.size(), 0.0);
            redbsp.clear();
            redbsp.assign(h->t_axis.size(), 0.0);

            // cache gauge transformation coefficients
            // these have to be recomputed for each k-configuration, because they are time and shape-dependent
            // we take advantage of the caller-provided caches to store them; they should be correctly sized

            for(unsigned int j = 0; j < h->t_axis.size(); ++j)
              {
                h->mdl->compute_gauge_xfm_2(h->tk, h->background[j], k.k1_comoving, k.k2_comoving, k.k3_comoving, h->t_axis[j].t, gauge_xfm2_123[j]);
                h->mdl->compute_gauge_xfm_2(h->tk, h->background[j], k.k2_comoving, k.k1_comoving, k.k3_comoving, h->t_axis[j].t, gauge_xfm2_213[j]);
                h->mdl->compute_gauge_xfm_2(h->tk, h->background[j], k.k3_comoving, k.k1_comoving, k.k2_comoving, h->t_axis[j].t, gauge_xfm2_312[j]);
              }

            // linear component of the gauge transformation
            derived_data::SQL_time_config_query tquery("1=1");    // no filtering on time configurations
            for(unsigned int l = 0; l < 2*N_fields; ++l)
              {
                for(unsigned int m = 0; m < 2*N_fields; ++m)
                  {
                    for(unsigned int n = 0; n < 2*N_fields; ++n)
                      {
                        // pull threepf data for this component
                        cf_time_data_tag<number> tag = h->pipe.new_cf_time_data_tag(cf_data_type::cf_threepf, h->mdl->flatten(l,m,n), k.serial);

                        // have to take a copy of the data line, rather than use a reference, because shifting the derivative will modify it in place
                        std::vector<number> threepf_line = h->t_handle.lookup_tag(tag);

                        // shift field so it represents a derivative correlation function, not a momentum one
                        this->shifter.shift(h->tk, h->mdl, h->pipe, tquery, threepf_line, h->t_axis, l, m, n, k);

                        for(unsigned int j = 0; j < h->t_axis.size(); ++j)
                          {
                            number component = h->dN[j][l]*h->dN[j][m]*h->dN[j][n]*threepf_line[j];
                            zeta_threepf[j] += component;
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
                            // the indices are N_lm, N_p, N_q, so the 2pfs we sum over are
                            // sigma_lp(k2)*sigma_mq(k3) etc.

                            cf_time_data_tag<number> k1_re_lp_tag = h->pipe.new_cf_time_data_tag(cf_data_type::cf_twopf_re, h->mdl->flatten(l,p), k.k1_serial);
                            cf_time_data_tag<number> k1_im_lp_tag = h->pipe.new_cf_time_data_tag(cf_data_type::cf_twopf_im, h->mdl->flatten(l,p), k.k1_serial);

                            cf_time_data_tag<number> k2_re_lp_tag = h->pipe.new_cf_time_data_tag(cf_data_type::cf_twopf_re, h->mdl->flatten(l,p), k.k2_serial);
                            cf_time_data_tag<number> k2_im_lp_tag = h->pipe.new_cf_time_data_tag(cf_data_type::cf_twopf_im, h->mdl->flatten(l,p), k.k2_serial);

                            cf_time_data_tag<number> k2_re_mq_tag = h->pipe.new_cf_time_data_tag(cf_data_type::cf_twopf_re, h->mdl->flatten(m,q), k.k2_serial);
                            cf_time_data_tag<number> k2_im_mq_tag = h->pipe.new_cf_time_data_tag(cf_data_type::cf_twopf_im, h->mdl->flatten(m,q), k.k2_serial);

                            cf_time_data_tag<number> k3_re_mq_tag = h->pipe.new_cf_time_data_tag(cf_data_type::cf_twopf_re, h->mdl->flatten(m,q), k.k3_serial);
                            cf_time_data_tag<number> k3_im_mq_tag = h->pipe.new_cf_time_data_tag(cf_data_type::cf_twopf_im, h->mdl->flatten(m,q), k.k3_serial);

                            // can only take reference for the last lookup, because previous items may be evited
                            const std::vector<number>  k1_re_lp = h->t_handle.lookup_tag(k1_re_lp_tag);
                            const std::vector<number>  k1_im_lp = h->t_handle.lookup_tag(k1_im_lp_tag);
                            const std::vector<number>  k2_re_lp = h->t_handle.lookup_tag(k2_re_lp_tag);
                            const std::vector<number>  k2_im_lp = h->t_handle.lookup_tag(k2_im_lp_tag);
                            const std::vector<number>  k2_re_mq = h->t_handle.lookup_tag(k2_re_mq_tag);
                            const std::vector<number>  k2_im_mq = h->t_handle.lookup_tag(k2_im_mq_tag);
                            const std::vector<number>  k3_re_mq = h->t_handle.lookup_tag(k3_re_mq_tag);
                            const std::vector<number>& k3_im_mq = h->t_handle.lookup_tag(k3_im_mq_tag);

                            for(unsigned int j = 0; j < h->t_axis.size(); ++j)
                              {
                                number component1 = gauge_xfm2_123[j][h->mdl->flatten(l,m)] * h->dN[j][p] * h->dN[j][q] * (k2_re_lp[j]*k3_re_mq[j] - k2_im_lp[j]*k3_im_mq[j]);
                                number component2 = gauge_xfm2_213[j][h->mdl->flatten(l,m)] * h->dN[j][p] * h->dN[j][q] * (k1_re_lp[j]*k3_re_mq[j] - k1_im_lp[j]*k3_im_mq[j]);
                                number component3 = gauge_xfm2_312[j][h->mdl->flatten(l,m)] * h->dN[j][p] * h->dN[j][q] * (k1_re_lp[j]*k2_re_mq[j] - k1_im_lp[j]*k2_im_mq[j]);

                                zeta_threepf[j] += component1;
                                zeta_threepf[j] += component2;
                                zeta_threepf[j] += component3;
                              }
                          }
                      }
                  }
              }

            // compute reduced bispectrum
            std::vector<number> twopf_k1;
            std::vector<number> twopf_k2;
            std::vector<number> twopf_k3;

            twopf_kconfig k1;
            k1.serial         = k.k1_serial;
            k1.k_comoving     = k.k1_comoving;
            k1.k_conventional = k.k1_conventional;

            twopf_kconfig k2;
            k2.serial         = k.k2_serial;
            k2.k_comoving     = k.k2_comoving;
            k2.k_conventional = k.k2_conventional;

            twopf_kconfig k3;
            k3.serial         = k.k3_serial;
            k3.k_comoving     = k.k3_comoving;
            k3.k_conventional = k.k3_conventional;

            this->twopf(h, twopf_k1, k1);
            this->twopf(h, twopf_k2, k2);
            this->twopf(h, twopf_k3, k3);

            // build the reduced bispectrum
            for(unsigned int j = 0; j < h->t_axis.size(); ++j)
              {
                number form_factor = (6.0/5.0) * ( twopf_k1[j]*twopf_k2[j] + twopf_k1[j]*twopf_k3[j] + twopf_k2[j]*twopf_k3[j] );

                redbsp[j] = zeta_threepf[j] / form_factor;
              }
          }

      }   // namespace derived_data

  }   // namespace transport


#endif //__zeta_timeseries_compute_H_
