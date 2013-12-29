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

#include "transport/default_symbols.h"
#include "transport/utilities/latex_output.h"
#include "transport/utilities/asciitable.h"
#include "transport/utilities/latex_output.h"
#include "transport/concepts/flattener.h"
#include "transport/containers/label_gadget.h"
#include "transport/containers/index_selector.h"
#include "transport/plotgadgets/plot_gadget.h"
#include "transport/messages_en.h"

#include "transport/containers/background.h"
#include "transport/containers/twopf.h"


namespace transport
  {
      // handle weirdness with friend template functions
      // see http://www.cplusplus.com/forum/general/45776/
      template <typename number> class threepf;

      template <typename number>
      std::ostream& operator<<(std::ostream& out, threepf<number>& obj);

      class threepf_local_shape
        {
          public:
            double operator()(double k1, double k2, double k3)
              {
                return( 1.0/(k1*k1*k1*k2*k2*k2) + 1.0/(k1*k1*k1*k3*k3*k3) + 1.0/(k2*k2*k2*k3*k3*k3) );
              }

            std::string name()
              {
                return("local");
              }
        };

      template <typename number>
      class threepf
        {
          public:
            threepf(const threepf_task<number>* t, const std::vector< std::vector<number> >& b,
              const std::vector< std::vector< std::vector<number> > >& tpf_re,
              const std::vector< std::vector< std::vector<number> > >& tpf_im,
              const std::vector< std::vector< std::vector<number> > >& thpf,
              model<number>* p)
              : labels(p->get_N_fields(), p->get_field_names(), p->get_f_latex_names()),
                tk(t),
                backg(t, b, p),
                twopf_re(t, b, tpf_re, p),
                twopf_im(t, b, tpf_im, p),
                samples(thpf),
                parent(p),
                wrap_width(DEFAULT_WRAP_WIDTH), output_dotphi(DEFAULT_OUTPUT_DOTPHI),
                plot_precision(DEFAULT_PLOT_PRECISION)
              {}

            background<number>& get_background();
            twopf<number>&      get_real_twopf();
            twopf<number>&      get_imag_twopf();

            // FLAT-HYPERSURFACE FIELD-FLUCTUATION THREE-POINT FUNCTIONS

            // plot time evolution of the components of the 3pf
            // switch 'dotphi' to use either the (field, momentum) or (field, dot(field)) basis
            void components_time_history(plot_gadget<number>* gadget, std::string output,
              index_selector<3>* selector, std::string format="pdf", bool logy=true, bool dotphi=true);

            // plot time evolution of a given shape, ie., the 3-point function normalized
            // to a fixed reference bispectrum
            template <typename ShapeFunctor>
            void components_time_history(plot_gadget<number>* gadget, std::string output,
              ShapeFunctor shape, index_selector<3>* selector, std::string format="pdf", bool logy=true, bool dotphi=true);

            // ZETA THREE-POINT FUNCTIONS

            // plot time evolution of the <zeta zeta zeta> 3pf
            void zeta_time_history(plot_gadget<number>* gadget, std::string output,
              std::string format="pdf", bool logy=true);

            // plot time evolution of the <zeta zeta zeta> shape
            template <typename ShapeFunctor>
            void zeta_time_history(plot_gadget<number>* gadget, std::string output,
              ShapeFunctor shape,
              std::string format="pdf", bool logy=true);

            // plot time evolution of the 'reduced bispectrum', defined by
            // (6/5) fNL_red(k1, k2, k3) ( P(k1) P(k2) + perms ) = B(k1, k2, k3)
            // this coincides with fNL_local *if* the bispectrum B(k1, k2, k3)
            // is itself purely local. Otherwise, it is momentum dependent
            // and fNL_local should instead be extracted via projection
            void reduced_bispectrum_time_history(plot_gadget<number>* gadget, std::string output,
              std::string format="pdf", bool logy=false);

            index_selector<3>* manufacture_selector();

            // provide << operator to output data to a stream
            friend std::ostream& operator<< <>(std::ostream& out, threepf& obj);

            unsigned int get_wrap_width();
            void         set_wrap_width(unsigned int w);

          protected:

            // make titles for plots, listing the momenta involved
            std::string                        make_threepf_title(const struct threepf_kconfig& config, bool latex);

            // return a time history for a given set of components and a fixed k-configuration
            std::vector< std::vector<number> > construct_kconfig_momentum_time_history(index_selector<3>* selector, unsigned int i);

            template <typename ShapeFunctor>
            std::vector< std::vector<number> > construct_kconfig_momentum_time_history(index_selector<3>* selector, unsigned int i, ShapeFunctor shape);

            // return a time history for correlation functions of \dot{\phi}_i (rather than the canonical momentum p_i)
            // for a given set of components and fixed k-configuration
            std::vector< std::vector<number> > construct_kconfig_dotphi_time_history(index_selector<3>* selector, unsigned int i);

            template <typename ShapeFunctor>
            std::vector< std::vector<number> > construct_kconfig_dotphi_time_history(index_selector<3>* selector, unsigned int i, ShapeFunctor shape);

            // compute the shift in the correlation function from p_i -> \dot{\phi}_i for a
            // given index assignment, k-configuration and time
            number construct_dotphi_shift(unsigned int i, unsigned int l, unsigned int m, unsigned int n, unsigned int r);

            // compute the shift in the correlation function when changing from momentum to d/dN
            // __m,__kmode_m always specify the operator being shifted, but
            // __pos indicated where it stands within the correlation function
            number dotphi_shift(unsigned int __m, unsigned int __kmode_m, unsigned int __n, unsigned int __kmode_n,
                                unsigned int __r, unsigned int __kmode_r, unsigned int __time, unsigned int __pos);

            // return a time history for the correlation function of zeta, for a fixed k-configuration
            std::vector< std::vector<number> > construct_zeta_time_history(unsigned int i);

            // return a time history for a given shape function, ie., the bispectrum normalized to a specific reference
            // bispectrum
            template <typename ShapeFunctor>
            std::vector< std::vector<number> > construct_zeta_time_history(unsigned int i, ShapeFunctor shape);

            // return a time history for the reduced bispectrum of zeta and a fixed k-configuration
            std::vector< std::vector<number> > construct_reduced_bispectrum_time_history(unsigned int i);

            model<number>*                                          parent;            // parent model object

            label_gadget                                            labels;            // holds names (and LaTeX names) for fields

            const threepf_task<number>*                             tk;                // task object used to generate this solution

            background<number>                                      backg;             // container for background
            twopf<number>                                           twopf_re;          // container for real 2pf
            twopf<number>                                           twopf_im;          // container for imaginary 2pf
            const std::vector< std::vector< std::vector<number> > > samples;           // list of samples of 3pf
              // first index: time of observation
              // second index: 2pf component
              // third index: k (ordered according to 'kconfig_list' below)

            unsigned int                                            wrap_width;        // position to wrap when output to stream
            bool                                                    output_dotphi;     // output correlation funtions of dot(phi), or the canonical momentum?
            unsigned int                                            plot_precision;    // precision to use when making plot labels
        };


//  IMPLEMENTATION -- CLASS threepf


      template <typename number>
      void threepf<number>::components_time_history(plot_gadget<number>* gadget, std::string output,
                                                    index_selector<3>* selector, std::string format, bool logy, bool dotphi)
        {
          std::vector<threepf_kconfig> kconfig_list = this->tk->get_sample();
          std::vector<double> sample_points = this->tk->get_sample_times();
          std::vector<std::string> labels = this->labels.make_labels(selector, gadget->latex_labels());

          // loop over all momentum configurations
#pragma omp for schedule(guided)
          for(int i = 0; i < kconfig_list.size(); i++)
            {
              std::vector< std::vector<number> > data = dotphi ? this->construct_kconfig_dotphi_time_history(selector, i) :
                                                                 this->construct_kconfig_momentum_time_history(selector, i);

              std::ostringstream fnam;
              fnam << output << "_" << i;

              std::ostringstream tag;
              tag << (dotphi ? "derivatives" : "momenta");

              gadget->set_format(format);
              gadget->plot(fnam.str(), this->make_threepf_title(kconfig_list[i], gadget->latex_labels()),
                           sample_points, data, labels, PICK_N_LABEL, THREEPF_LABEL, false, logy, tag.str());
            }
        }


      template <typename number>
      template <typename ShapeFunctor>
      void threepf<number>::components_time_history(plot_gadget<number>* gadget, std::string output,
                                                   ShapeFunctor shape, index_selector<3>* selector, std::string format, bool logy, bool dotphi)
        {
          std::vector<threepf_kconfig> kconfig_list = this->tk->get_sample();
          std::vector<double> sample_points = this->tk->get_sample_times();
          std::vector<std::string> labels = this->labels.make_labels(selector, gadget->latex_labels());

          // loop over all momentum configurations
#pragma omp for schedule(guided)
          for(int i = 0; i < kconfig_list.size(); i++)
            {
              std::vector< std::vector<number> > data = dotphi ? this->construct_kconfig_dotphi_time_history(selector, i, shape) :
                                                                 this->construct_kconfig_momentum_time_history(selector, i, shape);

              std::ostringstream fnam;
              fnam << output << "_" << i;

              std::ostringstream tag;
              tag << (dotphi ? "derivatives" : "momenta");
              tag << ", reference shape = " << shape.name();

              gadget->set_format(format);
              gadget->plot(fnam.str(), this->make_threepf_title(kconfig_list[i], gadget->latex_labels()),
                           sample_points, data, labels, PICK_N_LABEL, SHAPE_LABEL, false, logy, tag.str());
            }
        }


      template <typename number>
      void threepf<number>::zeta_time_history(plot_gadget<number>* gadget, std::string output,
                                              std::string format, bool logy)
        {
          std::vector<threepf_kconfig> kconfig_list = this->tk->get_sample();
          std::vector<double> sample_points = this->tk->get_sample_times();

          // loop over all momentum configurations
#pragma omp for schedule(guided)
          for(int i = 0; i < kconfig_list.size(); i++)
            {
              std::vector< std::vector<number> > data = this->construct_zeta_time_history(i);

              std::ostringstream fnam;
              fnam << output << "_" << i;

              std::vector<std::string> labels(1);
              labels[0] = this->labels.make_zeta_bispectrum_label(gadget->latex_labels());

              gadget->set_format(format);
              gadget->plot(fnam.str(), this->make_threepf_title(kconfig_list[i], gadget->latex_labels()),
                           sample_points, data, labels, PICK_N_LABEL, THREEPF_LABEL, false, logy);
            }
        }


    template <typename number>
    template <typename ShapeFunctor>
    void threepf<number>::zeta_time_history(plot_gadget<number>* gadget, std::string output,
                                            ShapeFunctor shape, std::string format, bool logy)
      {
        std::vector<threepf_kconfig> kconfig_list = this->tk->get_sample();
        std::vector<double> sample_points = this->tk->get_sample_times();

        // loop over all momentum configurations
#pragma omp for schedule(guided)
        for(int i = 0; i < kconfig_list.size(); i++)
          {
            std::vector< std::vector<number> > data = this->construct_zeta_time_history(i, shape);

            std::ostringstream fnam;
            fnam << output << "_" << i;

            std::vector<std::string> labels(1);
            labels[0] = this->labels.make_shape_bispectrum_label(gadget->latex_labels());

            std::ostringstream tag;
            tag << "reference shape = " << shape.name();

            gadget->set_format(format);
            gadget->plot(fnam.str(), this->make_threepf_title(kconfig_list[i], gadget->latex_labels()),
                         sample_points, data, labels, PICK_N_LABEL, "", false, logy, tag.str());
          }
      }


      template <typename number>
      void threepf<number>::reduced_bispectrum_time_history(plot_gadget<number> *gadget, std::string output, std::string format, bool logy)
        {
          std::vector<threepf_kconfig> kconfig_list = this->tk->get_sample();
          std::vector<double> sample_points = this->tk->get_sample_times();

          // loop over all momentum configurations
#pragma omp for schedule(guided)
          for(int i = 0; i < kconfig_list.size(); i++)
            {
              std::vector< std::vector<number> > data = this->construct_reduced_bispectrum_time_history(i);
              
              std::ostringstream fnam;
              fnam << output << "_" << i;
              
              std::vector<std::string> labels(1);
              labels[0] = this->labels.make_reduced_bispectrum_label(gadget->latex_labels());
              
              gadget->set_format(format);
              gadget->plot(fnam.str(), this->make_threepf_title(kconfig_list[i], gadget->latex_labels()),
                           sample_points, data, labels, PICK_N_LABEL, "", false, logy);
            }
        }


      template <typename number>
      std::string threepf<number>::make_threepf_title(const threepf_kconfig& config, bool latex)
        {
          std::ostringstream title;

          if(latex)
            {
              title << "$" << KT_SYMBOL        << " = " << output_latex_number(config.k_t, this->plot_precision)   << "$, "
                    << "$" << FLS_ALPHA_SYMBOL << " = " << output_latex_number(config.alpha, this->plot_precision) << "$, "
                    << "$" << FLS_BETA_SYMBOL  << " = " << output_latex_number(config.beta, this->plot_precision)  << "$";
            }
          else
            {
              title << std::setprecision(this->plot_precision);
              title << KT_NAME        << " = " << config.k_t   << ", "
                    << FLS_ALPHA_NAME << " = " << config.alpha << ", "
                    << FLS_BETA_NAME  << " = " << config.beta;
            }

          return(title.str());
        }


      // for a specific k-configuration, return the time history of a set of components
      // (index order: first index = time, second index = component)
      template <typename number>
      std::vector< std::vector<number> > threepf<number>::construct_kconfig_momentum_time_history(index_selector<3>* selector, unsigned int i)
        {
          std::vector< std::vector<number> > data(this->tk->get_N_sample_times());

          // we want data to be a time series of the 3pf components,
          // depending whether they are enabled by the index_selector
          for(int l = 0; l < this->tk->get_sample_times().size(); l++)
            {
              for(int m = 0; m < 2*this->parent->get_N_fields(); m++)
                {
                  for(int n = 0; n < 2*this->parent->get_N_fields(); n++)
                    {
                      for(int r = 0; r < 2*this->parent->get_N_fields(); r++)
                        {
                          std::array<unsigned int, 3> index_set = { (unsigned int)m, (unsigned int)n, (unsigned int)r };
                          if(selector->is_on(index_set))
                            {
                              unsigned int samples_index = this->parent->flatten(m,n,r);

                              data[l].push_back(this->samples[l][samples_index][i]);
                            }
                        }
                    }
                }
            }

          return(data);
        }


      template <typename number>
      template <typename ShapeFunctor>
      std::vector< std::vector<number> > threepf<number>::construct_kconfig_momentum_time_history(index_selector<3>* selector, unsigned int i, ShapeFunctor shape)
        {
          std::vector<threepf_kconfig> kconfig_list = this->tk->get_sample();

          std::vector< std::vector<number> > raw = this->construct_kconfig_momentum_time_history(selector, i);

          std::vector< std::vector<number> > data(raw.size());

          // normalize to the shape function
          for(int l  = 0; l < raw.size(); l++)
            {
              data[l].resize(raw[l].size());

              for(int m = 0; m < raw[l].size(); m++)
                {
                  number form_factor = shape(this->tk->get_k_comoving(kconfig_list[i].index[0]),
                                             this->tk->get_k_comoving(kconfig_list[i].index[1]),
                                             this->tk->get_k_comoving(kconfig_list[i].index[2]));

                  data[l][m] = raw[l][m] / form_factor;
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
          assert(i < this->tk->get_sample().size());

          std::vector< std::vector<number> > data(this->tk->get_N_sample_times());

          for(int l = 0; l < this->tk->get_N_sample_times(); l++)
            {
              for(int m = 0; m < 2*this->parent->get_N_fields(); m++)
                {
                  for(int n = 0; n < 2*this->parent->get_N_fields(); n++)
                    {
                      for(int r = 0; r < 2*this->parent->get_N_fields(); r++)
                        {
                          std::array<unsigned int, 3> index_set = { (unsigned int)m, (unsigned int)n, (unsigned int)r };
                          if(selector->is_on(index_set))
                            {
                              unsigned int samples_index = this->parent->flatten(m,n,r);

                              number value = this->samples[l][samples_index][i];

                              // now shift the correlation function if it involves momentum index
                              number shift = this->construct_dotphi_shift(i, l, m, n, r);

                              value += shift;

                              data[l].push_back(value);
                            }
                        }
                    }
                }
            }

          return(data);
        }


    template <typename number>
    template <typename ShapeFunctor>
    std::vector< std::vector<number> > threepf<number>::construct_kconfig_dotphi_time_history(index_selector<3>* selector, unsigned int i, ShapeFunctor shape)
      {
        std::vector<threepf_kconfig> kconfig_list = this->tk->get_sample();

        std::vector< std::vector<number> > raw = this->construct_kconfig_dotphi_time_history(selector, i);

        std::vector< std::vector<number> > data(raw.size());

        // normalize to the shape function
        for(int l  = 0; l < raw.size(); l++)
          {
            data[l].resize(raw[l].size());

            for(int m = 0; m < raw[l].size(); m++)
              {
                number form_factor = shape(this->tk->get_k_comoving(kconfig_list[i].index[0]),
                                           this->tk->get_k_comoving(kconfig_list[i].index[1]),
                                           this->tk->get_k_comoving(kconfig_list[i].index[2]));

                data[l][m] = raw[l][m] / form_factor;
              }
          }

        return(data);
      }


      template <typename number>
      number threepf<number>::construct_dotphi_shift(unsigned int i, unsigned int l, unsigned int m, unsigned int n, unsigned int r)
        {
          std::vector<threepf_kconfig> kconfig_list = this->tk->get_sample();
          number shift = 0.0;

          if (m >= this->parent->get_N_fields())
            {
              shift += this->dotphi_shift(m, kconfig_list[i].index[0],
                                          n, kconfig_list[i].index[1],
                                          r, kconfig_list[i].index[2], l, 0);
            }
          if (n >= this->parent->get_N_fields())
            {
              shift += this->dotphi_shift(n, kconfig_list[i].index[1],
                                          m, kconfig_list[i].index[0],
                                          r, kconfig_list[i].index[2], l, 1);
            }
          if (r >= this->parent->get_N_fields())
            {
              shift += this->dotphi_shift(r, kconfig_list[i].index[2],
                                          m, kconfig_list[i].index[0],
                                          n, kconfig_list[i].index[1], l, 2);
            }

          return(shift);
        }


      // compute the shift in a correlation function from the canonical momentum p_i to the time derivative q'_i,
      // ' = d/dN
      // __m,__kmode_m always represent the operator being shifted
      // __n,__kmode_n and __r,__kmode_r represent the other operators in the 3pf
      // they may be either fields or momenta, but we assume that they are in order, that is,
      // __r,__kmode_r follows __n,__kmode_n within the 3pf
      template <typename number>
      number threepf<number>::dotphi_shift(unsigned int __m, unsigned int __kmode_m, unsigned int __n, unsigned int __kmode_n,
                                           unsigned int __r, unsigned int __kmode_r, unsigned int __time, unsigned int __pos)
        {
          assert(__pos < 3);

          std::vector<double> sample_points = this->tk->get_sample_times();
          std::vector<number> __fields = this->backg.get_value(__time);

          std::vector< std::vector< std::vector<number> > > __B_nrm;
          this->parent->B(this->tk->get_params(), __fields, this->tk->get_k_comoving(__kmode_n), this->tk->get_k_comoving(__kmode_r), this->tk->get_k_comoving(__kmode_m), sample_points[__time], __B_nrm);
          std::vector< std::vector< std::vector<number> > > __C_mnr;
          this->parent->C(this->tk->get_params(), __fields, this->tk->get_k_comoving(__kmode_m), this->tk->get_k_comoving(__kmode_n), this->tk->get_k_comoving(__kmode_r), sample_points[__time], __C_mnr);

          std::vector<number> __twopf_re_n = this->twopf_re.get_value(__time, __kmode_n);
          std::vector<number> __twopf_re_r = this->twopf_re.get_value(__time, __kmode_r);
          std::vector<number> __twopf_im_n = this->twopf_im.get_value(__time, __kmode_n);
          std::vector<number> __twopf_im_r = this->twopf_im.get_value(__time, __kmode_r);
          
          number rval = 0.0;
          
          assert(__m >= this->parent->get_N_fields());
          assert(__m <  2*this->parent->get_N_fields());
          auto __m_species = this->parent->species(__m);
          
          for(int __i = 0; __i < this->parent->get_N_fields(); __i++)
            {
              for(int __j = 0; __j < this->parent->get_N_fields(); __j++)
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
                        // flatten the two possible sets of index contractions: (i,n),(j,r) and (j,r),(i,n)
                        __i_field_twopf_index_n    = this->parent->flatten(__i,__n);
                        __i_field_twopf_index_r    = this->parent->flatten(__i,__r);
                        __j_field_twopf_index_n    = this->parent->flatten(__j,__n);
                        __j_field_twopf_index_r    = this->parent->flatten(__j,__r);
                        __i_dotfield_twopf_index_n = this->parent->flatten(this->parent->momentum(__i),__n);
                        __i_dotfield_twopf_index_r = this->parent->flatten(this->parent->momentum(__i),__r);
                        break;
                      
                      case 1: // index __m is in the middle, to the left of __r but the right of __n
                        // flatten the two possible sets of index contractions: (n,i),(j,r) and (n,j),(i,r)
                        __i_field_twopf_index_n    = this->parent->flatten(__n,__i);
                        __i_field_twopf_index_r    = this->parent->flatten(__i,__r);
                        __j_field_twopf_index_n    = this->parent->flatten(__n,__j);
                        __j_field_twopf_index_r    = this->parent->flatten(__j,__r);
                        __i_dotfield_twopf_index_n = this->parent->flatten(__n,this->parent->momentum(__i));
                        __i_dotfield_twopf_index_r = this->parent->flatten(this->parent->momentum(__i),__r);
                        break;
                      
                      case 2: // index __m is on the far right, to the right of both __n and __r
                        // flatten the two possible sets of index contractions: (n,i),(r,j) and (n,j),(r,i)
                        __i_field_twopf_index_n    = this->parent->flatten(__n,__i);
                        __i_field_twopf_index_r    = this->parent->flatten(__r,__i);
                        __j_field_twopf_index_n    = this->parent->flatten(__n,__j);
                        __j_field_twopf_index_r    = this->parent->flatten(__r,__j);
                        __i_dotfield_twopf_index_n = this->parent->flatten(__n,this->parent->momentum(__i));
                        __i_dotfield_twopf_index_r = this->parent->flatten(__r,this->parent->momentum(__i));
                        break;
                        
                      default:
                        assert(false);
                    }

                  rval -= (1.0/2.0) * __B_nrm[__i][__j][__m_species]
                            * (  __twopf_re_n[__i_field_twopf_index_n] * __twopf_re_r[__j_field_twopf_index_r]
                               - __twopf_im_n[__i_field_twopf_index_n] * __twopf_im_r[__j_field_twopf_index_r]);

                  rval -= (1.0/2.0) * __B_nrm[__j][__i][__m_species]
                            * (  __twopf_re_n[__j_field_twopf_index_n] * __twopf_re_r[__i_field_twopf_index_r]
                               - __twopf_im_n[__j_field_twopf_index_n] * __twopf_im_r[__i_field_twopf_index_r]);

                  rval -= __C_mnr[__m_species][__i][__j]
                            * (  __twopf_re_n[__i_dotfield_twopf_index_n] * __twopf_re_r[__j_field_twopf_index_r]
                               - __twopf_im_n[__i_dotfield_twopf_index_n] * __twopf_im_r[__j_field_twopf_index_r]);

                  rval -= __C_mnr[__m_species][__j][__i]
                            * (  __twopf_re_n[__j_field_twopf_index_n]    * __twopf_re_r[__i_dotfield_twopf_index_r]
                               - __twopf_im_n[__j_field_twopf_index_n]    * __twopf_im_r[__i_dotfield_twopf_index_r]);
                }
            }

          return(rval);
        }

      template <typename number>
      std::vector< std::vector<number> > threepf<number>::construct_zeta_time_history(unsigned int i)
        {
          std::vector<threepf_kconfig> kconfig_list = this->tk->get_sample();
          std::vector< std::vector<number> > data(this->tk->get_N_sample_times());

          for(int j = 0; j < this->tk->get_N_sample_times(); j++)
            {
              data[j].resize(1);    // only one component of < zeta zeta zeta >

              // compute gauge transformations
              std::vector<number> dN;
              std::vector< std::vector<number> > ddN;

              this->parent->compute_gauge_xfm_1(this->tk->get_params(), this->backg.get_value(j), dN);
              this->parent->compute_gauge_xfm_2(this->tk->get_params(), this->backg.get_value(j), ddN);

              // get twopf values for this timeslices and appropriate k-modes
              std::vector<number> twopf_re_k1 = this->twopf_re.get_value(j, kconfig_list[i].index[0]);
              std::vector<number> twopf_im_k1 = this->twopf_im.get_value(j, kconfig_list[i].index[0]);
              std::vector<number> twopf_re_k2 = this->twopf_re.get_value(j, kconfig_list[i].index[1]);
              std::vector<number> twopf_im_k2 = this->twopf_im.get_value(j, kconfig_list[i].index[1]);
              std::vector<number> twopf_re_k3 = this->twopf_re.get_value(j, kconfig_list[i].index[2]);
              std::vector<number> twopf_im_k3 = this->twopf_im.get_value(j, kconfig_list[i].index[2]);

              // compute contribution from intrinsic threepf
              data[j][0] = 0;
              for(int m = 0; m < 2*this->parent->get_N_fields(); m++)
                {
                  for(int n = 0; n < 2*this->parent->get_N_fields(); n++)
                    {
                      for(int r = 0; r < 2*this->parent->get_N_fields(); r++)
                        {
                          unsigned int samples_index = this->parent->flatten(m,n,r);

                          number value = this->samples[j][samples_index][i];

                          // shift to dot(phi) if it involves momenta
                          value += this->construct_dotphi_shift(i, j, m, n, r);

                          data[j][0] += dN[m]*dN[n]*dN[r]*value;
                        }
                    }
                }

              // compute contribution from gauge transformation
              number shift = 0;
              for(int l = 0; l < 2*this->parent->get_N_fields(); l++)
                {
                  for(int m = 0; m < 2*this->parent->get_N_fields(); m++)
                    {
                      for(int n = 0; n < 2*this->parent->get_N_fields(); n++)
                        {
                          for(int r = 0; r < 2*this->parent->get_N_fields(); r++)
                            {
                              // l, m to left of n and r
                              unsigned int ln_1_index = this->parent->flatten(l,n);
                              unsigned int lr_1_index = this->parent->flatten(l,r);
                              unsigned int mn_1_index = this->parent->flatten(m,n);
                              unsigned int mr_1_index = this->parent->flatten(m,r);

                              // l, m to left of r but right of n
                              unsigned int ln_2_index = this->parent->flatten(n,l);
                              unsigned int lr_2_index = this->parent->flatten(l,r);
                              unsigned int mn_2_index = this->parent->flatten(n,m);
                              unsigned int mr_2_index = this->parent->flatten(m,r);

                              // l, m to right of n and r
                              unsigned int ln_3_index = this->parent->flatten(n,l);
                              unsigned int lr_3_index = this->parent->flatten(r,l);
                              unsigned int mn_3_index = this->parent->flatten(n,m);
                              unsigned int mr_3_index = this->parent->flatten(r,m);

                              shift += (1.0/2.0) * ddN[l][m]*dN[n]*dN[r]*
                                (  twopf_re_k2[ln_1_index]*twopf_re_k3[mr_1_index]
                                 + twopf_re_k2[lr_1_index]*twopf_re_k3[mn_1_index]
                                 - twopf_im_k2[ln_1_index]*twopf_im_k3[mr_1_index]
                                 - twopf_im_k2[lr_1_index]*twopf_im_k3[mn_1_index]);

                              shift += (1.0/2.0) * ddN[l][m]*dN[n]*dN[r]*
                                (  twopf_re_k1[ln_2_index]*twopf_re_k3[mr_2_index]
                                 + twopf_re_k1[lr_2_index]*twopf_re_k3[mn_2_index]
                                 - twopf_im_k1[ln_2_index]*twopf_im_k3[mr_2_index]
                                 - twopf_im_k1[lr_2_index]*twopf_im_k3[mn_2_index]);

                              shift += (1.0/2.0) * ddN[l][m]*dN[n]*dN[r]*
                                (  twopf_re_k1[ln_3_index]*twopf_re_k2[mr_3_index]
                                 + twopf_re_k1[lr_3_index]*twopf_re_k2[mn_3_index]
                                 - twopf_im_k1[ln_3_index]*twopf_im_k2[mr_3_index]
                                 - twopf_im_k1[lr_3_index]*twopf_im_k2[mn_3_index]);
                            }
                        }
                    }
                }
              data[j][0] += shift;
            }

          return(data);
        }


      template <typename number>
      template <typename ShapeFunctor>
      std::vector< std::vector<number> > threepf<number>::construct_zeta_time_history(unsigned int i, ShapeFunctor shape)
        {
          std::vector<threepf_kconfig> kconfig_list = this->tk->get_sample();
          std::vector< std::vector<number> > data(this->tk->get_N_sample_times());

          std::vector< std::vector<number> > threepf = this->construct_zeta_time_history(i);

          for(int j = 0; j < this->tk->get_N_sample_times(); j++)
            {
              data[j].resize(1);    // only one component

              number form_factor = shape(this->tk[kconfig_list[i].index[0]],
                                         this->tk[kconfig_list[i].index[1]],
                                         this->tk[kconfig_list[i].index[2]]);

              data[j][0] = threepf[j][0] / form_factor;
            }

          return(data);
        }


      template <typename number>
      std::vector< std::vector<number> > threepf<number>::construct_reduced_bispectrum_time_history(unsigned int i)
        {
          std::vector<threepf_kconfig> kconfig_list = this->tk->get_sample();
          std::vector< std::vector<number> > data(this->tk->get_N_sample_times());
          
          std::vector< std::vector<number> > threepf  = this->construct_zeta_time_history(i);
          std::vector< std::vector<number> > twopf_k1 = this->twopf_re.construct_zeta_time_history(kconfig_list[i].index[0]);
          std::vector< std::vector<number> > twopf_k2 = this->twopf_re.construct_zeta_time_history(kconfig_list[i].index[1]);
          std::vector< std::vector<number> > twopf_k3 = this->twopf_re.construct_zeta_time_history(kconfig_list[i].index[2]);
          
          for(int j = 0; j < this->tk->get_N_sample_times(); j++)
            {
              data[j].resize(1);    // only one component, fNL(k1, k2, k3)
              
              number form_factor = (6.0/5.0) * (twopf_k1[j][0]*twopf_k2[j][0] + twopf_k1[j][0]*twopf_k3[j][0] + twopf_k2[j][0]*twopf_k3[j][0]);
              
              data[j][0] = threepf[j][0] / form_factor;
            }
          
          return(data);
        }


      template <typename number>
      index_selector<3>* threepf<number>::manufacture_selector()
        {
          return new index_selector<3>(this->parent->get_N_fields());
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
              out << __CPP_TRANSPORT_THREEPF_MESSAGE << " " << KT_NAME        << " = " << obj.kconfig_list[i].k_t   << ","
                                                     << " " << FLS_ALPHA_NAME << " = " << obj.kconfig_list[i].alpha << ","
                                                     << " " << FLS_BETA_NAME  << " = " << obj.kconfig_list[i].beta
                                                     << std::endl << std::endl;

              if(obj.output_dotphi)
                {
                  std::vector< std::vector<number> > data = obj.construct_kconfig_dotphi_time_history(selector, i);
                  writer.write(__CPP_TRANSPORT_EFOLDS, labels, obj.sample_points, data);
                }
              else
                {
                  std::vector< std::vector<number> > data = obj.construct_kconfig_momentum_time_history(selector, i);
                  writer.write(__CPP_TRANSPORT_EFOLDS, labels, obj.sample_points, data);
                }
            }

          delete selector;

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
