//
// Created by David Seery on 10/04/15.
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


#ifndef CPPTRANSPORT_ZETA_COMPUTE_H
#define CPPTRANSPORT_ZETA_COMPUTE_H


#include <vector>

#include "transport-runtime/models/model_forward_declare.h"

#include "transport-runtime/tasks/integration_tasks.h"
#include "transport-runtime/tasks/task_configurations.h"

#include "transport-runtime/derived-products/derived-content/correlation-functions/compute-gadgets/common.h"


namespace transport
  {

    template <typename number>
    class postprocess_delegate
      {
        
        // TYPES
        
      protected:
    
        //! type alias for operator position
        using position = derived_data::derived_data_impl::operator_position;
        
        //! type alias for which index is fixed
        using fixed = derived_data::derived_data_impl::fixed_index;
    
    
        // CONSTRUCTOR, DESTRUCTOR

      public:

        postprocess_delegate(model<number>* m, twopf_db_task<number>* tk);

        ~postprocess_delegate() = default;


        // INTERFACE

      public:

        //! compute the zeta twopf
        void zeta_twopf(const std::vector<number>& twopf, const std::vector<number>& twopf_si,
                        const std::vector<number>& bg, number& zeta_twopf, number& ns,
                        std::vector<number>& gauge_xfm1, bool precomputed);

        //! compute the zeta threepf and reduced bispectrum;
        //! the input three-point function *should already have been shifted* so that it represents
        //! a time-derivative 3pf rather than a momentum 3pf
        void zeta_threepf(const threepf_kconfig& kconfig, double t, const std::vector<number>& threepf,
                          const std::vector<number>& k1_re, const std::vector<number>& k1_im,
                          const std::vector<number>& k2_re, const std::vector<number>& k2_im,
                          const std::vector<number>& k3_re, const std::vector<number>& k3_im,
                          const std::vector<number>& bg,
                          number& zeta_threepf, number& redbsp, std::vector<number>& gauge_xfm1,
                          std::vector<number>& gauge_xfm2_123, std::vector<number>& gauge_xfm2_213, std::vector<number>& gauge_xfm2_312);

		    //! shift the (l,m,n) component of a threepf from momenta to derivatives;
        //! the threepf component should be passed in 'value', and the processed
        //! version is returned by reference
        void shift(unsigned int l, unsigned int m, unsigned int n,
                   const threepf_kconfig& kconfig, double t,
                   const std::vector<number>& k1_re, const std::vector<number>& k1_im,
                   const std::vector<number>& k2_re, const std::vector<number>& k2_im,
                   const std::vector<number>& k3_re, const std::vector<number>& k3_im,
                   const std::vector<number>& bg,
                   number& value);

      protected:

        //! internal version of zeta 2pf calculation that does not include calculation of the spectral index
        //! the gauge transformation is required to be pre-computed
        number zeta_twopf_no_ns(const std::vector<number>& twopf, const std::vector<number>& bg, std::vector<number>& gauge_xfm1);


        // INTERNAL API

      protected:

		    //! compute the shift from momenta to derivatives
        number compute_shift(double t, unsigned int p, unsigned int q, unsigned int r,
                             double p_comoving, double q_comoving, double r_comoving,
                             const std::vector<number>& p_re, const std::vector<number>& p_im,
                             const std::vector<number>& q_re, const std::vector<number>& q_im,
                             const std::vector<number>& r_re, const std::vector<number>& r_im,
                             const std::vector<number>& bg, position pos);


        // INTERNAL DATA

      protected:

        //! cache number of fields
        unsigned int Nfields;

        //! cache pointer to model instance
        model<number>* mdl;

        //! cache point to task instance
        twopf_db_task<number>* parent_task;

        //! B-tensor cache
        std::vector<number> B_qrp;

        //! C-tensor cache (need two, because need two different index arrangements)
        std::vector<number> C_pqr;

        //! C-tensor cache (need two, because need two different index arrangements)
        std::vector<number> C_prq;

      };


    template <typename number>
    postprocess_delegate<number>::postprocess_delegate(model<number> *m, twopf_db_task<number>* tk)
      : mdl(m),
        Nfields(m->get_N_fields()),
        parent_task(tk)
      {
        assert(this->mdl != nullptr);
        assert(this->parent_task != nullptr);

        B_qrp.resize(Nfields*Nfields*Nfields);
        C_pqr.resize(Nfields*Nfields*Nfields);
        C_prq.resize(Nfields*Nfields*Nfields);
      }


		template <typename number>
    void
    postprocess_delegate<number>::zeta_twopf
      (const std::vector<number>& twopf, const std::vector<number>& twopf_si, const std::vector<number>& bg,
       number& zeta_twopf, number& ns, std::vector<number>& gauge_xfm, bool precomputed)
			{
        zeta_twopf = 0.0;
        number zeta_si = 0.0;

        if(!precomputed) this->mdl->compute_gauge_xfm_1(this->parent_task, bg, gauge_xfm);

        // compute twopf
        for(unsigned int m = 0; m < 2*this->Nfields; ++m)
          {
            for(unsigned int n = 0; n < 2*this->Nfields; ++n)
              {
                number tpf_term = gauge_xfm[m] * gauge_xfm[n] * twopf[this->mdl->flatten(m, n)];
                number si_term = gauge_xfm[m] * gauge_xfm[n] * twopf_si[this->mdl->flatten(m, n)];

                zeta_twopf += tpf_term;
                zeta_si += si_term;
              }
          }

        // the zeta spectral index is given by the ratio
        // (N_a N_b n^ab) / (N_c N_d Sigma^cd) = (N_a N_b n^ab) / P_zeta.
        // However, our definition of n^ab gives the spectral index for the raw power spectrum P
        // rather than the dimensionless power spectrum \cal{P}, so we need to adjust it by a +3 offset.
        // Then we have another +1 because the scalar spectral index is defined as n_s-1 rather than just n_s
        ns = 4.0 + zeta_si/zeta_twopf;
			}


    template <typename number>
    number
    postprocess_delegate<number>::zeta_twopf_no_ns
      (const std::vector<number>& twopf, const std::vector<number>& bg, std::vector<number>& gauge_xfm)
      {
        number zeta_twopf = 0.0;

        // compute twopf
        for(unsigned int m = 0; m < 2*this->Nfields; ++m)
          {
            for(unsigned int n = 0; n < 2*this->Nfields; ++n)
              {
                number tpf_term = gauge_xfm[m] * gauge_xfm[n] * twopf[this->mdl->flatten(m, n)];

                zeta_twopf += tpf_term;
              }
          }

        return zeta_twopf;
      }


    template <typename number>
    void
    postprocess_delegate<number>::zeta_threepf
      (const threepf_kconfig& kconfig, double t, const std::vector<number>& threepf,
       const std::vector<number>& k1_re, const std::vector<number>& k1_im,
       const std::vector<number>& k2_re, const std::vector<number>& k2_im,
       const std::vector<number>& k3_re, const std::vector<number>& k3_im,
       const std::vector<number>& bg,
       number& zeta_threepf, number& redbsp,
       std::vector<number>& gauge_xfm1,
       std::vector<number>& gauge_xfm2_123, std::vector<number>& gauge_xfm2_213, std::vector<number>& gauge_xfm2_312)
      {
        zeta_threepf = 0.0;

        const double k1 = kconfig.k1_comoving;
        const double k2 = kconfig.k2_comoving;
        const double k3 = kconfig.k3_comoving;

        const double k1k2 = k3*k3/(k1*k2);
        const double k1k3 = k2*k2/(k1*k3);
        const double k2k3 = k1*k1/(k2*k3);

        // compute linear gauge transformation coefficients for this time, making use of user-supplied cache which should be correctly sized
        this->mdl->compute_gauge_xfm_1(this->parent_task, bg, gauge_xfm1);

        // compute quadratic gauge transformation coefficients, making use of user-supplied caches which should be correctly sized
        this->mdl->compute_gauge_xfm_2(this->parent_task, bg, k1, k2, k3, t, gauge_xfm2_123);
        this->mdl->compute_gauge_xfm_2(this->parent_task, bg, k2, k1, k3, t, gauge_xfm2_213);
        this->mdl->compute_gauge_xfm_2(this->parent_task, bg, k3, k1, k2, t, gauge_xfm2_312);

        // compute linear part of gauge transformation
        for(unsigned int l = 0; l < 2*this->Nfields; ++l)
          {
            for(unsigned int m = 0; m < 2*this->Nfields; ++m)
              {
                for(unsigned int n = 0; n < 2*this->Nfields; ++n)
                  {
                    // no need to shift 3pf; the input is assumed already to have been shifted
                    zeta_threepf += gauge_xfm1[l]*gauge_xfm1[m]*gauge_xfm1[n] * threepf[this->mdl->flatten(l,m,n)];
                  }
              }
          }

        // quadratic part of gauge transformation
        for(unsigned int l = 0; l < 2*this->Nfields; ++l)
          {
            for(unsigned int m = 0; m < 2*this->Nfields; ++m)
              {
                for(unsigned int p = 0; p < 2*this->Nfields; ++p)
                  {
                    for(unsigned int q = 0; q < 2*this->Nfields; ++q)
                      {
                        // as of 14 Jan 2016 the database stores dimensionless quantities k^3 * 2pf and (k1 k2 k3)^2 * 3pf
                        // to accommodate this we need factors k1k2, k1k3, k2k3 to convert from k^3 * 2pf objects to appropriately
                        // normalized 3pf shapes

                        number component1 = gauge_xfm2_123[this->mdl->flatten(l,m)]*gauge_xfm1[p]*gauge_xfm1[q]
                                            * k2k3
                                            * (k2_re[this->mdl->flatten(l,p)]*k3_re[this->mdl->flatten(m,q)]
                                               - k2_im[this->mdl->flatten(l,p)]*k3_im[this->mdl->flatten(m,q)]);
                        number component2 = gauge_xfm2_213[this->mdl->flatten(l,m)]*gauge_xfm1[p]*gauge_xfm1[q]
                                            * k1k3
                                            * (k1_re[this->mdl->flatten(l,p)]*k3_re[this->mdl->flatten(m,q)]
                                               - k1_im[this->mdl->flatten(l,p)]*k3_im[this->mdl->flatten(m,q)]);
                        number component3 = gauge_xfm2_312[this->mdl->flatten(l,m)]*gauge_xfm1[p]*gauge_xfm1[q]
                                            * k1k2
                                            * (k1_re[this->mdl->flatten(l,p)]*k2_re[this->mdl->flatten(m,q)]
                                               - k1_im[this->mdl->flatten(l,p)]*k2_im[this->mdl->flatten(m,q)]);

                        zeta_threepf += component1;
                        zeta_threepf += component2;
                        zeta_threepf += component3;
                      }
                  }
              }
          }

        // compute reduced bispectrum

        // internal 'no ns' version of zeta 2pf calculation requires that the gauge transformation is
        // precomputed
        number z_tpf_k1 = this->zeta_twopf_no_ns(k1_re, bg, gauge_xfm1);
        number z_tpf_k2 = this->zeta_twopf_no_ns(k2_re, bg, gauge_xfm1);
        number z_tpf_k3 = this->zeta_twopf_no_ns(k3_re, bg, gauge_xfm1);

        number form_factor = (6.0/5.0) * (z_tpf_k1*z_tpf_k2*k1k2 + z_tpf_k1*z_tpf_k3*k1k3 + z_tpf_k2*z_tpf_k3*k2k3);
        redbsp = zeta_threepf / form_factor;
      }


    template <typename number>
    void postprocess_delegate<number>::shift(unsigned int l, unsigned int m, unsigned int n,
                                             const threepf_kconfig& kconfig, double t,
                                             const std::vector<number>& k1_re, const std::vector<number>& k1_im,
                                             const std::vector<number>& k2_re, const std::vector<number>& k2_im,
                                             const std::vector<number>& k3_re, const std::vector<number>& k3_im,
                                             const std::vector<number>& bg,
                                             number& value)
      {
        if(this->mdl->is_momentum(l))
          value += this->compute_shift(t, l, m, n, kconfig.k1_comoving, kconfig.k2_comoving, kconfig.k3_comoving,
                                       k1_re, k1_im, k2_re, k2_im, k3_re, k3_im, bg, position::left);

        if(this->mdl->is_momentum(m))
          value += this->compute_shift(t, m, l, n, kconfig.k2_comoving, kconfig.k1_comoving, kconfig.k3_comoving,
                                       k2_re, k2_im, k1_re, k1_im, k3_re, k3_im, bg, position::middle);

        if(this->mdl->is_momentum(n))
          value += this->compute_shift(t, n, l, m, kconfig.k3_comoving, kconfig.k1_comoving, kconfig.k2_comoving,
                                       k3_re, k3_im, k1_re, k1_im, k2_re, k2_im, bg, position::middle);
      }


    template <typename number>
    number
    postprocess_delegate<number>::compute_shift
      (double t, unsigned int p, unsigned int q, unsigned int r,
       double p_comoving, double q_comoving, double r_comoving,
       const std::vector<number>& p_re, const std::vector<number>& p_im,
       const std::vector<number>& q_re, const std::vector<number>& q_im,
       const std::vector<number>& r_re, const std::vector<number>& r_im,
       const std::vector<number>& bg, position pos)
      {
        // p is the index of the momentum insertion
        // q, r are the indices of the remaining insertions
        
        // extract components of the B and C tensors needed to make the shift;
        // recall that B and C are both symmetric on their first two indices (but not under other exchanges)
        // also, because B and C carry momenta, we need to be careful when considering index permutations,
        // because the momenta must be permuted likewise.
        // In this case, the symmetry of B means that we can live with a single momentum assignment
        // whereas for C we need two
        this->mdl->B(this->parent_task, bg, q_comoving, r_comoving, p_comoving, t, this->B_qrp);
        this->mdl->C(this->parent_task, bg, p_comoving, q_comoving, r_comoving, t, this->C_pqr);
        this->mdl->C(this->parent_task, bg, p_comoving, r_comoving, q_comoving, t, this->C_prq);

        // initialize shift to zero
        number shift = 0.0;
        
        // map p to a species index
        unsigned int p_species = this->mdl->species(p);

        // we need to know which index on each twopf is fixed, and which is summed over
        auto q_fixed = fixed::first;
        auto r_fixed = fixed::first;

        switch(pos)
          {
            case position::left:    // our operator is on the far left-hand side, so is to the left of both the q and r operators
              {
                q_fixed = fixed::second;
                r_fixed = fixed::second;
                break;
              }

            case position::middle:  // our operator is in the middle, to the right of the q operator but to the left of the r operator
              {
                q_fixed = fixed::first;
                r_fixed = fixed::second;
                break;
              }

            case position::right:   // our operator is on the right, to the right of both the q and r operators
              {
                q_fixed = fixed::first;
                r_fixed = fixed::first;
                break;
              }
          }

        for(unsigned int m = 0; m < this->Nfields; ++m)
          {
            for(unsigned int n = 0; n < this->Nfields; ++n)
              {
                const unsigned int q_m_id = this->mdl->flatten((q_fixed == fixed::first ? q : m),
                                                               (q_fixed == fixed::second ? q : m));
                const unsigned int q_n_id = this->mdl->flatten((q_fixed == fixed::first ? q : n),
                                                               (q_fixed == fixed::second ? q : n));

                const unsigned int r_n_id = this->mdl->flatten((r_fixed == fixed::first ? r : n),
                                                               (r_fixed == fixed::second ? r : n));

                const unsigned int mom_q_m_id = this->mdl->flatten((q_fixed == fixed::first ? this->mdl->momentum(q) : m),
                                                                   (q_fixed == fixed::second ? this->mdl->momentum(q) : m));
                const unsigned int mom_r_m_id = this->mdl->flatten((r_fixed == fixed::first ? this->mdl->momentum(r) : m),
                                                                   (r_fixed == fixed::second ? this->mdl->momentum(r) : m));

                // as of 14 Jan 2016, we store dimensionless objects in the database (k^3 * 2pf, (k1 k2 k3)^2 * 3pf)
                // in the database, because these objects do not depend on our conventions about normalizing
                // comoving ks and therefore are easier for an end-user to interpret

                // to handle that, we need to include a factor dimless_conversion which converts from k^3 * 2pf to
                // the appropriate shape-function factor

                const double dimless_conversion = p_comoving*p_comoving/(q_comoving*r_comoving);

                shift -= this->B_qrp[this->mdl->fields_flatten(m,n,p_species)]
                         * dimless_conversion
                         * ( q_re[q_m_id]*r_re[r_n_id] - q_im[q_m_id]*r_im[r_n_id] );

                shift -= this->C_pqr[this->mdl->fields_flatten(p_species,m,n)]
                         * dimless_conversion
                         * ( q_re[mom_q_m_id]*r_re[r_n_id] - q_im[mom_q_m_id]*r_im[r_n_id] );

                shift -= this->C_prq[this->mdl->fields_flatten(p_species,m,n)]
                         * dimless_conversion
                         * ( q_re[q_n_id]*r_re[mom_r_m_id] - q_im[q_n_id]*r_re[mom_r_m_id] );
              }
          }

        return(shift);
      }


  }   // namespace transport


#endif //CPPTRANSPORT_ZETA_COMPUTE_H
