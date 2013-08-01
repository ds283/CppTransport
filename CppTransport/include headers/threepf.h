//
// Created by David Seery on 30/06/2013.
// Copyright (c) 2013 University of Sussex. All rights reserved.
//
// To change the template use AppCode | Preferences | File Templates.
//

#ifndef __CPP_TRANSPORT_THREEPF_H_
#define __CPP_TRANSPORT_THREEPF_H_

#include <string>
#include <sstream>
#include <vector>
#include <array>

#include <assert.h>
#include <math.h>

#include "asciitable.h"
#include "messages_en.h"

#include "plot_gadget.h"
#include "gauge_xfm_gadget.h"

#define THREEPF_SYMBOL             "\\alpha"
#define ZETA_SYMBOL                "\\zeta"
#define PRIME_SYMBOL               "\\prime"

#define KT_SYMBOL                  "k_t"
#define FLS_ALPHA_SYMBOL           "\\alpha"
#define FLS_BETA_SYMBOL            "\\beta"

#define DEFAULT_THREEPF_WRAP_WIDTH (135)
#define DEFAULT_OUTPUT_DOTPHI      (true)

namespace transport
  {
      // DATA PRODUCTS -- objects wrapping the various data products produced by each model

      // handle weirdness with friend template functions
      // see http://www.cplusplus.com/forum/general/45776/
      template <typename number> class threepf;

      template <typename number>
      std::ostream& operator<<(std::ostream& out, threepf<number>& obj);

      struct threepf_kconfig
        {
          std::array<unsigned int, 3> indices;

          double                      k_t;
          double                      alpha;
          double                      beta;
        };

      template <typename number>
      class threepf
        {
          public:
            threepf(unsigned int N_f, const std::vector<std::string>& f_names, const std::vector<std::string>& l_names,
              const std::vector<double>& ks, const std::vector<double>& com_ks, double Nst,
              const std::vector<number>& sp, const std::vector< std::vector<number> >& b,
              const std::vector< std::vector< std::vector<number> > >& tpf_re,
              const std::vector< std::vector< std::vector<number> > >& tpf_im,
              const std::vector< std::vector< std::vector<number> > >& thpf,
              const std::vector< struct threepf_kconfig >& kl,
              gauge_xfm_gadget<number>* gx, tensor_gadget<number>* t)
              : N_fields(N_f), field_names(f_names), latex_names(l_names),
                Nstar(Nst), sample_points(sp), sample_ks(ks), sample_com_ks(com_ks),
                backg(N_f, f_names, l_names, sp, b, t->clone()),
                twopf_re(N_f, f_names, l_names, ks, com_ks, Nst, sp, b, tpf_re, gx->clone(), t->clone()),
                twopf_im(N_f, f_names, l_names, ks, com_ks, Nst, sp, b, tpf_im, gx->clone(), t->clone()),
                samples(thpf), kconfig_list(kl),
                gauge_xfm(gx), tensors(t),
                wrap_width(DEFAULT_THREEPF_WRAP_WIDTH), output_dotphi(DEFAULT_OUTPUT_DOTPHI)
              {}
            ~threepf() { /*delete this->gauge_xfm; delete this->tensors;*/ }

            background<number>& get_background();
            twopf<number>&      get_real_twopf();
            twopf<number>&      get_imag_twopf();

            void components_time_history(plot_gadget<number>* gadget, std::string output,
              index_selector<3>* selector, std::string format = "pdf", bool logy=true);

            void components_dotphi_time_history(plot_gadget<number>* gadget, std::string output,
              index_selector<3>* selector, std::string format = "pdf", bool logy=true);

            void zeta_time_history(plot_gadget<number>* gadget, std::string output,
              std::string format = "pdf", bool logy=true);

            index_selector<3>* manufacture_selector();

            // provide << operator to output data to a stream
            friend std::ostream& operator<< <>(std::ostream& out, threepf& obj);

            unsigned int get_wrap_width();
            void         set_wrap_width(unsigned int w);

          protected:

            // make a list of labels for the chosen index selection
            std::vector< std::string>          make_labels(index_selector<3>* selector, bool latex);

            // return a time history for a given set of components and a fixed k-configuration
            std::vector< std::vector<number> > construct_kconfig_time_history(index_selector<3>* selector, unsigned int i);

            // return a time history for correlation functions of \dot{\phi}_i (rather than the canonical momentum p_i)
            // for a given set of components and fixed k-configuration
            std::vector< std::vector<number> > construct_kconfig_dotphi_time_history(index_selector<3>* selector, unsigned int i);

            // compute the shift in the correlation function when changing from momentum to d/dN
            double dotphi_shift(unsigned int __m, unsigned int __kmode_m, unsigned int __n, unsigned int __kmode_n,
                                unsigned int __r, unsigned int __kmode_r, unsigned int __time_slice, unsigned int __pos);
          
            unsigned int                                            N_fields;          // number of fields
            const std::vector<std::string>                          field_names;       // vector of names - includes momenta
            const std::vector<std::string>                          latex_names;       // vector of LaTeX names - excludes momenta

            gauge_xfm_gadget<number>*                               gauge_xfm;         // gauge transformation gadget
          
            tensor_gadget<number>*                                  tensors;           // tensor calculation gadget

            const double                                            Nstar;             // when was horizon-crossing for the mode k=1?

            const std::vector<double>                               sample_points;     // list of times at which we hold samples
            const std::vector<double>                               sample_ks;         // list of ks for which we hold samples,
                                                                                       // normalized to horizon crossing
            const std::vector<double>                               sample_com_ks;     // list of ks, in comoving units

            background<number>                                      backg;             // container for background
            twopf<number>                                           twopf_re;          // container for real 2pf
            twopf<number>                                           twopf_im;          // container for imaginary 2pf
            const std::vector< std::vector< std::vector<number> > > samples;           // list of samples of 3pf
              // first index: time of observation
              // second index: 2pf component
              // third index: k (ordered according to 'kconfig_list' below)
            const std::vector< struct threepf_kconfig >             kconfig_list;      // list of k-configurations,
                                                                                       // in the same order as third index of 'samples'

            unsigned int                                            wrap_width;        // position to wrap when output to stream
            bool                                                    output_dotphi;     // output correlation funtions of dot(phi), or the canonical momentum?
        };


//  IMPLEMENTATION -- CLASS threepf

      template <typename number>
      void threepf<number>::components_time_history(plot_gadget<number>* gadget, std::string output,
        index_selector<3>* selector, std::string format, bool logy)
        {
          std::vector< std::string > labels = this->make_labels(selector, gadget->latex_labels());

          // loop over all combinations of k-modes
          for(int i = 0; i < this->kconfig_list.size(); i++)
            {
              std::vector< std::vector<number> > data = this->construct_kconfig_time_history(selector, i);

              std::ostringstream fnam;
              fnam << output << "_" << i;

              std::ostringstream title;
              title << "$" << KT_SYMBOL        << " = " << this->kconfig_list[i].k_t   << "$, "
                    << "$" << FLS_ALPHA_SYMBOL << " = " << this->kconfig_list[i].alpha << "$, "
                    << "$" << FLS_BETA_SYMBOL  << " = " << this->kconfig_list[i].beta  << "$";

              gadget->set_format(format);
              gadget->plot(fnam.str(), title.str(), this->sample_points, data, labels, "$N$", "three-point function", false, logy);
            }
        }


      template <typename number>
      void threepf<number>::components_dotphi_time_history(plot_gadget<number>* gadget, std::string output,
                                                           index_selector<3>* selector, std::string format, bool logy)
        {
          std::vector< std::string > labels = this->make_labels(selector, gadget->latex_labels());

          // loop over all combinations of k-modes
          for(int i = 0; i < this->kconfig_list.size(); i++)
            {
              std::vector< std::vector<number> > data = this->construct_kconfig_dotphi_time_history(selector, i);

              std::ostringstream fnam;
              fnam << output << "_" << i;

              std::ostringstream title;
              title << "$" << KT_SYMBOL        << " = " << this->kconfig_list[i].k_t   << "$, "
                << "$" << FLS_ALPHA_SYMBOL << " = " << this->kconfig_list[i].alpha << "$, "
                << "$" << FLS_BETA_SYMBOL  << " = " << this->kconfig_list[i].beta  << "$";

              gadget->set_format(format);
              gadget->plot(fnam.str(), title.str(), this->sample_points, data, labels, "$N$", "three-point function", false, logy);
            }
        }


      template <typename number>
      void threepf<number>::zeta_time_history(plot_gadget<number>* gadget, std::string output,
        std::string format, bool logy)
        {
          // loop over all combinations of k-modes
          for(int i = 0; i < this->kconfig_list.size(); i++)
            {
              std::vector< std::vector<number> > data(this->sample_points.size());

              for(int j = 0; j < this->sample_points.size(); j++)
                {
                  data[j].resize(1);    // only one components of < zeta zeta zeta >

                  // compute gauge transformations
                  std::vector<number> dN;
                  std::vector< std::vector<number> > ddN;

                  this->gauge_xfm->compute_gauge_xfm_1(this->backg.get_value(j), dN);
                  this->gauge_xfm->compute_gauge_xfm_2(this->backg.get_value(j), ddN);

                  // get twopf values for this timeslices and appropriate k-modes
                  std::vector<number> twopf_re_k1 = this->twopf_re.get_value(j, kconfig_list[i].indices[0]);
                  std::vector<number> twopf_im_k1 = this->twopf_im.get_value(j, kconfig_list[i].indices[0]);
                  std::vector<number> twopf_re_k2 = this->twopf_re.get_value(j, kconfig_list[i].indices[1]);
                  std::vector<number> twopf_im_k2 = this->twopf_im.get_value(j, kconfig_list[i].indices[1]);
                  std::vector<number> twopf_re_k3 = this->twopf_re.get_value(j, kconfig_list[i].indices[2]);
                  std::vector<number> twopf_im_k3 = this->twopf_im.get_value(j, kconfig_list[i].indices[2]);

                  // intrinsic threepf
                  data[j][0] = 0;
                  for(int m = 0; m < 2*this->N_fields; m++)
                    {
                      for(int n = 0; n < 2*this->N_fields; n++)
                        {
                          for(int r = 0; r < 2*this->N_fields; r++)
                            {
                              unsigned int samples_index = (2*this->N_fields*2*this->N_fields)*m + (2*this->N_fields)*n + r;
                              data[j][0] += dN[m]*dN[n]*dN[r]*this->samples[j][samples_index][i];
                            }
                        }
                    }

                  // gauge transformation contribution
                  for(int l = 0; l < 2*this->N_fields; l++)
                    {
                      for(int m = 0; m < 2*this->N_fields; m++)
                        {
                          for(int n = 0; n < 2*this->N_fields; n++)
                            {
                              for(int r = 0; r < 2*this->N_fields; r++)
                                {
                                  // l, m to left of n and r
                                  unsigned int ln_1_index = (2*this->N_fields)*l + n;
                                  unsigned int lr_1_index = (2*this->N_fields)*l + r;
                                  unsigned int mn_1_index = (2*this->N_fields)*m + n;
                                  unsigned int mr_1_index = (2*this->N_fields)*m + r;

                                  // l, m to left of r but right of n
                                  unsigned int ln_2_index = (2*this->N_fields)*n + l;
                                  unsigned int lr_2_index = (2*this->N_fields)*l + r;
                                  unsigned int mn_2_index = (2*this->N_fields)*n + m;
                                  unsigned int mr_2_index = (2*this->N_fields)*l + r;

                                  // l, m to right of n and r
                                  unsigned int ln_3_index = (2*this->N_fields)*n + l;
                                  unsigned int lr_3_index = (2*this->N_fields)*r + l;
                                  unsigned int mn_3_index = (2*this->N_fields)*n + m;
                                  unsigned int mr_3_index = (2*this->N_fields)*r + m;

                                  data[j][0] += (1.0/2.0) * ddN[l][m]*dN[n]*dN[r]*(  twopf_re_k2[ln_1_index]*twopf_re_k3[mr_1_index]
                                                                                   + twopf_re_k2[lr_1_index]*twopf_re_k3[mn_1_index]
                                                                                   - twopf_im_k2[ln_1_index]*twopf_im_k3[mr_1_index]
                                                                                   - twopf_im_k2[lr_1_index]*twopf_im_k3[mn_1_index]);

                                  data[j][0] += (1.0/2.0) * ddN[l][m]*dN[n]*dN[r]*(  twopf_re_k1[ln_2_index]*twopf_re_k3[mr_2_index]
                                                                                   + twopf_re_k1[lr_2_index]*twopf_re_k3[mn_2_index]
                                                                                   - twopf_im_k1[ln_2_index]*twopf_im_k3[mr_2_index]
                                                                                   - twopf_im_k1[lr_2_index]*twopf_im_k3[mn_2_index]);

                                  data[j][0] += (1.0/2.0) * ddN[l][m]*dN[n]*dN[r]*(  twopf_re_k1[ln_3_index]*twopf_re_k2[mr_3_index]
                                                                                   + twopf_re_k1[lr_3_index]*twopf_re_k2[mn_3_index]
                                                                                   - twopf_im_k1[ln_3_index]*twopf_im_k2[mr_3_index]
                                                                                   - twopf_im_k1[lr_3_index]*twopf_im_k2[mn_3_index]);
                                }
                            }
                        }
                    }
                }

              std::ostringstream fnam;
              fnam << output << "_" << i;

              std::ostringstream title;
              title << "$" << KT_SYMBOL        << " = " << this->kconfig_list[i].k_t   << "$, "
                << "$" << FLS_ALPHA_SYMBOL << " = " << this->kconfig_list[i].alpha << "$, "
                << "$" << FLS_BETA_SYMBOL  << " = " << this->kconfig_list[i].beta  << "$";

              std::vector<std::string> labels(1);
              std::ostringstream l;

              l << "$" << THREEPF_SYMBOL << "_{" << ZETA_SYMBOL << " " << ZETA_SYMBOL << " " << ZETA_SYMBOL << "}$";
              labels[0] = l.str();

              gadget->set_format(format);
              gadget->plot(fnam.str(), title.str(), this->sample_points, data, labels, "$N$", "three-point function", false, logy);
            }
        }


      template <typename number>
      std::vector< std::string > threepf<number>::make_labels(index_selector<3>* selector, bool latex)
      {
        std::vector< std::string > labels;

        for(int m = 0; m < 2*this->N_fields; m++)
          {
            for(int n = 0; n < 2*this->N_fields; n++)
              {
                for(int r = 0; r < 2*this->N_fields; r++)
                  {
                    std::array<unsigned int, 3> index_set = { (unsigned int)m, (unsigned int)n, (unsigned int)r };
                    if(selector->is_on(index_set))
                      {
                        std::ostringstream label;

                        if(latex)
                          {
                            label << "$" << THREEPF_SYMBOL << "_{"
                                  << this->latex_names[m % this->N_fields] << (m >= this->N_fields ? PRIME_SYMBOL : "") << " "
                                  << this->latex_names[n % this->N_fields] << (n >= this->N_fields ? PRIME_SYMBOL : "") << " "
                                  << this->latex_names[r % this->N_fields] << (r >= this->N_fields ? PRIME_SYMBOL : "") << "}$";
                          }
                        else
                          {
                            label << this->field_names[m % this->N_fields] << (m >= this->N_fields ? "'" : "") << ", "
                                  << this->field_names[n % this->N_fields] << (n >= this->N_fields ? "'" : "") << ", "
                                  << this->field_names[r % this->N_fields] << (r >= this->N_fields ? "'" : "");
                          }
                        labels.push_back(label.str());
                      }
                  }
              }
          }

        return(labels);
      }


      // for a specific k-configuration, return the time history of a set of components
      // (index order: first index = time, second index = component)
      template <typename number>
      std::vector< std::vector<number> > threepf<number>::construct_kconfig_time_history(index_selector<3>* selector, unsigned int i)
        {
          std::vector< std::vector<number> > data(this->sample_points.size());

          // we want data to be a time series of the 3pf components,
          // depending whether they are enabled by the index_selector
          for(int l = 0; l < this->sample_points.size(); l++)
            {
              for(int m = 0; m < 2*this->N_fields; m++)
                {
                  for(int n = 0; n < 2*this->N_fields; n++)
                    {
                      for(int r = 0; r < 2*this->N_fields; r++)
                        {
                          std::array<unsigned int, 3> index_set = { (unsigned int)m, (unsigned int)n, (unsigned int)r };
                          if(selector->is_on(index_set))
                            {
                              unsigned int samples_index = (2*this->N_fields*2*this->N_fields)*m + 2*this->N_fields*n + r;

                              data[l].push_back(this->samples[l][samples_index][i]);
                            }
                        }
                    }
                }
            }

          return(data);
        }


      // for a specific k-configuration, return the time history of a set of components of correlation
      // functions involving \dot{\phi} (rather than the Hamiltonian momentum, which is what is actually calculated)
      // (index order: first index = time, second index = component)
      template <typename number>
      std::vector< std::vector<number> > threepf<number>::construct_kconfig_dotphi_time_history(index_selector<3>* selector, unsigned int i)
        {
          assert(i < this->kconfig_list.size());

          std::vector< std::vector<number> > data(this->sample_points.size());

          for(int l = 0; l < this->sample_points.size(); l++)
            {
              for(int m = 0; m < 2*this->N_fields; m++)
                {
                  for(int n = 0; n < 2*this->N_fields; n++)
                    {
                      for(int r = 0; r < 2*this->N_fields; r++)
                        {
                          std::array<unsigned int, 3> index_set = { (unsigned int)m, (unsigned int)n, (unsigned int)r };
                          if(selector->is_on(index_set))
                            {
                              unsigned int samples_index = (2*this->N_fields*2*this->N_fields)*m + 2*this->N_fields*n + r;

                              number value = this->samples[l][samples_index][i];

                              // now shift the correlation function if it involves momentum indices
                              if (m >= this->N_fields)
                                {
                                  value += this->dotphi_shift(m, this->kconfig_list[i].indices[0],
                                                              n, this->kconfig_list[i].indices[1],
                                                              r, this->kconfig_list[i].indices[2], l, 0);
                                }
                              if (n >= this->N_fields)
                                {
                                  value += this->dotphi_shift(n, this->kconfig_list[i].indices[1],
                                                              m, this->kconfig_list[i].indices[0],
                                                              r, this->kconfig_list[i].indices[2], l, 1);
                                }
                              if (r >= this->N_fields)
                                {
                                  value += this->dotphi_shift(r, this->kconfig_list[i].indices[2],
                                                              m, this->kconfig_list[i].indices[0],
                                                              n, this->kconfig_list[i].indices[1], l, 2);
                                }

                              data[l].push_back(value);
                            }
                        }
                    }
                }
            }

          return(data);
        }


      // compute the shift in a correlation function from the canonical momentum p_i to the time derivative q'_i,
      // ' = d/dN
      template <typename number>
      double threepf<number>::dotphi_shift(unsigned int __m, unsigned int __kmode_m, unsigned int __n, unsigned int __kmode_n,
                                           unsigned int __r, unsigned int __kmode_r, unsigned int __time_slice, unsigned int __pos)
        {
          assert(__pos < 3);

          std::vector<number> __fields = this->backg.get_value(__time_slice);

          std::vector< std::vector< std::vector<number> > > __B =
            this->tensors->B(__fields, this->sample_com_ks[__kmode_n], this->sample_com_ks[__kmode_r], this->sample_com_ks[__kmode_m], this->sample_points[__time_slice]);
          std::vector< std::vector< std::vector<number> > > __C =
            this->tensors->C(__fields, this->sample_com_ks[__kmode_m], this->sample_com_ks[__kmode_n], this->sample_com_ks[__kmode_r], this->sample_points[__time_slice]);
          
          std::vector<number> __twopf_re_n = this->twopf_re.get_value(__time_slice, __kmode_n);
          std::vector<number> __twopf_re_r = this->twopf_re.get_value(__time_slice, __kmode_r);
          std::vector<number> __twopf_im_n = this->twopf_im.get_value(__time_slice, __kmode_n);
          std::vector<number> __twopf_im_r = this->twopf_im.get_value(__time_slice, __kmode_r);
          
          double rval = 0.0;
          
          assert(__m >= this->N_fields);
          auto __m_species = __m % this->N_fields;
          
          for(int __i = 0; __i < this->N_fields; __i++)
            {
              for(int __j = 0; __j < this->N_fields; __j++)
                {
                  unsigned int __i_field_twopf_index_r    = 0;
                  unsigned int __i_field_twopf_index_n    = 0;
                  unsigned int __j_field_twopf_index_r    = 0;
                  unsigned int __j_field_twopf_index_n    = 0;
                  unsigned int __i_dotfield_twopf_index_r = 0;
                  unsigned int __i_dotfield_twopf_index_n = 0;

                  switch(__pos)
                    {
                      case 0: // index __m is on the far left, to the left of both __n and __r
                        __i_field_twopf_index_n    = (2*this->N_fields)*__i + __n;
                        __i_field_twopf_index_r    = (2*this->N_fields)*__i + __r;
                        __j_field_twopf_index_n    = (2*this->N_fields)*__j + __n;
                        __j_field_twopf_index_r    = (2*this->N_fields)*__j + __r;
                        __i_dotfield_twopf_index_n = (2*this->N_fields)*(__i + this->N_fields) + __n;
                        __i_dotfield_twopf_index_r = (2*this->N_fields)*(__i + this->N_fields) + __r;
                        break;
                      
                      case 1: // index __m is in the middle, to the left of __n but the right of __r
                        __i_field_twopf_index_n    = (2*this->N_fields)*__i + __n;
                        __i_field_twopf_index_r    = (2*this->N_fields)*__r + __i;
                        __j_field_twopf_index_n    = (2*this->N_fields)*__j + __n;
                        __j_field_twopf_index_r    = (2*this->N_fields)*__r + __j;
                        __i_dotfield_twopf_index_n = (2*this->N_fields)*(__i + this->N_fields) + __n;
                        __i_dotfield_twopf_index_r = (2*this->N_fields)*__r + (__i + this->N_fields);
                        break;
                      
                      case 2: // index __m is on the far right, to the right of both __n and __r
                        __i_field_twopf_index_n    = (2*this->N_fields)*__n + __i;
                        __i_field_twopf_index_r    = (2*this->N_fields)*__r + __i;
                        __j_field_twopf_index_n    = (2*this->N_fields)*__n + __j;
                        __j_field_twopf_index_r    = (2*this->N_fields)*__r + __j;
                        __i_dotfield_twopf_index_n = (2*this->N_fields)*__n + (__i + this->N_fields);
                        __i_dotfield_twopf_index_r = (2*this->N_fields)*__r + (__i + this->N_fields);
                        break;
                        
                      default:
                        assert(false);
                    }

                  rval += (1.0/2.0) * __B[__i][__j][__m_species]
                                    * (  __twopf_re_n[__i_field_twopf_index_n]*__twopf_re_r[__j_field_twopf_index_r]
                                       + __twopf_re_n[__j_field_twopf_index_n]*__twopf_re_r[__i_field_twopf_index_r]
                                       - __twopf_im_n[__i_field_twopf_index_n]*__twopf_im_r[__j_field_twopf_index_r]
                                       - __twopf_im_n[__j_field_twopf_index_n]*__twopf_im_r[__i_field_twopf_index_r]);

                  rval += __C[__m_species][__i][__j]
                                    * (  __twopf_re_n[__i_dotfield_twopf_index_n]*__twopf_re_r[__j_field_twopf_index_r]
                                       + __twopf_re_n[__j_field_twopf_index_n]*__twopf_re_r[__i_dotfield_twopf_index_r]
                                       - __twopf_im_n[__i_dotfield_twopf_index_n]*__twopf_im_r[__j_field_twopf_index_r]
                                       - __twopf_im_n[__j_field_twopf_index_n]*__twopf_im_r[__i_dotfield_twopf_index_r]);
                }
            }

          return(rval);
        }


      template <typename number>
      index_selector<3>* threepf<number>::manufacture_selector()
        {
          return new index_selector<3>(this->N_fields);
        }

      template <typename number>
      background<number>& threepf<number>::get_background()
        {
          return(this->backg);
        }

      template <typename number>
      twopf<number>& threepf<number>::get_real_twopf()
        {
          return(this->twopf_re);
        }

      template <typename number>
      twopf<number>& threepf<number>::get_imag_twopf()
        {
          return(this->twopf_im);
        }

      template <typename number>
      std::ostream& operator<<(std::ostream& out, threepf<number>& obj)
        {
          transport::asciitable<number> writer(out);
          writer.set_display_width(obj.get_wrap_width());

          index_selector<3>* selector = obj.manufacture_selector();

          std::vector<std::string> labels = obj.make_labels(selector, false);

          for(int i = 0; i < obj.sample_ks.size(); i++)
            {
              if(i > 0) out << std::endl;
              out << __CPP_TRANSPORT_THREEPF_MESSAGE << " k_t = " << obj.kconfig_list[i].k_t << ","
                                                     << " alpha = " << obj.kconfig_list[i].alpha << ","
                                                     << " beta = " << obj.kconfig_list[i].beta
                                                     << std::endl << std::endl;

              if(obj.output_dotphi)
                {
                  std::vector< std::vector<number> > data = obj.construct_kconfig_dotphi_time_history(selector, i);
                  writer.write(__CPP_TRANSPORT_EFOLDS, labels, obj.sample_points, data);
                }
              else
                {
                  std::vector< std::vector<number> > data = obj.construct_kconfig_time_history(selector, i);
                  writer.write(__CPP_TRANSPORT_EFOLDS, labels, obj.sample_points, data);
                }
            }

          return(out);
        }

      template <typename number>
      void threepf<number>::set_wrap_width(unsigned int w)
        {
          this->wrap_width = (w > 0 ? w : this->wrap_width);
        }

       template <typename number>
       unsigned int threepf<number>::get_wrap_width()
        {
          return(this->wrap_width);
        }


  }   // namespace transport

#endif // __CPP_TRANSPORT_THREEPF_H_
