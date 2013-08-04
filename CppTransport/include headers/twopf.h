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
#include "latex_output.h"
#include "messages_en.h"

#include "plot_gadget.h"
#include "gauge_xfm_gadget.h"


#define TWOPF_SYMBOL               "\\Sigma"
#define TWOPF_NAME                 "Sigma"
#define ZETA_SYMBOL                "\\zeta"
#define ZETA_NAME                  "zeta"
#define DIMENSIONLESS_TWOPF_SYMBOL "\\mathcal{P}"
#define DIMENSIONLESS_TWOPF_NAME   "P"
#define PRIME_SYMBOL               "\\prime"
#define PRIME_NAME                 "'"
#define K_SYMBOL                   "k"
#define K_NAME                     "k"

#define N_LABEL_LATEX              "$N$"
#define N_LABEL                    "N"
#define TWOPF_LABEL                "two-point function"

#define PICK_N_LABEL               (gadget->latex_labels() ? N_LABEL_LATEX : N_LABEL)

#define DEFAULT_TWOPF_WRAP_WIDTH   (135)
#define DEFAULT_TWOPF_PRECISION    (3)

namespace transport
  {
      // DATA PRODUCTS -- objects wrapping the various data products produced by each model

      // handle weirdness with friend template functions
      // see http://www.cplusplus.com/forum/general/45776/
      template <typename number> class twopf;

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
              gauge_xfm_gadget<number>* gx, tensor_gadget<number>* t)
              : N_fields(N_f), field_names(f_names), latex_names(l_names),
                Nstar(Nst), sample_points(sp), sample_ks(ks), sample_com_ks(com_ks),
                backg(N_f, f_names, l_names, sp, b, t->clone()), samples(twopf),
                gauge_xfm(gx), tensors(t),
                wrap_width(DEFAULT_TWOPF_WRAP_WIDTH)
              {}
            ~twopf() { /*delete this->gauge_xfm; delete this->tensors*/ }

            background<number>& get_background();

            void components_time_history(plot_gadget<number>* gadget, std::string output,
              index_selector<2>* selector, std::string format = "pdf", bool logy=true);
            void zeta_time_history      (plot_gadget<number>* gadget, std::string output,
              std::string format = "pdf", bool dimensionless = true, bool logy=true);

            index_selector<2>* manufacture_selector();

            // provide << operator to output data to a stream
            friend std::ostream& operator<< <>(std::ostream& out, twopf& obj);

            unsigned int get_wrap_width();
            void         set_wrap_width(unsigned int w);
          
            friend class threepf<number>;

          protected:
            // make a list of labels for the chosen index selection
            std::vector< std::string>          make_labels(index_selector<2>* selector, bool latex);
            std::string                        make_zeta_label(bool dimensionless, bool latex);

            std::string                        make_twopf_title(double k, bool latex);

            // return a time history for a given set of components and a fixed k-number
            std::vector< std::vector<number> > construct_kmode_time_history(index_selector<2>* selector, unsigned int i);
          
            // return values for a given kvalue and timeslice
            std::vector< number >              get_value(unsigned int time, unsigned int kmode);

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
            const std::vector< std::vector< std::vector<number> > > samples;           // list of samples of 2pf
              // first index: time of observation
              // second index: 2pf component
              // third index: k

            unsigned int                                            wrap_width;        // position to wrap when output to stream
        };


//  IMPLEMENTATION -- CLASS twopf


      template <typename number>
      void twopf<number>::components_time_history(plot_gadget<number>* gadget, std::string output,
        index_selector<2>* selector, std::string format, bool logy)
        {
          std::vector< std::string > labels = this->make_labels(selector, gadget->latex_labels());

          // loop over k-modes
          for(int i = 0; i < this->sample_ks.size(); i++)
            {
              std::vector< std::vector<number> > data = this->construct_kmode_time_history(selector, i);

              std::ostringstream fnam;
              fnam << output << "_" << i;

              gadget->set_format(format);
              gadget->plot(fnam.str(), this->make_twopf_title(this->sample_ks[i], gadget->latex_labels()),
                           this->sample_points, data, labels, PICK_N_LABEL, TWOPF_LABEL, false, logy);
            }
        }


      template <typename number>
      std::vector< std::string > twopf<number>::make_labels(index_selector<2>* selector, bool latex)
        {
          std::vector< std::string > labels;

          for(int m = 0; m < 2*this->N_fields; m++)
            {
              for(int n = 0; n < 2*this->N_fields; n++)
                {
                  std::array<unsigned int, 2> index_set = { (unsigned int)m, (unsigned int)n, };
                  if(selector->is_on(index_set))
                    {
                      std::ostringstream label;

                      if(latex)
                        {
                          label << "$" << TWOPF_SYMBOL << "_{"
                                << this->latex_names[m % this->N_fields] << (m >= this->N_fields ? PRIME_SYMBOL : "") << " "
                                << this->latex_names[n % this->N_fields] << (n >= this->N_fields ? PRIME_SYMBOL : "") << "}$";
                        }
                      else
                        {
                          label << this->field_names[m % this->N_fields] << (m >= this->N_fields ? PRIME_NAME : "") << ", "
                                << this->field_names[n % this->N_fields] << (n >= this->N_fields ? PRIME_NAME : "");
                        }
                      labels.push_back(label.str());
                    }
                }
            }

          return(labels);
        }


      template <typename number>
      std::string twopf<number>::make_zeta_label(bool dimensionless, bool latex)
        {
          std::ostringstream label;

          if(latex)
            {
              if(dimensionless)
                {
                  label << "$" << DIMENSIONLESS_TWOPF_SYMBOL << "_{" << ZETA_SYMBOL << "}$";
                }
              else
                {
                  label << "$" << TWOPF_SYMBOL << "_{" << ZETA_SYMBOL << " " << ZETA_SYMBOL << "}$";
                }
            }
          else
            {
              if(dimensionless)
                {
                  label << DIMENSIONLESS_TWOPF_NAME << "(" << ZETA_NAME << ")";
                }
              else
                {
                  label << TWOPF_NAME << "(" << ZETA_NAME << ")";
                }
            }

          return(label.str());
        }


      template <typename number>
      std::string twopf<number>::make_twopf_title(double k, bool latex)
        {
          std::ostringstream title;

          if(latex)
            {
              title << "$" << K_SYMBOL << " = " << output_latex_number(k, DEFAULT_TWOPF_PRECISION) << "$";
            }
          else
            {
              title << std::setprecision(DEFAULT_TWOPF_PRECISION);
              title << K_NAME << " = " << k;
            }

          return(title.str());
        }


      // for a specific k-mode, return the time history of a set of components
      // (index order: first index = time, second index = component)
      template <typename number>
      std::vector< std::vector<number> > twopf<number>::construct_kmode_time_history(index_selector<2>* selector, unsigned int i)
        {
          std::vector< std::vector<number> > data(this->sample_points.size());

          for(int j = 0; j < this->sample_points.size(); j++)
            {
              // now, for the chosen k-mode i, slice up the time series
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

           return(data);
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

              std::vector<std::string> labels(1);
              labels[0] = this->make_zeta_label(dimensionless, gadget->latex_labels());

              gadget->set_format(format);
              gadget->plot(fnam.str(), this->make_twopf_title(this->sample_ks[i], gadget->latex_labels()),
                           this->sample_points, data, labels, PICK_N_LABEL, TWOPF_LABEL, false, logy);
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
          writer.set_display_width(obj.get_wrap_width());

          index_selector<2>* selector = obj.manufacture_selector();

          std::vector<std::string> labels = obj.make_labels(selector, false);

          for(int i = 0; i < obj.sample_ks.size(); i++)
            {
              if(i > 0) out << std::endl;
              out << __CPP_TRANSPORT_TWOPF_MESSAGE << " k = " << obj.sample_ks[i] << std::endl << std::endl;

              std::vector< std::vector<number> > data = obj.construct_kmode_time_history(selector, i);

              writer.write(__CPP_TRANSPORT_EFOLDS, labels, obj.sample_points, data);
            }

          return(out);
        }

      template <typename number>
      std::vector<number> twopf<number>::get_value(unsigned int time, unsigned int kmode)
        {
          std::vector<number> rval(2*this->N_fields * 2*this->N_fields);
          
          assert(time < this->sample_points.size());
          assert(kmode < this->sample_ks.size());
          
          for(int i = 0; i < 2*this->N_fields*2*this->N_fields; i++)
            {
              rval[i] = this->samples[time][i][kmode];
            }
          
          return(rval);
        }

      template <typename number>
      void twopf<number>::set_wrap_width(unsigned int w)
        {
          this->wrap_width = (w > 0 ? w : this->wrap_width);
        }

       template <typename number>
       unsigned int twopf<number>::get_wrap_width()
        {
          return(this->wrap_width);
        }


  }   // namespace transport

#endif // __CPP_TRANSPORT_TWOPF_H_
