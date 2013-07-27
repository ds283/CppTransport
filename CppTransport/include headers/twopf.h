//
// Created by David Seery on 30/06/2013.
// Copyright (c) 2013 University of Sussex. All rights reserved.
//
// To change the template use AppCode | Preferences | File Templates.
//

#ifndef __CPP_TRANSPORT_TWOPF_H_
#define __CPP_TRANSPORT_TWOPF_H_

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


#define TWOPF_SYMBOL               "\\Sigma"
#define ZETA_SYMBOL                "\\zeta"
#define DIMENSIONLESS_TWOPF_SYMBOL "\\mathcal{P}"
#define PRIME_SYMBOL               "\\prime"


namespace transport
  {
      // DATA PRODUCTS -- objects wrapping the various data products produced by each model

      // handle weirdness with friend template functions
      // see http://www.cplusplus.com/forum/general/45776/
      template <typename number>
      class twopf;

      template <typename number>
      std::ostream& operator<<(std::ostream& out, twopf<number>& obj);


      template <typename number>
      class twopf
        {
          public:
            twopf(unsigned int N_f, const std::vector<std::string>& f_names, const std::vector<std::string>& l_names,
              const std::vector<double>& ks, const std::vector<double>& com_ks, double Nst,
              const std::vector<number>& sp, const std::vector< std::vector<number> >& b,
              const std::vector< std::vector< std::vector<number> > >& twopf,
              gauge_xfm_gadget<number>* gx)
              : N_fields(N_f), field_names(f_names), latex_names(l_names),
                Nstar(Nst), sample_points(sp), sample_ks(ks), sample_com_ks(com_ks),
                backg(N_f, f_names, l_names, sp, b), samples(twopf),
                gauge_xfm(gx)
              {}

            background<number>& get_background();

            void components_time_history(plot_gadget<number>*gadget, std::string output,
              index_selector<2>* selector, std::string format = "pdf", bool logy=true);
            void zeta_time_history      (plot_gadget<number>*gadget, std::string output,
              std::string format = "pdf", bool dimensionless = true, bool logy=true);

            index_selector<2>* manufacture_selector();

            // provide << operator to output data to a stream
            friend std::ostream& operator<< <>(std::ostream& out, twopf& obj);

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
            const std::vector< std::vector< std::vector<number> > > samples;           // list of samples of 2pf
              // first index: time of observation
              // second index: 2pf component
              // third index: k
        };


//  IMPLEMENTATION -- CLASS twopf


      template <typename number>
      void twopf<number>::components_time_history(plot_gadget<number>* gadget, std::string output,
        index_selector<2>* selector, std::string format, bool logy)
        {
          // loop over k-modes
          for(int i = 0; i < this->sample_ks.size(); i++)
            {
              std::vector< std::vector<number> > data(this->sample_points.size());
              std::vector< std::string >         labels;

              // we want data to be a time series of the 2pf components,
              // depending whether they are enabled by the index_selector
              for(int j = 0; j < this->sample_points.size(); j++)
                {
                  // now, for this k-mode, slice up the time series
                  for(int m = 0; m < 2*this->N_fields; m++)
                    {
                      for(int n = 0; n < 2*this->N_fields; n++)
                        {
                          std::array<unsigned int, 2> index_set = { (unsigned int)m, (unsigned int)n };
                          if(selector->is_on(index_set))
                            {
                              unsigned int samples_index = 2*this->N_fields*m + n;

                              data[j].push_back(this->samples[j][samples_index][i]);
                            }
                        }
                    }
                }

              for(int m = 0; m < 2*this->N_fields; m++)
                {
                  for(int n = 0; n < 2*this->N_fields; n++)
                    {
                      std::array<unsigned int, 2> index_set = { (unsigned int)m, (unsigned int)n, };
                      if(selector->is_on(index_set))
                        {
                          std::ostringstream label;
                          label << "$" << TWOPF_SYMBOL << "_{"
                                << this->latex_names[m % this->N_fields] << (m >= this->N_fields ? PRIME_SYMBOL : "") << " "
                                << this->latex_names[n % this->N_fields] << (n >= this->N_fields ? PRIME_SYMBOL : "") << "}$";
                          labels.push_back(label.str());
                        }
                    }
                }

              std::ostringstream fnam;
              fnam << output << "_" << i;

              std::ostringstream title;
              title << "$k = " << this->sample_ks[i] << "$";

              gadget->set_format(format);
              gadget->plot(fnam.str(), title.str(), this->sample_points, data, labels, "$N$", "two-point function", false, logy);
            }
        }

      template <typename number>
      void twopf<number>::zeta_time_history(plot_gadget<number>* gadget, std::string output,
        std::string format, bool dimensionless, bool logy)
        {
          // loop over k-modes
          for(int i = 0; i < this->sample_ks.size(); i++)
            {
              std::vector< std::vector<number> > data(this->sample_points.size());

              // now arrange data to consist of the <zeta zeta> 2pf
              for(int j = 0; j < this->sample_points.size(); j++)
                {
                  data[j].resize(1);    // only one components of <zeta zeta>

                  // compute gauge transformation
                  std::vector<number> dN;
                  this->gauge_xfm->compute_gauge_xfm_1(this->backg.get_value(j), dN);

                  data[j][0] = 0;
                  for(int m = 0; m < 2*this->N_fields; m++)
                    {
                      for(int n = 0; n < 2*this->N_fields; n++)
                        {
                          unsigned int samples_index = 2*this->N_fields*m + n;
                          data[j][0] += dN[m]*dN[n]*this->samples[j][samples_index][i];
                        }
                    }

                  if(dimensionless)   // plotting the dimensionless power spectrum?
                    {
                      data[j][0] *= pow(this->sample_com_ks[i], 3.0) / (2.0*M_PI*M_PI);
                    }
                }

              std::ostringstream fnam;
              fnam << output << "_" << i;

              std::ostringstream title;
              title << "$k = " << this->sample_ks[i] << "$";

              std::vector<std::string> labels(1);
              std::ostringstream l;
              if(dimensionless)
                {
                  l << "$" << DIMENSIONLESS_TWOPF_SYMBOL << "_{" << ZETA_SYMBOL << "}$";
                }
              else
                {
                  l << "$" << TWOPF_SYMBOL << "_{" << ZETA_SYMBOL << " " << ZETA_SYMBOL << "}$";
                }
              labels[0] = l.str();

              gadget->set_format(format);
              gadget->plot(fnam.str(), title.str(), this->sample_points, data, labels, "$N$", "two-point function", false, logy);
            }
        }

      template<typename number>
      index_selector<2>* twopf<number>::manufacture_selector()
        {
          return new index_selector<2>(this->N_fields);
        }

      template <typename number>
      background<number>& twopf<number>::get_background()
        {
          return(this->background);
        }

      template <typename number>
      std::ostream& operator<<(std::ostream& out, twopf<number>& obj)
        {
          transport::asciitable<number> writer(out);

          out << obj.backg << std::endl;

          std::vector<std::string> labels;
          for(int i = 0; i < 2*obj.N_fields; i++)
            {
              for(int j = 0; j < 2*obj.N_fields; j++)
                {
                  std::ostringstream l;
                  l << obj.field_names[i] << ", " << obj.field_names[j];
                  labels.push_back(l.str());
                }
            }

          std::vector< std::vector<number> > twopf_components(obj.sample_points.size());
          for(int i = 0; i < obj.sample_ks.size(); i++)
            {
              if(i > 0) out << std::endl;
              out << "k = " << obj.sample_ks[i] << std::endl << std::endl;

              for(int j = 0; j < obj.sample_points.size(); j++)
                {
                  twopf_components[j].resize(2*obj.N_fields * 2*obj.N_fields);

                  for(int m = 0; m < 2*obj.N_fields; m++)
                    {
                      for(int n = 0; n < 2*obj.N_fields; n++)
                        {
                          unsigned int index = 2*obj.N_fields*m + n;

                          number temp = obj.samples[j][index][i];

                          twopf_components[j][index] = temp;
                        }
                    }
                }

              writer.write("e-folds", labels, obj.sample_points, twopf_components);
            }

          return(out);
        }


  }   // namespace transport

#endif // __CPP_TRANSPORT_TWOPF_H_
