//
// Created by David Seery on 25/06/2013.
// Copyright (c) 2013 University of Sussex. All rights reserved.
//
// To change the template use AppCode | Preferences | File Templates.
//

#ifndef __CPP_TRANSPORT_TRANSPORT_H_
#define __CPP_TRANSPORT_TRANSPORT_H_

#include <string>
#include <vector>

// basic class from which everything else is derived
namespace transport
  {
      class model
        {
          public:
            model::model(const std::string& n, const std::string& a, const std::string& t,
                         unsigned int N,
                         const std::string* f_names, const std::string* l_names);

            const std::string&              get_name();         // return name of model implemented by this instance
            const std::string&              get_author();       // return authors of model implemented by this instance
            const std::string&              get_tag();          // return tagline for model implemented by this instance

            unsigned int                    get_N_fields();     // return number of fields

            const std::vector<std::string>& get_field_names();  // return vector of field names
            const std::vector<std::string>& get_latex_names();  // return vector of LaTeX names

          protected:
            const std::string               name;               // name of model
            const std::string               author;             // authors
            const std::string               tag;                // tagline, perhaps used to indicate works to reference

            const unsigned int              N_fields;           // number of fields in the model
            std::vector<std::string>        field_names;        // vector of field names
            std::vector<std::string>        latex_names;        // vector of LaTeX names
        };

      class canonical_model
        {
          public:
            canonical_model::canonical_model(const std::string& n, const std::string &a, const std::string& t,
                                             unsigned int N,
                                             const std::string* f_names, const std::string* l_names);
        };


//  IMPLEMENTATION -- CLASS MODEL


      model::model(const std::string& n, const std::string& a, const std::string& t,
                   unsigned int N,
                   const std::string* f_names, const std::string* l_names)
        : name(n), author(a), tag(t), N_fields(N)
        {
          field_names.assign(f_names, f_names + N);
          latex_names.assign(l_names, l_names + N);
        }


      const std::string& model::get_name()
        {
          return(this->name);
        }


      const std::string& model::get_author()
        {
          return(this->name);
        }


      const std::string& model::get_tag()
        {
          return(this->name);
        }


      unsigned int model::get_N_fields()
        {
          return(this->N_fields);
        }


      const std::vector<std::string>& model::get_field_names()
        {
          return(this->field_names);
        }


      const std::vector<std::string>& model::get_latex_names()
        {
          return(this->latex_names);
        }


//  IMPLEMENTATION -- CLASS CANONICAL_MODEL


       canonical_model::canonical_model(const std::string& n, const std::string& a, const std::string& t,
                                        unsigned int N,
                                        const std::string* f_names, const std::string* l_names)
         : model(n, a, t, N, f_names, l_names)   // invoke constructor of parent class to initialize const members
         {
           return;
         }


  } // namespace transport


#endif __CPP_TRANSPORT_TRANSPORT_H_