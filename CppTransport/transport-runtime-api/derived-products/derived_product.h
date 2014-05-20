//
// Created by David Seery on 13/05/2014.
// Copyright (c) 2014 University of Sussex. All rights reserved.
//


#ifndef __derived_product_H_
#define __derived_product_H_


#include <float.h>


#include "transport-runtime-api/serialization/serializable.h"

// need repository in order to get the details of a repository<number>::output_group
// (can't forward-declare because it is a nested class)
#include "transport-runtime-api/manager/repository.h"

// need data_manager in order to get the details of a data_manager<number>>data_pipe
// (can't forward-declare because it is a nested class)
#include "transport-runtime-api/manager/data_manager.h"

#include "transport-runtime-api/derived-products/utilities/wrapper.h"

#include "transport-runtime-api/defaults.h"
#include "transport-runtime-api/messages.h"
#include "transport-runtime-api/exceptions.h"


#define __CPP_TRANSPORT_NODE_DERIVED_PRODUCT_TYPE              "derived-product-type"

#define __CPP_TRANSPORT_NODE_DERIVED_PRODUCT_GENERAL_TIME_PLOT "general-time-plot"

#define __CPP_TRANSPORT_NODE_DERIVED_PRODUCT_FILENAME          "filename"


namespace transport
	{

		// forward-declare model class
    template <typename number> class model;

		// forward-declare class task.
		// task.h includes this header, so we cannot include task.h otherwise we create
		// a circular dependency.
		template <typename number> class task;
		template <typename number> class integration_task;
		template <typename number> class twopf_list_task;

		namespace derived_data
			{

		    //! A derived product represents some particular post-processing
		    //! of the integration data, perhaps to produce a plot,
		    //! extract the data in some suitable format, etc.
		    template <typename number>
		    class derived_product: public serializable
			    {

		      public:

		        // CONSTRUCTOR, DESTRUCTOR

				    //! Construct a derived product object.
		        derived_product(const std::string& nm, const std::string& fnam)
		          : name(nm), filename(fnam)
			        {
			        }

				    //! Deserialization constructor
				    derived_product(const std::string& nm, serialization_reader* reader)
				      : name(nm)
					    {
						    assert(reader != nullptr);

						    reader->read_value(__CPP_TRANSPORT_NODE_DERIVED_PRODUCT_FILENAME, filename);
					    }

		        virtual ~derived_product() = default;


		        // DERIVED PRODUCT INTERFACE

		      public:

				    //! Get name of this derived data product
				    const std::string& get_name() const { return(this->name); }

				    //! Get filename associated with this derived data product
				    const std::string& get_filename() { return(this->name); }

		        //! Apply the analysis represented by this derived product to a given
		        //! output group
		        virtual void derive(typename data_manager<number>::datapipe& pipe) = 0;


				    // SERIALIZATION -- implements a 'serializable' interface

		      public:

				    virtual void serialize(serialization_writer& writer) const override;


				    // CLONE

		      public:

				    virtual derived_product<number>* clone() const = 0;

		        // INTERNAL DATA

		      protected:

		        //! Name of this derived product
		        const std::string name;

		        //! Standardized filename
		        std::string filename;

				    //! Wrapped output utility
				    wrapped_output wrapper;
			    };


				template <typename number>
				void derived_product<number>::serialize(serialization_writer& writer) const
					{
						this->write_value_node(writer, __CPP_TRANSPORT_NODE_DERIVED_PRODUCT_FILENAME, this->filename);
					}


			}   // namespace derived_data

	}   // namespace transport


#endif //__derived_product_H_
