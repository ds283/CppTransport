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

#include "vexcl/vexcl.hpp"

#include "boost/numeric/odeint.hpp"
#include "boost/numeric/odeint/external/vexcl/vexcl_resize.hpp"

// basic class from which everything else is derived
namespace transport
  {
      class model
        {
          public:
            model(const std::string& n, const std::string& a, const std::string& t,
                  unsigned int N_f, unsigned int N_p,
                  const std::vector<std::string>& f_names, const std::vector<std::string>& fl_names,
                  const std::vector<std::string>& p_names, const std::vector<std::string>& pl_names);

            const std::string&              get_name();           // return name of model implemented by this instance
            const std::string&              get_author();         // return authors of model implemented by this instance
            const std::string&              get_tag();            // return tagline for model implemented by this instance

            unsigned int                    get_N_fields();       // return number of fields
            unsigned int                    get_N_params();       // return number of parameters

            const std::vector<std::string>& get_field_names();    // return vector of field names
            const std::vector<std::string>& get_f_latex_names();  // return vector of LaTeX field names
            const std::vector<std::string>& get_param_names();    // return vector of parameter names
            const std::vector<std::string>& get_p_latex_names();  // return vector of LaTeX parameter names

          protected:
            const std::string               name;                 // name of model
            const std::string               author;               // authors
            const std::string               tag;                  // tagline, perhaps used to indicate works to reference

            const unsigned int              N_fields;             // number of fields in the model
            const unsigned int              N_params;             // number of parameters in the model

            std::vector<std::string>        field_names;          // vector of field names
            std::vector<std::string>        f_latex_names;        // vector of LaTeX names for fields

            std::vector<std::string>        param_names;          // vector of parameter names
            std::vector<std::string>        p_latex_names;        // vector of parameter LaTeX names
        };

      class canonical_model : public model
        {
          public:
            canonical_model(const std::string& n, const std::string &a, const std::string& t,
                            unsigned int N_f, unsigned int N_p,
                            const std::vector<std::string>& f_names, const std::vector<std::string>& fl_names,
                            const std::vector<std::string>& p_names, const std::vector<std::string>& pl_names);
        };


//  IMPLEMENTATION -- CLASS MODEL


      model::model(const std::string& n, const std::string& a, const std::string& t,
                   unsigned int N, unsigned int N_p,
                   const std::vector<std::string>& f_names, const std::vector<std::string>& fl_names,
                   const std::vector<std::string>& p_names, const std::vector<std::string>& pl_names)
        : name(n), author(a), tag(t), N_fields(N), N_params(N_p),
          field_names(f_names), f_latex_names(fl_names),
          param_names(p_names), p_latex_names(pl_names)
        {
          if(f_names.size()       != f_latex_names.size() ||
             f_names.size()       != N_fields ||
             f_latex_names.size() != N_fields)
            {
              std::cerr << "Error: supplied number of field names (or LaTeX versions) do not match expected number of fields\n";
              exit(EXIT_FAILURE);
            }

          if(p_names.size()       != p_latex_names.size() ||
             p_names.size()       != N_params ||
             p_latex_names.size() != N_params)
            {
              std::cerr << "Error: supplied number of parameter names (or LaTeX versions) do not match expected number of fields\n";
              exit(EXIT_FAILURE);
            }
          return;
        }


      const std::string& model::get_name()
        {
          return(this->name);
        }


      const std::string& model::get_author()
        {
          return(this->author);
        }


      const std::string& model::get_tag()
        {
          return(this->tag);
        }


      unsigned int model::get_N_fields()
        {
          return(this->N_fields);
        }


      unsigned int model::get_N_params()
        {
          return(this->N_params);
        }


      const std::vector<std::string>& model::get_field_names()
        {
          return(this->field_names);
        }


      const std::vector<std::string>& model::get_f_latex_names()
        {
          return(this->f_latex_names);
        }


      const std::vector<std::string>& model::get_param_names()
        {
          return(this->param_names);
        }


      const std::vector<std::string>& model::get_p_latex_names()
        {
          return(this->p_latex_names);
        }


//  IMPLEMENTATION -- CLASS CANONICAL_MODEL


       canonical_model::canonical_model(const std::string& n, const std::string& a, const std::string& t,
                                        unsigned int N_f, unsigned int N_p,
                                        const std::vector<std::string>& f_names, const std::vector<std::string>& fl_names,
                                        const std::vector<std::string>& p_names, const std::vector<std::string>& pl_names)
         : model(n, a, t, N_f, N_p, f_names, fl_names, p_names, pl_names)   // invoke constructor of parent class to initialize const members
         {
           return;
         }


  } // namespace transport


#endif __CPP_TRANSPORT_TRANSPORT_H_