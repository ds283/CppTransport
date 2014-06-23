//
// Created by David Seery on 22/06/2014.
// Copyright (c) 2014 University of Sussex. All rights reserved.
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
// need repository in order to get the details of a repository<number>::output_group_record
// (can't forward-declare because it is a nested class)
#include "transport-runtime-api/manager/repository.h"

// need data_manager in order to get the details of a data_manager<number>::datapipe
// (can't forward-declare because it is a nested class)
#include "transport-runtime-api/manager/data_manager.h"

#include "transport-runtime-api/derived-products/derived-content/derived_line.h"

#include "transport-runtime-api/derived-products/utilities/index_selector.h"
#include "transport-runtime-api/derived-products/utilities/wrapper.h"
#include "transport-runtime-api/derived-products/utilities/filter.h"


#define __CPP_TRANSPORT_NODE_PRODUCT_FNL_TEMPLATE             "template"
#define __CPP_TRANSPORT_NODE_PRODUCT_FNL_TEMPLATE_LOCAL       "local"
#define __CPP_TRANSPORT_NODE_PRODUCT_FNL_TEMPLATE_EQUILATERAL "equilateral"
#define __CPP_TRANSPORT_NODE_PRODUCT_FNL_TEMPLATE_ORTHOGONAL  "orthogonal"
#define __CPP_TRANSPORT_NODE_PRODUCT_FNL_TEMPLATE_DBI         "DBI"


namespace transport
  {

    // forward-declare model class
    template <typename number> class model;

    // forward-declare task classes.
    // task.h includes this header, so we cannot include task.h otherwise we create
    // a circular dependency.
    template <typename number> class task;
    template <typename number> class integration_task;
    template <typename number> class threepf_task;

    namespace derived_data
      {

        //! general fNL content producer
        //! As usual, we derive virtually from derived_line<>, so concrete
        //! classes must call its constructor explicitly
        template <typename number>
        class fNL_line: public virtual derived_line<number>
          {

          public:

            typedef enum { fNLlocal, fNLequi, fNLortho, fNLDBI } fNLtype;

            // CONSTRUCTOR, DESTRUCTOR

          public:

            //! Basic user-facing constructor
            fNL_line(const threepf_task<number>& tk);

            //! Deserialization constructor
            fNL_line(serialization_reader* reader);

            virtual ~fNL_line() = default;


            // LABELLING SERVICES

            //! make a LaTeX label for one of our lines
            std::string make_LaTeX_label() const;

            //! make a non-LaTeX label
            std::string make_non_LaTeX_label() const;


            // SET/GET FNL TEMPLATE

          public:

            //! get type of fNL
            fNLtype get_type() const { return(this->type); }

            //! set type of fNL
            void set_type(fNLtype t) { this->type = t; }


            // COMPUTE SELECTED FNL SHAPE FUNCTION

          public:

            //! compute shape function for a given bispectrum
            void shape_function(const std::vector<number>& bispectrum, const std::vector<number>& twopf_k1,
                                const std::vector<number>& twopf_k2, const std::vector<number>& twopf_k3,
                                std::vector<number>& shape) const;

		        //! compute shape function for the selected template
		        void shape_function(const std::vector<number>& twopf_k1, const std::vector<number>& twopf_k2,
		                            const std::vector<number>& twopf_k3, std::vector<number>& shape) const;


		        // BISPECTRUM SHAPES AND TEMPLATES

          protected:

		        //! compute reference shape
		        number reference_bispectrum(number Pk1, number Pk2, number Pk3) const;

		        //! compute local template
		        number local_template(number Pk1, number Pk2, number Pk3) const;

		        //! compute equilateral template
		        number equi_template(number Pk1, number Pk2, number Pk3) const;

		        //! compute orthogonal template
		        number ortho_template(number Pk1, number Pk2, number Pk3) const;

		        //! DBI template
		        number DBI_template(number Pk1, number Pk2, number Pk3) const;


            // WRITE TO A STREAM

          public:

            //! write self-details to a stream
            virtual void write(std::ostream& out) override;


            // SERIALIZATION -- implements a 'serializable' interface

          public:

            //! Serialize this object
            virtual void serialize(serialization_writer& writer) const override;


            // INTERNAL DATA

          protected:

            //! record which fNL template we are using
            fNLtype type;

          };


        template <typename number>
        fNL_line<number>::fNL_line(const threepf_task<number>& tk)
          : type(fNLlocal)
          {
            // we could store all kconfiguration numbers we're going to use, but it's
            // more space efficient to just recover them at runtime
          }


        template <typename number>
        fNL_line<number>::fNL_line(serialization_reader* reader)
          : type(fNLlocal)
          {
            assert(reader != nullptr);
            if(reader == nullptr) throw runtime_exception(runtime_exception::RUNTIME_ERROR, __CPP_TRANSPORT_PRODUCT_WAVENUMBER_SERIES_NULL_READER);

            std::string type_str;
            reader->read_value(__CPP_TRANSPORT_NODE_PRODUCT_FNL_TEMPLATE, type_str);

            if     (type_str == __CPP_TRANSPORT_NODE_PRODUCT_FNL_TEMPLATE_LOCAL)       type = fNLlocal;
            else if(type_str == __CPP_TRANSPORT_NODE_PRODUCT_FNL_TEMPLATE_EQUILATERAL) type = fNLequi;
            else if(type_str == __CPP_TRANSPORT_NODE_PRODUCT_FNL_TEMPLATE_ORTHOGONAL)  type = fNLortho;
            else if(type_str == __CPP_TRANSPORT_NODE_PRODUCT_FNL_TEMPLATE_DBI)         type = fNLDBI;
            else
              {
                std::ostringstream msg;
                msg << __CPP_TRANSPORT_PRODUCT_FNL_LINE_UNKNOWN_TEMPLATE << " '" << type_str << "'";
                throw runtime_exception(runtime_exception::SERIALIZATION_ERROR, msg.str());
              }
          }


        template <typename number>
        std::string fNL_line<number>::make_LaTeX_label() const
          {
            std::ostringstream label;

            switch(this->type)
              {
                case fNLlocal:
                  label << __CPP_TRANSPORT_LATEX_FNL_LOCAL_SYMBOL;
                  break;

                case fNLequi:
                  label << __CPP_TRANSPORT_LATEX_FNL_EQUI_SYMBOL;
                  break;

                case fNLortho:
                  label << __CPP_TRANSPORT_LATEX_FNL_ORTHO_SYMBOL;
                  break;

                case fNLDBI:
                  label << __CPP_TRANSPORT_LATEX_FNL_DBI_SYMBOL;
                  break;

                default:
                  assert(false);
                throw runtime_exception(runtime_exception::RUNTIME_ERROR, __CPP_TRANSPORT_PRODUCT_FNL_LINE_UNKNOWN_TEMPLATE);
              }

            return(label.str());
          }


        template <typename number>
        std::string fNL_line<number>::make_non_LaTeX_label() const
          {
            std::ostringstream label;

            switch(this->type)
              {
                case fNLlocal:
                  label << __CPP_TRANSPORT_NONLATEX_FNL_LOCAL_SYMBOL;
                  break;

                case fNLequi:
                  label << __CPP_TRANSPORT_NONLATEX_FNL_EQUI_SYMBOL;
                  break;

                case fNLortho:
                  label << __CPP_TRANSPORT_NONLATEX_FNL_ORTHO_SYMBOL;
                  break;

                case fNLDBI:
                  label << __CPP_TRANSPORT_NONLATEX_FNL_DBI_SYMBOL;
                  break;

                default:
                  assert(false);
                  throw runtime_exception(runtime_exception::RUNTIME_ERROR, __CPP_TRANSPORT_PRODUCT_FNL_LINE_UNKNOWN_TEMPLATE);
              }

            return(label.str());
          }


		    // compute shape function for a supplied bispectrum
        template <typename number>
        void fNL_line<number>::shape_function(const std::vector<number>& bispectrum, const std::vector<number>& twopf_k1,
                                              const std::vector<number>& twopf_k2, const std::vector<number>& twopf_k3,
                                              std::vector<number>& shape) const
	        {
            shape.clear();
            shape.resize(bispectrum.size());

            for(unsigned int j = 0; j < bispectrum.size(); j++)
	            {
                number Bref = this->reference_bispectrum(twopf_k1[j], twopf_k2[j], twopf_k3[j]);

                shape[j] = bispectrum[j] / Bref;
	            }
	        }


		    // compute shape function for the intended template
		    template <typename number>
		    void fNL_line<number>::shape_function(const std::vector<number>& twopf_k1, const std::vector<number>& twopf_k2,
		                                          const std::vector<number>& twopf_k3, std::vector<number>& shape) const
			    {
				    shape.clear();
				    shape.resize(twopf_k1.size());

				    for(unsigned int j = 0; j < twopf_k1.size(); j++)
					    {
						    number T = 0.0;

				        switch(this->type)
					        {
				            case fNLlocal:
					            T = this->local_template(twopf_k1[j], twopf_k2[j], twopf_k3[j]);
					            break;

				            case fNLequi:
					            T = this->equi_template(twopf_k1[j], twopf_k2[j], twopf_k3[j]);
					            break;

				            case fNLortho:
					            T = this->ortho_template(twopf_k1[j], twopf_k2[j], twopf_k3[j]);
					            break;

				            case fNLDBI:
					            T = this->DBI_template(twopf_k1[j], twopf_k2[j], twopf_k3[j]);
					            break;

				            default:
					            assert(false);
					            throw runtime_exception(runtime_exception::RUNTIME_ERROR, __CPP_TRANSPORT_PRODUCT_FNL_LINE_UNKNOWN_TEMPLATE);
					        }

						    number Bref = this->reference_bispectrum(twopf_k1[j], twopf_k2[j], twopf_k3[j]);

						    shape[j] = T/Bref;
					    }

          }


        // compute reference bispectrum -- currently uses the constant bispectrum
        template <typename number>
        number fNL_line<number>::reference_bispectrum(number Pk1, number Pk2, number Pk3) const
	        {
            return(pow(Pk1*Pk2*Pk3, 2.0/3.0));
	        }


		    // local template
		    template <typename number>
		    number fNL_line<number>::local_template(number Pk1, number Pk2, number Pk3) const
			    {
				    return(2.0 * (Pk1*Pk2 + Pk1*Pk3 + Pk2*Pk3));
			    }


		    // equilateral template
		    template <typename number>
		    number fNL_line<number>::equi_template(number Pk1, number Pk2, number Pk3) const
			    {
				    return(6.0 * (-Pk1*Pk2 - Pk1*Pk3 -Pk2*Pk3 - 2.0*pow(Pk1*Pk2*Pk3, 2.0/3.0)
                          + pow(Pk1*Pk2*Pk2, 1.0/3.0)*Pk3 + pow(Pk1*Pk3*Pk3, 1.0/3.0)*Pk2
                          + pow(Pk2*Pk1*Pk1, 1.0/3.0)*Pk3 + pow(Pk2*Pk3*Pk3, 1.0/3.0)*Pk1
                          + pow(Pk3*Pk1*Pk1, 1.0/3.0)*Pk2 + pow(Pk3*Pk2*Pk2, 1.0/3.0)*Pk1) );
			    }


		    // orthogonal template
		    template <typename number>
		    number fNL_line<number>::ortho_template(number Pk1, number Pk2, number Pk3) const
			    {
				    return(6.0 * (-3.0*Pk1*Pk2 - 3.0*Pk1*Pk3 - 3.0*Pk2*Pk3 - 8.0*pow(Pk1*Pk2*Pk3, 2.0/3.0)
                          + 3.0*pow(Pk1*Pk2*Pk2, 1.0/3.0)*Pk3 + 3.0*pow(Pk1*Pk3*Pk3, 1.0/3.0)*Pk2
                          + 3.0*pow(Pk2*Pk1*Pk1, 1.0/3.0)*Pk3 + 3.0*pow(Pk2*Pk3*Pk3, 1.0/3.0)*Pk1
                          + 3.0*pow(Pk3*Pk1*Pk1, 1.0/3.0)*Pk2 + 3.0*pow(Pk3*Pk2*Pk2, 1.0/3.0)*Pk1) );
			    }


		    // DBI template
		    template <typename number>
		    number fNL_line<number>::DBI_template(number Pk1, number Pk2, number Pk3) const
			    {
				    return(1.0);
			    }


        template <typename number>
        void fNL_line<number>::serialize(serialization_writer& writer) const
          {
            switch(this->type)
              {
                case fNLlocal:
                  writer.write_value(__CPP_TRANSPORT_NODE_PRODUCT_FNL_TEMPLATE, std::string(__CPP_TRANSPORT_NODE_PRODUCT_FNL_TEMPLATE_LOCAL));
                  break;

                case fNLequi:
                  writer.write_value(__CPP_TRANSPORT_NODE_PRODUCT_FNL_TEMPLATE, std::string(__CPP_TRANSPORT_NODE_PRODUCT_FNL_TEMPLATE_EQUILATERAL));
                  break;

                case fNLortho:
                  writer.write_value(__CPP_TRANSPORT_NODE_PRODUCT_FNL_TEMPLATE, std::string(__CPP_TRANSPORT_NODE_PRODUCT_FNL_TEMPLATE_ORTHOGONAL));
                  break;

                case fNLDBI:
                  writer.write_value(__CPP_TRANSPORT_NODE_PRODUCT_FNL_TEMPLATE, std::string(__CPP_TRANSPORT_NODE_PRODUCT_FNL_TEMPLATE_DBI));
                  break;

                default:
                  assert(false);
                  throw runtime_exception(runtime_exception::RUNTIME_ERROR, __CPP_TRANSPORT_PRODUCT_FNL_LINE_UNKNOWN_TEMPLATE);
              }
          }


        template <typename number>
        void fNL_line<number>::write(std::ostream& out)
          {
            out << "FIXME" << std::endl;
          }

      }   // namespace derived_data

  }   // namespace transport


#endif //__fNL_line_H_
