//
// Created by David Seery on 04/12/2013.
// Copyright (c) 2013-15 University of Sussex. All rights reserved.
//



#ifndef __cpp_cse_H_
#define __cpp_cse_H_

#include "cse.h"
#include "msg_en.h"

namespace cpp
  {

    class cpp_cse: public cse
      {

	      // CONSTRUCTOR, DESTRUCTOR

      public:

        cpp_cse(unsigned int s, language_printer& p, bool d=true, std::string k=OUTPUT_DEFAULT_CPP_KERNEL_NAME)
          : cse(s, p, d, k)
          {
          }


		    // INTERNAL API

      protected:

        virtual std::string print         (const GiNaC::ex& expr, symbol_f symf) override;
        virtual std::string print_operands(const GiNaC::ex& expr, std::string op, symbol_f symf) override;
        std::string print_power           (const GiNaC::ex& expr, symbol_f symf);

      };

  } // namespace cpp


#endif //__cpp_cse_H_
