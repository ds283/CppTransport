//
// Created by David Seery on 30/06/2013.
// Copyright (c) 2013 University of Sussex. All rights reserved.
//
// To change the template use AppCode | Preferences | File Templates.
//

#ifndef __CPP_TRANSPORT_BACKGROUND_H_
#define __CPP_TRANSPORT_BACKGROUND_H_

#include <vector>
#include <assert.h>

#include "transport/default_symbols.h"
#include "transport/tasks/task.h"
#include "transport/models/model.h"
#include "transport/utilities/asciitable.h"
#include "transport/utilities/latex_output.h"
#include "transport/containers/label_gadget.h"
#include "transport/containers/index_selector.h"
#include "transport/plotgadgets/plot_gadget.h"
#include "transport/messages_en.h"


namespace transport
  {
      // DATA PRODUCTS -- objects wrapping the various data products produced by each model

      // handle weirdness with friend template functions
      // see http://www.cplusplus.com/forum/general/45776/
      template <typename number> class background;
      template <typename number> class twopf;
      template <typename number> class threepf;

      template <typename number>
      std::ostream& operator<<(std::ostream& out, background<number>& obj);


      template <typename number>
      class background
        {
          public:
            background(const task<number>* t, const std::vector< std::vector<number> >& s, model<number>* p)
              : labels(p->get_N_fields(), p->get_field_names(), p->get_f_latex_names()),
                tk(t), samples(s),
                parent(p),
                wrap_width(DEFAULT_WRAP_WIDTH),
                plot_precision(DEFAULT_PLOT_PRECISION)
              {}

            index_selector<1>* manufacture_selector();
            index_selector<2>* manufacture_2_selector();
            index_selector<3>* manufacture_3_selector();

            void plot(plot_gadget<number>*gadget, std::string output, index_selector<1>* selector,
                      std::string title="", std::string format="pdf", bool logy=false);

            void plot_u2(plot_gadget<number>* gadget, double k,
                         std::string output, index_selector<2>* selector, std::string title="", std::string format="pdf", bool logy=true);
            void plot_u3(plot_gadget<number>* gadget, double k1, double k2, double k3,
                         std::string output, index_selector<3>* selector, std::string title="", std::string format="pdf", bool logy=true);

            // provide << operator to output data to a stream
            friend std::ostream& operator<< <>(std::ostream& out, background& obj);

            unsigned int get_wrap_width();
            void         set_wrap_width(unsigned int w);

            friend class twopf<number>;
            friend class threepf<number>;

          protected:
            const std::vector<number>& get_value (unsigned int n);

            std::string                make_title(double k, bool latex);
            std::string                make_title(double k1, double k2, double k3, bool latex);

            std::vector< std::vector<number> > construct_fields_time_history(index_selector<1>* selector);

            std::vector< std::vector<number> > construct_u2_time_history(index_selector<2>* selector, double k);
            std::vector< std::vector<number> > construct_u3_time_history(index_selector<3>* selector, double k1, double k2, double k3);

            model<number>*                           parent;            // parent model object

            label_gadget                             labels;            // holds names (and LaTeX names) of fields, and makes labels

            const task<number>*                      tk;                // holds task object which generated this solution

            const std::vector< std::vector<number> > samples;           // list of samples
              // first index: time of observation
              // second index: field label

            unsigned int                             wrap_width;        // position to wrap when output to stream
            unsigned int                             plot_precision;    // precision to use when making plot labels
        };


//  IMPLEMENTATION -- CLASS background


      template <typename number>
      void background<number>::plot(plot_gadget<number>* gadget, std::string output, index_selector<1>* selector,
                                    std::string title, std::string format, bool logy)
        {
          std::vector< std::string > labels = this->labels.make_labels(selector, gadget->latex_labels());
          std::vector< std::vector<number> > data = this->construct_fields_time_history(selector);

          gadget->set_format(format);
          gadget->plot(output, title, this->tk->get_sample_times(), data, labels, PICK_N_LABEL, FIELDS_LABEL, false, logy);
        }


      template <typename number>
      std::vector< std::vector<number> > background<number>::construct_fields_time_history(index_selector<1>* selector)
        {
          std::vector<double> sample_points = this->tk->get_sample_times();

          std::vector< std::vector<number> > data(sample_points.size());

          for(int l = 0; l < sample_points.size(); l++)
            {
              for(int m = 0; m < 2*this->parent->get_N_fields(); m++)
                {
                  std::array<unsigned int, 1> index_set = { (unsigned int)m };
                  if(selector->is_on(index_set))
                    {
                      data[l].push_back(this->samples[l][this->parent->flatten(m)]);
                    }
                }
            }

          return(data);
        }


      template <typename number>
      const std::vector<number>& background<number>::get_value(unsigned int n)
        {
          std::vector<double> sample_points = this->tk->get_sample_times();
          assert(n < sample_points.size());

          return(this->samples[n]);
        }

      template <typename number>
      std::ostream& operator<<(std::ostream& out, background<number>& obj)
        {
          transport::asciitable<number> writer(out);
          writer.set_display_width(obj.get_wrap_width());

          index_selector<1>* selector = obj.manufacture_selector();
          std::vector<std::string> labels = obj.labels.make_labels(selector, false);

          writer.write(__CPP_TRANSPORT_EFOLDS, labels, obj.tk->get_sample_times(), obj.samples);

          delete selector;

          return(out);
        }

      template <typename number>
      void background<number>::set_wrap_width(unsigned int w)
        {
          this->wrap_width = (w > 0 ? w : this->wrap_width);
        }

      template <typename number>
      unsigned int background<number>::get_wrap_width()
        {
          return(this->wrap_width);
        }

      template <typename number>
      index_selector<1>* background<number>::manufacture_selector()
        {
          return new index_selector<1>(this->parent->get_N_fields());
        }

      template <typename number>
      index_selector<2>* background<number>::manufacture_2_selector()
        {
          return new index_selector<2>(this->parent->get_N_fields());
        }

      template <typename number>
      index_selector<3>* background<number>::manufacture_3_selector()
        {
          return new index_selector<3>(this->parent->get_N_fields());
        }


      template <typename number>
      std::string background<number>::make_title(double k, bool latex)
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


      template <typename number>
      std::string background<number>::make_title(double k1, double k2, double k3, bool latex)
        {
          std::ostringstream title;

          if(latex)
            {
              title << "$" << K1_SYMBOL << " = " << output_latex_number(k1, this->plot_precision) << "$, "
                    << "$" << K2_SYMBOL << " = " << output_latex_number(k2, this->plot_precision) << "$, "
                    << "$" << K3_SYMBOL << " = " << output_latex_number(k3, this->plot_precision) << "$";
            }
          else
            {
              title << std::setprecision(this->plot_precision);
              title << K1_NAME << " = " << k1 << ", "
                    << K2_NAME << " = " << k2 << ", "
                    << K3_NAME << " = " << k3;
            }

          return(title.str());
        }


      template <typename number>
      void background<number>::plot_u2(plot_gadget<number>* gadget, double k,
        std::string output, index_selector<2>* selector, std::string title, std::string format, bool logy)
        {
          std::vector< std::string > labels = this->labels.make_labels(selector, gadget->latex_labels());

          std::vector< std::vector<number> > data = this->construct_u2_time_history(selector, k);

          gadget->set_format(format);
          gadget->plot(output, title != "" ? title : this->make_title(k, gadget->latex_labels()),
                       this->tk->get_sample_times(), data, labels, PICK_N_LABEL, PICK_U2_LABEL, false, logy);
        }


      template <typename number>
      void background<number>::plot_u3(plot_gadget<number>* gadget, double k1, double k2, double k3,
        std::string output, index_selector<3>* selector, std::string title, std::string format, bool logy)
        {
          std::vector< std::string > labels = this->labels.make_labels(selector, gadget->latex_labels());

          std::vector< std::vector<number> > data = this->construct_u3_time_history(selector, k1, k2, k3);

          gadget->set_format(format);
          gadget->plot(output, title != "" ? title : this->make_title(k1, k2, k3, gadget->latex_labels()),
                       this->tk->get_sample_times(), data, labels, PICK_N_LABEL, PICK_U3_LABEL, false, logy);
        }


      template <typename number>
      std::vector< std::vector<number> > background<number>::construct_u2_time_history(index_selector<2>* selector, double k)
        {
          std::vector<double> sample_points = this->tk->get_sample_times();

          std::vector< std::vector<number> > data(sample_points.size());

          for(int i = 0; i < sample_points.size(); i++)
            {
              std::vector< std::vector<number> > u2;
              this->tensors->u2(this->samples[i], k, sample_points[i], u2);

              for(int m = 0; m < 2*this->N_fields; m++)
                {
                  for(int n = 0; n < 2*this->N_fields; n++)
                    {
                      std::array<unsigned int, 2> index_set = { (unsigned int)m, (unsigned int)n };

                      if(selector->is_on(index_set))
                        {
                          data[i].push_back(u2[m][n]);
                        }
                    }
                }
            }

          return(data);
        }


      template <typename number>
      std::vector< std::vector<number> > background<number>::construct_u3_time_history(index_selector<3>* selector, double k1, double k2, double k3)
        {
          std::vector<double> sample_points = this->tk->get_sample_times();

          std::vector< std::vector<number> > data(sample_points.size());

          for(int i = 0; i < sample_points.size(); i++)
            {
              std::vector< std::vector< std::vector<number> > > u3;
              this->tensors->u3(this->samples[i], k1, k2, k3, sample_points[i], u3);

              for(int m = 0; m < 2*this->N_fields; m++)
                {
                  for(int n = 0; n < 2*this->N_fields; n++)
                    {
                      for(int r = 0; r < 2*this->N_fields; r++)
                        {
                          std::array<unsigned int, 3> index_set = { (unsigned int)m, (unsigned int)n, (unsigned int)r };

                          if(selector->is_on(index_set))
                            {
                              data[i].push_back(u3[m][n][r]);
                            }
                        }
                    }
                }
            }

          return(data);
        }


  }   // namespace transport

#endif // __CPP_TRANSPORT_BACKGROUND_H_
