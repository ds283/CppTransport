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


namespace transport
  {

    template <typename number>
    class zeta_compute
      {

      public:

        typedef enum { left, middle, right } operator_position;

        // CONSTRUCTOR, DESTRUCTOR

      public:

        zeta_compute(model<number>* m, twopf_list_task<number>* tk);

        ~zeta_compute() = default;


        // INTERFACE

      public:

        //! compute the zeta twopf
        void zeta_twopf(const std::vector<number>& twopf, const std::vector<number>& bg, number& zeta_twopf, number& single_src);

        //! compute the zeta threepf and reduced bispectrum
        void zeta_threepf(const threepf_kconfig& kconfig, double t, const std::vector<number>& threepf,
                          const std::vector<number>& k1_re, const std::vector<number>& k1_im,
                          const std::vector<number>& k2_re, const std::vector<number>& k2_im,
                          const std::vector<number>& k3_re, const std::vector<number>& k3_im,
                          const std::vector<number>& bg,
                          number& zeta_threepf, number& redbsp, number& single_src);

      protected:

        //! compute the zeta twopf -- internal version
		    //! if 'precomputed' is set, we avoid computing the gauge transformation and use a cached version
        void zeta_twopf(const std::vector<number>& twopf, const std::vector<number>& bg,
                        number& zeta_twopf, number& single_src, bool precomputed);


        // INTERNAL API

      protected:

		    //! shift a component of a threepf from momenta to derivatives
        void shift(unsigned int l, unsigned int m, unsigned int n,
                   const threepf_kconfig& kconfig, double t, const std::vector<number>& threepf,
                   const std::vector<number>& k1_re, const std::vector<number>& k1_im,
                   const std::vector<number>& k2_re, const std::vector<number>& k2_im,
                   const std::vector<number>& k3_re, const std::vector<number>& k3_im,
                   const std::vector<number>& bg,
                   number value);

		    //! compute the shift from momenta to derivatives
        number compute_shift(double t, unsigned int p, unsigned int q, unsigned int r,
                             double p_comoving, double q_comoving, double r_comoving,
                             const std::vector<number>& p_re, const std::vector<number>& p_im,
                             const std::vector<number>& q_re, const std::vector<number>& q_im,
                             const std::vector<number>& r_re, const std::vector<number>& r_im,
                             const std::vector<number>& bg, operator_position pos);


        // INTERNAL DATA

      protected:

        //! cache number of fields
        unsigned int Nfields;

        //! cache pointer to model instance
        model<number>* mdl;

        //! cache point to task instance
        twopf_list_task<number>* parent_task;


        //! First-order gauge xfm cache
        std::vector<number> dN;

        //! Second-order gauge xfm cache - k1, k2, k3 order
        std::vector< std::vector<number> > ddN123;

        //! Second-order gauge xfm cache - k2, k1, k3 order
        std::vector< std::vector<number> > ddN213;

        //! Second-order gauge xfm cache - k3, k1, k2 order
        std::vector< std::vector<number> > ddN312;

        //! B-tensor cache
        std::vector< std::vector< std::vector<number> > > B_qrp;

        //! C-tensor cache (need two, because need two different index arrangements)
        std::vector< std::vector< std::vector<number> > > C_pqr;

        //! C-tensor cache (need two, because need two different index arrangements)
        std::vector< std::vector< std::vector<number> > > C_prq;

      };


    template <typename number>
    zeta_compute<number>::zeta_compute(model<number> *m, twopf_list_task<number>* tk)
      : mdl(m),
        Nfields(m->get_N_fields()),
        parent_task(tk)
      {
        assert(this->mdl != nullptr);
        assert(this->parent_task != nullptr);
      }


		template <typename number>
		void zeta_compute<number>::zeta_twopf(const std::vector<number>& twopf, const std::vector<number>& bg, number& zeta_twopf, number& single_src)
			{
        this->zeta_twopf(twopf, bg, zeta_twopf, single_src, false);
			}


    template <typename number>
    void zeta_compute<number>::zeta_twopf(const std::vector<number>& twopf, const std::vector<number>& bg, number& zeta_twopf, number& single_src, bool precomputed)
      {
        zeta_twopf = 0.0;

        if(!precomputed) this->mdl->compute_gauge_xfm_1(this->parent_task, bg, this->dN);

        // compute twopf
        for(unsigned int m = 0; m < 2*this->Nfields; ++m)
          {
            for(unsigned int n = 0; n < 2*this->Nfields; ++n)
              {
                number component = dN[m]*dN[n]*twopf[this->mdl->flatten(m, n)];
                zeta_twopf += component;
              }
          }

        // compute fraction attributable to a single field
        single_src = 0.0;
        for(unsigned int m = 0; m < this->Nfields; ++m)
          {
            unsigned int species  = this->mdl->species(m);
            unsigned int momentum = this->mdl->momentum(m);
            number field_contrib  = dN[species]*dN[species]*twopf[this->mdl->flatten(species,species)]
                                    + dN[momentum]*dN[momentum]*twopf[this->mdl->flatten(momentum,momentum)];
            number field_fraction = field_contrib/zeta_twopf;

            if(field_fraction > single_src) single_src = field_fraction;
          }
      }


    template <typename number>
    void zeta_compute<number>::zeta_threepf(const threepf_kconfig& kconfig, double t, const std::vector<number>& threepf,
                                            const std::vector<number>& k1_re, const std::vector<number>& k1_im,
                                            const std::vector<number>& k2_re, const std::vector<number>& k2_im,
                                            const std::vector<number>& k3_re, const std::vector<number>& k3_im,
                                            const std::vector<number>& bg,
                                            number& zeta_threepf, number& redbsp, number& single_src)
      {
        zeta_threepf = 0.0;

        // compute gauge transformation coefficients for this time
        this->mdl->compute_gauge_xfm_1(this->parent_task, bg, this->dN);

        this->mdl->compute_gauge_xfm_2(this->parent_task, bg, kconfig.k1_comoving, kconfig.k2_comoving, kconfig.k3_comoving, t, this->ddN123);
        this->mdl->compute_gauge_xfm_2(this->parent_task, bg, kconfig.k2_comoving, kconfig.k1_comoving, kconfig.k3_comoving, t, this->ddN213);
        this->mdl->compute_gauge_xfm_2(this->parent_task, bg, kconfig.k3_comoving, kconfig.k1_comoving, kconfig.k2_comoving, t, this->ddN312);

        // compute linear part of gauge transformation
        for(unsigned int l = 0; l < 2*this->Nfields; ++l)
          {
            for(unsigned int m = 0; m < 2*this->Nfields; ++m)
              {
                for(unsigned int n = 0; n < 2*this->Nfields; ++n)
                  {
                    number tpf = threepf[this->mdl->flatten(l,m,n)];

                    // shift tpf so it represents a derivative correlation function (if any of l, m, n are momenta), not a momentum one
                    this->shift(l, m, n, kconfig, t, threepf, k1_re, k1_im, k2_re, k2_im, k3_re, k3_im, bg, tpf);

                    number component = dN[l]*dN[m]*dN[n]*tpf;

                    zeta_threepf += component;
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
                        number component1 = ddN123[l][m]*dN[p]*dN[q]*(k2_re[this->mdl->flatten(l,p)]*k3_re[this->mdl->flatten(m,q)] - k2_im[this->mdl->flatten(l,p)]*k3_im[this->mdl->flatten(m,q)]);
                        number component2 = ddN213[l][m]*dN[p]*dN[q]*(k1_re[this->mdl->flatten(l,p)]*k3_re[this->mdl->flatten(m,q)] - k1_im[this->mdl->flatten(l,p)]*k3_im[this->mdl->flatten(m,q)]);
                        number component3 = ddN312[l][m]*dN[p]*dN[q]*(k1_re[this->mdl->flatten(l,p)]*k2_re[this->mdl->flatten(m,q)] - k1_im[this->mdl->flatten(l,p)]*k2_im[this->mdl->flatten(m,q)]);

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
        number scratch;
        this->zeta_twopf(k1_re, bg, z_tpf_k1, scratch, true);  // 'true' flags that the gauge transformation is precomputed, so there is no need to recompute it
        this->zeta_twopf(k2_re, bg, z_tpf_k2, scratch, true);  // 'true' flags that the gauge transformation is precomputed, so there is no need to recompute it
        this->zeta_twopf(k3_re, bg, z_tpf_k3, scratch, true);  // 'true' flags that the gauge transformation is precomputed, so there is no need to recompute it

        number form_factor = (6.0/5.0) * (z_tpf_k1*z_tpf_k2 + z_tpf_k1*z_tpf_k3 + z_tpf_k2*z_tpf_k3);
        redbsp = zeta_threepf / form_factor;

        // compute fraction attributable to a single field
        single_src = 0.0;
        for(unsigned int m = 0; m < this->Nfields; ++m)
          {
            unsigned int species  = this->mdl->species(m);
            unsigned int momentum = this->mdl->momentum(m);

            number tpf_field   = threepf[this->mdl->flatten(species,species,species)];
            number tpf_momenta = threepf[this->mdl->flatten(momentum,momentum,momentum)];

            // shift tpf_momenta so it represents a derivative correlation function, not a momentum one
            this->shift(momentum, momentum, momentum, kconfig, t, threepf, k1_re, k1_im, k2_re, k2_im, k3_re, k3_im, bg, tpf_momenta);

            // first, contribution from linear part of the gauge transformation
            number field_contrib  = dN[species]*dN[species]*dN[species]*tpf_field
                                    + dN[momentum]*dN[momentum]*dN[momentum]*tpf_momenta;

            // second, contribution from quadratic part of the gauge transformation
            number component1 = ddN123[species][species]*dN[species]*dN[species]*(k2_re[this->mdl->flatten(species,species)]*k3_re[this->mdl->flatten(species,species)] - k2_im[this->mdl->flatten(species,species)]*k3_im[this->mdl->flatten(species,species)]);
            number component2 = ddN213[species][species]*dN[species]*dN[species]*(k1_re[this->mdl->flatten(species,species)]*k3_re[this->mdl->flatten(species,species)] - k1_im[this->mdl->flatten(species,species)]*k3_im[this->mdl->flatten(species,species)]);
            number component3 = ddN312[species][species]*dN[species]*dN[species]*(k1_re[this->mdl->flatten(species,species)]*k2_re[this->mdl->flatten(species,species)] - k1_im[this->mdl->flatten(species,species)]*k2_im[this->mdl->flatten(species,species)]);

            number component4 = ddN123[momentum][momentum]*dN[momentum]*dN[momentum]*(k2_re[this->mdl->flatten(momentum,momentum)]*k3_re[this->mdl->flatten(momentum,momentum)] - k2_im[this->mdl->flatten(momentum,momentum)]*k3_im[this->mdl->flatten(momentum,momentum)]);
            number component5 = ddN213[momentum][momentum]*dN[momentum]*dN[momentum]*(k1_re[this->mdl->flatten(momentum,momentum)]*k3_re[this->mdl->flatten(momentum,momentum)] - k1_im[this->mdl->flatten(momentum,momentum)]*k3_im[this->mdl->flatten(momentum,momentum)]);
            number component6 = ddN312[momentum][momentum]*dN[momentum]*dN[momentum]*(k1_re[this->mdl->flatten(momentum,momentum)]*k2_re[this->mdl->flatten(momentum,momentum)] - k1_im[this->mdl->flatten(momentum,momentum)]*k2_im[this->mdl->flatten(momentum,momentum)]);

            field_contrib += component1 + component2 + component3 + component4 + component5 + component6;

            number field_fraction = field_contrib/zeta_threepf;

            if(field_fraction > single_src) single_src = field_fraction;
          }
      }


    template <typename number>
    void zeta_compute<number>::shift(unsigned int l, unsigned int m, unsigned int n,
                                     const threepf_kconfig& kconfig, double t, const std::vector<number>& threepf,
                                     const std::vector<number>& k1_re, const std::vector<number>& k1_im,
                                     const std::vector<number>& k2_re, const std::vector<number>& k2_im,
                                     const std::vector<number>& k3_re, const std::vector<number>& k3_im,
                                     const std::vector<number>& bg,
                                     number value)
      {
        if(this->mdl->is_momentum(l)) value += this->compute_shift(t, l, m, n, kconfig.k1_comoving, kconfig.k2_comoving, kconfig.k3_comoving,
                                                                   k1_re, k1_im, k2_re, k2_im, k3_re, k3_im, bg, left);
        if(this->mdl->is_momentum(m)) value += this->compute_shift(t, m, l, n, kconfig.k2_comoving, kconfig.k1_comoving, kconfig.k3_comoving,
                                                                   k2_re, k2_im, k1_re, k1_im, k3_re, k3_im, bg, middle);
        if(this->mdl->is_momentum(n)) value += this->compute_shift(t, n, l, m, kconfig.k3_comoving, kconfig.k1_comoving, kconfig.k2_comoving,
                                                                   k3_re, k3_im, k1_re, k1_im, k2_re, k2_im, bg, middle);
      }


    template <typename number>
    number zeta_compute<number>::compute_shift(double t, unsigned int p, unsigned int q, unsigned int r,
                                               double p_comoving, double q_comoving, double r_comoving,
                                               const std::vector<number>& p_re, const std::vector<number>& p_im,
                                               const std::vector<number>& q_re, const std::vector<number>& q_im,
                                               const std::vector<number>& r_re, const std::vector<number>& r_im,
                                               const std::vector<number>& bg, operator_position pos)
      {
        this->mdl->B(this->parent_task, bg, q_comoving, r_comoving, p_comoving, t, this->B_qrp);
        this->mdl->C(this->parent_task, bg, p_comoving, q_comoving, r_comoving, t, this->C_pqr);
        this->mdl->C(this->parent_task, bg, p_comoving, r_comoving, q_comoving, t, this->C_prq);

        number shift = 0.0;
        unsigned int p_species = this->mdl->species(p);

        typedef enum { first_index, second_index } fixed_index;

        // we need to know which index on each twopf is fixed, and which is summed over
        fixed_index q_fixed = first_index;
        fixed_index r_fixed = first_index;

        switch(pos)
          {
            case left:    // our operator is on the far left-hand side, so is to the left of both the q and r operators
              q_fixed = second_index;
              r_fixed = second_index;
              break;

            case middle:  // our operator is in the middle, to the right of the q operator but to the left of the r operator
              q_fixed = first_index;
              r_fixed = second_index;
              break;

            case right:   // our operator is on the right, to the right of both the q and r operators
              q_fixed = first_index;
              r_fixed = first_index;
              break;

            default:
              assert(false);
          }

        for(unsigned int m = 0; m < this->Nfields; ++m)
          {
            for(unsigned int n = 0; n < this->Nfields; ++n)
              {
                unsigned int q_m_id = this->mdl->flatten((q_fixed == first_index ? q : m), (q_fixed == second_index ? q : m));
                unsigned int q_n_id = this->mdl->flatten((q_fixed == first_index ? q : n), (q_fixed == second_index ? q : n));

                unsigned int r_n_id = this->mdl->flatten((r_fixed == first_index ? r : n), (r_fixed == second_index ? r : n));

                unsigned int mom_q_m_id = this->mdl->flatten((q_fixed == first_index ? this->mdl->momentum(q) : m), (q_fixed == second_index ? this->mdl->momentum(q) : m));
                unsigned int mom_r_m_id = this->mdl->flatten((r_fixed == first_index ? this->mdl->momentum(r) : m), (r_fixed == second_index ? this->mdl->momentum(r) : m));

                shift -= this->B_qrp[m][n][p_species] * ( q_re[q_m_id]*r_re[r_n_id] - q_im[q_m_id]*r_im[r_n_id] );

                shift -= this->C_pqr[p_species][m][n] * ( q_re[mom_q_m_id]*r_re[r_n_id] - q_im[mom_q_m_id]*r_im[r_n_id] );
                shift -= this->C_prq[p_species][m][n] * ( q_re[q_n_id]*r_re[mom_r_m_id] - q_im[q_n_id]*r_re[mom_r_m_id] );
              }
          }

        return(shift);
      }


  }   // namespace transport


#endif //__zeta_compute_H_
