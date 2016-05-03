//
// Created by David Seery on 26/05/2014.
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


#ifndef CPPTRANSPORT_PRODUCT_LINE_ASCIITABLE_H
#define CPPTRANSPORT_PRODUCT_LINE_ASCIITABLE_H


#include <iostream>
#include <fstream>
#include <algorithm>


#include "transport-runtime/derived-products/line-collections/line_collection.h"
#include "transport-runtime/derived-products/line-collections/data_line.h"

#include "transport-runtime/utilities/asciitable.h"

#include "transport-runtime/defaults.h"
#include "transport-runtime/messages.h"
#include "transport-runtime/exceptions.h"

#include "boost/filesystem/operations.hpp"
#include "boost/log/utility/formatting_ostream.hpp"


#define CPPTRANSPORT_NODE_PRODUCT_LINE_ASCIITABLE_ROOT        "line-asciitable"

#define CPPTRANSPORT_NODE_PRODUCT_LINE_ASCIITABLE_PRECISION   "precision"
#define CPPTRANSPORT_NODE_PRODUCT_LINE_ASCIITABLE_X_LABEL     "x-label"
#define CPPTRANSPORT_NODE_PRODUCT_LINE_ASCIITABLE_X_LABEL_SET "x-label-set"


namespace transport
	{

		namespace derived_data
			{

				template <typename number>
		    class line_asciitable: public line_collection<number>
			    {

				    // LINE_ASCIITABLE: CONSTRUCTOR, DESTRUCTOR

		      public:

				    //! Basic user-facing constructor
				    line_asciitable(const std::string& name, const boost::filesystem::path& filename, unsigned int prec=CPPTRANSPORT_DEFAULT_TABLE_PRECISION)
		          : line_collection<number>(name, filename),
				        x_label_set(false),
				        precision(prec)
				    {
						    if(this->filename.extension().string() != ".txt" &&
							     this->filename.extension().string() != ".dat" &&
							     this->filename.extension().string() != ".data")
							    {
						        std::ostringstream msg;
								    msg << CPPTRANSPORT_PRODUCT_LINE_ASCIITABLE_UNSUPPORTED_FORMAT << " " << filename.extension();
								    throw runtime_exception(exception_type::DERIVED_PRODUCT_ERROR, msg.str());
							    }
					    }

				    //! Deserialization constructor
				    line_asciitable(const std::string& name, Json::Value& reader, task_finder<number> finder);

				    virtual ~line_asciitable() = default;


            // INTERFACE -- DERIVED PRODUCT

          public:

            //! Get type of this derived data product
            derived_product_type get_type() const override final { return(derived_product_type::line_table); }


		        // LINE MANAGEMENT

		      public:

		        //! Add a line to the collection
		        virtual line_asciitable<number>& add_line(const derived_line<number>& line) override { this->line_collection<number>::add_line(line); this->apply_default_labels(!this->x_label_set); return *this; }

            //! overload += to do the same thing
            virtual line_asciitable<number>& operator+=(const derived_line<number>& line) override { return this->add_line(line); }


		        // GENERATE TABLE -- implements a 'derived_product' interface

		      public:

						//! Generate our derived output
		        virtual std::list<std::string> derive(datapipe<number>& pipe, const std::list<std::string>& tags,
                                                  slave_message_buffer& messages, local_environment& env, argument_cache& args) override;


		      protected:

				    //! Make table
            void make_table(datapipe<number>& pipe, const std::deque<double>& axis,
                            const typename std::vector<typename line_collection<number>::output_line>& data,
                            local_environment& env, argument_cache& args) const;


		        // GET AND SET BASIC TABLE ATTRIBUTES

		      public:

				    //! get x-axis text label
				    const std::string& get_x_label() const { return(this->x_label); }

				    //! set x-axis text label
				    line_asciitable<number>& set_x_label(const std::string& l) { this->internal_set_x_label(l); this->x_label_set = true; return *this; }

		      protected:

				    void internal_set_x_label(const std::string& l) { this->x_label = l; }

		      public:

		        // SETTING DEFAULTS

		      public:

		        //! (re-)set a default set of labels; should account for the LaTeX setting if desired
		        virtual void apply_default_labels(bool x_label_set) {}

		        //! (re-)set a default list of settings
		        virtual void apply_default_settings() {}


		        // CLONE

		      public:

		        //! Copy this object
		        virtual line_asciitable<number>* clone() const override { return new line_asciitable<number>(static_cast<const line_asciitable<number>&>(*this)); }


		        // SERIALIZATION -- implements a 'serializable' interface

		      public:

		        virtual void serialize(Json::Value& writer) const override;


		        // WRITE SELF TO A STANDARD STREAM

		      public:

		        template <typename Stream> void write(Stream& out);


				    // INTERNAL DATA

		      private:

				    //! Number of digits precision to use
				    unsigned int precision;

				    //! x-axis label
				    std::string x_label;

		        //! which labels have been explicitly set?
		        bool x_label_set;

			    };


				template <typename number>
				line_asciitable<number>::line_asciitable(const std::string& name, Json::Value& reader, task_finder<number> finder)
					: line_collection<number>(name, reader, finder)
					{
						precision   = reader[CPPTRANSPORT_NODE_PRODUCT_LINE_ASCIITABLE_ROOT][CPPTRANSPORT_NODE_PRODUCT_LINE_ASCIITABLE_PRECISION].asUInt();
						x_label     = reader[CPPTRANSPORT_NODE_PRODUCT_LINE_ASCIITABLE_ROOT][CPPTRANSPORT_NODE_PRODUCT_LINE_ASCIITABLE_X_LABEL].asString();
						x_label_set = reader[CPPTRANSPORT_NODE_PRODUCT_LINE_ASCIITABLE_ROOT][CPPTRANSPORT_NODE_PRODUCT_LINE_ASCIITABLE_X_LABEL_SET].asBool();
					}


				template <typename number>
				std::list<std::string> line_asciitable<number>::derive(datapipe<number>& pipe, const std::list<std::string>& tags,
                                                               slave_message_buffer& messages, local_environment& env, argument_cache& args)
					{
            slave_message_context ctx(messages, this->name);

            // generate output from our constituent lines
				    std::list< data_line<number> > derived_lines;
						this->obtain_output(pipe, tags, derived_lines, messages);

						// merge this output onto a single axis
				    std::deque<double> axis;
						typename std::vector< typename line_collection<number>::output_line > output_lines;
						this->merge_lines(pipe, derived_lines, axis, output_lines);

						// make table
						this->make_table(pipe, axis, output_lines, env, args);

            // get content groups which were used
            std::list<std::string> used_groups = this->extract_content_groups(derived_lines);

						// commit product
						pipe.commit(this, used_groups);

            return(used_groups);
					}


				template <typename number>
        void line_asciitable<number>::make_table(datapipe<number>& pipe, const std::deque<double>& axis,
                                                 const typename std::vector<typename line_collection<number>::output_line>& data,
                                                 local_environment& env, argument_cache& args) const
					{
						// extract paths from the datapipe
				    boost::filesystem::path temp_root = pipe.get_abs_tempdir_path();

						// obtain our output filename
				    boost::filesystem::path table_file = temp_root / this->filename;

				    std::ofstream out;
						out.open(table_file.string().c_str(), std::ios_base::trunc | std::ios_base::out);

						if(out.is_open())
							{
						    asciitable writer(out, env, args);
						    writer.set_precision(this->precision);
								writer.set_wrap_status(false);    // don't want to wrap the content

								// copy labels into vector of labels
						    std::vector<std::string> labels;
						    for(const typename line_collection<number>::output_line& line : data)
							    {
								    labels.push_back(line.get_label());
							    }

								// copy axis deque into the vector expected by asciitable
						    std::vector<double> x;
						    std::copy(axis.begin(), axis.end(), std::back_inserter(x));

								// copy values into array ys
                // the entries in ys are stored columnwise
						    std::vector< std::vector<number> > ys(labels.size());

								for(unsigned int j = 0; j < ys.size(); ++j)
									{
										ys[j].resize(x.size());
									}

								for(unsigned int i = 0; i < data.size(); ++i)
									{
								    typename std::deque< typename line_collection<number>::output_value > values = data[i].get_values();

										assert(values.size() == x.size());

										for(unsigned int j = 0; j < values.size(); ++j)
											{
												ys[i][j] = values[j].format_number();
											}
									}

								writer.write(this->x_label, labels, x, ys, "");
							}
						else
							{
						    std::ostringstream msg;
								msg << CPPTRANSPORT_DERIVED_PRODUCT_FAILED << " " << table_file;
								throw runtime_exception(exception_type::DERIVED_PRODUCT_ERROR, msg.str());
							}
					}

				template <typename number>
				void line_asciitable<number>::serialize(Json::Value& writer) const
					{
						writer[CPPTRANSPORT_NODE_DERIVED_PRODUCT_TYPE] = std::string(CPPTRANSPORT_NODE_DERIVED_PRODUCT_LINE_ASCIITABLE);

				    writer[CPPTRANSPORT_NODE_PRODUCT_LINE_ASCIITABLE_ROOT][CPPTRANSPORT_NODE_PRODUCT_LINE_ASCIITABLE_PRECISION]   = this->precision;
				    writer[CPPTRANSPORT_NODE_PRODUCT_LINE_ASCIITABLE_ROOT][CPPTRANSPORT_NODE_PRODUCT_LINE_ASCIITABLE_X_LABEL]     = this->x_label;
				    writer[CPPTRANSPORT_NODE_PRODUCT_LINE_ASCIITABLE_ROOT][CPPTRANSPORT_NODE_PRODUCT_LINE_ASCIITABLE_X_LABEL_SET] = this->x_label_set;

						// call next serializer
						this->line_collection<number>::serialize(writer);
					}


				template <typename number>
        template <typename Stream>
				void line_asciitable<number>::write(Stream& out)
					{
						// call next writer
						this->line_collection<number>::write(out);

						out << CPPTRANSPORT_PRODUCT_LINE_ASCIITABLE_LABEL_PRECISION << " " << this->precision << '\n';
					}


		    template <typename number, typename Char, typename Traits>
		    std::basic_ostream<Char, Traits>& operator<<(std::basic_ostream<Char, Traits>& out, line_asciitable<number>& obj)
			    {
		        obj.write(out);
		        return(out);
			    }


        template <typename number, typename Char, typename Traits, typename Allocator>
        boost::log::basic_formatting_ostream<Char, Traits, Allocator>& operator<<(boost::log::basic_formatting_ostream<Char, Traits, Allocator>& out, line_asciitable<number>& obj)
          {
            obj.write(out);
            return(out);
          }


			}   // namespace derived_data

	}   // namespace transport


#endif //CPPTRANSPORT_PRODUCT_LINE_ASCIITABLE_H
