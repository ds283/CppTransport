//
// Created by David Seery on 30/06/2013.
// Copyright (c) 2013 University of Sussex. All rights reserved.
//
// To change the template use AppCode | Preferences | File Templates.
//

#ifndef __CPP_TRANSPORT_BACKGROUND_H_
#define __CPP_TRANSPORT_BACKGROUND_H_

#include <vector>
#include <assert.h>

#include "asciitable.h"
#include "plot_gadget.h"


namespace transport
  {
      // DATA PRODUCTS -- objects wrapping the various data products produced by each model

      // handle weirdness with friend template functions
      // see http://www.cplusplus.com/forum/general/45776/
      template <typename number>
      class background;

      template <typename number>
      std::ostream& operator<<(std::ostream& out, background<number>& obj);


      template <typename number>
      class background
        {
          public:
            background(unsigned int N_f, const std::vector<std::string>& f_names, const std::vector<std::string>& l_names,
              const std::vector<double>& sp, const std::vector< std::vector<number> >& s)
              : N_fields(N_f), field_names(f_names), latex_names(l_names), sample_points(sp), samples(s)
              {}

            void                        plot      (plot_gadget<number>*gadget, std::string output, std::string title = "");

            const std::vector<number>&  get_value (unsigned int n);

            // provide << operator to output data to a stream
            friend std::ostream& operator<< <>(std::ostream& out, background& obj);

          protected:
            unsigned int                             N_fields;          // number of fields
            const std::vector<std::string>           field_names;       // vector of names - includes momenta
            const std::vector<std::string>           latex_names;       // vector of LaTeX names - excludes momenta

            const std::vector<double>                sample_points;     // list of times at which we hold samples for the background

            const std::vector< std::vector<number> > samples;           // list of samples
              // first index: time of observation
              // second index: field label
        };


//  IMPLEMENTATION -- CLASS background


      template <typename number>
      void background<number>::plot(plot_gadget<number>* gadget, std::string output, std::string title)
        {
          gadget->plot(output, title, this->sample_points, this->samples, this->field_names, "N", "fields", false, false);
        }

      template <typename number>
      const std::vector<number>& background<number>::get_value(unsigned int n)
        {
          assert(n < this->sample_points.size());

          return(this->samples[n]);
        }

      template <typename number>
      std::ostream& operator<<(std::ostream& out, background<number>& obj)
        {
          transport::asciitable<number> writer(out);

          writer.write("e-folds", obj.field_names, obj.sample_points, obj.samples);

          return(out);
        }


  }   // namespace transport

#endif // __CPP_TRANSPORT_BACKGROUND_H_
