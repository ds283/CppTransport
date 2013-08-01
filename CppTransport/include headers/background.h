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

#include "asciitable.h"
#include "plot_gadget.h"
#include "index_selector.h"
#include "tensor_gadget.h"
#include "messages_en.h"

#define U2_SYMBOL     "u"
#define U3_SYMBOL     "u"
#define PRIME_SYMBOL  "\\prime"

#define DEFAULT_BACKGROUND_WRAP_WIDTH (135)

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
            background(unsigned int N_f, const std::vector<std::string>& f_names, const std::vector<std::string>& l_names,
              const std::vector<double>& sp, const std::vector< std::vector<number> >& s, tensor_gadget<number>* t)
              : N_fields(N_f), field_names(f_names), latex_names(l_names), sample_points(sp), samples(s),
                tensors(t),
                wrap_width(DEFAULT_BACKGROUND_WRAP_WIDTH)
              {}
            ~background() { /*delete this->tensors;*/ }

            index_selector<2>* manufacture_2_selector();
            index_selector<3>* manufacture_3_selector();

            void plot(plot_gadget<number>*gadget, std::string output, std::string title="", std::string format="pdf", bool logy=false);

            void plot_u2(plot_gadget<number>* gadget, double k,
                         std::string output, index_selector<2>* selector, std::string title="", std::string format="pdf", bool logy=true);
            void plot_u3(plot_gadget<number>* gadget, double k1, double k2, double k3,
                         std::string output, index_selector<3>* selector, std::string title="", std::string format="pdf", bool logy=true);

            // provide << operator to output data to a stream
            friend std::ostream& operator<< <>(std::ostream& out, background& obj);

            // need to find a better way of solving this problem
            // this is needed to normalize the comoving ks correctly in each $$__MODEL class
            const std::vector<number>&  __INTERNAL_ONLY_get_value (unsigned int n);

            unsigned int get_wrap_width();
            void         set_wrap_width(unsigned int w);

            friend class twopf<number>;
            friend class threepf<number>;

          protected:
            const std::vector<number>& get_value (unsigned int n);

            std::vector< std::string > make_labels(index_selector<2>* selector, bool latex);
            std::vector< std::string > make_labels(index_selector<3>* selector, bool latex);

            std::vector< std::vector<number> > construct_u2_time_history(index_selector<2>* selector, double k);
            std::vector< std::vector<number> > construct_u3_time_history(index_selector<3>* selector, double k1, double k2, double k3);

            unsigned int                             N_fields;          // number of fields
            const std::vector<std::string>           field_names;       // vector of names - includes momenta
            const std::vector<std::string>           latex_names;       // vector of LaTeX names - excludes momenta

            tensor_gadget<number>*                   tensors;           // tensor calculation gadget

            const std::vector<double>                sample_points;     // list of times at which we hold samples for the background

            const std::vector< std::vector<number> > samples;           // list of samples
              // first index: time of observation
              // second index: field label

            unsigned int                             wrap_width;        // position to wrap when output to stream
        };


//  IMPLEMENTATION -- CLASS background


      template <typename number>
      void background<number>::plot(plot_gadget<number>* gadget, std::string output, std::string title, std::string format, bool logy)
        {
          gadget->set_format(format);
          gadget->plot(output, title, this->sample_points, this->samples, this->field_names, "N", "fields", false, logy);
        }

      template <typename number>
      const std::vector<number>& background<number>::get_value(unsigned int n)
        {
          assert(n < this->sample_points.size());

          return(this->samples[n]);
        }

      template <typename number>
      const std::vector<number>& background<number>::__INTERNAL_ONLY_get_value(unsigned int n)
        {
          return(this->get_value(n));
        }

      template <typename number>
      std::ostream& operator<<(std::ostream& out, background<number>& obj)
        {
          transport::asciitable<number> writer(out);
          writer.set_display_width(obj.get_wrap_width());

          writer.write(__CPP_TRANSPORT_EFOLDS, obj.field_names, obj.sample_points, obj.samples);

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
      index_selector<2>* background<number>::manufacture_2_selector()
        {
          return new index_selector<2>(this->N_fields);
        }

      template <typename number>
      index_selector<3>* background<number>::manufacture_3_selector()
        {
          return new index_selector<3>(this->N_fields);
        }


      template <typename number>
      std::vector< std::string > background<number>::make_labels(index_selector<2>* selector, bool latex)
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
                          label << "$" << U2_SYMBOL << "_{"
                            << this->latex_names[m % this->N_fields] << (m >= this->N_fields ? PRIME_SYMBOL : "") << " "
                            << this->latex_names[n % this->N_fields] << (n >= this->N_fields ? PRIME_SYMBOL : "") << "}$";
                        }
                      else
                        {
                          label << this->field_names[m % this->N_fields] << (m >= this->N_fields ? "'" : "") << ", "
                            << this->field_names[n % this->N_fields] << (n >= this->N_fields ? "'" : "");
                        }
                      labels.push_back(label.str());
                    }
                }
            }

          return(labels);
        }


      template <typename number>
      std::vector< std::string > background<number>::make_labels(index_selector<3>* selector, bool latex)
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
                              label << "$" << U3_SYMBOL << "_{"
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


      template <typename number>
      void background<number>::plot_u2(plot_gadget<number>* gadget, double k,
        std::string output, index_selector<2>* selector, std::string title, std::string format, bool logy)
        {
          std::vector< std::string > labels = this->make_labels(selector, gadget->latex_labels());

          std::vector< std::vector<number> > data = this->construct_u2_time_history(selector, k);

          std::ostringstream t;
          t << "$k = " << k << "$";

          gadget->set_format(format);
          gadget->plot(output, title != "" ? title : t.str(), this->sample_points, data, labels, "$N$", "u2-tensor", false, logy);
        }


      template <typename number>
      void background<number>::plot_u3(plot_gadget<number>* gadget, double k1, double k2, double k3,
        std::string output, index_selector<3>* selector, std::string title, std::string format, bool logy)
        {
          std::vector< std::string > labels = this->make_labels(selector, gadget->latex_labels());

          std::vector< std::vector<number> > data = this->construct_u3_time_history(selector, k1, k2, k3);

          std::ostringstream t;
          t << "$k_1 = " << k1 << "$, $k_2 = " << k2 << "$, $k_3 = " << k3 << "$";

          gadget->set_format(format);
          gadget->plot(output, title != "" ? title : t.str(), this->sample_points, data, labels, "$N$", "u3-tensor", false, logy);
        }


      template <typename number>
      std::vector< std::vector<number> > background<number>::construct_u2_time_history(index_selector<2>* selector, double k)
        {
          std::vector< std::vector<number> > data(this->sample_points.size());

          for(int i = 0; i < this->sample_points.size(); i++)
            {
              std::vector< std::vector<number> > u2 = this->tensors->u2(this->samples[i], k, this->sample_points[i]);

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
          std::vector< std::vector<number> > data(this->sample_points.size());

          for(int i = 0; i < this->sample_points.size(); i++)
            {
              std::vector< std::vector<number> > u3 = this->tensors->u3(this->samples[i], k1, k2, k3, this->sample_points[i]);

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
