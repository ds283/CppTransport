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

#define DEFAULT_WRAP_WIDTH         (135)


namespace transport
  {
      // DATA PRODUCTS -- objects wrapping the various data products produced by each model

      // handle weirdness with friend template functions
      // see http://www.cplusplus.com/forum/general/45776/
      template <typename number>
      class threepf;

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
              gauge_xfm_gadget<number>* gx)
              : N_fields(N_f), field_names(f_names), latex_names(l_names),
                Nstar(Nst), sample_points(sp), sample_ks(ks), sample_com_ks(com_ks),
                backg(N_f, f_names, l_names, sp, b),
                twopf_re(N_f, f_names, l_names, ks, com_ks, Nst, sp, b, tpf_re, gx),
                twopf_im(N_f, f_names, l_names, ks, com_ks, Nst, sp, b, tpf_im, gx),
                samples(thpf), kconfig_list(kl),
                gauge_xfm(gx),
                wrap_width(DEFAULT_WRAP_WIDTH)
              {}

            background<number>& get_background();
            twopf<number>&      get_real_twopf();
            twopf<number>&      get_imag_twopf();

            void components_time_history(plot_gadget<number>*gadget, std::string output,
              index_selector<3>* selector, std::string format = "pdf", bool logy=true);
            void zeta_time_history      (plot_gadget<number>*gadget, std::string output,
              std::string format = "pdf", bool dimensionless = true, bool logy=true);

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

            unsigned int                                            N_fields;          // number of fields
            const std::vector<std::string>                          field_names;       // vector of names - includes momenta
            const std::vector<std::string>                          latex_names;       // vector of LaTeX names - excludes momenta

            gauge_xfm_gadget<number>*                               gauge_xfm;         // gauge transformation gadget

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

      template<typename number>
      index_selector<3>* threepf<number>::manufacture_selector()
        {
          return new index_selector<3>(this->N_fields);
        }

      template <typename number>
      background<number>& threepf<number>::get_background()
        {
          return(this->background);
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

              std::vector< std::vector<number> > data = obj.construct_kconfig_time_history(selector, i);

              writer.write(__CPP_TRANSPORT_EFOLDS, labels, obj.sample_points, data);
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
