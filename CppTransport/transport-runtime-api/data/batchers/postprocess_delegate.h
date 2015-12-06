//
// Created by David Seery on 10/04/15.
// Copyright (c) 2015 University of Sussex. All rights reserved.
//


#ifndef __zeta_compute_H_
#define __zeta_compute_H_


#include <vector>

#include "transport-runtime-api/models/model_forward_declare.h"

#include "transport-runtime-api/tasks/integration_tasks.h"
#include "transport-runtime-api/tasks/task_configurations.h"

#include "transport-runtime-api/derived-products/derived-content/correlation-functions/compute-gadgets/common.h"


namespace transport
  {

    template <typename number>
    class postprocess_delegate
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        postprocess_delegate(model<number>* m, twopf_list_task<number>* tk);

        ~postprocess_delegate() = default;


        // INTERFACE

      public:

        //! compute the zeta twopf
        void zeta_twopf(const std::vector<number>& twopf, const std::vector<number>& bg, number& zeta_twopf, std::vector<number>& gauge_xfm1, bool precomputed=false);

        //! compute the zeta threepf and reduced bispectrum
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


        // INTERNAL API

      protected:

		    //! compute the shift from momenta to derivatives
        number compute_shift(double t, unsigned int p, unsigned int q, unsigned int r,
                             double p_comoving, double q_comoving, double r_comoving,
                             const std::vector<number>& p_re, const std::vector<number>& p_im,
                             const std::vector<number>& q_re, const std::vector<number>& q_im,
                             const std::vector<number>& r_re, const std::vector<number>& r_im,
                             const std::vector<number>& bg, derived_data::derived_data_impl::operator_position pos);


        // INTERNAL DATA

      protected:

        //! cache number of fields
        unsigned int Nfields;

        //! cache pointer to model instance
        model<number>* mdl;

        //! cache point to task instance
        twopf_list_task<number>* parent_task;

        //! B-tensor cache
        std::vector<number> B_qrp;

        //! C-tensor cache (need two, because need two different index arrangements)
        std::vector<number> C_pqr;

        //! C-tensor cache (need two, because need two different index arrangements)
        std::vector<number> C_prq;

      };


    template <typename number>
    postprocess_delegate<number>::postprocess_delegate(model<number> *m, twopf_list_task<number>* tk)
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
		void postprocess_delegate<number>::zeta_twopf(const std::vector<number>& twopf, const std::vector<number>& bg, number& zeta_twopf, std::vector<number>& gauge_xfm, bool precomputed)
			{
        zeta_twopf = 0.0;

        if(!precomputed) this->mdl->compute_gauge_xfm_1(this->parent_task, bg, gauge_xfm);

        // compute twopf
        for(unsigned int m = 0; m < 2*this->Nfields; ++m)
          {
            for(unsigned int n = 0; n < 2*this->Nfields; ++n)
              {
                number component = gauge_xfm[m]*gauge_xfm[n]*twopf[this->mdl->flatten(m, n)];
                zeta_twopf += component;
              }
          }
			}


    template <typename number>
    void postprocess_delegate<number>::zeta_threepf(const threepf_kconfig& kconfig, double t, const std::vector<number>& threepf,
                                            const std::vector<number>& k1_re, const std::vector<number>& k1_im,
                                            const std::vector<number>& k2_re, const std::vector<number>& k2_im,
                                            const std::vector<number>& k3_re, const std::vector<number>& k3_im,
                                            const std::vector<number>& bg,
                                            number& zeta_threepf, number& redbsp, std::vector<number>& gauge_xfm1,
                                            std::vector<number>& gauge_xfm2_123, std::vector<number>& gauge_xfm2_213, std::vector<number>& gauge_xfm2_312)
      {
        zeta_threepf = 0.0;

        // compute linear gauge transformation coefficients for this time, making use of user-supplied cache which should be correctly sized
        this->mdl->compute_gauge_xfm_1(this->parent_task, bg, gauge_xfm1);

        // compute quadratic gauge transformation coefficients, making use of user-supplied caches which should be correctly sized
        this->mdl->compute_gauge_xfm_2(this->parent_task, bg, kconfig.k1_comoving, kconfig.k2_comoving, kconfig.k3_comoving, t, gauge_xfm2_123);
        this->mdl->compute_gauge_xfm_2(this->parent_task, bg, kconfig.k2_comoving, kconfig.k1_comoving, kconfig.k3_comoving, t, gauge_xfm2_213);
        this->mdl->compute_gauge_xfm_2(this->parent_task, bg, kconfig.k3_comoving, kconfig.k1_comoving, kconfig.k2_comoving, t, gauge_xfm2_312);

        // compute linear part of gauge transformation
        for(unsigned int l = 0; l < 2*this->Nfields; ++l)
          {
            for(unsigned int m = 0; m < 2*this->Nfields; ++m)
              {
                for(unsigned int n = 0; n < 2*this->Nfields; ++n)
                  {
                    number tpf = threepf[this->mdl->flatten(l,m,n)];

                    // shift tpf so it represents a derivative correlation function (if any of l, m, n are momenta), not a momentum one
                    this->shift(l, m, n, kconfig, t, k1_re, k1_im, k2_re, k2_im, k3_re, k3_im, bg, tpf);

                    zeta_threepf += gauge_xfm1[l]*gauge_xfm1[m]*gauge_xfm1[n]*tpf;
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
                        number component1 = gauge_xfm2_123[this->mdl->flatten(l,m)]*gauge_xfm1[p]*gauge_xfm1[q]*(k2_re[this->mdl->flatten(l,p)]*k3_re[this->mdl->flatten(m,q)] - k2_im[this->mdl->flatten(l,p)]*k3_im[this->mdl->flatten(m,q)]);
                        number component2 = gauge_xfm2_213[this->mdl->flatten(l,m)]*gauge_xfm1[p]*gauge_xfm1[q]*(k1_re[this->mdl->flatten(l,p)]*k3_re[this->mdl->flatten(m,q)] - k1_im[this->mdl->flatten(l,p)]*k3_im[this->mdl->flatten(m,q)]);
                        number component3 = gauge_xfm2_312[this->mdl->flatten(l,m)]*gauge_xfm1[p]*gauge_xfm1[q]*(k1_re[this->mdl->flatten(l,p)]*k2_re[this->mdl->flatten(m,q)] - k1_im[this->mdl->flatten(l,p)]*k2_im[this->mdl->flatten(m,q)]);

                        zeta_threepf += component1;
                        zeta_threepf += component2;
                        zeta_threepf += component3;
                      }
                  }
              }
          }

        // compute reduced bispectrum
        number z_tpf_k1;
        number z_tpf_k2;
        number z_tpf_k3;
        this->zeta_twopf(k1_re, bg, z_tpf_k1, gauge_xfm1, true);  // 'true' flags that the gauge transformation is precomputed, so there is no need to recompute it
        this->zeta_twopf(k2_re, bg, z_tpf_k2, gauge_xfm1, true);  // 'true' flags that the gauge transformation is precomputed, so there is no need to recompute it
        this->zeta_twopf(k3_re, bg, z_tpf_k3, gauge_xfm1, true);  // 'true' flags that the gauge transformation is precomputed, so there is no need to recompute it

        number form_factor = (6.0/5.0) * (z_tpf_k1*z_tpf_k2 + z_tpf_k1*z_tpf_k3 + z_tpf_k2*z_tpf_k3);
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
                                       k1_re, k1_im, k2_re, k2_im, k3_re, k3_im, bg,
                                       derived_data::derived_data_impl::operator_position::left_pos);

        if(this->mdl->is_momentum(m))
          value += this->compute_shift(t, m, l, n, kconfig.k2_comoving, kconfig.k1_comoving, kconfig.k3_comoving,
                                       k2_re, k2_im, k1_re, k1_im, k3_re, k3_im, bg,
                                       derived_data::derived_data_impl::operator_position::middle_pos);

        if(this->mdl->is_momentum(n))
          value += this->compute_shift(t, n, l, m, kconfig.k3_comoving, kconfig.k1_comoving, kconfig.k2_comoving,
                                       k3_re, k3_im, k1_re, k1_im, k2_re, k2_im, bg,
                                       derived_data::derived_data_impl::operator_position::middle_pos);
      }


    template <typename number>
    number postprocess_delegate<number>::compute_shift(double t, unsigned int p, unsigned int q, unsigned int r,
                                               double p_comoving, double q_comoving, double r_comoving,
                                               const std::vector<number>& p_re, const std::vector<number>& p_im,
                                               const std::vector<number>& q_re, const std::vector<number>& q_im,
                                               const std::vector<number>& r_re, const std::vector<number>& r_im,
                                               const std::vector<number>& bg, derived_data::derived_data_impl::operator_position pos)
      {
        this->mdl->B(this->parent_task, bg, q_comoving, r_comoving, p_comoving, t, this->B_qrp);
        this->mdl->C(this->parent_task, bg, p_comoving, q_comoving, r_comoving, t, this->C_pqr);
        this->mdl->C(this->parent_task, bg, p_comoving, r_comoving, q_comoving, t, this->C_prq);

        number shift = 0.0;
        unsigned int p_species = this->mdl->species(p);

        // we need to know which index on each twopf is fixed, and which is summed over
        derived_data::derived_data_impl::fixed_index q_fixed = derived_data::derived_data_impl::fixed_index::first_index;
        derived_data::derived_data_impl::fixed_index r_fixed = derived_data::derived_data_impl::fixed_index::first_index;

        switch(pos)
          {
            case derived_data::derived_data_impl::operator_position::left_pos:    // our operator is on the far left-hand side, so is to the left of both the q and r operators
              q_fixed = derived_data::derived_data_impl::fixed_index::second_index;
              r_fixed = derived_data::derived_data_impl::fixed_index::second_index;
              break;

            case derived_data::derived_data_impl::operator_position::middle_pos:  // our operator is in the middle, to the right of the q operator but to the left of the r operator
              q_fixed = derived_data::derived_data_impl::fixed_index::first_index;
              r_fixed = derived_data::derived_data_impl::fixed_index::second_index;
              break;

            case derived_data::derived_data_impl::operator_position::right_pos:   // our operator is on the right, to the right of both the q and r operators
              q_fixed = derived_data::derived_data_impl::fixed_index::first_index;
              r_fixed = derived_data::derived_data_impl::fixed_index::first_index;
              break;
          }

        for(unsigned int m = 0; m < this->Nfields; ++m)
          {
            for(unsigned int n = 0; n < this->Nfields; ++n)
              {
                unsigned int q_m_id = this->mdl->flatten((q_fixed == derived_data::derived_data_impl::fixed_index::first_index ? q : m), (q_fixed == derived_data::derived_data_impl::fixed_index::second_index ? q : m));
                unsigned int q_n_id = this->mdl->flatten((q_fixed == derived_data::derived_data_impl::fixed_index::first_index ? q : n), (q_fixed == derived_data::derived_data_impl::fixed_index::second_index ? q : n));

                unsigned int r_n_id = this->mdl->flatten((r_fixed == derived_data::derived_data_impl::fixed_index::first_index ? r : n), (r_fixed == derived_data::derived_data_impl::fixed_index::second_index ? r : n));

                unsigned int mom_q_m_id = this->mdl->flatten((q_fixed == derived_data::derived_data_impl::fixed_index::first_index ? this->mdl->momentum(q) : m), (q_fixed == derived_data::derived_data_impl::fixed_index::second_index ? this->mdl->momentum(q) : m));
                unsigned int mom_r_m_id = this->mdl->flatten((r_fixed == derived_data::derived_data_impl::fixed_index::first_index ? this->mdl->momentum(r) : m), (r_fixed == derived_data::derived_data_impl::fixed_index::second_index ? this->mdl->momentum(r) : m));

                shift -= this->B_qrp[this->mdl->fields_flatten(m,n,p_species)] * ( q_re[q_m_id]*r_re[r_n_id] - q_im[q_m_id]*r_im[r_n_id] );

                shift -= this->C_pqr[this->mdl->fields_flatten(p_species,m,n)] * ( q_re[mom_q_m_id]*r_re[r_n_id] - q_im[mom_q_m_id]*r_im[r_n_id] );
                shift -= this->C_prq[this->mdl->fields_flatten(p_species,m,n)] * ( q_re[q_n_id]*r_re[mom_r_m_id] - q_im[q_n_id]*r_re[mom_r_m_id] );
              }
          }

        return(shift);
      }


  }   // namespace transport


#endif //__zeta_compute_H_
