//
// Created by David Seery on 13/05/2014.
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


#ifndef CPPTRANSPORT_DERIVED_PRODUCT_H
#define CPPTRANSPORT_DERIVED_PRODUCT_H


#include <float.h>

#include <utility>


#include "transport-runtime/serialization/serializable.h"

#include "transport-runtime/derived-products/utilities/wrapper.h"

// forward-declare datapipe if needed
#include "transport-runtime/data/datapipe/datapipe_forward_declare.h"

// forward-declare model class if needed
#include "transport-runtime/models/model_forward_declare.h"

// forward-declare tasks if needed
#include "transport-runtime/tasks/tasks_forward_declare.h"

// forward-declare repository records if needed
#include "transport-runtime/repository/records/repository_records_forward_declare.h"

// get environment objects
#include "transport-runtime/manager/environment.h"
#include "transport-runtime/manager/argument_cache.h"
#include "transport-runtime/manager/detail/slave_message_buffer.h"

// get enumeration classes
#include "transport-runtime/derived-products/enumerations.h"
#include "transport-runtime/derived-products/derived_product_type.h"

#include "transport-runtime/defaults.h"
#include "transport-runtime/messages.h"
#include "transport-runtime/exceptions.h"

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


        //! used to specify which properties are needed in a content group for each task
        //! that supplies data to a derived_product<>
        class content_group_specifiers
          {
          public:

            //! constructor
            content_group_specifiers()
              : ics{false},
                statistics{false},
                spectral_data{false}
              {
              }

            //! destructor
            ~content_group_specifiers() = default;


            // GET/SET DATA

          public:

            bool requires_ics() const { return ics; }

            void set_ics(bool i) { ics = i; }

            bool requires_statistics() const { return statistics; }

            void set_statistics(bool s) { statistics = s; }

            bool requires_spectral_data() const { return spectral_data; }

            void set_spectral_data(bool sd) { spectral_data = sd; }


            // INTERNAL DATA

          protected:

            //! true if we require a content group with initial conditions data (not currently used)
            bool ics;

            //! true if we require a content group with per-configuraiton statistics
            bool statistics;

            //! true if we require a content group with spectral data
            bool spectral_data;

          };


        template <typename number>
        class derivable_task_list
          {
          public:
            using type = std::list< std::pair< derivable_task<number>*, std::unique_ptr<content_group_specifiers> > >;
            using element_type = typename type::value_type;
          };


        namespace derivable_task_list_impl
          {

            template <typename number>
            class NameComparator
              {

                // CONSTRUCTOR, DESTRUCTOR

              public:

                //! constructor
                NameComparator() = default;

                //! destructor
                ~NameComparator() = default;


                // INTERFACE

              public:

                //! compare two derivable_task_list<number>::type elements by name
                bool operator()(const typename derivable_task_list<number>::element_type& A,
                                const typename derivable_task_list<number>::element_type& B)
                  {
                    const auto& A_tk = A.first;
                    const auto& B_tk = B.first;

                    if(A_tk == nullptr || B_tk == nullptr) return false;
                    return A_tk->get_name() < B_tk->get_name();
                  }

              };


            template <typename number>
            class NameEquality
              {

                // CONSTRUCTOR, DESTRUCTOR

              public:

                //! constructor
                NameEquality() = default;

                //! destructor
                ~NameEquality() = default;


                // INTERFACE

              public:

                //! compare two derivable_task_list<number>::type elements by name
                bool operator()(const typename derivable_task_list<number>::element_type& A,
                                const typename derivable_task_list<number>::element_type& B)
                  {
                    const auto& A_tk = A.first;
                    const auto& B_tk = B.first;

                    if(A_tk == nullptr || B_tk == nullptr) return false;
                    return A_tk->get_name() == B_tk->get_name();
                  }

              };

          }   // namespace derivable_task_list_impl


		    //! A derived product represents some particular post-processing
		    //! of the integration data, perhaps to produce a plot,
		    //! extract the data in some suitable format, etc.
		    template <typename number>
		    class derived_product: public serializable
			    {

		      public:

		        // CONSTRUCTOR, DESTRUCTOR

				    //! Construct a derived product object.
		        derived_product(std::string nm, boost::filesystem::path fnam)
		          : name(std::move(nm)), filename(std::move(fnam))
			        {
			        }


            //! deserialization constructor
            derived_product(std::string  nm, Json::Value& reader);

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

		        //! Apply the analysis represented by this derived product to a given content group,
		        //! specified by an attached datapipe<> object
		        virtual std::list<std::string> derive(datapipe<number>& pipe, const std::list<std::string>& tags,
                                                  slave_message_buffer& messages, local_environment& env, argument_cache& args) = 0;


            // DERIVED PRODUCTS -- AGGREGATE CONSTITUENT TASKS

          public:

            //! Collect a list of tasks which this derived product depends on;
            //! used by the repository to autocommit any necessary tasks
            virtual typename derivable_task_list<number>::type get_task_list() const = 0;


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
        derived_product<number>::derived_product(std::string nm, Json::Value& reader)
          : name(std::move(nm)),
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
