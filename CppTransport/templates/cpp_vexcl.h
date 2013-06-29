//
// DO NOT EDIT: GENERATED AUTOMATICALLY BY $$__TOOL $$__VERSION
//
// '$$__HEADER' generated from '$$__SOURCE'
// processed on $$__DATE

#ifndef $$__GUARD
#define $$__GUARD

#include "transport/transport.h"

namespace transport
  {
      static std::vector<std::string> $$__MODEL_field_names = $$__FIELD_NAME_LIST;
      static std::vector<std::string> $$__MODEL_latex_names = $$__LATEX_NAME_LIST;
      static std::vector<std::string> $$__MODEL_param_names = $$__PARAM_NAME_LIST;
      static std::vector<std::string> $$__MODEL_platx_names = $$__PLATX_NAME_LIST;

      template <typename number>
      class $$__MODEL : public canonical_model<number>
        {
          public:
            $$__MODEL(const std::vector<number>& ps);
            ~$$__MODEL();

            // Integration of the model
            // ========================

            // Integrate the background, extracting values
            std::vector< std::vector<number> > background(vex::Context ctx,
              const std::vector<number> ics, number begin, number end, const std::vector<number> times);

          protected:

          private:
            const std::vector<number> params;
        };

      // IMPLEMENTATION -- CLASS $$__MODEL


      template <typename number>
      $$__MODEL<number>::$$__MODEL(const std::vector<number>& ps)
        : canonical_model<number>("$$__NAME", "$$__AUTHOR", "$$__TAG",
                                  $$__NUMBER_FIELDS, $$__NUMBER_PARAMS,
                                  $$__MODEL_field_names, $$__MODEL_latex_names,
                                  $$__MODEL_param_names, $$__MODEL_platx_names, ps),
          params(ps)
        {
          return;
        }


      template <typename number>
      $$__MODEL<number>::~$$__MODEL()
        {
          return;
        }

      
      template <typename number>
      std::vector< std::vector<number> > $$__MODEL<number>::background(vex::Context ctx,
        const std::vector<number> ics, number begin, number end, const std::vector<number> times)
        {
          std::vector<number> rics = ics;
          if(ics.size() == this->N_fields)  // initial conditions for momenta were not supplied
            {
              // supply the missing initial conditions using a slow-roll approximation
            }
          else if(ics.size() == 2*this->N_fields)  // initial conditions for momenta *were* supplied
            {
              // need do nothing
            }
          else
            {
              std::cerr << "Unexpected number of initial conditions (expected " << $$__NUMBER_FIELDS << " or " << 2*$$__NUMBER_FIELDS << "\n";
              exit(EXIT_FAILURE);
            }
        }

  }   // namespace transport


#endif  // $$__GUARD
