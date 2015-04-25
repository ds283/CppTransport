//
// Created by David Seery on 15/12/14.
// Copyright (c) 2014-15 University of Sussex. All rights reserved.
//


#ifndef __tensor_twopf_line_H_
#define __tensor_twopf_line_H_


#include <iostream>
#include <sstream>
#include <string>
#include <list>
#include <vector>
#include <stdexcept>

#include "transport-runtime-api/serialization/serializable.h"

// get details of datapipe<number>
#include "transport-runtime-api/data/datapipe/datapipe.h"

// forward-declare model class if needed
#include "transport-runtime-api/models/model_forward_declare.h"

// forward-declare tasks if needed
#include "transport-runtime-api/tasks/tasks_forward_declare.h"

// forward-declare repository records if needed
#include "transport-runtime-api/repository/records/repository_records_forward_declare.h"

#include "transport-runtime-api/derived-products/utilities/index_selector.h"
#include "transport-runtime-api/derived-products/utilities/wrapper.h"
#include "transport-runtime-api/derived-products/utilities/filter.h"

#include "transport-runtime-api/derived-products/derived-content/concepts/lines/derived_line.h"
#include "transport-runtime-api/derived-products/derived-content/utilities/integration_task_gadget.h"


namespace transport
	{

    namespace derived_data
	    {

		    //! general tensor twopf content producer, suitable
		    //! for producing content usable in eg. a 2d plot or table
		    //! Note that we derive virtually from derived_line<> to solve the diamond
		    //! problem -- concrete classes may inherit several derived_line<> attributes,
		    //! eg. wavenumber_series<> and tensor_twopf_line<>
		    template <typename number>
		    class tensor_twopf_line: public virtual derived_line<number>
			    {

			      // CONSTRUCTOR, DESTRUCTOR

		      public:

				    //! Basic user-facing constructor
				    tensor_twopf_line(const twopf_list_task<number>& tk, index_selector<2>& sel, filter::twopf_kconfig_filter& kfilter);

				    //! Deserialization constructor
				    tensor_twopf_line(Json::Value& reader, typename repository_finder<number>::task_finder& finder);

				    virtual ~tensor_twopf_line() = default;


				    // LABELLING SERVICES

		      public:

				    //! make a LaTeX label
				    std::string make_LaTeX_label(unsigned int m, unsigned int n) const;

				    //! make a non-LaTeX label
				    std::string make_non_LaTeX_label(unsigned int m, unsigned int n) const;


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

				    //! integration task gadget
		        integration_task_gadget<number> gadget;

				    //! record which indices are active in this group
				    index_selector<2> active_indices;

			    };


		    // constructor DOESN'T CALL the correct derived_line<> constructor; concrete classes must call it for themselves
		    template <typename number>
		    tensor_twopf_line<number>::tensor_twopf_line(const twopf_list_task<number>& tk, index_selector<2>& sel, filter::twopf_kconfig_filter& kfilter)
		      : derived_line<number>(tk),
		        gadget(tk),
		        active_indices(sel)
			    {
				    if(active_indices.get_number_fields() != 2)
					    {
				        std::ostringstream msg;
						    msg << __CPP_TRANSPORT_PRODUCT_INDEX_MISMATCH << " ("
								    << __CPP_TRANSPORT_PRODUCT_INDEX_MISMATCH_A << " " << active_indices.get_number_fields() << ", "
								    << __CPP_TRANSPORT_PRODUCT_INDEX_MISMATCH_B << " 2)";
				        throw runtime_exception(runtime_exception::RUNTIME_ERROR, msg.str());
					    }

				    // set up a list of serial numbers corresponding to the k-configurations for this derived line
		        try
			        {
		            this->f.filter_twopf_kconfig_sample(kfilter, tk.get_twopf_database(), this->kconfig_sample_sns);
			        }
				    catch(runtime_exception& xe)
					    {
						    if(xe.get_exception_code() == runtime_exception::FILTER_EMPTY)
							    {
						        std::ostringstream msg;
						        msg << __CPP_TRANSPORT_PRODUCT_WAVENUMBER_SERIES_EMPTY_FILTER << " '" << this->get_parent_task()->get_name() << "'";
						        throw runtime_exception(runtime_exception::DERIVED_PRODUCT_ERROR, msg.str());
							    }
						    else throw xe;
					    }
			    }


		    // deserialization constructor DOESN'T CALL the correct derived_line<> deserialization constructor
		    // because of virtual inheritance; concrete classes must call it for themselves
		    template <typename number>
		    tensor_twopf_line<number>::tensor_twopf_line(Json::Value& reader, typename repository_finder<number>::task_finder& finder)
			    : derived_line<number>(reader),
			      gadget(),
		        active_indices(reader)
			    {
				    assert(this->parent_task != nullptr);
		        gadget.set_task(this->parent_task, finder);
			    }


		    template <typename number>
		    std::string tensor_twopf_line<number>::make_LaTeX_label(unsigned int m, unsigned int n) const
			    {
				    assert(m <= 1);
				    assert(n <= 1);
				    if(m > 1 || n > 1)
					    {
				        std::ostringstream msg;
						    msg << __CPP_TRANSPORT_PRODUCT_INDEX_BOUNDS << " (" << m << "," << n << ")";
						    throw runtime_exception(runtime_exception::RUNTIME_ERROR, msg.str());
					    }

		        std::ostringstream label;
		        label << std::setprecision(this->precision);

		        if(m == 0)      label << __CPP_TRANSPORT_LATEX_TENSOR_SYMBOL;
				    else if(m == 1) label << __CPP_TRANSPORT_LATEX_TENSOR_MOMENTUM_SYMBOL;

				    label << " ";

				    if(n == 0)      label << __CPP_TRANSPORT_LATEX_TENSOR_SYMBOL;
				    else if(n == 1) label << __CPP_TRANSPORT_LATEX_TENSOR_MOMENTUM_SYMBOL;

				    return(label.str());
			    }


        template <typename number>
        std::string tensor_twopf_line<number>::make_non_LaTeX_label(unsigned int m, unsigned int n) const
	        {
            assert(m <= 1);
            assert(n <= 1);
            if(m > 1 || n > 1)
	            {
                std::ostringstream msg;
                msg << __CPP_TRANSPORT_PRODUCT_INDEX_BOUNDS << " (" << m << "," << n << ")";
                throw runtime_exception(runtime_exception::RUNTIME_ERROR, msg.str());
	            }

            std::ostringstream label;
            label << std::setprecision(this->precision);

            if(m == 0)      label << __CPP_TRANSPORT_NONLATEX_TENSOR_SYMBOL;
            else if(m == 1) label << __CPP_TRANSPORT_NONLATEX_TENSOR_MOMENTUM_SYMBOL;

            label << " ";

            if(n == 0)      label << __CPP_TRANSPORT_NONLATEX_TENSOR_SYMBOL;
            else if(n == 1) label << __CPP_TRANSPORT_NONLATEX_TENSOR_MOMENTUM_SYMBOL;

            return(label.str());
	        }


		    template <typename number>
		    void tensor_twopf_line<number>::serialize(Json::Value& writer) const
			    {
				    this->active_indices.serialize(writer);
			    }


		    template <typename number>
		    void tensor_twopf_line<number>::write(std::ostream& out)
			    {
		        std::vector<std::string> names = { __CPP_TRANSPORT_NONLATEX_TENSOR_SYMBOL, __CPP_TRANSPORT_NONLATEX_TENSOR_MOMENTUM_SYMBOL };

				    out << " " << __CPP_TRANSPORT_PRODUCT_WAVENUMBER_SERIES_LABEL_TENSOR_TWOPF << std::endl;
		        out << "  " << __CPP_TRANSPORT_PRODUCT_LINE_COLLECTION_LABEL_INDICES << " ";
		        this->active_indices.write(out, names);
		        out << std::endl;
			    }

	    }
	}

#endif //__tensor_twopf_line_H_
