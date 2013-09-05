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

#include "default_symbols.h"
#include "index_selector.h"


namespace transport
  {

    class label_gadget
      {
      public:
        label_gadget(unsigned int N, const std::vector<std::string> f, const std::vector<std::string> l);

        std::vector< std::string > make_labels(index_selector<1>* selector, bool latex);
        std::vector< std::string > make_labels(index_selector<2>* selector, bool latex);
        std::vector< std::string > make_labels(index_selector<3>* selector, bool latex);

      protected:
        unsigned int N_fields;                                                      // number of fields
        const std::vector<std::string> field_names;                                 // vector of names - includes momenta
        const std::vector<std::string> latex_names;                                 // vector of LaTeX names - excludes momenta
      };


    inline label_gadget::label_gadget(unsigned int N, const std::vector<std::string> f, const std::vector<std::string> l)
      : N_fields(N), field_names(f), latex_names(l)
      {
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


  }   // namespace transport


#endif // __CPP_TRANSPORT_LABEL_GADGET_H_