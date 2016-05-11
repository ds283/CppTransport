//
// Created by David Seery on 22/06/2014.
// --@@
// Copyright (c) 2016 University of Sussex. All rights reserved.
//
// This file is part of the CppTransport platform.
//
// CppTransport is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 2 of the License, or
// (at your option) any later version.
//
// CppTransport is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with CppTransport.  If not, see <http://www.gnu.org/licenses/>.
//
// @license: GPL-2
// @contributor: David Seery <D.Seery@sussex.ac.uk>
// --@@
//


#ifndef __fNL_line_H_
#define __fNL_line_H_


#include <iostream>
#include <sstream>
#include <string>
#include <list>
#include <vector>
#include <stdexcept>

#include "transport-runtime/serialization/serializable.h"

// get details of datapipe<number>
#include "transport-runtime/data/datapipe/datapipe_decl.h"

#include "transport-runtime/derived-products/derived-content/concepts/derived_line.h"

// forward-declare model class if needed
#include "transport-runtime/models/model_forward_declare.h"

// forward-declare tasks if needed
#include "transport-runtime/tasks/tasks_forward_declare.h"

// forward-declare repository records if needed
#include "transport-runtime/repository/records/repository_records_forward_declare.h"

// template types
#include "transport-runtime/derived-products/derived-content/correlation-functions/template_types.h"

#include "transport-runtime/derived-products/utilities/index_selector.h"
#include "transport-runtime/derived-products/utilities/wrapper.h"


#define CPPTRANSPORT_NODE_PRODUCT_FNL_TEMPLATE             "template"
#define CPPTRANSPORT_NODE_PRODUCT_FNL_TEMPLATE_LOCAL       "local"
#define CPPTRANSPORT_NODE_PRODUCT_FNL_TEMPLATE_EQUILATERAL "equilateral"
#define CPPTRANSPORT_NODE_PRODUCT_FNL_TEMPLATE_ORTHOGONAL  "orthogonal"
#define CPPTRANSPORT_NODE_PRODUCT_FNL_TEMPLATE_DBI         "DBI"


namespace transport
  {

    namespace derived_data
      {

        //! general fNL content producer
        //! As usual, we derive virtually from derived_line<>, so concrete
        //! classes must call its constructor explicitly.
		    //! The template to be plotted is inherited
        template <typename number>
        class fNL_line: public virtual derived_line<number>
          {

            // CONSTRUCTOR, DESTRUCTOR

          public:

            //! Basic user-facing constructor
            fNL_line(const fNL_task<number>& tk);

            //! Deserialization constructor
            fNL_line(Json::Value& reader);

            virtual ~fNL_line() = default;


            // INTERFACE

          public:

            //! get template type
            bispectrum_template get_template() const { return(this->type); }


            // LABELLING SERVICES

          public:

            //! make a LaTeX label for one of our lines
            std::string make_LaTeX_label() const;

            //! make a non-LaTeX label
            std::string make_non_LaTeX_label() const;


            // WRITE TO A STREAM

          public:

            //! write self-details to a stream
            virtual void write(std::ostream& out) override;


            // SERIALIZATION -- implements a 'serializable' interface

          public:

            //! Serialize this object
            virtual void serialize(Json::Value& writer) const override;


            // INTERNAL DATA

          protected:

            //! record which fNL template we are using
            bispectrum_template type;

          };


        template <typename number>
        fNL_line<number>::fNL_line(const fNL_task<number>& tk)
          : derived_line<number>(tk),  // not called because of virtual inheritance; here to silence Intel compiler warning
            type(tk.get_template())
          {
          }


        template <typename number>
        fNL_line<number>::fNL_line(Json::Value& reader)
          : derived_line<number>(reader),  // not called because of virtual inheritance; here to silence Intel compiler warning
            type(bispectrum_template::local)
          {
            std::string type_str = reader[CPPTRANSPORT_NODE_PRODUCT_FNL_TEMPLATE].asString();

            if     (type_str == CPPTRANSPORT_NODE_PRODUCT_FNL_TEMPLATE_LOCAL)       type = bispectrum_template::local;
            else if(type_str == CPPTRANSPORT_NODE_PRODUCT_FNL_TEMPLATE_EQUILATERAL) type = bispectrum_template::equilateral;
            else if(type_str == CPPTRANSPORT_NODE_PRODUCT_FNL_TEMPLATE_ORTHOGONAL)  type = bispectrum_template::orthogonal;
            else if(type_str == CPPTRANSPORT_NODE_PRODUCT_FNL_TEMPLATE_DBI)         type = bispectrum_template::DBI;
            else
              {
                std::ostringstream msg;
                msg << CPPTRANSPORT_PRODUCT_FNL_LINE_UNKNOWN_TEMPLATE << " '" << type_str << "'";
                throw runtime_exception(exception_type::SERIALIZATION_ERROR, msg.str());
              }
          }


        template <typename number>
        std::string fNL_line<number>::make_LaTeX_label() const
          {
            std::ostringstream label;

            switch(this->type)
              {
                case bispectrum_template::local:
                  label << CPPTRANSPORT_LATEX_FNL_LOCAL_SYMBOL;
                  break;

                case bispectrum_template::equilateral:
                  label << CPPTRANSPORT_LATEX_FNL_EQUI_SYMBOL;
                  break;

                case bispectrum_template::orthogonal:
                  label << CPPTRANSPORT_LATEX_FNL_ORTHO_SYMBOL;
                  break;

                case bispectrum_template::DBI:
                  label << CPPTRANSPORT_LATEX_FNL_DBI_SYMBOL;
                  break;

                default:
                  assert(false);
                  throw runtime_exception(exception_type::RUNTIME_ERROR, CPPTRANSPORT_PRODUCT_FNL_LINE_UNKNOWN_TEMPLATE);
              }

            return(label.str());
          }


        template <typename number>
        std::string fNL_line<number>::make_non_LaTeX_label() const
          {
            std::ostringstream label;

            switch(this->type)
              {
                case bispectrum_template::local:
                  label << CPPTRANSPORT_NONLATEX_FNL_LOCAL_SYMBOL;
                  break;

                case bispectrum_template::equilateral:
                  label << CPPTRANSPORT_NONLATEX_FNL_EQUI_SYMBOL;
                  break;

                case bispectrum_template::orthogonal:
                  label << CPPTRANSPORT_NONLATEX_FNL_ORTHO_SYMBOL;
                  break;

                case bispectrum_template::DBI:
                  label << CPPTRANSPORT_NONLATEX_FNL_DBI_SYMBOL;
                  break;

                default:
                  assert(false);
                  throw runtime_exception(exception_type::RUNTIME_ERROR, CPPTRANSPORT_PRODUCT_FNL_LINE_UNKNOWN_TEMPLATE);
              }

            return(label.str());
          }


        template <typename number>
        void fNL_line<number>::serialize(Json::Value& writer) const
          {
            switch(this->type)
              {
                case bispectrum_template::local:
                  writer[CPPTRANSPORT_NODE_PRODUCT_FNL_TEMPLATE] = std::string(CPPTRANSPORT_NODE_PRODUCT_FNL_TEMPLATE_LOCAL);
                  break;

                case bispectrum_template::equilateral:
                  writer[CPPTRANSPORT_NODE_PRODUCT_FNL_TEMPLATE] = std::string(CPPTRANSPORT_NODE_PRODUCT_FNL_TEMPLATE_EQUILATERAL);
                  break;

                case bispectrum_template::orthogonal:
                  writer[CPPTRANSPORT_NODE_PRODUCT_FNL_TEMPLATE] = std::string(CPPTRANSPORT_NODE_PRODUCT_FNL_TEMPLATE_ORTHOGONAL);
                  break;

                case bispectrum_template::DBI:
                  writer[CPPTRANSPORT_NODE_PRODUCT_FNL_TEMPLATE] = std::string(CPPTRANSPORT_NODE_PRODUCT_FNL_TEMPLATE_DBI);
                  break;

                default:
                  assert(false);
                  throw runtime_exception(exception_type::RUNTIME_ERROR, CPPTRANSPORT_PRODUCT_FNL_LINE_UNKNOWN_TEMPLATE);
              }
          }


        template <typename number>
        void fNL_line<number>::write(std::ostream& out)
          {
            out << "FIXME" << '\n';
          }

      }   // namespace derived_data

  }   // namespace transport


#endif //__fNL_line_H_
