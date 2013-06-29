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
namespace transport
  {
      // basic class from which all other model representations are derived
      template <typename number>
      class model
        {
          public:
            model(const std::string& n, const std::string& a, const std::string& t,
                  unsigned int N_f, unsigned int N_p,
                  const std::vector<std::string>& f_names, const std::vector<std::string>& fl_names,
                  const std::vector<std::string>& p_names, const std::vector<std::string>& pl_names,
                  const std::vector<number>& ps);

            const std::string&              get_name();           // return name of model implemented by this instance
            const std::string&              get_author();         // return authors of model implemented by this instance
            const std::string&              get_tag();            // return tagline for model implemented by this instance

            unsigned int                    get_N_fields();       // return number of fields
            unsigned int                    get_N_params();       // return number of parameters

            const std::vector<std::string>& get_field_names();    // return vector of field names
            const std::vector<std::string>& get_f_latex_names();  // return vector of LaTeX field names
            const std::vector<std::string>& get_param_names();    // return vector of parameter names
            const std::vector<std::string>& get_p_latex_names();  // return vector of LaTeX parameter names

            const std::vector<number>&      get_parameters();

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

            std::vector<number>             parameters;           // parameter values
        };

      // a canonical model: allows an arbitrary number of fields, but flat field-space metric
      // only has a potential
      template <typename number>
      class canonical_model : public model<number>
        {
          public:
            canonical_model(const std::string& n, const std::string &a, const std::string& t,
                            unsigned int N_f, unsigned int N_p,
                            const std::vector<std::string>& f_names, const std::vector<std::string>& fl_names,
                            const std::vector<std::string>& p_names, const std::vector<std::string>& pl_names,
                            const std::vector<number>& ps);
        };


//  IMPLEMENTATION -- CLASS MODEL


      template <typename number>
      model<number>::model(const std::string& n, const std::string& a, const std::string& t,
                           unsigned int N, unsigned int N_p,
                           const std::vector<std::string>& f_names, const std::vector<std::string>& fl_names,
                           const std::vector<std::string>& p_names, const std::vector<std::string>& pl_names,
                           const std::vector<number>& ps)
        : name(n), author(a), tag(t), N_fields(N), N_params(N_p),
          field_names(f_names), f_latex_names(fl_names),
          param_names(p_names), p_latex_names(pl_names),
          parameters(ps)
        {
          if(field_names.size()   != f_latex_names.size() ||
             field_names.size()   != N_fields ||
             f_latex_names.size() != N_fields)
            {
              std::cerr << "Error: supplied number of field names (or LaTeX versions) do not match expected number of fields\n";
              exit(EXIT_FAILURE);
            }

          if(param_names.size()   != p_latex_names.size() ||
             param_names.size()   != N_params ||
             p_latex_names.size() != N_params)
            {
              std::cerr << "Error: supplied number of parameter names (or LaTeX versions) do not match expected number of fields\n";
              exit(EXIT_FAILURE);
            }

          if(parameters.size()    != N_params)
            {
              std::cerr << "Error: unexpected number of parameters supplied\n";
              exit(EXIT_FAILURE);
            }

          return;
        }


      template <typename number>
      const std::string& model<number>::get_name()
        {
          return(this->name);
        }


      template <typename number>
      const std::string& model<number>::get_author()
        {
          return(this->author);
        }


      template <typename number>
      const std::string& model<number>::get_tag()
        {
          return(this->tag);
        }

      template <typename number>
      unsigned int model<number>::get_N_fields()
        {
          return(this->N_fields);
        }


      template <typename number>
      unsigned int model<number>::get_N_params()
        {
          return(this->N_params);
        }


      template <typename number>
      const std::vector<std::string>& model<number>::get_field_names()
        {
          return(this->field_names);
        }


      template <typename number>
      const std::vector<std::string>& model<number>::get_f_latex_names()
        {
          return(this->f_latex_names);
        }


      template <typename number>
      const std::vector<std::string>& model<number>::get_param_names()
        {
          return(this->param_names);
        }


      template <typename number>
      const std::vector<std::string>& model<number>::get_p_latex_names()
        {
          return(this->p_latex_names);
        }

      template <typename number>
      const std::vector<number>& model<number>::get_parameters()
        {
          return(this->parameters);
        }


//  IMPLEMENTATION -- CLASS CANONICAL_MODEL


      template <typename number>
      canonical_model<number>::canonical_model(const std::string& n, const std::string& a, const std::string& t,
                                               unsigned int N_f, unsigned int N_p,
                                               const std::vector<std::string>& f_names, const std::vector<std::string>& fl_names,
                                               const std::vector<std::string>& p_names, const std::vector<std::string>& pl_names,
                                               const std::vector<number>& ps)
        : model<number>(n, a, t, N_f, N_p, f_names, fl_names, p_names, pl_names, ps)   // invoke constructor of parent class to initialize const members
        {
          return;
        }


  } // namespace transport


#endif __CPP_TRANSPORT_TRANSPORT_H_