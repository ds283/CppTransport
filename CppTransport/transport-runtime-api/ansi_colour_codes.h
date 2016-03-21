//
// Created by David Seery on 24/06/15.
// Copyright (c) 2015-2016 University of Sussex. All rights reserved.
//

#ifndef CPPTRANSPORT_ANSI_COLOUR_CODES_H
#define CPPTRANSPORT_ANSI_COLOUR_CODES_H

#include <map>


namespace transport
  {

    constexpr auto ANSI_RED          = "\033[31m";
    constexpr auto ANSI_GREEN        = "\033[32m";
    constexpr auto ANSI_MAGENTA      = "\033[35m";

    constexpr auto ANSI_BOLD_RED     = "\033[1;31m";
    constexpr auto ANSI_BOLD_GREEN   = "\033[1;32m";
    constexpr auto ANSI_BOLD_MAGENTA = "\033[1;35m";

    constexpr auto ANSI_BOLD         = "\033[1m";

    constexpr auto ANSI_NORMAL       = "\033[0m";

    enum class ANSI_colour
      {
        red, green, magenta, bold_red, bold_green, bold_magenta, bold, normal
      };


    namespace ANSI_colour_codes_impl
      {

        static std::map<ANSI_colour, std::string> colour_code_map =
          {
            { ANSI_colour::red, ANSI_RED },
            { ANSI_colour::green, ANSI_GREEN },
            { ANSI_colour::magenta, ANSI_MAGENTA },
            { ANSI_colour::bold_red, ANSI_BOLD_RED },
            { ANSI_colour::bold_green, ANSI_BOLD_GREEN },
            { ANSI_colour::bold_magenta, ANSI_BOLD_MAGENTA },
            { ANSI_colour::bold, ANSI_BOLD },
            { ANSI_colour::normal, ANSI_NORMAL }
          };

      }


    class ColourCode
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! constructor
        ColourCode(ANSI_colour c)
          : colour(c)
          {
          }


        // STREAM OVERLOAD OPERATOR

        friend std::ostream& operator<<(std::ostream& out, const ColourCode& obj)
          {
            std::string escape_seq = ANSI_colour_codes_impl::colour_code_map[obj.colour];

            for(char ch : escape_seq)
              {
                // put() qualifies as unformatted output, so this method prevents the ANSI
                // escape codes from distorting any formatting requirements, eg. for tabular
                // output using asciitable
                out.put(ch);
              }

            return(out);
          }

        // INTERNAL DATA

      private:

        //! colour tag
        ANSI_colour colour;

      };

  }


#endif //CPPTRANSPORT_ANSI_COLOUR_CODES_H
