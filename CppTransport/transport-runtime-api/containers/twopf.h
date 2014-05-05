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

#include "transport-runtime-api/default_symbols.h"
#include "transport-runtime-api/utilities/asciitable.h"
#include "transport-runtime-api/utilities/latex_output.h"
#include "transport-runtime-api/concepts/flattener.h"
#include "transport-runtime-api/containers/label_gadget.h"
#include "transport-runtime-api/containers/index_selector.h"
#include "transport-runtime-api/plotgadgets/plot_gadget.h"
#include "transport-runtime-api/messages.h"

#include "transport-runtime-api/containers/background.h"


namespace transport
  {
      // handle weirdness with friend template functions
      // see http://www.cplusplus.com/forum/general/45776/
      template <typename number> class twopf;

      template <typename number>
      std::ostream& operator<<(std::ostream& out, twopf<number>& obj);


      template <typename number>
      class twopf
        {
          public:
            twopf(const twopf_list_task<number>* t, const std::vector< std::vector<number> >& b,
              const std::vector< std::vector< std::vector<number> > >& twopf,
              model<number>* p)
              : labels(p->get_N_fields(), p->get_field_names(), p->get_f_latex_names()),
                tk(t), backg(t, b, p), samples(twopf),
                parent(p),
                wrap_width(DEFAULT_WRAP_WIDTH),
                plot_precision(DEFAULT_PLOT_PRECISION)
              {}

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
            std::string                        make_zeta_label(bool dimensionless, bool latex);

            std::string                        make_twopf_title(double k, bool latex);

            // return a time history for a given set of components and a fixed k-mode
            std::vector< std::vector<number> > construct_kmode_time_history(index_selector<2>* selector, unsigned int i);

            // return a time history for the <zeta zeta> correlation function and a fixed k-mode
            std::vector< std::vector<number> > construct_zeta_time_history(unsigned int i, bool dimensionless=false);
          
            // return values for a given kvalue and timeslice
            std::vector< number >              get_value(unsigned int time, unsigned int kmode);

            model<number>*                                          parent;            // parent model object

            label_gadget                                            labels;            // holds names (and LaTeX names) of fields

            const twopf_list_task<number>*                          tk;                // task object used to generate this solution

            background<number>                                      backg;             // container for background
            const std::vector< std::vector< std::vector<number> > > samples;           // list of samples of 2pf
              // first index: time of observation
              // second index: 2pf component
              // third index: k

            unsigned int                                            wrap_width;        // position to wrap when output to stream
            unsigned int                                            plot_precision;    // precision to use when making plot labels
        };


//  IMPLEMENTATION -- CLASS twopf


      template <typename number>
      void twopf<number>::components_time_history(plot_gadget<number>* gadget, std::string output,
        index_selector<2>* selector, std::string format, bool logy)
        {
          std::vector<double> sample_points = this->tk->get_sample_times();
          std::vector<double> sample_ks     = this->tk->get_k_list();

          std::vector< std::string > labels = this->labels.make_labels(selector, gadget->latex_labels());

          // loop over k-modes
#pragma omp for schedule(guided)
          for(int i = 0; i < sample_ks.size(); i++)
            {
              std::vector< std::vector<number> > data = this->construct_kmode_time_history(selector, i);

              std::ostringstream fnam;
              fnam << output << "_" << i;

              gadget->set_format(format);
              gadget->plot(fnam.str(), this->make_twopf_title(sample_ks[i], gadget->latex_labels()),
                           sample_points, data, labels, PICK_N_LABEL, TWOPF_LABEL, false, logy);
            }
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
              title << "$" << K_SYMBOL << " = " << output_latex_number(k, this->plot_precision) << "$";
            }
          else
            {
              title << std::setprecision(this->plot_precision);
              title << K_NAME << " = " << k;
            }

          return(title.str());
        }


      // for a specific k-mode, return the time history of a set of components
      // (index order: first index = time, second index = component)
      template <typename number>
      std::vector< std::vector<number> > twopf<number>::construct_kmode_time_history(index_selector<2>* selector, unsigned int i)
        {
          std::vector< std::vector<number> > data(this->tk->get_sample_times().size());

          for(int j = 0; j < this->tk->get_sample_times().size(); j++)
            {
              // now, for the chosen k-mode i, slice up the time series
              for(int m = 0; m < 2*this->parent->get_N_fields(); m++)
                {
                  for(int n = 0; n < 2*this->parent->get_N_fields(); n++)
                    {
                      std::array<unsigned int, 2> index_set = { (unsigned int)m, (unsigned int)n };
                      if(selector->is_on(index_set))
                        {
                          unsigned int samples_index = this->parent->flatten(m,n);

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
          std::vector<double> sample_points = this->tk->get_sample_times();
          std::vector<double> sample_ks     = this->tk->get_k_list();

          // loop over k-modes
#pragma omp for schedule(guided)
          for(int i = 0; i < sample_ks.size(); i++)
            {
              std::vector< std::vector<number> > data = this->construct_zeta_time_history(i, dimensionless);

              std::ostringstream fnam;
              fnam << output << "_" << i;

              std::vector<std::string> labels(1);
              labels[0] = this->make_zeta_label(dimensionless, gadget->latex_labels());

              gadget->set_format(format);
              gadget->plot(fnam.str(), this->make_twopf_title(sample_ks[i], gadget->latex_labels()),
                           sample_points, data, labels, PICK_N_LABEL, TWOPF_LABEL, false, logy);
            }
        }


      template <typename number>
      std::vector< std::vector<number> > twopf<number>::construct_zeta_time_history(unsigned int i, bool dimensionless)
        {
          std::vector< std::vector<number> > data(this->tk->get_sample_times().size());

          // now arrange data to consist of the <zeta zeta> 2pf
          for(int j = 0; j < this->tk->get_sample_times().size(); j++)
            {
              data[j].resize(1);    // only one components of <zeta zeta>

              // compute gauge transformation
              std::vector<number> dN;
              this->parent->compute_gauge_xfm_1(this->tk->get_params(), this->backg.get_value(j), dN);

              data[j][0] = 0;
              for(int m = 0; m < 2*this->parent->get_N_fields(); m++)
                {
                  for(int n = 0; n < 2*this->parent->get_N_fields(); n++)
                    {
                      unsigned int samples_index = this->parent->flatten(m,n);
                      data[j][0] += dN[m]*dN[n]*this->samples[j][samples_index][i];
                    }
                }

              if(dimensionless)   // are we plotting the dimensionless power spectrum?
                {
                  // can access comoving k list by overloading
                  data[j][0] *= pow(this->tk->get_k_comoving(i), 3.0) / (2.0*M_PI*M_PI);
                }
            }

          return(data);
        }


      template <typename number>
      index_selector<2>* twopf<number>::manufacture_selector()
        {
          return new index_selector<2>(this->parent->get_N_fields());
        }

      template <typename number>
      background<number>& twopf<number>::get_background()
        {
          return(this->backg);
        }


      template <typename number>
      std::ostream& operator<<(std::ostream& out, twopf<number>& obj)
        {
          transport::asciitable<number> writer(out);
          writer.set_display_width(obj.get_wrap_width());

          index_selector<2>* selector = obj.manufacture_selector();
          std::vector<std::string> labels = obj.labels.make_labels(selector, false);

          for(int i = 0; i < obj.sample_ks.size(); i++)
            {
              if(i > 0) out << std::endl;
              out << __CPP_TRANSPORT_TWOPF_MESSAGE << " " << K_NAME << " = " << obj.sample_ks[i] << std::endl << std::endl;

              std::vector< std::vector<number> > data = obj.construct_kmode_time_history(selector, i);

              writer.write(__CPP_TRANSPORT_EFOLDS, labels, obj.sample_points, data);
            }

          delete selector;

          return(out);
        }

      template <typename number>
      std::vector<number> twopf<number>::get_value(unsigned int time, unsigned int kmode)
        {
          std::vector<number> rval(2*this->parent->get_N_fields() * 2*this->parent->get_N_fields());
          
          assert(time < this->tk->get_sample_times().size());
          assert(kmode < this->tk->get_k_list().size());
          
          for(int i = 0; i < 2*this->parent->get_N_fields() * 2*this->parent->get_N_fields(); i++)
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
