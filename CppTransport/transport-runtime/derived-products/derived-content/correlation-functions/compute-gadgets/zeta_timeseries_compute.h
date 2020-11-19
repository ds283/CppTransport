//
// Created by David Seery on 22/06/2014.
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


#ifndef CPPTRANSPORT_ZETA_TIMESERIES_COMPUTE_H
#define CPPTRANSPORT_ZETA_TIMESERIES_COMPUTE_H


#include <vector>
#include <memory>


// need data_manager for datapipe
#include "transport-runtime/data/data_manager.h"


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
                handle(datapipe<number>& pipe, twopf_db_task<number>* tk, const SQL_time_query& tq, unsigned int Nf);

		            //! destructor is default
                ~handle() = default;


                // EXPOSE ATTRIBUTES OF ATTACHED CONTENT GROUP

                //! introspect attached content group
                const integration_payload& get_payload() const { return this->payload; }

                //! does attached group have spectral data?
                bool has_spectral_data() const { return this->has_spectral; }


                // INTERNAL DATA

              protected:

                //! datapipe object
                datapipe<number>& pipe;

                //! cache reference to repository integration payload record
                const integration_payload& payload;

                //! model pointer
                model<number>* mdl;

                //! task pointer
                twopf_db_task<number>* tk;

		            //! database SQL query for time axis
		            const SQL_time_query tquery;

                //! datapipe handle for this set of serial numbers
                typename datapipe<number>::time_data_handle& t_handle;

                //! time configuration data corresponding to this SQL query, pulled from the datapipe
                std::vector<time_config> t_axis;

                //! number of fields
                const unsigned int N_fields;

                //! does the payload have spectral data?
                const bool has_spectral;

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
            std::unique_ptr<handle> make_handle(datapipe<number>& pipe, twopf_db_task<number>* tk, const SQL_time_query& tq, unsigned int Nf) const;


            // COMPUTE ZETA PRODUCTS

          public:

            //! compute a time series for the zeta two-point function
            void twopf(handle& h, std::vector<number>& zeta_twopf, std::vector<number>& zeta_ns,
                       std::vector< std::vector<number> >& gauge_xfm1, const twopf_kconfig& k) const;

            //! compute a time series for the zeta three-point function
            void threepf(handle& h, std::vector<number>& zeta_threepf, std::vector<number>& redbsp,
                         std::vector< std::vector<number> >& gauge_xfm2_123, std::vector< std::vector<number> >& gauge_xfm_213,
                         std::vector< std::vector<number> >& gauge_xfm2_312, const threepf_kconfig& k) const;

          protected:

            //! compute a time series for the zeta two-point function (doesn't copy gauge xfms)
            void twopf(handle& h, std::vector<number>& zeta_twopf, std::vector<number>& ns, const twopf_kconfig& k) const;

            //! compute a time series for the zeta two-point function, without spectral index data (doesn't copy gauge xfms)
            void twopf_no_ns(handle& h, std::vector<number>& zeta_twopf, const twopf_kconfig& k) const;

          };


        // HANDLE METHODS


        template <typename number>
        zeta_timeseries_compute<number>::handle::handle(datapipe<number>& p, twopf_db_task<number>* t, const SQL_time_query& tq, unsigned int Nf)
          : pipe{p},
            payload{p.get_attached_integration_record()->get_payload()},
            has_spectral{payload.has_spectral_data()},    // payload guaranteed to be constructed due to declaration order
            tk{t},
            tquery{tq},
            t_handle{p.new_time_data_handle(tq)},
            N_fields{Nf}
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
        std::unique_ptr<typename zeta_timeseries_compute<number>::handle>
        zeta_timeseries_compute<number>::make_handle(datapipe<number>& pipe, twopf_db_task<number>* t, const SQL_time_query& tq, unsigned int Nf) const
          {
            return std::make_unique<handle>(pipe, t, tq, Nf);
          }


        template <typename number>
        void zeta_timeseries_compute<number>::twopf
            (typename zeta_timeseries_compute<number>::handle& h, std::vector<number>& zeta_twopf,
             std::vector<number>& zeta_ns, const twopf_kconfig& k) const
          {
            unsigned int N_fields = h.N_fields;

            // zero data vectors
            zeta_twopf.clear();
            zeta_ns.clear();

            const auto t_axis_size = h.t_axis.size();
            zeta_twopf.assign(t_axis_size, 0.0);
            zeta_ns.assign(t_axis_size, 0.0);

            // compute zeta twopf
            for(unsigned int m = 0; m < 2*N_fields; ++m)
              {
                for(unsigned int n = 0; n < 2*N_fields; ++n)
                  {
                    // pull twopf data from the datapipe for this (m,n) element
                    cf_time_data_tag<number> tpf_tag =
                      h.pipe.new_cf_time_data_tag(cf_data_type::cf_twopf_re, h.mdl->flatten(m, n), k.serial);

                    // pull twopf data for this component; can use a reference to avoid copying
                    const std::vector<number>& line = h.t_handle.lookup_tag(tpf_tag);

                    for(unsigned int j = 0; j < t_axis_size; ++j)
                      {
                        number component = h.dN[j][m] * h.dN[j][n] * line[j];
                        zeta_twopf[j] += component;
                      }
                    
                    if(h.has_spectral)
                      {
                        // pull twopf 'spectral index' data from the datapipe for this (m,n) element
                        cf_time_data_tag<number> tpf_si_tag =
                          h.pipe.new_cf_time_data_tag(cf_data_type::cf_twopf_si_re, h.mdl->flatten(m, n), k.serial);
                        
                        const std::vector<number>& si_line = h.t_handle.lookup_tag(tpf_si_tag);
                        
                        for(unsigned int j = 0; j < t_axis_size; ++j)
                          {
                            number component = h.dN[j][m] * h.dN[j][n] * si_line[j];
                            zeta_ns[j] += component;
                          }
                      }
                  }
              }
            
            // if recording spectral data, go through and take ratio (N_a N_b n^ab) / (N_a N_b Sigma^ab)
            if(h.has_spectral)
              {
                for(unsigned int j = 0; j < t_axis_size; ++j)
                  {
                    zeta_ns[j] = 4.0 + zeta_ns[j] / zeta_twopf[j];
                  }
              }
          }


        template <typename number>
        void zeta_timeseries_compute<number>::twopf_no_ns
          (typename zeta_timeseries_compute<number>::handle& h, std::vector<number>& zeta_twopf, const twopf_kconfig& k) const
          {
            unsigned int N_fields = h.N_fields;

            // zero data vector
            zeta_twopf.clear();

            const auto t_axis_size = h.t_axis.size();
            zeta_twopf.assign(t_axis_size, 0.0);

            // compute zeta twopf
            for(unsigned int m = 0; m < 2*N_fields; ++m)
              {
                for(unsigned int n = 0; n < 2*N_fields; ++n)
                  {
                    // pull twopf data from the datapipe for this (m,n) element
                    cf_time_data_tag<number> tpf_tag =
                      h.pipe.new_cf_time_data_tag(cf_data_type::cf_twopf_re, h.mdl->flatten(m, n), k.serial);

                    // pull twopf data for this component; can use a reference to avoid copying
                    const std::vector<number>& line = h.t_handle.lookup_tag(tpf_tag);

                    for(unsigned int j = 0; j < t_axis_size; ++j)
                      {
                        number component = h.dN[j][m] * h.dN[j][n] * line[j];
                        zeta_twopf[j] += component;
                      }
                  }
              }
          }


        template <typename number>
        void
        zeta_timeseries_compute<number>::twopf
          (typename zeta_timeseries_compute<number>::handle& h, std::vector<number>& zeta_twopf,
           std::vector<number>& zeta_ns, std::vector< std::vector<number> >& gauge_xfm1, const twopf_kconfig& k) const
          {
            this->twopf(h, zeta_twopf, zeta_ns, k);

            // copy gauge xfm into return list
            gauge_xfm1 = h.dN;
          }


        template <typename number>
        void
        zeta_timeseries_compute<number>::threepf
          (typename zeta_timeseries_compute<number>::handle& h, std::vector<number>& zeta_threepf,
           std::vector<number>& redbsp, std::vector< std::vector<number> >& gauge_xfm2_123,
           std::vector< std::vector<number> >& gauge_xfm2_213, std::vector< std::vector<number> >& gauge_xfm2_312,
           const threepf_kconfig& k) const
          {
            unsigned int N_fields = h.N_fields;

            const double k1 = k.k1_comoving;
            const double k2 = k.k2_comoving;
            const double k3 = k.k3_comoving;

            const double k1k2 = k3*k3/(k1*k2);
            const double k1k3 = k2*k2/(k1*k3);
            const double k2k3 = k1*k1/(k2*k3);

            // zeta_threepf, redbsp, gauge_xfm2_123, gauge_xfm2_213, gauge_xfm2_312 should be sized appropriately:
            // first axis is t_axis.size()
            // for gauge xfm2 values, second axis should be 2*Nfields * 2*Nfields
            assert(zeta_threepf.size() == h.t_axis.size());
            assert(redbsp.size() == h.t_axis.size());
            assert(gauge_xfm2_123.size() == h.t_axis.size());
            assert(gauge_xfm2_213.size() == h.t_axis.size());
            assert(gauge_xfm2_312.size() == h.t_axis.size());

            // cache gauge transformation coefficients
            // these have to be recomputed for each k-configuration, because they are time and shape-dependent
            // we take advantage of the caller-provided caches to store them; they should be correctly sized

            for(unsigned int j = 0; j < h.t_axis.size(); ++j)
              {
                assert(gauge_xfm2_123[j].size() == 2*N_fields * 2*N_fields);
                assert(gauge_xfm2_213[j].size() == 2*N_fields * 2*N_fields);
                assert(gauge_xfm2_312[j].size() == 2*N_fields * 2*N_fields);

                h.mdl->compute_gauge_xfm_2(h.tk, h.background[j], k1, k2, k3, h.t_axis[j].t, gauge_xfm2_123[j]);
                h.mdl->compute_gauge_xfm_2(h.tk, h.background[j], k2, k1, k3, h.t_axis[j].t, gauge_xfm2_213[j]);
                h.mdl->compute_gauge_xfm_2(h.tk, h.background[j], k3, k1, k2, h.t_axis[j].t, gauge_xfm2_312[j]);
              }

            // linear component of the gauge transformation
            for(unsigned int l = 0; l < 2*N_fields; ++l)
              {
                for(unsigned int m = 0; m < 2*N_fields; ++m)
                  {
                    for(unsigned int n = 0; n < 2*N_fields; ++n)
                      {
                        // pull threepf data for this component
                        cf_time_data_tag<number> tag =
                          h.pipe.new_cf_time_data_tag(cf_data_type::cf_threepf_Nderiv, h.mdl->flatten(l,m,n), k.serial);

                        const std::vector<number>& threepf_line = h.t_handle.lookup_tag(tag);

                        for(unsigned int j = 0; j < h.t_axis.size(); ++j)
                          {
                            number component = h.dN[j][l]*h.dN[j][m]*h.dN[j][n] * threepf_line[j];
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

                            cf_time_data_tag<number> k1_re_lp_tag =
                              h.pipe.new_cf_time_data_tag(cf_data_type::cf_twopf_re, h.mdl->flatten(l,p), k.k1_serial);
                            cf_time_data_tag<number> k1_im_lp_tag =
                              h.pipe.new_cf_time_data_tag(cf_data_type::cf_twopf_im, h.mdl->flatten(l,p), k.k1_serial);

                            cf_time_data_tag<number> k2_re_lp_tag =
                              h.pipe.new_cf_time_data_tag(cf_data_type::cf_twopf_re, h.mdl->flatten(l,p), k.k2_serial);
                            cf_time_data_tag<number> k2_im_lp_tag =
                              h.pipe.new_cf_time_data_tag(cf_data_type::cf_twopf_im, h.mdl->flatten(l,p), k.k2_serial);

                            cf_time_data_tag<number> k2_re_mq_tag =
                              h.pipe.new_cf_time_data_tag(cf_data_type::cf_twopf_re, h.mdl->flatten(m,q), k.k2_serial);
                            cf_time_data_tag<number> k2_im_mq_tag =
                              h.pipe.new_cf_time_data_tag(cf_data_type::cf_twopf_im, h.mdl->flatten(m,q), k.k2_serial);

                            cf_time_data_tag<number> k3_re_mq_tag =
                              h.pipe.new_cf_time_data_tag(cf_data_type::cf_twopf_re, h.mdl->flatten(m,q), k.k3_serial);
                            cf_time_data_tag<number> k3_im_mq_tag =
                              h.pipe.new_cf_time_data_tag(cf_data_type::cf_twopf_im, h.mdl->flatten(m,q), k.k3_serial);

                            // can only take reference for the last lookup, because previous items may be evicted
                            const std::vector<number>  k1_re_lp = h.t_handle.lookup_tag(k1_re_lp_tag);
                            const std::vector<number>  k1_im_lp = h.t_handle.lookup_tag(k1_im_lp_tag);
                            const std::vector<number>  k2_re_lp = h.t_handle.lookup_tag(k2_re_lp_tag);
                            const std::vector<number>  k2_im_lp = h.t_handle.lookup_tag(k2_im_lp_tag);
                            const std::vector<number>  k2_re_mq = h.t_handle.lookup_tag(k2_re_mq_tag);
                            const std::vector<number>  k2_im_mq = h.t_handle.lookup_tag(k2_im_mq_tag);
                            const std::vector<number>  k3_re_mq = h.t_handle.lookup_tag(k3_re_mq_tag);
                            const std::vector<number>& k3_im_mq = h.t_handle.lookup_tag(k3_im_mq_tag);

                            // as of 14 Jan 2016 the database stores dimensionless quantities k^3 * 2pf, (k1 k2 k3) * 3pf
                            // this means we need some dimensionless factors k1k2, k1k3, k2k3 to convert between the different normalization conventions

                            for(unsigned int j = 0; j < h.t_axis.size(); ++j)
                              {
                                number component1 = gauge_xfm2_123[j][h.mdl->flatten(l,m)] * h.dN[j][p] * h.dN[j][q] * k2k3 * (k2_re_lp[j]*k3_re_mq[j] - k2_im_lp[j]*k3_im_mq[j]);
                                number component2 = gauge_xfm2_213[j][h.mdl->flatten(l,m)] * h.dN[j][p] * h.dN[j][q] * k1k3 * (k1_re_lp[j]*k3_re_mq[j] - k1_im_lp[j]*k3_im_mq[j]);
                                number component3 = gauge_xfm2_312[j][h.mdl->flatten(l,m)] * h.dN[j][p] * h.dN[j][q] * k1k2 * (k1_re_lp[j]*k2_re_mq[j] - k1_im_lp[j]*k2_im_mq[j]);

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

            twopf_kconfig k1_config;
            k1_config.serial         = k.k1_serial;
            k1_config.k_comoving     = k.k1_comoving;
            k1_config.k_conventional = k.k1_conventional;

            twopf_kconfig k2_config;
            k2_config.serial         = k.k2_serial;
            k2_config.k_comoving     = k.k2_comoving;
            k2_config.k_conventional = k.k2_conventional;

            twopf_kconfig k3_config;
            k3_config.serial         = k.k3_serial;
            k3_config.k_comoving     = k.k3_comoving;
            k3_config.k_conventional = k.k3_conventional;

            this->twopf_no_ns(h, twopf_k1, k1_config);
            this->twopf_no_ns(h, twopf_k2, k2_config);
            this->twopf_no_ns(h, twopf_k3, k3_config);

            // build the reduced bispectrum
            for(unsigned int j = 0; j < h.t_axis.size(); ++j)
              {
                number form_factor = (6.0/5.0) * ( twopf_k1[j]*twopf_k2[j]*k1k2 + twopf_k1[j]*twopf_k3[j]*k1k3 + twopf_k2[j]*twopf_k3[j]*k2k3 );

                redbsp[j] = zeta_threepf[j] / form_factor;
              }
          }

      }   // namespace derived_data

  }   // namespace transport


#endif //CPPTRANSPORT_ZETA_TIMESERIES_COMPUTE_H
