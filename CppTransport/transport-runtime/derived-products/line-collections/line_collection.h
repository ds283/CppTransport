//
// Created by David Seery on 25/05/2014.
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


#ifndef CPPTRANSPORT_LINE_COLLECTION_H
#define CPPTRANSPORT_LINE_COLLECTION_H


#include <iostream>
#include <sstream>
#include <string>
#include <cmath>

#include "transport-runtime/derived-products/derived_product.h"
#include "transport-runtime/derived-products/derived-content/concepts/derived_line.h"
#include "transport-runtime/derived-products/derived-content/concepts/derived_line_helper.h"
#include "transport-runtime/derived-products/line-collections/data_line.h"

#include "transport-runtime/defaults.h"
#include "transport-runtime/messages.h"
#include "transport-runtime/exceptions.h"

#include "boost/filesystem/operations.hpp"


namespace transport
	{

		namespace derived_data
			{

        // Specializations of arithmetic operators to build aggregations of derived_line<number> objects

        // two derived lines can be summed into a vector
        // we have to use a vector of pointers to avoid truncating the derived_line objects stored inside,
        // but also for performance reasons because multiple additions will generate copies
        template <typename number>
        std::vector< std::shared_ptr< derived_line<number> > > operator+(const derived_line<number>& lhs, const derived_line<number>& rhs)
          {
            std::vector< std::shared_ptr< derived_line<number> > > list;
            list.reserve(2);

            list.emplace_back(lhs.clone());
            list.emplace_back(rhs.clone());

            return list;
          }

        // a vector of derived lines and another derived line can be summed into a bigger vector
        template <typename number>
        std::vector< std::shared_ptr< derived_line<number> > > operator+(const std::vector< std::shared_ptr< derived_line<number> > >& lhs, const derived_line<number>& rhs)
          {
            std::vector< std::shared_ptr< derived_line<number> > > list = lhs;

            list.emplace_back(rhs.clone());

            return list;
          }


        constexpr auto CPPTRANSPORT_NODE_PRODUCT_LINE_COLLECTION_ROOT = "line-collection";

        constexpr auto CPPTRANSPORT_NODE_PRODUCT_LINE_COLLECTION_LOGX = "log-x";
        constexpr auto CPPTRANSPORT_NODE_PRODUCT_LINE_COLLECTION_LOGY = "log-y";
        constexpr auto CPPTRANSPORT_NODE_PRODUCT_LINE_COLLECTION_ABSY = "abs-y";
        constexpr auto CPPTRANSPORT_NODE_PRODUCT_LINE_COLLECTION_LATEX = "latex";

        constexpr auto CPPTRANSPORT_NODE_PRODUCT_LINE_COLLECTION_LINE_ARRAY = "line-array";

				//! A line-collection is a specialization of a derived product that produces
				//! derived data from a collection of 2d lines

				template <typename number>
				class line_collection: public derived_product<number>
					{

				  public:

						//! An output_value is an atomic plotting element.
						//! It represents a y-value, wrapping a 'number' value,
						//! but also has a flag indicating whether this particular value
						//! exists or is just a placeholder.
						//! output_value also knows how to format itself for different
						//! output devices
				    class output_value
					    {

				      public:

				        output_value(double v)
					        : exists(true),
					          value(v)
					        {
					        }

				        output_value()
					        : exists(false)
					        {
					        }

				        ~output_value() = default;


						    // INTERFACE

				      public:

						    //! check whether this is a real value
						    bool is_present() const { return(this->exists); }


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


						//! An output_line is a collection of output values, together with a label.
						//! output_lines inherit their type from their parent data_line
				    class output_line
					    {

				      public:

				        output_line(const std::string& l, value_type v, data_line_type d)
					        : label(l),
				            value(v),
				            data_type(d)
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

						    //! Get value type (inherited from parent line)
						    value_type get_value_type() const { return(this->value); }

						    //! Get data line type (inherited from parent line)
						    data_line_type get_data_line_type() const { return(this->data_type); }


				        // INTERNAL DATA

				      private:

				        //! this line's label
				        std::string label;

						    //! this line's value, inherited from its parent
						    value_type value;

						    //! this line's data type, inherited from its parent
						    data_line_type data_type;

				        //! this line's data points
				        std::deque<output_value> values;

					    };


						// LINE_COLLECTION: CONSTRUCTOR, DESTRUCTOR

				  public:

						//! Basic user-facing constructor
						line_collection(const std::string& name, const boost::filesystem::path& filename)
				      : log_x(false),
				        log_y(false),
				        abs_y(false),
				        use_LaTeX(false),
				        derived_product<number>(name, filename)
							{
							}

						//! Override copy constructor to perform a deep copy on the derived_line<> objects we own
						line_collection(const line_collection<number>& obj);

						//! Deserialization constructor
						line_collection(const std::string& name, Json::Value& reader, task_finder<number> finder);

            //! destructor is default
						virtual ~line_collection() = default;


				    // LINE MANAGEMENT

				  public:

				    //! Add a line to the collection
				    virtual line_collection<number>& add_line(const derived_line<number>& line);

            //! overload += to do the same thing
            virtual line_collection<number>& operator+=(const derived_line<number>& line) { return this->add_line(line); }

            //! add a vector of derived lines (eg. produced by operator+ overload between derived lines)
            virtual line_collection<number>& operator+=(const std::vector< std::shared_ptr< derived_line<number> > > list)
              {
                for(const std::shared_ptr< derived_line<number> >& p : list) this->add_line(*p);
                return *this;
              }

            //! Get list of lines in collection
            const std::list< std::unique_ptr< derived_line<number> > >& get_lines() const { return(this->lines); }

						//! Get x-axis type
						axis_value get_x_axis_value() const;


				  protected:

				    //! Merge axes and value data into a single series
				    void merge_lines(datapipe<number>& pipe, const std::list< data_line<number> >& input, std::deque<double>& axis, std::vector<output_line>& data) const;

						//! Obtain output from our lines
				    void obtain_output(datapipe<number>& pipe, const std::list<std::string>& tags,
                               std::list< data_line<number> >& derived_lines, slave_message_buffer& messages) const;


            // DERIVED PRODUCTS -- AGGREGATE CONSTITUENT TASKS -- implements a 'derived_product' interface

          public:

            //! Collect a list of tasks which this derived product depends on;
            //! used by the repository to autocommit any necessary tasks
            virtual void get_task_list(typename std::list< derivable_task<number>* >& list) const override;


            // AGGREGATE OUTPUT GROUPS FROM A LIST OF LINES

          public:

            //! Collect a list of content groups used to derive content for this task
            std::list<std::string> extract_content_groups(const std::list< data_line<number> >& list) const;


						// GET AND SET BASIC LINE PROPERTIES

          public:

				    //! get logarithmic x-axis setting
				    bool get_log_x() const { return(this->log_x); }

				    //! set logarithmic x-axis setting
				    line_collection<number>& set_log_x(bool g) { this->log_x = g; return *this; }

				    //! get logarithmic y-axis setting
				    bool get_log_y() const { return(this->log_y); }

				    //! set logarithmic y-axis setting
				    line_collection<number>& set_log_y(bool g) { this->log_y = g; return *this; }

				    //! get abs-y-axis setting
				    bool get_abs_y() const { return(this->abs_y); }

				    //! set abs-y-axis setting
				    line_collection<number>& set_abs_y(bool g) { this->abs_y = g; return *this; }

				    //! get default LaTeX labels setting
				    bool get_use_LaTeX() const { return(this->use_LaTeX); }

				    //! set default LaTeX labels setting
				    line_collection<number>& set_use_LaTeX(bool g) { this->use_LaTeX = g; return *this; }


						// SERIALIZATION -- implements a 'serializable' interface

				  public:

						virtual void serialize(Json::Value& writer) const override;


						// WRITE SELF TO A STANDARD STREAM

				  public:

						template <typename Stream> void write(Stream& out);


						// INTERNAL DATA

				  protected:

				    // PLOT DATA

				    //! List of data_line objects to be plotted on the graph.
				    std::list< std::unique_ptr< derived_line<number> > > lines;


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
				line_collection<number>::line_collection(const std::string& name, Json::Value& reader, task_finder<number> finder)
					: derived_product<number>(name, reader)
					{
						// read in line management attributes
				    log_x     = reader[CPPTRANSPORT_NODE_PRODUCT_LINE_COLLECTION_ROOT][CPPTRANSPORT_NODE_PRODUCT_LINE_COLLECTION_LOGX].asBool();
				    log_y     = reader[CPPTRANSPORT_NODE_PRODUCT_LINE_COLLECTION_ROOT][CPPTRANSPORT_NODE_PRODUCT_LINE_COLLECTION_LOGY].asBool();
				    abs_y     = reader[CPPTRANSPORT_NODE_PRODUCT_LINE_COLLECTION_ROOT][CPPTRANSPORT_NODE_PRODUCT_LINE_COLLECTION_ABSY].asBool();
				    use_LaTeX = reader[CPPTRANSPORT_NODE_PRODUCT_LINE_COLLECTION_ROOT][CPPTRANSPORT_NODE_PRODUCT_LINE_COLLECTION_LATEX].asBool();

						// read in line specifications
				    Json::Value& line_array = reader[CPPTRANSPORT_NODE_PRODUCT_LINE_COLLECTION_ROOT][CPPTRANSPORT_NODE_PRODUCT_LINE_COLLECTION_LINE_ARRAY];
						assert(line_array.isArray());

				    lines.clear();
						for(Json::Value::iterator t = line_array.begin(); t != line_array.end(); ++t)
					    {
                std::unique_ptr< derived_line<number> > data = derived_line_helper::deserialize<number>(*t, finder);
				        lines.push_back(std::move(data));
					    }
					}


		    template <typename number>
		    line_collection<number>::line_collection(const line_collection<number>& obj)
			    : derived_product<number>(obj),
		        log_x(obj.log_x),
			      log_y(obj.log_y),
		        abs_y(obj.abs_y),
			      use_LaTeX(obj.use_LaTeX)
			    {
		        lines.clear();

		        for(const std::unique_ptr< derived_line<number> >& line : obj.lines)
			        {
		            lines.emplace_back(line->clone());
			        }
			    }


		    template <typename number>
		    line_collection<number>& line_collection<number>::add_line(const derived_line<number>& line)
			    {
				    // check that the axis-type used by this line is compatible with any existing lines

				    // if lines are already present in the collection they must share the same
				    // x-axis type (or be convertible to it)
				    if(this->lines.size() > 0)
					    {
						    if(line.get_current_x_axis_value() != this->lines.front()->get_current_x_axis_value())
							    throw runtime_exception(exception_type::DERIVED_PRODUCT_ERROR, CPPTRANSPORT_PRODUCT_LINE_COLLECTION_AXIS_MISMATCH);
					    }

		        this->lines.emplace_back(line.clone());

            return(*this);
			    }


				template <typename number>
				axis_value line_collection<number>::get_x_axis_value() const
					{
						if(this->lines.size() > 0)
							{
						    return(this->lines.front()->get_current_x_axis_value());
							}
						else
							{
								return(axis_value::unset);
							}

					}


		    template <typename number>
		    void line_collection<number>::obtain_output(datapipe<number>& pipe, const std::list<std::string>& tags,
                                                    std::list< data_line<number> >& derived_lines, slave_message_buffer& messages) const
			    {
            for(const std::unique_ptr< derived_line<number> >& line : this->lines)
			        {
		            line->derive_lines(pipe, derived_lines, tags, messages);
			        }
			    }


		    template <typename number>
		    void line_collection<number>::merge_lines(datapipe<number>& pipe, const std::list< data_line<number> >& input,
                                                  std::deque<double>& axis, std::vector<output_line>& output) const
			    {
		        // step through our plot lines, merging axis data and excluding any lines which are unplottable

		        // FIRST, build a list of plottable lines container in 'input',
		        // and work out whether we need to take the absolute value
		        output.clear();

		        std::vector< std::vector< std::pair<double, number> > > data;
		        std::vector<bool> data_absy;

            for(const data_line<number>& line : input)
			        {
		            const std::vector< std::pair<double, number> >& line_data = line.get_data_points();

		            bool need_abs_y = false;
		            bool nonzero_values = false;

		            if(this->log_y)
			            {
		                for(typename std::vector< std::pair<double, number> >::const_iterator u = line_data.begin(); (!need_abs_y || !nonzero_values) && u != line_data.end(); ++u)
			                {
		                    if(u->second <= 0.0) need_abs_y = true;
		                    if(u->second > 0.0 || u->second < 0.0) nonzero_values = true;
			                }

		                // issue warnings if required
		                if(need_abs_y && !this->abs_y && nonzero_values)        // can plot the line, but have to abs() it
			                BOOST_LOG_SEV(pipe.get_log(), datapipe<number>::log_severity_level::normal) << ":: Warning: data line '" << line.get_non_LaTeX_label() << "' contains negative or zero values; plotting absolute values instead because of logarithmic y-axis";
		                else if(need_abs_y && !this->abs_y && !nonzero_values)  // can't plot the line
			                BOOST_LOG_SEV(pipe.get_log(), datapipe<number>::log_severity_level::normal) << ":: Warning: data line '" << line.get_non_LaTeX_label() << "' contains no positive values and can't be plotted on a logarithmic y-axis -- skipping this line";
			            }

				        bool nonzero_axis = true;
				        if(this->log_x)
					        {
				            for(typename std::vector< std::pair<double, number> >::const_iterator u = line_data.begin(); nonzero_axis && u != line_data.end(); ++u)
					            {
						            if(u->first <= 0.0) nonzero_axis = false;
					            }

						        // warn if line can't be plotted
						        if(!nonzero_axis)
							        BOOST_LOG_SEV(pipe.get_log(), datapipe<number>::log_severity_level::normal) << ":: Warning: data line '" << line.get_non_LaTeX_label() << "' contains nonpositive x-axis values and can't be plotted on a logarithmic x-axis -- skipping this line";
					        }

		            // if we can plot the line, push it onto the queue to be processed.
		            // The line isn't plottable if:
		            //    ** the y-axis is logarithmic but the data has no nonzero values
				        //    ** the x-axis is logarithmic but there are some nonpositive points
		            if((!this->log_x || (this->log_x && nonzero_axis)) && (!this->log_y || (this->log_y && nonzero_values)))
			            {
		                output.push_back(output_line(this->use_LaTeX ? line.get_LaTeX_label() : line.get_non_LaTeX_label(), line.get_value_type(), line.get_data_line_type()));
		                data_absy.push_back(this->abs_y || need_abs_y);

				            data.push_back(line.get_data_points());
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
		            for(unsigned int i = 0; finished && i < output.size(); ++i)
			            {
		                if(data[i].size() > 0) finished = false;
			            }

		            if(!finished)
			            {
		                // find next point to add to merged x-axis (we work from the far right because std::vector can only pop from the end)
		                double next_axis_point = -std::numeric_limits<double>::max();
		                for(unsigned int i = 0; i < output.size(); ++i)
			                {
		                    if(data[i].size() > 0)
			                    {
				                    const std::pair<double, number>& point = data[i].back();
		                        if(point.first > next_axis_point) next_axis_point = point.first;
			                    }
			                }

		                if(next_axis_point != -std::numeric_limits<double>::max())
			                {
		                    // push point to merged axis
		                    axis.push_front(next_axis_point);

		                    // find data points on each line, if they exist, corresponding to this axis point
		                    for(unsigned int i = 0; i < output.size(); ++i)
			                    {
		                        if(data[i].size() > 0)
			                        {
				                        const std::pair<double, number>& point = data[i].back();
		                            if(std::abs((point.first - next_axis_point)/point.first) < CPPTRANSPORT_AXIS_MERGE_TOLERANCE)   // yes, this line has a match
			                            {
		                                output[i].push_front(output_value(data_absy[i] ? std::abs(point.second) : point.second));

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
		                    BOOST_LOG_SEV(pipe.get_log(), datapipe<number>::log_severity_level::error) << ":: Error: failed to find new axis point to merge; giving up";
		                    finished = true;
			                }
			            }
			        }
			    }


        namespace line_collection_impl
          {

            template <typename number>
            class DerivedLineNameComparator
              {

                // CONSTRUCTOR, DESTRUCTOR

              public:

                //! constructor
                DerivedLineNameComparator() = default;

                //! destructor
                ~DerivedLineNameComparator() = default;


                // INTERFACE

              public:

                //! compare two derivable_tasks by name
                bool operator()(derivable_task<number>* A, derivable_task<number>* B)
                  {
                    if(A == nullptr || B == nullptr) return false;
                    return A->get_name() < B->get_name();
                  }

              };


            template <typename number>
            class DerivedLineNameEquality
              {

                // CONSTRUCTOR, DESTRUCTOR

              public:

                //! constructor
                DerivedLineNameEquality() = default;

                //! destructor
                ~DerivedLineNameEquality() = default;


                // INTERFACE

              public:

                //! compare two derivable_tasks by name
                bool operator()(derivable_task<number>* A, derivable_task<number>* B)
                  {
                    if(A == nullptr || B == nullptr) return false;
                    return A->get_name() == B->get_name();
                  }

              };

          }   // namespace line_collection_impl


        template <typename number>
        void line_collection<number>::get_task_list(typename std::list< derivable_task<number>* >& list) const
          {
            list.clear();

            // collect data from each derived_line
            for(const std::unique_ptr< derived_line<number> >& line : this->lines)
              {
                list.push_back(line->get_parent_task());
              }

            // sort into lexical order
            list.sort(line_collection_impl::DerivedLineNameComparator<number>());

            // remove duplicates
            list.unique(line_collection_impl::DerivedLineNameEquality<number>());
          }


        template <typename number>
        std::list<std::string> line_collection<number>::extract_content_groups(const std::list< data_line<number> >& list) const
          {
            std::list<std::string> groups;

            for(const data_line<number>& line : list)
              {
                std::list<std::string> line_groups = line.get_parent_groups();
                groups.merge(line_groups);
              }

            groups.sort();
            groups.unique();

            return(groups);
          }


		    template <typename number>
		    void line_collection<number>::serialize(Json::Value& writer) const
			    {
		        writer[CPPTRANSPORT_NODE_PRODUCT_LINE_COLLECTION_ROOT][CPPTRANSPORT_NODE_PRODUCT_LINE_COLLECTION_LOGX]  = this->log_x;
		        writer[CPPTRANSPORT_NODE_PRODUCT_LINE_COLLECTION_ROOT][CPPTRANSPORT_NODE_PRODUCT_LINE_COLLECTION_LOGY]  = this->log_y;
		        writer[CPPTRANSPORT_NODE_PRODUCT_LINE_COLLECTION_ROOT][CPPTRANSPORT_NODE_PRODUCT_LINE_COLLECTION_ABSY]  = this->abs_y;
		        writer[CPPTRANSPORT_NODE_PRODUCT_LINE_COLLECTION_ROOT][CPPTRANSPORT_NODE_PRODUCT_LINE_COLLECTION_LATEX] = this->use_LaTeX;

		        Json::Value line_array(Json::arrayValue);
            for(const std::unique_ptr< derived_line<number> >& line : this->lines)
			        {
		            Json::Value line_element(Json::objectValue);
		            line->serialize(line_element);
				        line_array.append(line_element);
			        }
		        writer[CPPTRANSPORT_NODE_PRODUCT_LINE_COLLECTION_ROOT][CPPTRANSPORT_NODE_PRODUCT_LINE_COLLECTION_LINE_ARRAY] = line_array;

		        // call next serialization
		        this->derived_product<number>::serialize(writer);
			    }


		    template <typename number>
        template <typename Stream>
		    void line_collection<number>::write(Stream& out)
			    {
		        // call derived_product writer
		        this->derived_product<number>::write(out);

		        unsigned int count = 0;

		        this->wrapper.wrap_list_item(out, this->log_x, CPPTRANSPORT_PRODUCT_LINE_COLLECTION_LABEL_LOGX, count);
		        this->wrapper.wrap_list_item(out, this->log_y, CPPTRANSPORT_PRODUCT_LINE_COLLECTION_LABEL_LOGY, count);
				    this->wrapper.wrap_list_item(out, this->abs_y, CPPTRANSPORT_PRODUCT_LINE_COLLECTION_LABEL_ABSY, count);
				    this->wrapper.wrap_list_item(out, this->use_LaTeX, CPPTRANSPORT_PRODUCT_LINE_COLLECTION_LABEL_LATEX, count);

						this->wrapper.wrap_newline(out);

		        out << CPPTRANSPORT_PRODUCT_LINE_COLLECTION_LABEL_TITLE_A << " '" << this->get_name() << "', " << CPPTRANSPORT_PRODUCT_LINE_COLLECTION_LABEL_TITLE_B << '\n' << '\n';

				    unsigned int line_counter = 1;
				    for(typename std::list< derived_line<number>* >::iterator t = this->lines.begin(); t != this->lines.end(); ++t, ++line_counter)
					    {
						    out << CPPTRANSPORT_PRODUCT_LINE_COLLECTION_LABEL_LINE << " " << line_counter << ":" << '\n';
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


#endif //CPPTRANSPORT_LINE_COLLECTION_H
