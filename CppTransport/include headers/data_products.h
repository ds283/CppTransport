//
// Created by David Seery on 30/06/2013.
// Copyright (c) 2013 University of Sussex. All rights reserved.
//
// To change the template use AppCode | Preferences | File Templates.
//

#ifndef __CPP_TRANSPORT_DATA_PRODUCTS_H_
#define __CPP_TRANSPORT_DATA_PRODUCTS_H_

#include <string>
#include <vector>

#include "asciitable.h"
#include "messages_en.h"

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
      class twopf;

      template <typename number>
      std::ostream& operator<<(std::ostream& out, twopf<number>& obj);

      template <typename number>
      class background
        {
          public:
            background(unsigned int N_f, const std::vector<std::string>& f_names,
              const std::vector<double>& sp, const std::vector< std::vector<number> >& s)
              : N_fields(N_f), field_names(f_names), sample_points(sp), samples(s)
              {}

            // provide << operator to output data to a stream
            friend std::ostream& operator<< <>(std::ostream& out, background& obj);

          protected:
            unsigned int                             N_fields;          // number of fields
            const std::vector<std::string>           field_names;       // vector of field names

            const std::vector<double>                sample_points;     // list of times at which we hold samples for the background

            const std::vector< std::vector<number> > samples;           // list of samples
            // samples are tuples of the for (fields, d(fields)/dN)
        };

      template <typename number>
      class twopf
        {
          public:
            twopf(unsigned int N_f, const std::vector<std::string>& f_names, const std::vector<double> ks,
              const std::vector<number>& sp, const std::vector< std::vector<number> >& b,
              const std::vector< std::vector< std::vector<number> > >& twopf)
              : N_fields(N_f), field_names(f_names), sample_points(sp), sample_ks(ks),
                backg(N_f, f_names, sp, b), samples(twopf)
              {}

            // provide << operator to output data to a stream
            friend std::ostream& operator<< <>(std::ostream& out, twopf& obj);

          protected:
            unsigned int                                            N_fields;          // number of fields
            const std::vector<std::string>                          field_names;       // vector of field names

            const std::vector<double>                               sample_points;     // list of times at which we hold samples
            const std::vector<double>                               sample_ks;         // list of ks for which we hold samples

            background<number>                                      backg;             // container for background
            const std::vector< std::vector< std::vector<number> > > samples;           // list of samples of 2pf
        };

//  IMPLEMENTATION -- CLASS background

      template <typename number>
      std::ostream& operator<<(std::ostream& out, background<number>& obj)
        {
          transport::asciitable<number> writer(out);

          writer.write("e-folds", obj.field_names, obj.sample_points, obj.samples);

          return(out);
        }

//  IMPLEMENTATION -- CLASS twopf

      template <typename number>
      std::ostream& operator<<(std::ostream& out, twopf<number>& obj)
        {
          transport::asciitable<number> writer(out);

          out << obj.backg;

          return(out);
        }

  }   // namespace transport

#endif // __CPP_TRANSPORT_MODEL_CLASSES_H_
