//
// Created by David Seery on 22/06/2014.
// Copyright (c) 2014-15 University of Sussex. All rights reserved.
//


#ifndef __fNL_line_H_
#define __fNL_line_H_


#include <iostream>
#include <sstream>
#include <string>
#include <list>
#include <vector>
#include <stdexcept>

#include "transport-runtime-api/serialization/serializable.h"

// get details of datapipe<number>
#include "transport-runtime-api/data/datapipe/datapipe.h"

#include "transport-runtime-api/derived-products/derived-content/concepts/derived_line.h"

// forward-declare model class if needed
#include "transport-runtime-api/models/model_forward_declare.h"

// forward-declare tasks if needed
#include "transport-runtime-api/tasks/tasks_forward_declare.h"

// forward-declare repository records if needed
#include "transport-runtime-api/repository/records/repository_records_forward_declare.h"

// template types
#include "transport-runtime-api/derived-products/derived-content/correlation-functions/template_types.h"

#include "transport-runtime-api/derived-products/utilities/index_selector.h"
#include "transport-runtime-api/derived-products/utilities/wrapper.h"


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
            template_type type;

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
            type(fNL_local_template)
          {
            std::string type_str = reader[CPPTRANSPORT_NODE_PRODUCT_FNL_TEMPLATE].asString();

            if     (type_str == CPPTRANSPORT_NODE_PRODUCT_FNL_TEMPLATE_LOCAL)       type = fNL_local_template;
            else if(type_str == CPPTRANSPORT_NODE_PRODUCT_FNL_TEMPLATE_EQUILATERAL) type = fNL_equi_template;
            else if(type_str == CPPTRANSPORT_NODE_PRODUCT_FNL_TEMPLATE_ORTHOGONAL)  type = fNL_ortho_template;
            else if(type_str == CPPTRANSPORT_NODE_PRODUCT_FNL_TEMPLATE_DBI)         type = fNL_DBI_template;
            else
              {
                std::ostringstream msg;
                msg << CPPTRANSPORT_PRODUCT_FNL_LINE_UNKNOWN_TEMPLATE << " '" << type_str << "'";
                throw runtime_exception(runtime_exception::SERIALIZATION_ERROR, msg.str());
              }
          }


        template <typename number>
        std::string fNL_line<number>::make_LaTeX_label() const
          {
            std::ostringstream label;

            switch(this->type)
              {
                case fNL_local_template:
                  label << CPPTRANSPORT_LATEX_FNL_LOCAL_SYMBOL;
                  break;

                case fNL_equi_template:
                  label << CPPTRANSPORT_LATEX_FNL_EQUI_SYMBOL;
                  break;

                case fNL_ortho_template:
                  label << CPPTRANSPORT_LATEX_FNL_ORTHO_SYMBOL;
                  break;

                case fNL_DBI_template:
                  label << CPPTRANSPORT_LATEX_FNL_DBI_SYMBOL;
                  break;

                default:
                  assert(false);
                  throw runtime_exception(runtime_exception::RUNTIME_ERROR, CPPTRANSPORT_PRODUCT_FNL_LINE_UNKNOWN_TEMPLATE);
              }

            return(label.str());
          }


        template <typename number>
        std::string fNL_line<number>::make_non_LaTeX_label() const
          {
            std::ostringstream label;

            switch(this->type)
              {
                case fNL_local_template:
                  label << CPPTRANSPORT_NONLATEX_FNL_LOCAL_SYMBOL;
                  break;

                case fNL_equi_template:
                  label << CPPTRANSPORT_NONLATEX_FNL_EQUI_SYMBOL;
                  break;

                case fNL_ortho_template:
                  label << CPPTRANSPORT_NONLATEX_FNL_ORTHO_SYMBOL;
                  break;

                case fNL_DBI_template:
                  label << CPPTRANSPORT_NONLATEX_FNL_DBI_SYMBOL;
                  break;

                default:
                  assert(false);
                  throw runtime_exception(runtime_exception::RUNTIME_ERROR, CPPTRANSPORT_PRODUCT_FNL_LINE_UNKNOWN_TEMPLATE);
              }

            return(label.str());
          }


        template <typename number>
        void fNL_line<number>::serialize(Json::Value& writer) const
          {
            switch(this->type)
              {
                case fNL_local_template:
                  writer[CPPTRANSPORT_NODE_PRODUCT_FNL_TEMPLATE] = std::string(CPPTRANSPORT_NODE_PRODUCT_FNL_TEMPLATE_LOCAL);
                  break;

                case fNL_equi_template:
                  writer[CPPTRANSPORT_NODE_PRODUCT_FNL_TEMPLATE] = std::string(CPPTRANSPORT_NODE_PRODUCT_FNL_TEMPLATE_EQUILATERAL);
                  break;

                case fNL_ortho_template:
                  writer[CPPTRANSPORT_NODE_PRODUCT_FNL_TEMPLATE] = std::string(CPPTRANSPORT_NODE_PRODUCT_FNL_TEMPLATE_ORTHOGONAL);
                  break;

                case fNL_DBI_template:
                  writer[CPPTRANSPORT_NODE_PRODUCT_FNL_TEMPLATE] = std::string(CPPTRANSPORT_NODE_PRODUCT_FNL_TEMPLATE_DBI);
                  break;

                default:
                  assert(false);
                  throw runtime_exception(runtime_exception::RUNTIME_ERROR, CPPTRANSPORT_PRODUCT_FNL_LINE_UNKNOWN_TEMPLATE);
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
