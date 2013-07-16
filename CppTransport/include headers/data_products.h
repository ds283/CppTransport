//
// Created by David Seery on 30/06/2013.
// Copyright (c) 2013 University of Sussex. All rights reserved.
//
// To change the template use AppCode | Preferences | File Templates.
//

#ifndef __CPP_TRANSPORT_DATA_PRODUCTS_H_
#define __CPP_TRANSPORT_DATA_PRODUCTS_H_

#include <string>
#include <sstream>
#include <vector>

#include "asciitable.h"
#include "messages_en.h"

#include "plot_gadget.h"
#include "gauge_xfm_gadget.h"

#define TWOPF_SYMBOL "\\Sigma"
#define ZETA_SYMBOL  "\\zeta"

namespace transport
  {
      // DATA PRODUCTS -- objects wrapping the various data products produced by each model

      // handle weirdness with friend template functions
      // see http://www.cplusplus.com/forum/general/45776/
      template <typename number>
      class background;

      template <typename number>
      std::ostream& operator<<(std::ostream& out, background<number>& obj);

      template <typename number>
      class twopf;

      template <typename number>
      std::ostream& operator<<(std::ostream& out, twopf<number>& obj);

      template <typename number>
      class background
        {
          public:
            background(unsigned int N_f, const std::vector<std::string>& f_names, const std::vector<std::string>& l_names,
              const std::vector<double>& sp, const std::vector< std::vector<number> >& s)
              : N_fields(N_f), field_names(f_names), latex_names(l_names), sample_points(sp), samples(s)
              {}

            void                        plot      (plot_gadget<number>* maker, std::string output, std::string title = "");

            const std::vector<number>&  get_value (unsigned int n);

            // provide << operator to output data to a stream
            friend std::ostream& operator<< <>(std::ostream& out, background& obj);

          protected:
            unsigned int                             N_fields;          // number of fields
            const std::vector<std::string>           field_names;       // vector of names - includes momenta
            const std::vector<std::string>           latex_names;       // vector of LaTeX names - excludes momenta

            const std::vector<double>                sample_points;     // list of times at which we hold samples for the background

            const std::vector< std::vector<number> > samples;           // list of samples
              // first index: time of observation
              // second index: field label
        };

      template <typename number>
      class twopf
        {
          public:
            twopf(unsigned int N_f, const std::vector<std::string>& f_names, const std::vector<std::string>& l_names,
              const std::vector<double>& ks, double Nst,
              const std::vector<number>& sp, const std::vector< std::vector<number> >& b,
              const std::vector< std::vector< std::vector<number> > >& twopf,
              gauge_xfm_gadget<number>* gx)
              : N_fields(N_f), field_names(f_names), latex_names(l_names),
                Nstar(Nst), sample_points(sp), sample_ks(ks),
                backg(N_f, f_names, l_names, sp, b), samples(twopf),
                gauge_xfm(gx)
              {}

            void fields_time_history(plot_gadget<number>* maker, std::string output, std::string format = "pdf");
            void zeta_time_history  (plot_gadget<number>* maker, std::string output, std::string format = "pdf");

            // provide << operator to output data to a stream
            friend std::ostream& operator<< <>(std::ostream& out, twopf& obj);

          protected:
            unsigned int                                            N_fields;          // number of fields
            const std::vector<std::string>                          field_names;       // vector of names - includes momenta
            const std::vector<std::string>                          latex_names;       // vector of LaTeX names - excludes momenta

            gauge_xfm_gadget<number>*                               gauge_xfm;         // gauge transformation gadget

            const double                                            Nstar;             // when was horizon-crossing for the mode k=1?

            const std::vector<double>                               sample_points;     // list of times at which we hold samples
            const std::vector<double>                               sample_ks;         // list of ks for which we hold samples

            background<number>                                      backg;             // container for background
            const std::vector< std::vector< std::vector<number> > > samples;           // list of samples of 2pf
              // first index: time of observation
              // second index: 2pf component
              // third index: k
        };

//  IMPLEMENTATION -- CLASS background

      template <typename number>
      void background<number>::plot(plot_gadget<number>* maker, std::string output, std::string title)
        {
          maker->plot(output, title, this->sample_points, this->samples, this->field_names, "N", "fields", false, false);
        }

      template <typename number>
      const std::vector<number>& background<number>::get_value(unsigned int n)
        {
          assert(n < this->sample_points.size());

          return(this->samples[n]);
        }

      template <typename number>
      std::ostream& operator<<(std::ostream& out, background<number>& obj)
        {
          transport::asciitable<number> writer(out);

          writer.write("e-folds", obj.field_names, obj.sample_points, obj.samples);

          return(out);
        }

//  IMPLEMENTATION -- CLASS twopf

      template <typename number>
      void twopf<number>::fields_time_history(plot_gadget<number>* maker, std::string output, std::string format)
        {
          // loop over k-modes
          for(int i = 0; i < this->sample_ks.size(); i++)
            {
              std::vector< std::vector<number> > data(this->sample_points.size());

              // we want data to be a time series of the 2pf components,
              // and there are N_fields^2 of those
              for(int j = 0; j < this->sample_points.size(); j++)
                {
                  data[j].resize(this->N_fields*this->N_fields);

                  // now, for this k-mode, slice up the time series
                  for(int m = 0; m < this->N_fields; m++)
                    {
                      for(int n = 0; n < this->N_fields; n++)
                        {
                          unsigned int samples_index = 2*this->N_fields*m + n;
                          unsigned int data_index    = this->N_fields*m + n;

                          data[j][data_index] = this->samples[j][samples_index][i];
                        }
                    }
                }

              std::ostringstream fnam;
              fnam << output << "_" << i;

              std::ostringstream title;
              title << "$k = " << this->sample_ks[i] << "$";

              std::vector<std::string> labels(this->N_fields*this->N_fields);
              for(int i = 0; i < this->N_fields; i++)
                {
                  for(int j = 0; j < this->N_fields; j++)
                    {
                      std::ostringstream l;
                      l << "$" << TWOPF_SYMBOL << "_{" << this->latex_names[i] << " " << this->latex_names[j] << "}$";
                      labels[this->N_fields*i+j] = l.str();
                    }
                }

              maker->set_format(format);
              maker->plot(fnam.str(), title.str(), this->sample_points, data, labels, "$N$", "two-point function", false, true);
            }
        }

      template <typename number>
      void twopf<number>::zeta_time_history(plot_gadget<number>* maker, std::string output, std::string format)
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
                }

              std::ostringstream fnam;
              fnam << output << "_" << i;

              std::ostringstream title;
              title << "$k = " << this->sample_ks[i] << "$";

              std::vector<std::string> labels(1);
              std::ostringstream l;
              l << "$" << TWOPF_SYMBOL << "_{" << ZETA_SYMBOL << " " << ZETA_SYMBOL << "}$";
              labels[0] = l.str();

              maker->set_format(format);
              maker->plot(fnam.str(), title.str(), this->sample_points, data, labels, "$N$", "two-point function", false, true);
            }
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

#endif // __CPP_TRANSPORT_MODEL_CLASSES_H_
