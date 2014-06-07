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

#include "boost/filesystem/operations.hpp"


#define __CPP_TRANSPORT_NODE_DERIVED_PRODUCT_TYPE              "derived-product-type"

#define __CPP_TRANSPORT_NODE_DERIVED_PRODUCT_LINE_PLOT2D       "line-plot2d"
#define __CPP_TRANSPORT_NODE_DERIVED_PRODUCT_LINE_ASCIITABLE   "line-asciitable"

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
		        derived_product(const std::string& nm, const boost::filesystem::path& fnam)
		          : name(nm), filename(fnam)
			        {
			        }

				    //! Deserialization constructor
				    derived_product(const std::string& nm, serialization_reader* reader, typename repository<number>::task_finder finder)
				      : name(nm)
					    {
						    assert(reader != nullptr);

				        std::string fnam;
						    reader->read_value(__CPP_TRANSPORT_NODE_DERIVED_PRODUCT_FILENAME, fnam);
						    filename = fnam;
					    }

		        virtual ~derived_product() = default;


		        // DERIVED PRODUCTS -- QUERY BASIC DATA

		      public:

				    //! Get name of this derived data product
				    const std::string& get_name() const { return(this->name); }

				    //! Get filename associated with this derived data product
				    const boost::filesystem::path& get_filename() const { return(this->filename); }

		        //! Apply the analysis represented by this derived product to a given
		        //! output group
		        virtual void derive(typename data_manager<number>::datapipe& pipe, const std::list<std::string>& tags) = 0;


            // DERIVED PRODUCTS -- AGGREGATE CONSTITUENT TASKS

          public:

            //! Collect a list of tasks which this derived product depends on;
            //! used by the repository to autocommit any necessary integration tasks
            virtual void get_task_list(typename std::vector< typename std::pair< integration_task<number>*, model<number>* > >& list) const = 0;


				    // SERIALIZATION -- implements a 'serializable' interface

		      public:

				    virtual void serialize(serialization_writer& writer) const override;


				    // CLONE

		      public:

				    virtual derived_product<number>* clone() const = 0;


				    // WRITE TO STANDARD OUTPUT

		      public:

				    void write(std::ostream& out);

		        // INTERNAL DATA

		      protected:

		        //! Name of this derived product
		        const std::string name;

		        //! Standardized filename
		        boost::filesystem::path filename;

				    //! Wrapped output utility
				    wrapped_output wrapper;
			    };


				template <typename number>
				void derived_product<number>::serialize(serialization_writer& writer) const
					{
						this->write_value_node(writer, __CPP_TRANSPORT_NODE_DERIVED_PRODUCT_FILENAME, this->filename.string());
					}


				template <typename number>
				void derived_product<number>::write(std::ostream& out)
					{
						out << this->filename << std::endl;
					}


      }   // namespace derived_data

	}   // namespace transport


#endif //__derived_product_H_
