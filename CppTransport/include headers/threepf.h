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


namespace transport
  {
      // DATA PRODUCTS -- objects wrapping the various data products produced by each model

      // handle weirdness with friend template functions
      // see http://www.cplusplus.com/forum/general/45776/
      template <typename number>
      class threepf;

      template <typename number>
      std::ostream& operator<<(std::ostream& out, threepf<number>& obj);


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
              gauge_xfm_gadget<number>* gx)
              : N_fields(N_f), field_names(f_names), latex_names(l_names),
                Nstar(Nst), sample_points(sp), sample_ks(ks), sample_com_ks(com_ks),
                backg(N_f, f_names, l_names, sp, b),
                twopf_re(N_f, f_names, l_names, ks, com_ks, Nst, sp, b, tpf_re, gx),
                twopf_im(N_f, f_names, l_names, ks, com_ks, Nst, sp, b, tpf_im, gx),
                samples(thpf),
                gauge_xfm(gx)
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

          protected:
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
              // third index: k
        };


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
          out << obj.backg << std::endl;
          out << obj.twopf_re << std::endl;
          out << obj.twopf_im << std::endl;
        }

  }   // namespace transport

#endif // __CPP_TRANSPORT_THREEPF_H_
