//
// Created by David Seery on 12/08/2013.
// Copyright (c) 2013 University of Sussex. All rights reserved.
//
// To change the template use AppCode | Preferences | File Templates.
//

#ifndef __CPP_TRANSPORT_LABEL_GADGET_H_
#define __CPP_TRANSPORT_LABEL_GADGET_H_


#include <vector>
#include <array>
#include <string>
#include <sstream>

#include "transport/default_symbols.h"
#include "transport/containers/index_selector.h"


namespace transport
  {

    class label_gadget
      {
        public:
          label_gadget(unsigned int N, const std::vector<std::string> f, const std::vector<std::string> l);

          std::vector< std::string > make_labels(index_selector<1>* selector, bool latex);
          std::vector< std::string > make_labels(index_selector<2>* selector, bool latex);
          std::vector< std::string > make_labels(index_selector<3>* selector, bool latex);

          std::string make_zeta_bispectrum_label(bool latex);
          std::string make_reduced_bispectrum_label(bool latex);
          std::string make_shape_bispectrum_label(bool latex);

        protected:
          unsigned int                   N_fields;                                    // number of fields
          const std::vector<std::string> field_names;                                 // vector of names - excludes momenta
          const std::vector<std::string> latex_names;                                 // vector of LaTeX names - excludes momenta
      };


    inline label_gadget::label_gadget(unsigned int N, const std::vector<std::string> f, const std::vector<std::string> l)
      : N_fields(N), field_names(f), latex_names(l)
      {
        assert(field_names.size() == latex_names.size());
        assert(field_names.size() == N_fields);
        assert(latex_names.size() == N_fields);
      }


    inline std::vector< std::string > label_gadget::make_labels(index_selector<1>* selector, bool latex)
      {
        std::vector< std::string > labels;
        unsigned int dimension = selector->get_dimension();

        for(int m = 0; m < dimension*this->N_fields; m++)
          {
            std::array<unsigned int, 1> index_set = { (unsigned int)m };
            if(selector->is_on(index_set))
              {
                std::ostringstream label;

                if(latex)
                  {
                    label << "$" << this->latex_names[m % this->N_fields] << (m >= this->N_fields ? "^{" PRIME_SYMBOL "}" : "") << "$";
                  }
                else
                  {
                    label << this->field_names[m % this->N_fields] << (m >= this->N_fields ? PRIME_NAME : "");
                  }
                labels.push_back(label.str());
              }
          }

        return(labels);
      }


    inline std::vector< std::string > label_gadget::make_labels(index_selector<2>* selector, bool latex)
      {
        std::vector< std::string > labels;
        unsigned int dimension = selector->get_dimension();

        for(int m = 0; m < dimension*this->N_fields; m++)
          {
            for(int n = 0; n < dimension*this->N_fields; n++)
              {
                std::array<unsigned int, 2> index_set = { (unsigned int)m, (unsigned int)n, };
                if(selector->is_on(index_set))
                  {
                    std::ostringstream label;

                    if(latex)
                      {
                        label << "$" << this->latex_names[m % this->N_fields] << (m >= this->N_fields ? PRIME_SYMBOL : "") << " "
                              << this->latex_names[n % this->N_fields] << (n >= this->N_fields ? PRIME_SYMBOL : "")        << "$";
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


    inline std::vector< std::string > label_gadget::make_labels(index_selector<3> *selector, bool latex)
      {
        std::vector< std::string > labels;
        unsigned int dimension = selector->get_dimension();

        for(int m = 0; m < dimension*this->N_fields; m++)
          {
            for(int n = 0; n < dimension*this->N_fields; n++)
              {
                for(int r = 0; r < dimension*this->N_fields; r++)
                  {
                    std::array<unsigned int, 3> index_set = { (unsigned int)m, (unsigned int)n, (unsigned int)r };
                    if(selector->is_on(index_set))
                      {
                        std::ostringstream label;

                        if(latex)
                          {
                            label << "$" << this->latex_names[m % this->N_fields] << (m >= this->N_fields ? PRIME_SYMBOL : "") << " "
                                  << this->latex_names[n % this->N_fields] << (n >= this->N_fields ? PRIME_SYMBOL : "")        << " "
                                  << this->latex_names[r % this->N_fields] << (r >= this->N_fields ? PRIME_SYMBOL : "")        << "$";
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


    inline std::string label_gadget::make_zeta_bispectrum_label(bool latex)
      {
        std::ostringstream label;

        if(latex)
          {
            label << "$" << THREEPF_SYMBOL << "_{" << ZETA_SYMBOL << " " << ZETA_SYMBOL << " " << ZETA_SYMBOL << "}$";
          }
        else
          {
            label << THREEPF_NAME << "(" << ZETA_NAME << ", " << ZETA_NAME << ", " << ZETA_NAME << ")";
          }

        return(label.str());
      }


    inline std::string label_gadget::make_reduced_bispectrum_label(bool latex)
      {
        std::ostringstream label;

        if(latex)
          {
            label << "$" << REDUCED_BISPECTRUM_SYMBOL << "$";
          }
        else
          {
            label << REDUCED_BISPECTRUM_NAME;
          }

        return(label.str());
      }


    inline std::string label_gadget::make_shape_bispectrum_label(bool latex)
      {
        std::ostringstream label;

        if(latex)
          {
            label << "$" << SHAPE_BISPECTRUM_SYMBOL << "$";
          }
        else
          {
            label << SHAPE_BISPECTRUM_NAME;
          }

        return(label.str());
      }


  }   // namespace transport


#endif // __CPP_TRANSPORT_LABEL_GADGET_H_