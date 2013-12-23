//
// Created by David Seery on 22/12/2013.
// Copyright (c) 2013 University of Sussex. All rights reserved.
//


#ifndef __initial_conditions_H_
#define __initial_conditions_H_


#include <assert.h>
#include <vector>
#include <functional>


namespace transport
  {

    template <typename number>
    class initial_conditions
      {
      public:
        typedef std::function<void(const std::vector<number>&, std::vector<number>&)> ics_validator;

        typedef std::function<void(const std::vector<number>&, std::vector<number>&, double, double, double)> ics_finder;

        // construct initial conditions from directly-supplied data
        initial_conditions(const std::vector<number>& i, ics_validator v);

        // construct initial conditions offset from directly-supplied data
        initial_conditions(const std::vector<number>& i, double Ninit, double Ncross, double Npre,
                           ics_validator v, ics_finder f);

        const std::vector<number>& get_ics() const { return(this->ics); }

      protected:
        // values of fields and their derivatives at the initial time
        std::vector<number> ics;
      };


    template <typename number>
    initial_conditions<number>::initial_conditions(const std::vector<number>& i, ics_validator v)
      {
        // validate supplied initial conditions
        v(i, ics);
      }


    template <typename number>
    initial_conditions<number>::initial_conditions(const std::vector<number>& i, double Ninit, double Ncross, double Npre,
                                                   ics_validator v, ics_finder f)
      {
        std::vector<number> validated_ics;

        // validate supplied initial conditions
        v(i, validated_ics);

        // call supplied finder function to correctly offset these ics
        f(validated_ics, ics, Ninit, Ncross, Npre);
      }

  }


#endif //__initial_conditions_H_
