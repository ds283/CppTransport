//
// Created by David Seery on 13/05/2014.
// Copyright (c) 2014-2016 University of Sussex. All rights reserved.
//


#ifndef CPPTRANSPORT_DERIVED_PRODUCT_H
#define CPPTRANSPORT_DERIVED_PRODUCT_H


#include <float.h>


#include "transport-runtime-api/serialization/serializable.h"

#include "transport-runtime-api/derived-products/utilities/wrapper.h"

// forward-declare datapipe if needed
#include "transport-runtime-api/data/datapipe/datapipe_forward_declare.h"

// forward-declare model class if needed
#include "transport-runtime-api/models/model_forward_declare.h"

// forward-declare tasks if needed
#include "transport-runtime-api/tasks/tasks_forward_declare.h"

// forward-declare repository records if needed
#include "transport-runtime-api/repository/records/repository_records_forward_declare.h"

// get environment objects
#include "transport-runtime-api/manager/environment.h"
#include "transport-runtime-api/manager/argument_cache.h"
#include "transport-runtime-api/manager/detail/slave_message_buffer.h"

// get enumeration classes
#include "transport-runtime-api/derived-products/enumerations.h"
#include "transport-runtime-api/derived-products/derived_product_type.h"

#include "transport-runtime-api/defaults.h"
#include "transport-runtime-api/messages.h"
#include "transport-runtime-api/exceptions.h"

#include "boost/filesystem/operations.hpp"


namespace transport
	{

		namespace derived_data
			{

        constexpr auto CPPTRANSPORT_NODE_DERIVED_PRODUCT_TYPE                 = "derived-product-type";

        constexpr auto CPPTRANSPORT_NODE_DERIVED_PRODUCT_LINE_PLOT2D          = "line-plot2d";
        constexpr auto CPPTRANSPORT_NODE_DERIVED_PRODUCT_LINE_ASCIITABLE      = "line-asciitable";

        constexpr auto CPPTRANSPORT_NODE_DERIVED_PRODUCT_FILENAME             = "filename";
        constexpr auto CPPTRANSPORT_NODE_DERIVED_PRODUCT_DESCRIPTION          = "description";


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


            //! deserialization constructor
            derived_product(const std::string& nm, Json::Value& reader);

            //! destructor is default
		        virtual ~derived_product() = default;


		        // DERIVED PRODUCTS -- QUERY BASIC DATA

		      public:

				    //! Get name of this derived data product
				    const std::string& get_name() const { return(this->name); }

            //! Set description
            void set_description(std::string d) { this->description = std::move(d); }

            //! Get description
            const std::string& get_description() const { return(this->description); }

            //! Get type of this derived data product
            virtual derived_product_type get_type() const = 0;

				    //! Get filename associated with this derived data product
				    const boost::filesystem::path& get_filename() const { return(this->filename); }

		        //! Apply the analysis represented by this derived product to a given content group
		        virtual std::list<std::string> derive(datapipe<number>& pipe, const std::list<std::string>& tags,
                                                  slave_message_buffer& messages, local_environment& env, argument_cache& args) = 0;


            // DERIVED PRODUCTS -- AGGREGATE CONSTITUENT TASKS

          public:

            //! Collect a list of tasks which this derived product depends on;
            //! used by the repository to autocommit any necessary tasks
            virtual void get_task_list(typename std::list< derivable_task<number>* >& list) const = 0;


				    // SERIALIZATION -- implements a 'serializable' interface

		      public:

				    virtual void serialize(Json::Value& writer) const override;


				    // CLONE

		      public:

				    virtual derived_product<number>* clone() const = 0;


				    // WRITE TO STANDARD OUTPUT

		      public:

				    template <typename Stream> void write(Stream& out);


		        // INTERNAL DATA

		      protected:

		        //! Name of this derived product
		        const std::string name;

            //! Description for this derived product
            std::string description;

		        //! Standardized filename
		        boost::filesystem::path filename;

				    //! Wrapped output utility
				    wrapped_output wrapper;

			    };


        template <typename number>
        derived_product<number>::derived_product(const std::string& nm, Json::Value& reader)
          : name(nm),
            description(reader[CPPTRANSPORT_NODE_DERIVED_PRODUCT_DESCRIPTION].asString())
          {
            filename = reader[CPPTRANSPORT_NODE_DERIVED_PRODUCT_FILENAME].asString();
          }


        template <typename number>
				void derived_product<number>::serialize(Json::Value& writer) const
					{
            writer[CPPTRANSPORT_NODE_DERIVED_PRODUCT_DESCRIPTION] = this->description;
						writer[CPPTRANSPORT_NODE_DERIVED_PRODUCT_FILENAME] = this->filename.string();
					}


				template <typename number>
        template <typename Stream>
				void derived_product<number>::write(Stream& out)
					{
						out << CPPTRANSPORT_DERIVED_PRODUCT_FILENAME << ": " << this->filename << '\n';
					}


      }   // namespace derived_data

	}   // namespace transport


#endif //CPPTRANSPORT_DERIVED_PRODUCT_H
