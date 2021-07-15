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





namespace transport
	{

		namespace derived_data
			{

        constexpr auto CPPTRANSPORT_NODE_PRODUCT_LINE_ASCIITABLE_ROOT        = "line-asciitable";

        constexpr auto CPPTRANSPORT_NODE_PRODUCT_LINE_ASCIITABLE_PRECISION   = "precision";
        constexpr auto CPPTRANSPORT_NODE_PRODUCT_LINE_ASCIITABLE_X_LABEL     = "x-label";
        constexpr auto CPPTRANSPORT_NODE_PRODUCT_LINE_ASCIITABLE_X_LABEL_SET = "x-label-set";
        constexpr auto CPPTRANSPORT_NODE_PRODUCT_LINE_ASCIITABLE_FORMAT      = "format";
        constexpr auto CPPTRANSPORT_NODE_PRODUCT_LINE_ASCIITABLE_JUSTIFIED   = "justified";
        constexpr auto CPPTRANSPORT_NODE_PRODUCT_LINE_ASCIITABLE_CSV         = "csv";
        constexpr auto CPPTRANSPORT_NODE_PRODUCT_LINE_ASCIITABLE_TSV         = "tsv";

				enum class table_format { justified, csv, tsv };

				template <typename number>
		    class line_asciitable: public line_collection<number>
			    {

				    // LINE_ASCIITABLE: CONSTRUCTOR, DESTRUCTOR

		      public:

				    //! Basic user-facing constructor takes name, output filename, table format specifier
				    //! and optional precision.
				    //! Table format is also deduced from the output filename extension if needed.
				    line_asciitable(const std::string& name, const boost::filesystem::path& filename,
                            table_format f=table_format::justified,
                            unsigned int prec=CPPTRANSPORT_DEFAULT_TABLE_PRECISION)
		          : line_collection<number>(name, filename),
                format(f),
				        x_label_set(false),
				        precision(prec)
				    {
						    if(this->filename.extension().string() != ".txt" &&
							     this->filename.extension().string() != ".dat" &&
							     this->filename.extension().string() != ".data" &&
                   this->filename.extension().string() != ".csv" &&
                   this->filename.extension().string() != ".tsv")
							    {
						        std::ostringstream msg;
								    msg << CPPTRANSPORT_PRODUCT_LINE_ASCIITABLE_UNSUPPORTED_FORMAT << " " << filename.extension();
								    throw runtime_exception(exception_type::DERIVED_PRODUCT_ERROR, msg.str());
							    }

                // set format to CSV or TSV if it is specified
                if(this->filename.extension().string() == ".csv") format = table_format::csv;
                if(this->filename.extension().string() == ".tsv") format = table_format::tsv;
					    }

				    //! Deserialization constructor
				    line_asciitable(const std::string& name, Json::Value& reader, task_finder<number> finder);

				    //! Destructor can be default
				    virtual ~line_asciitable() = default;


            // INTERFACE -- DERIVED PRODUCT

          public:

            //! Get type of this derived data product
            derived_product_type get_type() const override final { return(derived_product_type::line_table); }


		        // LINE MANAGEMENT

		      public:

		        //! Add a line to the collection
		        line_asciitable<number>& add_line(const derived_line<number>& line) override
              {
                this->line_collection<number>::add_line(line);
                this->apply_default_labels(!this->x_label_set);
                return *this;
              }

            //! overload += to do the same thing
            line_asciitable<number>& operator+=(const derived_line<number>& line) override { return this->add_line(line); }

            //! add a vector of derived lines (eg. produced by operator+ overload between derived lines)
            line_asciitable<number>& operator+=(const std::vector< std::shared_ptr< derived_line<number> > > list) override
              {
                this->line_collection<number>::operator+=(list);
                this->apply_default_labels(!this->x_label_set);
                return *this;
              }


		        // GENERATE TABLE -- implements a 'derived_product' interface

		      public:

						//! Generate our derived output
            content_group_name_set derive(datapipe<number>& pipe, const tag_list& tags,
                                          slave_message_buffer& messages, local_environment& env, argument_cache& args) override;


		      protected:

				    //! Make table
            void make_table(datapipe<number>& pipe, const typename line_collection<number>::merged_line_set& data,
                            local_environment& env, argument_cache& args) const;


		        // GET AND SET BASIC TABLE ATTRIBUTES

		      public:

				    //! get x-axis text label
				    const std::string& get_x_label() const { return(this->x_label); }

				    //! set x-axis text label
				    line_asciitable<number>& set_x_label(const std::string& l) { this->internal_set_x_label(l); this->x_label_set = true; return *this; }

            //! get table format
            table_format get_format() const { return(this->format); }

            //! set table format
            line_asciitable<number>& set_format(table_format f) { this->format = f; return *this; }

		      protected:

				    void internal_set_x_label(const std::string& l) { this->x_label = l; }


		        // SETTING DEFAULTS

		      public:

		        //! (re-)set a default set of labels; should account for the LaTeX setting if desired
		        virtual void apply_default_labels(bool x_label_set) {}

		        //! (re-)set a default list of settings
		        virtual void apply_default_settings() {}


		        // CLONE

		      public:

		        //! Copy this object
		        line_asciitable<number>* clone() const override { return new line_asciitable<number>(static_cast<const line_asciitable<number>&>(*this)); }


		        // SERIALIZATION -- implements a 'serializable' interface

		      public:

		        void serialize(Json::Value& writer) const override;


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

            //! table format
            table_format format;

			    };


				template <typename number>
				line_asciitable<number>::line_asciitable(const std::string& name, Json::Value& reader, task_finder<number> finder)
					: line_collection<number>(name, reader, finder)
					{
						precision   = reader[CPPTRANSPORT_NODE_PRODUCT_LINE_ASCIITABLE_ROOT][CPPTRANSPORT_NODE_PRODUCT_LINE_ASCIITABLE_PRECISION].asUInt();
						x_label     = reader[CPPTRANSPORT_NODE_PRODUCT_LINE_ASCIITABLE_ROOT][CPPTRANSPORT_NODE_PRODUCT_LINE_ASCIITABLE_X_LABEL].asString();
						x_label_set = reader[CPPTRANSPORT_NODE_PRODUCT_LINE_ASCIITABLE_ROOT][CPPTRANSPORT_NODE_PRODUCT_LINE_ASCIITABLE_X_LABEL_SET].asBool();

            std::string fm = reader[CPPTRANSPORT_NODE_PRODUCT_LINE_ASCIITABLE_ROOT][CPPTRANSPORT_NODE_PRODUCT_LINE_ASCIITABLE_FORMAT].asString();
            if     (fm == std::string(CPPTRANSPORT_NODE_PRODUCT_LINE_ASCIITABLE_JUSTIFIED)) format = table_format::justified;
            else if(fm == std::string(CPPTRANSPORT_NODE_PRODUCT_LINE_ASCIITABLE_CSV))       format = table_format::csv;
            else if(fm == std::string(CPPTRANSPORT_NODE_PRODUCT_LINE_ASCIITABLE_TSV))       format = table_format::tsv;
            else
              {
                std::ostringstream msg;
                msg << CPPTRANSPORT_PRODUCT_LINE_ASCIITABLE_UNSUPPORTED_TYPE << " '" << fm << "'";
                throw runtime_exception(exception_type::DERIVED_PRODUCT_ERROR, msg.str());
              }
					}


				template <typename number>
				content_group_name_set line_asciitable<number>::derive(datapipe<number>& pipe, const tag_list& tags,
                                                               slave_message_buffer& messages, local_environment& env, argument_cache& args)
					{
            slave_message_context ctx(messages, this->name);

            // generate output from our constituent lines, writing any errors messages into the
            // slave message buffer
						auto data_lines = this->obtain_output(pipe, tags, messages);

						// merge these data lines into a merged_line_set: that consists of a single merged x-axis,
						// plus output_line versions of each data line that contain "mask" values if/where there are missing
						// x sample data points
            auto merged_lines = this->merge_lines(pipe, data_lines);

						// make table using the merged line set
						this->make_table(pipe, merged_lines, env, args);

            // get content groups which were used
            content_group_name_set used_groups = this->extract_content_groups(data_lines);

						// commit product
						pipe.commit(this, used_groups);

            return(used_groups);
					}


				template <typename number>
        void line_asciitable<number>::make_table(datapipe<number>& pipe,
                                                 const typename line_collection<number>::merged_line_set& data,
                                                 local_environment& env, argument_cache& args) const
					{
						// extract paths from the datapipe
				    boost::filesystem::path temp_root = pipe.get_abs_tempdir_path();

						// obtain our output filename
				    boost::filesystem::path table_file = temp_root / this->filename;

				    std::ofstream out;
						out.open(table_file.string().c_str(), std::ios_base::trunc | std::ios_base::out);

            const auto& axis = data.first;
            const auto& lines = data.second;

						if(out.is_open())
							{
						    asciitable writer(out, env, args);
						    writer.set_precision(this->precision);
								writer.set_wrap_status(false);    // don't want to wrap the content

								// copy labels into vector of labels
						    std::vector<std::string> labels;
						    labels.reserve(lines.size());
						    for(const auto& line : lines)
							    {
								    labels.push_back(line.get_label());
							    }

								// copy axis deque into the vector expected by asciitable
						    std::vector<double> x;
						    std::copy(axis.begin(), axis.end(), std::back_inserter(x));

								// copy values into array ys
                // the entries in ys are stored columnwise, ie. first index is column, second index is row
						    std::vector< std::vector<double> > ys(lines.size());
								for(auto& y : ys)
									{
										y.resize(x.size());
									}

                unsigned int i = 0;
								for(const auto& line : lines)
                  {
                    const auto& values = line.get_values();
                    assert(values.size() == x.size());

                    for(unsigned int j = 0; j < values.size(); ++j)
                      {
                        ys[i][j] = values[j].format_number();
                      }
                    ++i;
                  }

                switch(this->format)
                  {
                    case table_format::justified:
                      {
                        writer.write_formatted_data(this->x_label, labels, x, ys, "", asciitable_format::justified);
                        break;
                      }

                    case table_format::csv:
                      {
                        writer.write_formatted_data(this->x_label, labels, x, ys, "", asciitable_format::csv);
                        break;
                      }

                    case table_format::tsv:
                      {
                        writer.write_formatted_data(this->x_label, labels, x, ys, "", asciitable_format::tsv);
                        break;
                      }
                  }
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

            switch(this->format)
              {
                case table_format::justified:
                  {
                    writer[CPPTRANSPORT_NODE_PRODUCT_LINE_ASCIITABLE_ROOT][CPPTRANSPORT_NODE_PRODUCT_LINE_ASCIITABLE_FORMAT] = std::string(CPPTRANSPORT_NODE_PRODUCT_LINE_ASCIITABLE_JUSTIFIED);
                    break;
                  }

                case table_format::csv:
                  {
                    writer[CPPTRANSPORT_NODE_PRODUCT_LINE_ASCIITABLE_ROOT][CPPTRANSPORT_NODE_PRODUCT_LINE_ASCIITABLE_FORMAT] = std::string(CPPTRANSPORT_NODE_PRODUCT_LINE_ASCIITABLE_CSV);
                    break;
                  }

                case table_format::tsv:
                  {
                    writer[CPPTRANSPORT_NODE_PRODUCT_LINE_ASCIITABLE_ROOT][CPPTRANSPORT_NODE_PRODUCT_LINE_ASCIITABLE_FORMAT] = std::string(CPPTRANSPORT_NODE_PRODUCT_LINE_ASCIITABLE_TSV);
                    break;
                  }
              }

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
