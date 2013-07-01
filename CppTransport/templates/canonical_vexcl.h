//
// DO NOT EDIT: GENERATED AUTOMATICALLY BY $$__TOOL $$__VERSION
//
// '$$__HEADER' generated from '$$__SOURCE'
// processed on $$__DATE

#ifndef $$__GUARD   // avoid multiple inclusion
#define $$__GUARD

#include <math.h>

#include "transport/transport.h"

namespace transport
  {
      static std::vector<std::string> $$__MODEL_field_names = $$__FIELD_NAME_LIST;
      static std::vector<std::string> $$__MODEL_latex_names = $$__LATEX_NAME_LIST;
      static std::vector<std::string> $$__MODEL_param_names = $$__PARAM_NAME_LIST;
      static std::vector<std::string> $$__MODEL_platx_names = $$__PLATX_NAME_LIST;
      static std::vector<std::string> $$__MODEL_state_names = $$__STATE_NAME_LIST;

      template <typename number>
      class $$__MODEL : public canonical_model<number>
        {
          public:
            $$__MODEL(number Mp, const std::vector<number>& ps);
            ~$$__MODEL();

            // Functions inherited from canonical_model
            number
              V(std::vector<number> fields);

            // Integration of the model
            // ========================

            // Integrate the background, extracting values
            transport::background<number>
              background(vex::Context& ctx, const std::vector<number>& ics, const std::vector<double>& times);

          protected:
            void
              fix_initial_conditions(const std::vector<number>& ics, std::vector<number>& rics);
            void
              write_initial_conditions(const std::vector<number>& rics, std::ostream& stream);
        };


      // integration - background functor
      template <typename number>
      class $$__MODEL_background_functor
        {
          public:
            $$__MODEL_background_functor(const std::vector<number> p, number Mp) : parameters(p), M_Planck(Mp) {}
            void operator()(const std::vector<number>& x, std::vector<number>& dxdt, double t);

          private:
            const number              M_Planck;
            const std::vector<number> parameters;
        };


      // integration - observer object for background only
      template <typename number>
      class $$__MODEL_background_observer
        {
          public:
            $$__MODEL_background_observer(std::vector<number>& s, std::vector< std::vector<number> >& h) : slices(s), history(h) {}
            void operator()(const std::vector<number>& x, double t);

          private:
            std::vector<number>&                slices;
            std::vector< std::vector<number> >& history;
        };


      // IMPLEMENTATION -- CLASS $$__MODEL
      // ==============


      template <typename number>
      $$__MODEL<number>::$$__MODEL(number Mp, const std::vector<number>& ps)
        : canonical_model<number>("$$__NAME", "$$__AUTHOR", "$$__TAG", Mp,
                                  $$__NUMBER_FIELDS, $$__NUMBER_PARAMS,
                                  $$__MODEL_field_names, $$__MODEL_latex_names,
                                  $$__MODEL_param_names, $$__MODEL_platx_names, ps)
        {
          return;
        }


      template <typename number>
      $$__MODEL<number>::~$$__MODEL()
        {
          return;
        }


      template <typename number>
      number $$__MODEL<number>::V(std::vector<number> fields)
        {
          assert(fields.size() == $$__NUMBER_FIELDS);
          $$__SET_PARAMETERS{number, this->parameters}
          $$__SET_FIELDS{number, fields}

          number rval = $$__V;

          return(rval);
        }

      
      template <typename number>
      transport::background<number> $$__MODEL<number>::background(vex::Context& ctx,
        const std::vector<number>& ics, const std::vector<double>& times)
        {
          using namespace boost::numeric::odeint;

          std::vector<number> x = ics;

          fix_initial_conditions(ics, x);
          write_initial_conditions(x, std::cout);

          std::vector<number>                slices;
          std::vector< std::vector<number> > history;

          // set up an observer which writes to this history vector
          // I'd prefer to encapsulate the history within the observer object, but for some reason
          // that doesn't seem to work (maybe related to the way odeint uses templates?)
          $$__MODEL_background_observer<number> obs(slices, history);
          $$__MODEL_background_functor<number>  system(this->parameters, this->M_Planck);

          integrate_times( make_dense_output< $$__BACKG_STEPPER< std::vector<number> > >($$__BACKG_ABS_ERR, $$__BACKG_REL_ERR),
            system, x, times.begin(), times.end(), $$__BACKG_STEP_SIZE, obs);

          transport::background<number> backg(2*$$__NUMBER_FIELDS, $$__MODEL_state_names, slices, history);

          return(backg);
        }


      template <typename number>
      void $$__MODEL<number>::fix_initial_conditions(const std::vector<number>& ics, std::vector<number>& rics)
        {
          if(ics.size() == this->N_fields)  // initial conditions for momenta *were not* supplied -- need to compute them
            {
              // supply the missing initial conditions using a slow-roll approximation
              $$__SET_PARAMETERS{number, this->parameters}
              $$__SET_FIELDS{number,ics}
              $$__SET_SR_VELOCITY{number}
              rics.push_back($$__SR_VELOCITY[a]);
            }
          else if(ics.size() == 2*this->N_fields)  // initial conditions for momenta *were* supplied
            {
              // need do nothing
            }
          else
            {
              std::cerr << __CPP_TRANSPORT_WRONG_ICS_A << ics.size()
                        << __CPP_TRANSPORT_WRONG_ICS_B << $$__NUMBER_FIELDS
                        << __CPP_TRANSPORT_WRONG_ICS_C << 2*$$__NUMBER_FIELDS << ")" << std::endl;
              exit(EXIT_FAILURE);
            }
        }

      template <typename number>
      void $$__MODEL<number>::write_initial_conditions(const std::vector<number>& ics, std::ostream& stream)
        {
          stream << __CPP_TRANSPORT_SOLVING_ICS_MESSAGE << std::endl;

          assert(ics.size() == 2*this->N_fields);

          for(int i = 0; i < this->N_fields; i++)
            {
              stream << "  " << this->field_names[i] << " = " << ics[i]
                     << "; d(" << this->field_names[i] << ")/dN = " << ics[this->N_fields+i] << std::endl;
            }
        }

      // BACKGROUND INTEGRATION


      template <typename number>
      void $$__MODEL_background_functor<number>::operator()(const std::vector<number>& x, std::vector<number>& dxdt, double t)
        {
          $$__SET_PARAMETERS{number, this->parameters}
          $$__SET_ALL{number, x}
          $$__SET_U1_TENSOR{number}
          dxdt[$$__A] = $$__U1_TENSOR[A];
        }


      template <typename number>
      void $$__MODEL_background_observer<number>::operator()(const std::vector<number>& x, double t)
        {
          this->slices.push_back(t);
          this->history.push_back(x);
        }


  }   // namespace transport


#endif  // $$__GUARD
