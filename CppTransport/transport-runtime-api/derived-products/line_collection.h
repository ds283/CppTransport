//
// Created by David Seery on 25/05/2014.
// Copyright (c) 2014-15 University of Sussex. All rights reserved.
//


#ifndef __line_collection_H_
#define __line_collection_H_


#include <iostream>
#include <sstream>
#include <string>
#include <cmath>

#include "transport-runtime-api/derived-products/derived_product.h"
#include "transport-runtime-api/derived-products/derived-content/derived_line.h"
#include "transport-runtime-api/derived-products/derived-content/derived_line_helper.h"
#include "transport-runtime-api/derived-products/data_line.h"

#include "transport-runtime-api/messages.h"
#include "transport-runtime-api/exceptions.h"

#include "boost/filesystem/operations.hpp"


#define __CPP_TRANSPORT_NODE_PRODUCT_LINE_COLLECTION_ENFORCE_MVT     "enforce-max-value-types"
#define __CPP_TRANSPORT_NODE_PRODUCT_LINE_COLLECTION_MAX_VALUE_TYPES "max-value-types"
#define __CPP_TRANSPORT_NODE_PRODUCT_LINE_COLLECTION_LOGX            "log-x"
#define __CPP_TRANSPORT_NODE_PRODUCT_LINE_COLLECTION_LOGY            "log-y"
#define __CPP_TRANSPORT_NODE_PRODUCT_LINE_COLLECTION_ABSY            "abs-y"
#define __CPP_TRANSPORT_NODE_PRODUCT_LINE_COLLECTION_LATEX           "latex"

#define __CPP_TRANSPORT_NODE_PRODUCT_LINE_COLLECTION_LINE_ARRAY      "line-array"


namespace transport
	{

		namespace derived_data
			{

				//! A line-collection is a specialization of a derived product that produces
				//! derived data from a collection of 2d lines

				template <typename number>
				class line_collection: public derived_product<number>
					{

				  public:

				    class output_value
					    {

				      public:

				        output_value(double v)
					        : exists(true), value(v)
					        {
					        }

				        output_value()
					        : exists(false)
					        {
					        }

				        ~output_value() = default;


				        // FORMAT VALUE

				      public:

						    //! Format for output to a Python script
				        void format_python(std::ostream& out) const;

						    //! Format as a number
						    number format_number(void) const;


				        // INTERNAL DATA

				      private:

				        //! does this value exist?
				        bool exists;

				        //! numerical value, if exists
				        double value;
					    };


				    class output_line
					    {

				      public:

				        output_line(const std::string& l)
					        : label(l)
					        {
					        }

				        ~output_line() = default;


				        // INTERFACE

				      public:

				        //! Add a value at the back
				        void push_back(const output_value& v) { this->values.push_back(v); }
				        //! Add a value at the front
				        void push_front(const output_value& v) { this->values.push_front(v); }
				        //! Get values
				        const std::deque<output_value>& get_values() const { return(this->values); }

				        //! Get size
				        unsigned int size() const { return(this->values.size()); }

				        //! Get label
				        const std::string& get_label() const { return(this->label); }


				        // INTERNAL DATA

				      private:

				        //! this line's label
				        std::string label;

				        //! this line's data points
				        std::deque<output_value> values;

					    };


						// LINE_COLLECTION: CONSTRUCTOR, DESTRUCTOR

				  public:

						//! Basic user-facing constructor
						line_collection(const std::string& name, const boost::filesystem::path& filename, bool enforce_mvt=false, unsigned int mvt=0)
				      : enforce_max_value_types(enforce_mvt), max_value_types(mvt),
				        log_x(false), log_y(false), abs_y(false), use_LaTeX(false),
				        derived_product<number>(name, filename)
							{
							}

						//! Override copy constructor to perform a deep copy on the derived_line<> objects we own
						line_collection(const line_collection<number>& obj);

						//! Deserialization constructor
						line_collection(const std::string& name, serialization_reader* reader, typename repository_finder<number>::task_finder finder);

						virtual ~line_collection();


				    // LINE MANAGEMENT

				  public:

				    //! Add a line to the collection
				    void add_line(const derived_line<number>& line);

				  protected:

				    //! Merge axes and value data into a single series
				    void merge_lines(typename data_manager<number>::datapipe& pipe, const std::list< data_line<number> >& input, std::deque<double>& axis, std::vector<output_line>& data) const;

						//! Obtain output from our lines
				    void obtain_output(typename data_manager<number>::datapipe& pipe, const std::list<std::string>& tags, std::list< data_line<number> >& derived_lines) const;


            // DERIVED PRODUCTS -- AGGREGATE CONSTITUENT TASKS -- implements a 'derived_product' interface

          public:

            //! Collect a list of tasks which this derived product depends on;
            //! used by the repository to autocommit any necessary integration tasks
            virtual void get_task_list(typename std::vector< integration_task<number>* >& list) const override;


						// GET AND SET BASIC LINE PROPERTIES

          public:

				    //! get logarithmic x-axis setting
				    bool get_log_x() const { return(this->log_x); }
				    //! set logarithmic x-axis setting
				    void set_log_x(bool g) { this->log_x = g; }

				    //! get logarithmic y-axis setting
				    bool get_log_y() const { return(this->log_y); }
				    //! set logarithmic y-axis setting
				    void set_log_y(bool g) { this->log_y = g; }

				    //! get abs-y-axis setting
				    bool get_abs_y() const { return(this->abs_y); }
				    //! set abs-y-axis setting
				    void set_abs_y(bool g) { this->abs_y = g; }

				    //! get default LaTeX labels setting
				    bool get_use_LaTeX() const { return(this->use_LaTeX); }
				    //! set default LaTeX labels setting
				    void set_use_LaTeX(bool g) { this->use_LaTeX = g; }


						// SERIALIZATION -- implements a 'serializable' interface

				  public:

						virtual void serialize(serialization_writer& writer) const override;


						// WRITE SELF TO A STANDARD STREAM

				  public:

						void write(std::ostream& out);


						// INTERNAL DATA

				  protected:

				    // PLOT DATA

				    //! List of data_line objects to be plotted on the graph.
				    std::list< derived_line<number>* > lines;


						// ADMIN

						//! Enforce maximum number of value types?
						bool enforce_max_value_types;

				    //! Maximum number of different value-types to allow
				    unsigned int max_value_types;


						// HANDLING DATA

						// LINE HANDLING ATTRIBUTES

				    //! logarithmic x-axis?
				    bool log_x;

				    //! logarithmic y-axis?
				    bool log_y;

				    //! take absolute value on y-axis? (mostly useful with log-y data)
				    bool abs_y;

				    //! use LaTeX labels?
				    bool use_LaTeX;

					};


				template <typename number>
				line_collection<number>::line_collection(const std::string& name, serialization_reader* reader, typename repository_finder<number>::task_finder finder)
					: derived_product<number>(name, reader)
					{
						// extract data from reader
						assert(reader != nullptr);

						// read in line management attributes
						reader->read_value(__CPP_TRANSPORT_NODE_PRODUCT_LINE_COLLECTION_ENFORCE_MVT, enforce_max_value_types);
						reader->read_value(__CPP_TRANSPORT_NODE_PRODUCT_LINE_COLLECTION_MAX_VALUE_TYPES, max_value_types);
				    reader->read_value(__CPP_TRANSPORT_NODE_PRODUCT_LINE_COLLECTION_LOGX, log_x);
				    reader->read_value(__CPP_TRANSPORT_NODE_PRODUCT_LINE_COLLECTION_LOGY, log_y);
				    reader->read_value(__CPP_TRANSPORT_NODE_PRODUCT_LINE_COLLECTION_ABSY, abs_y);
				    reader->read_value(__CPP_TRANSPORT_NODE_PRODUCT_LINE_COLLECTION_LATEX, use_LaTeX);

						// read in line specifications
				    unsigned int num_lines = reader->start_array(__CPP_TRANSPORT_NODE_PRODUCT_LINE_COLLECTION_LINE_ARRAY);

				    lines.clear();
				    for(unsigned int i = 0; i < num_lines; i++)
					    {
				        reader->start_array_element();

				        derived_line<number>* data = derived_line_helper::deserialize<number>(reader, finder);
				        lines.push_back(data);

				        reader->end_array_element();
					    }

				    reader->end_element(__CPP_TRANSPORT_NODE_PRODUCT_LINE_COLLECTION_LINE_ARRAY);
					}


		    template <typename number>
		    line_collection<number>::line_collection(const line_collection<number>& obj)
			    : derived_product<number>(obj),
			      max_value_types(obj.max_value_types), enforce_max_value_types(obj.enforce_max_value_types),
		        log_x(obj.log_x), log_y(obj.log_y),
		        abs_y(obj.abs_y), use_LaTeX(obj.use_LaTeX)
			    {
		        lines.clear();

		        for(typename std::list< derived_line<number>* >::const_iterator t = obj.lines.begin(); t != obj.lines.end(); t++)
			        {
		            lines.push_back((*t)->clone());
			        }
			    }


		    template <typename number>
		    void line_collection<number>::add_line(const derived_line<number>& line)
			    {
				    // check that the axis-type used by this line is compatible with any existing lines
				    if(this->lines.size() > 0)
					    {
						    if(line.get_axis_type() != this->lines.front()->get_axis_type())
							    throw runtime_exception(runtime_exception::DERIVED_PRODUCT_ERROR, __CPP_TRANSPORT_PRODUCT_LINE_COLLECTION_AXIS_MISMATCH);
					    }

				    // check that adding this line won't introduce too many value types
				    if(this->enforce_max_value_types)
					    {
				        typename std::list< typename derived_line<number>::value_type > value_list;
				        value_list.push_back(line.get_value_type());

				        for(typename std::list< derived_line<number>* >::iterator t = this->lines.begin(); t != this->lines.end(); t++)
					        {
				            typename derived_line<number>::value_type value = (*t)->get_value_type();
				            if(std::find(value_list.begin(), value_list.end(), value) == value_list.end())
					            {
				                value_list.push_back(value);
					            }
					        }

				        if(value_list.size() > this->max_value_types)
					        throw runtime_exception(runtime_exception::DERIVED_PRODUCT_ERROR, __CPP_TRANSPORT_PRODUCT_LINE_COLLECTION_TOO_MANY_VALUES);
					    }

		        this->lines.push_back(line.clone());
			    }


		    template <typename number>
		    line_collection<number>::~line_collection()
			    {
		        for(typename std::list< derived_line<number>* >::const_iterator t = this->lines.begin(); t != this->lines.end(); t++)
			        {
		            delete *t;
			        }
			    }


		    template <typename number>
		    void line_collection<number>::obtain_output(typename data_manager<number>::datapipe& pipe, const std::list<std::string>& tags, std::list< data_line<number> >& derived_lines) const
			    {
		        for(typename std::list< derived_line<number>* >::const_iterator t = this->lines.begin(); t != this->lines.end(); t++)
			        {
		            (*t)->derive_lines(pipe, derived_lines, tags);
			        }
			    }


		    template <typename number>
		    void line_collection<number>::merge_lines(typename data_manager<number>::datapipe& pipe,
		                                              const std::list< data_line<number> >& input, std::deque<double>& axis, std::vector<output_line>& output) const
			    {
		        // step through our plot lines, merging axis data and excluding any lines which are unplottable

		        // FIRST, build a list of plottable lines container in 'input',
		        // and work out whether we need to take the absolute value
		        output.clear();

		        std::vector< std::vector< std::pair<double, number> > > data;
		        std::vector< bool >                                     data_absy;

		        for(typename std::list< data_line<number> >::const_iterator t = input.begin(); t != input.end(); t++)
			        {
		            const std::vector< std::pair<double, number> >& line_data = (*t).get_data_points();

		            bool need_abs_y = false;
		            bool nonzero_values = false;

		            if(this->log_y)
			            {
		                for(typename std::vector< std::pair<double, number> >::const_iterator u = line_data.begin(); (!need_abs_y || !nonzero_values) && u != line_data.end(); u++)
			                {
		                    if((*u).second <= 0.0) need_abs_y = true;
		                    if((*u).second > 0.0 || (*u).second < 0.0) nonzero_values = true;
			                }

		                // issue warnings if required
		                if(need_abs_y && !this->abs_y && nonzero_values)        // can plot the line, but have to abs() it
			                BOOST_LOG_SEV(pipe.get_log(), data_manager<number>::normal) << ":: Warning: data line '" << (*t).get_non_LaTeX_label() << "' contains negative or zero values; plotting absolute values instead because of logarithmic y-axis";
		                else if(need_abs_y && !this->abs_y && !nonzero_values)  // can't plot the line
			                BOOST_LOG_SEV(pipe.get_log(), data_manager<number>::normal) << ":: Warning: data line '" << (*t).get_non_LaTeX_label() << "' contains no positive values and can't be plotted on a logarithmic y-axis -- skipping this line";
			            }

				        bool nonzero_axis = true;
				        if(this->log_x)
					        {
				            for(typename std::vector< std::pair<double, number> >::const_iterator u = line_data.begin(); nonzero_axis && u != line_data.end(); u++)
					            {
						            if((*u).first <= 0.0) nonzero_axis = false;
					            }

						        // warn if line can't be plotted
						        if(!nonzero_axis)
							        BOOST_LOG_SEV(pipe.get_log(), data_manager<number>::normal) << ":: Warning: data line '" << (*t).get_non_LaTeX_label() << "' contains nonpositive x-axis values and can't be plotted on a logarithmic x-axis -- skipping this line";
					        }

		            // if we can plot the line, push it onto the queue to be processed.
		            // The line isn't plottable if:
		            //    ** the y-axis is logarithmic but the data has no nonzero values
				        //    ** the x-axis is logarithmic but there are some nonpositive points
		            if((!this->log_x || (this->log_x && nonzero_axis)) && (!this->log_y || (this->log_y && nonzero_values)))
			            {
		                output.push_back(output_line(this->use_LaTeX ? (*t).get_LaTeX_label() : (*t).get_non_LaTeX_label()));
		                data_absy.push_back(this->abs_y || need_abs_y);

				            data.push_back((*t).get_data_points());
			            }
			        }

		        // SECOND work through each axis, populating the single merged axis
				    // the data lines are all guaranteed to be sorted into ascending order, so we can rely on that
		        axis.clear();
		        bool finished = false;

		        while(!finished)
			        {
		            finished = true;

		            // any work left to do?
		            for(unsigned int i = 0; finished && i < output.size(); i++)
			            {
		                if(data[i].size() > 0) finished = false;
			            }

		            if(!finished)
			            {
		                // find next point to add to merged x-axis (we work from the far right because std::vector can only pop from the end)
		                double next_axis_point = -DBL_MAX;
		                for(unsigned int i = 0; i < output.size(); i++)
			                {
		                    if(data[i].size() > 0)
			                    {
				                    const std::pair<double, number>& point = data[i].back();
		                        if(point.first > next_axis_point) next_axis_point = point.first;
			                    }
			                }

		                if(next_axis_point != -DBL_MAX)
			                {
		                    // push point to merged axis
		                    axis.push_front(next_axis_point);

		                    // find data points on each line, if they exist, corresponding to this axis point
		                    for(unsigned int i = 0; i < output.size(); i++)
			                    {
		                        if(data[i].size() > 0)
			                        {
				                        const std::pair<double, number>& point = data[i].back();
		                            if(point.first == next_axis_point)   // yes, this line has a match
			                            {
		                                output[i].push_front(output_value(data_absy[i] ? fabs(point.second) : point.second));

		                                // remove point from this line
		                                data[i].pop_back();
			                            }
		                            else
			                            {
		                                output[i].push_front(output_value());
			                            }
			                        }
		                        else  // no match, add an empty component
			                        {
		                            output[i].push_front(output_value());
			                        }
			                    }
			                }
		                else
			                {
		                    BOOST_LOG_SEV(pipe.get_log(), data_manager<number>::error) << ":: Error: failed to find new axis point to merge; giving up";
		                    finished = true;
			                }
			            }
			        }
			    }


        template <typename number>
        void line_collection<number>::get_task_list(typename std::vector< integration_task<number>* >& list) const
          {
            list.clear();

            // collect data from each derived_line
            for(typename std::list< derived_line<number>* >::const_iterator t = this->lines.begin(); t != this->lines.end(); t++)
              {
                list.push_back((*t)->get_parent_task());
              }
          }


		    template <typename number>
		    void line_collection<number>::serialize(serialization_writer& writer) const
			    {
				    writer.write_value(__CPP_TRANSPORT_NODE_PRODUCT_LINE_COLLECTION_ENFORCE_MVT, this->enforce_max_value_types);
		        writer.write_value(__CPP_TRANSPORT_NODE_PRODUCT_LINE_COLLECTION_MAX_VALUE_TYPES, this->max_value_types);
		        writer.write_value(__CPP_TRANSPORT_NODE_PRODUCT_LINE_COLLECTION_LOGX, this->log_x);
		        writer.write_value(__CPP_TRANSPORT_NODE_PRODUCT_LINE_COLLECTION_LOGY, this->log_y);
		        writer.write_value(__CPP_TRANSPORT_NODE_PRODUCT_LINE_COLLECTION_ABSY, this->abs_y);
		        writer.write_value(__CPP_TRANSPORT_NODE_PRODUCT_LINE_COLLECTION_LATEX, this->use_LaTeX);

		        writer.start_array(__CPP_TRANSPORT_NODE_PRODUCT_LINE_COLLECTION_LINE_ARRAY, this->lines.size() == 0);
		        for(typename std::list< derived_line<number>* >::const_iterator t = this->lines.begin(); t != this->lines.end(); t++)
			        {
		            writer.start_node("arrayelt", false);    // node name ignored in array
		            (*t)->serialize(writer);
		            writer.end_element("arrayelt");
			        }
		        writer.end_element(__CPP_TRANSPORT_NODE_PRODUCT_LINE_COLLECTION_LINE_ARRAY);

		        // call next serialization
		        this->derived_product<number>::serialize(writer);
			    }


		    template <typename number>
		    void line_collection<number>::write(std::ostream& out)
			    {
		        // call next writer
		        this->derived_product<number>::write(out);

		        unsigned int count = 0;

		        out << __CPP_TRANSPORT_PRODUCT_LINE_COLLECTION_LABEL_TITLE_A << " '" << this->get_name() << "', " << __CPP_TRANSPORT_PRODUCT_LINE_COLLECTION_LABEL_TITLE_B << std::endl;

		        this->wrapper.wrap_list_item(out, this->log_x, __CPP_TRANSPORT_PRODUCT_LINE_COLLECTION_LABEL_LOGX, count);
		        this->wrapper.wrap_list_item(out, this->log_y, __CPP_TRANSPORT_PRODUCT_LINE_COLLECTION_LABEL_LOGY, count);
				    this->wrapper.wrap_list_item(out, this->abs_y, __CPP_TRANSPORT_PRODUCT_LINE_COLLECTION_LABEL_ABSY, count);
				    this->wrapper.wrap_list_item(out, this->use_LaTeX, __CPP_TRANSPORT_PRODUCT_LINE_COLLECTION_LABEL_LATEX, count);

						this->wrapper.wrap_newline(out);

				    for(typename std::list< derived_line<number>* >::iterator t = this->lines.begin(); t != this->lines.end(); t++)
					    {
				        (*t)->write(out);
				        this->wrapper.wrap_newline(out);
					    }
					}


		    template <typename number>
		    void line_collection<number>::output_value::format_python(std::ostream& out) const
			    {
		        if(this->exists)
			        {
		            out << this->value;
			        }
		        else
			        {
		            out << "np.nan";
			        }
			    }


				template <typename number>
				number line_collection<number>::output_value::format_number(void) const
					{
						if(this->exists)
							{
								return(this->value);
							}
						else
							{
								return(std::nan(""));
							}
					}


			}   // namespace derived_data

	}   // namespace transport

class line_collection
	{

	};


#endif //__line_collection_H_
