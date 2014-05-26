//
// Created by David Seery on 26/05/2014.
// Copyright (c) 2014 University of Sussex. All rights reserved.
//


#ifndef __line_asciitable_H_
#define __line_asciitable_H_


#include <iostream>
#include <fstream>
#include <algorithm>


#include "transport-runtime-api/derived-products/line_collection.h"
#include "transport-runtime-api/derived-products/data_line.h"

#include "transport-runtime-api/utilities/asciitable.h"

#include "transport-runtime-api/defaults.h"
#include "transport-runtime-api/messages.h"
#include "transport-runtime-api/exceptions.h"

#include "boost/filesystem/operations.hpp"


#define __CPP_TRANSPORT_NODE_PRODUCT_LINE_ASCIITABLE_PRECISION "precision"
#define __CPP_TRANSPORT_NODE_PRODUCT_LINE_ASCIITABLE_X_LABEL   "x-label"


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
				    line_asciitable(const std::string& name, const boost::filesystem::path& filename, unsigned int prec=__CPP_TRANSPORT_DEFAULT_TABLE_PRECISION)
		          : precision(prec), line_collection<number>(name, filename)
					    {
						    this->log_y     = false;
						    this->log_x     = false;
						    this->abs_y     = false;
						    this->use_LaTeX = false;

						    if(this->filename.extension().string() != ".txt" &&
							     this->filename.extension().string() != ".dat" &&
							     this->filename.extension().string() != ".data")
							    {
						        std::ostringstream msg;
								    msg << __CPP_TRANSPORT_PRODUCT_LINE_ASCIITABLE_UNSUPPORTED_FORMAT << " " << filename.extension();
								    throw runtime_exception(runtime_exception::DERIVED_PRODUCT_ERROR, msg.str());
							    }
					    }

				    //! Deserialization constructor
				    line_asciitable(const std::string& name, serialization_reader* reader, typename repository<number>::task_finder finder);

				    virtual ~line_asciitable() = default;


		        // GENERATE TABLE -- implements a 'derived_product' interface

		      public:

						//! Generate our derived output
		        virtual void derive(typename data_manager<number>::datapipe& pipe, const std::list<std::string>& tags);

		      protected:

				    //! Make table
		        void make_table(typename data_manager<number>::datapipe& pipe, const std::deque<double>& axis, const typename std::vector< typename line_collection<number>::output_line >& data) const;


		        // GET AND SET BASIC TABLE ATTRIBUTES

		      public:

				    //! get x-axis text label
				    const std::string& get_x_label() const { return(this->x_label); }
				    //! set x-axis text label
				    void set_x_label(const std::string& l) { this->x_label = l; }

		        // CLONE

		      public:

		        //! Copy this object
		        virtual derived_product<number>* clone() const { return new line_asciitable<number>(static_cast<const line_asciitable<number>&>(*this)); }

		        // SERIALIZATION -- implements a 'serializable' interface

		      public:

		        virtual void serialize(serialization_writer& writer) const override;


		        // WRITE SELF TO A STANDARD STREAM

		      public:

		        void write(std::ostream& out);


				    // INTERNAL DATA

		      private:

				    //! Number of digits precision to use
				    unsigned int precision;

				    //! x-axis label
				    std::string x_label;
			    };


				template <typename number>
				line_asciitable<number>::line_asciitable(const std::string& name, serialization_reader* reader, typename repository<number>::task_finder finder)
					: line_collection<number>(name, reader, finder)
					{
						assert(reader != nullptr);

						reader->read_value(__CPP_TRANSPORT_NODE_PRODUCT_LINE_ASCIITABLE_PRECISION, precision);
						reader->read_value(__CPP_TRANSPORT_NODE_PRODUCT_LINE_ASCIITABLE_X_LABEL, x_label);
					}


				template <typename number>
				void line_asciitable<number>::derive(typename data_manager<number>::datapipe& pipe, const std::list<std::string>& tags)
					{
						// generate output from our constituent lines
				    std::list< data_line<number> > derived_lines;
						this->obtain_output(pipe, tags, derived_lines);

						// merge this output onto a single axis
				    std::deque<double> axis;
						typename std::vector< typename line_collection<number>::output_line > output_lines;
						this->merge_lines(pipe, derived_lines, axis, output_lines);

						this->make_table(pipe, axis, output_lines);
					}


				template <typename number>
				void line_asciitable<number>::make_table(typename data_manager<number>::datapipe& pipe, const std::deque<double>& axis, const typename std::vector< typename line_collection<number>::output_line >& data) const
					{
						// extract paths from the datapipe
				    boost::filesystem::path temp_root = pipe.get_temporary_files_path();

						// obtain our output filename
				    boost::filesystem::path table_file = temp_root / this->filename;

				    std::ofstream out;
						out.open(table_file.string().c_str(), std::ios_base::trunc | std::ios_base::out);

						if(out.is_open())
							{
						    asciitable<number> writer(out);
						    writer.set_precision(this->precision);
								writer.set_wrap_status(false);    // don't want to wrap the content

								// copy labels into vector of labels
						    std::vector<std::string> labels;
						    for(typename std::vector< typename line_collection<number>::output_line >::const_iterator t = data.begin(); t != data.end(); t++)
							    {
								    labels.push_back((*t).get_label());
							    }

								// copy axis deque into the vector expected by asciitable
						    std::vector<double> x;
						    std::copy(axis.begin(), axis.end(), std::back_inserter(x));

								// copy values into array ys.
								// the outer array runs over time
								// the inner array runs over lines
						    std::vector< std::vector<number> > ys(x.size());

								for(unsigned int j = 0; j < ys.size(); j++)
									{
										ys[j].resize(data.size());
									}

								for(unsigned int i = 0; i < data.size(); i++)
									{
								    typename std::deque< typename line_collection<number>::output_value > values = data[i].get_values();

										assert(values.size() == x.size());

										for(unsigned int j = 0; j < values.size(); j++)
											{
												ys[j][i] = values[j].format_number();
											}
									}

								writer.write(this->x_label, labels, x, ys, "");
							}
						else
							{
						    std::ostringstream msg;
								msg << __CPP_TRANSPORT_DERIVED_PRODUCT_FAILED << " " << table_file;
								throw runtime_exception(runtime_exception::DERIVED_PRODUCT_ERROR, msg.str());
							}
					}

				template <typename number>
				void line_asciitable<number>::serialize(serialization_writer& writer) const
					{
						this->write_value_node(writer, __CPP_TRANSPORT_NODE_DERIVED_PRODUCT_TYPE, std::string(__CPP_TRANSPORT_NODE_DERIVED_PRODUCT_LINE_ASCIITABLE));

						this->write_value_node(writer, __CPP_TRANSPORT_NODE_PRODUCT_LINE_ASCIITABLE_PRECISION, this->precision);
						this->write_value_node(writer, __CPP_TRANSPORT_NODE_PRODUCT_LINE_ASCIITABLE_X_LABEL, this->x_label);

						// call next serializer
						this->line_collection<number>::serialize(writer);
					}


				template <typename number>
				void line_asciitable<number>::write(std::ostream& out)
					{
						// call next writer
						this->line_collection<number>::write(out);

						out << __CPP_TRANSPORT_PRODUCT_LINE_ASCIITABLE_LABEL_PRECISION << " " << this->precision << std::endl;
					}


		    template <typename number>
		    std::ostream& operator<<(std::ostream& out, line_asciitable<number>& obj)
			    {
		        obj.write(out);
		        return(out);
			    }


			}   // namespace derived_data

	}   // namespace transport


#endif //__line_asciitable_H_
