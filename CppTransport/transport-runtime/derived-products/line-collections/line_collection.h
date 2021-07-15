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
#include <utility>
#include <fstream>
#include <functional>
#include <cstdlib>
#include <unordered_map>
#include <unordered_set>

#include "transport-runtime/tasks/derivable_task.h"
#include "transport-runtime/derived-products/derived_product.h"
#include "transport-runtime/derived-products/derived-content/concepts/derived_line.h"
#include "transport-runtime/derived-products/derived-content/concepts/derived_line_helper.h"
#include "transport-runtime/derived-products/line-collections/data_line.h"

#include "transport-runtime/defaults.h"
#include "transport-runtime/messages.h"
#include "transport-runtime/exceptions.h"

#include "boost/filesystem/operations.hpp"
#include "transport-runtime/derived-products/line-collections/line_collection.h"
#include "transport-runtime/utilities/plot_environment.h"
#include "boost/log/utility/formatting_ostream.hpp"


namespace transport
	{

		namespace derived_data
			{

        // Specializations of arithmetic operators to build aggregations of derived_line<number> objects

        // two derived lines can be summed into a vector
        // we have to use a vector of pointers to avoid truncating the derived_line objects stored inside,
        // but also for performance reasons because multiple additions will generate copies
        template <typename number>
        std::vector< std::shared_ptr< derived_line<number> > >
        operator+(const derived_line<number>& lhs, const derived_line<number>& rhs)
          {
            std::vector< std::shared_ptr< derived_line<number> > > list;
            list.reserve(2);

            list.emplace_back(lhs.clone());
            list.emplace_back(rhs.clone());

            return list;
          }

        // a vector of derived lines and another derived line can be summed into a bigger vector
        template <typename number>
        std::vector< std::shared_ptr< derived_line<number> > >
        operator+(const std::vector< std::shared_ptr< derived_line<number> > >& lhs, const derived_line<number>& rhs)
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


        //! An output_value is an atomic "line data" element.
        //! It represents a y-value, wrapping a double value,
        //! but also has a flag indicating whether this particular value
        //! exists or is just a placeholder.
        //! output_value also knows how to format itself for different
        //! output devices
        class output_value
          {

          public:

            explicit output_value(double v)
              : exists(true),
                value(v)
              {
              }

            output_value()
              : exists(false),
                value{}
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
            double format_number() const;


            // INTERNAL DATA

          private:

            //! does this value exist?
            bool exists;

            //! numerical value, if exists
            double value;

          };


        //! container type for output_values representing an output data series
        using output_data_series = std::deque<output_value>;


        //! An output_line is a collection of output values, together with a label.
        //! output_lines inherit their type from their parent data_line
        class output_line
          {

          public:

            output_line(std::string  l, value_type v, data_line_type d)
              : label(std::move(l)),
                value(v),
                data_type(d)
              {
              }

            ~output_line() = default;


            // INTERFACE

          public:

            //! Add a value at the back
            void push_back(output_value v) { this->values.push_back(std::move(v)); }

            //! Add a value at the front
            void push_front(output_value v) { this->values.push_front(std::move(v)); }

            //! Emplace a value at the back
            template <typename ...Args>
            void emplace_back(Args&& ... args) { this->values.emplace_back(std::forward<Args>(args) ...); }

            //! Emplace a value at the front
            template <typename ...Args>
            void emplace_front(Args&& ... args) { this->values.emplace_front(std::forward<Args>(args) ...); }

            //! Get values
            const output_data_series& get_values() const { return(this->values); }

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
            output_data_series values;

          };


        //! A line-collection is a specialization of a derived_product<> that produces
				//! derived data from a collection of 2d lines
				template <typename number>
				class line_collection: public derived_product<number>
					{

				  public:

				    // LINE_COLLECTION: NAMED TYPES

            //! container type for the set of derived_line<> instances we hold
            using derived_line_set = std::list< std::unique_ptr< derived_line<number> > >;

            //! output_axis represents the combined set of x-axis points generated after merging a group of
            //! data lines
            using output_axis = std::deque<double>;

            //! container type for a group of output lines. An output line is a list of *values* and a label.
            //! output_line is an internal concept that is used by line_collection<> and its descendent classes;
            //! data_line<> is a global concept that is used by all derived content generators.
            //! The difference is that data_line<> consists of (x,y) pairs + metadata, and is self-container.
            //! An output_line is supposed to be associated with a merged axis, so consists only of *values*
            //! (no x sample points), and needs an output_axis instance to specify what those are.
            //! It also has less metadata than the original data_line<>
            using output_line_set = std::list<output_line>;

            //! container type for a merged group of data lines; consists of merged axis (in output_axis format)
            //! and a set of output_line instances (in output_line_set format)
            using merged_line_set = std::pair< output_axis, output_line_set >;

            //! output_line_ref_set is a container type for a set of references to the contents of an output_line_set
            //! object. This means we don't have to keep taking copies, but can instead just pass around
            //! iterators
            using output_line_ref_set = std::list<output_line_set::const_iterator>;

            //! binned output lines are represented as a map from value_type to a output_line_ref_set representing
            //! the lines in the bin labeled by value_type
            using binned_lines = std::unordered_map< value_type, output_line_ref_set >;

            //! we package binned lines together with the output_axis object, as for merged_line_set
            using binned_line_set = std::pair< const output_axis&, binned_lines >;


						// LINE_COLLECTION: CONSTRUCTOR, DESTRUCTOR

				  public:

						//! Basic user-facing constructor
						line_collection(std::string name, boost::filesystem::path filename)
				      : log_x{false},
				        log_y{false},
				        abs_y{false},
				        use_LaTeX{false},
				        derived_product<number>(std::move(name), std::move(filename))
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
                for(const auto& p : list) this->add_line(*p);
                return *this;
              }

            //! Get list of lines in collection
            const derived_line_set& get_lines() const { return(this->lines); }

						//! Get x-axis type
						axis_value get_x_axis_value() const;


				  protected:

				    //! Merge a set of x-axis and value data (represented by 'data_line', which is the output type
				    //! for each derived_line<> instance) into a single x-axis, with "masked" values inserted in the output
				    //! lines for any massing x-axis data points.
				    //! Returns a merged_line_set, which is a std::pair with first value representing the merged
				    //! x-axis, and second value representing the (masked) data lines (in 'output_line' format).
				    //! So this function can be regarded as turning a list of 'data_line' instances into 'output_line'
				    //! instances.
				    //! Notice that the datapipe<> is needed only for logging. No data is pulled from the pipe.
				    merged_line_set merge_lines(datapipe<number>& pipe, const data_line_set<number>& input) const;

						//! obtain output from our lines
				    data_line_set<number> obtain_output(datapipe<number>& pipe, const tag_list& tags, slave_message_buffer& messages) const;

            //! Bin lines according to their value type
            binned_line_set bin_lines(const merged_line_set& input, slave_message_buffer& messages,
                                      boost::optional<size_t> max_bins = boost::none);


            // DERIVED PRODUCTS -- AGGREGATE CONSTITUENT TASKS -- implements a 'derived_product' interface

          public:

            //! Collect a list of tasks which this derived product depends on;
            //! used by the repository to autocommit any necessary tasks, and by the task scheduler
            //! to determine which jobs should be scheduled to generate input data for any task
            //! containing this line
            typename derivable_task_set<number>::type get_task_dependencies() const override;


            // AGGREGATE OUTPUT GROUPS FROM A LIST OF LINES

          public:

            //! Collect a list of content groups used to derive content for this task
            content_group_name_set extract_content_groups(const std::list< data_line<number> >& list) const;


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

				    //! Group of data_line objects
				    derived_line_set lines;


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


				namespace line_collection_impl
          {

            template <typename number>
            using point = typename data_line<number>::point;

            template <typename number>
            using point_list = typename data_line<number>::point_list;

            template <typename number>
            class merge_line_data
              {
              public:

                //! constructor captures discardable point_list (we burn through this as part of the
                //! merge operation), a flag to indicate whether abs_y is required,
                //! and a nascent output_line
                merge_line_data(point_list<number> p, bool a, output_line o)
                  : points{std::move(p)},
                    abs_y(a),
                    line{std::move(o)}
                  {
                  }


                // ACCESS API

              public:

                //! get number of remaining points
                size_t remaining_points() const { return points.size(); }

                //! is empty?
                bool empty() const { return points.empty(); }

                //! get abs_y flag
                bool get_abs_y() const { return this->abs_y; }

                //! get next data point from back of points list
                const point<number>& get_back_point() const { return this->points.back(); }

                //! get output_line as rvalue reference, so can be moved to correct location
                output_line&& get_output_line() { return std::move(this->line); }


                // INSERT

              public:

                //! insert new point at front of output line
                void push_front(output_value p) { this->line.push_front(std::move(p)); }

                //! insert new point at back of output line
                void push_back(output_value p) { this->line.push_back(std::move(p)); }

                //! Emplace a value at the back
                template <typename ...Args>
                void emplace_back(Args&& ... args) { this->line.emplace_back(std::forward<Args>(args) ...); }

                //! Emplace a value at the front
                template <typename ...Args>
                void emplace_front(Args&& ... args) { this->line.emplace_front(std::forward<Args>(args) ...); }


                // POP

              public:

                void pop_back() { this->points.pop_back(); }


                // INTERNAL DATA

              private:

                //! list of points remaining to consider during the merge; discarded one-by-one during the operation
                point_list<number> points;

                //! flag to indicate whether abs_y is required
                bool abs_y;

                //! output line. Built up here, but eventually emplaced into the final output_line_set
                output_line line;

              };

          }   // namespace line_collection_impl


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
						for(auto& t : line_array)
					    {
                auto data = derived_line_helper::deserialize<number>(t, finder);
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

		        for(const auto& line : obj.lines)
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
		    data_line_set<number>
		    line_collection<number>::obtain_output(datapipe<number>& pipe, const tag_list& tags, slave_message_buffer& messages) const
			    {
			      data_line_set<number> data_lines;

            for(const auto& line : this->lines)
			        {
		            auto new_lines = line->derive_lines(pipe, tags, messages);
		            data_lines.splice(data_lines.end(), new_lines);
			        }

            return data_lines;
			    }


		    template <typename number>
		    typename line_collection<number>::merged_line_set
        line_collection<number>::merge_lines(datapipe<number>& pipe, const data_line_set<number>& input) const
			    {
			      output_axis axis;
			      output_line_set output;

		        std::list< line_collection_impl::merge_line_data<number> > merge_data;

            // step through our plot lines, merging axis data and excluding any lines which are unplottable

            // FIRST, build a list of plottable lines contained in 'input', and for each one work out whether we
            // need to take the absolute value of the data points

            for(const auto& line : input)
			        {
		            const auto& line_data = line.get_data_points();

		            bool need_abs_y = false;
		            bool nonzero_values = false;

		            if(this->log_y)
			            {
		                for(const auto& d : line_data)
			                {
		                    if(d.second <= 0.0) need_abs_y = true;
		                    // TODO: this test is probably not stable. Check for zero within finite tolerance
		                    if(d.second > 0.0 || d.second < 0.0) nonzero_values = true;
		                    if(need_abs_y && nonzero_values) break;
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
				            for(auto u = line_data.begin(); nonzero_axis && u != line_data.end(); ++u)
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
                    merge_data.emplace_back(line.get_data_points(), this->abs_y || need_abs_y,
                                            output_line{
                                              this->use_LaTeX ? line.get_LaTeX_label() : line.get_non_LaTeX_label(),
                                              line.get_value_type(), line.get_data_line_type()});
			            }
			        }

		        // SECOND work through each axis, populating the single merged axis
				    // the data lines are all guaranteed to be sorted into ascending order, so we can rely on that
		        axis.clear();

            boost::optional<double> next_axis_point;
		        do
			        {
                // find next point to add to merged x-axis (we work from the far right because std::vector can only
                // pop from the end), or determine there are no points left to merge
                next_axis_point = boost::none;

		            // any work left to do? if so, get next point from the *back* of the aggregate input line set
		            for(const auto& d : merge_data)
			            {
			              if(!d.empty())
                      {
                        const auto& point = d.get_back_point();
                        if(!next_axis_point || point.first > *next_axis_point) next_axis_point = point.first;
                      }
			            }

                if(!next_axis_point) break;

                const double p = *next_axis_point;

                // push point to *front* of merged axis
                axis.push_front(p);

                // find data points on each line, if they exist, corresponding to this axis point
                for(auto& d : merge_data)
                  {
                    // are there any points left to consider? if so, look at those, otherwise we emplace an empty
                    // point
                    if(!d.empty())
                      {
                        const auto& point = d.get_back_point();

                        if(std::abs((point.first - p)/point.first) < CPPTRANSPORT_AXIS_MERGE_TOLERANCE)   // yes, this line has a match
                          {
                            // output_value currently stores everything using double, so we may need an
                            // explicit downcast at this point
                            // TODO: consider whether output_value should retain the full 'number' type
                            auto value = static_cast<double>(point.second);

                            // emplace this value at the front of the output line held by d
                            d.emplace_front(d.get_abs_y() ? std::abs(value) : value);

                            // remove point from this line
                            d.pop_back();
                          }
                        else
                          {
                            // this data line doesn't contain the point p, so much an empty ("masked") element
                            d.push_front(output_value{});
                          }
                      }
                    else  // no match, add an empty("masked") element
                      {
                        d.push_front(output_value{});
                      }
                  }
			        } while(next_axis_point);

		        // emplace the newly constructed output lines in the output_line_set
		        for(auto& d : merge_data)
              {
                output.push_back(d.get_output_line());
              }

            return { std::move(axis), std::move(output) };
			    }


        template <typename number>
        typename line_collection<number>::binned_line_set
        line_collection<number>::bin_lines(const merged_line_set& input, slave_message_buffer& messages,
                                           boost::optional<size_t> max_bins)
          {
            // loop through available input lines
            binned_lines binned;

            const auto& merged_lines = input.second;
            for(auto t = merged_lines.cbegin(); t != merged_lines.cend(); ++t)
              {
                // find bin for this line; will construct empty bin if this value_type has not previously
                // been encountered
                auto& bin = binned[t->get_value_type()];

                // copy output line into bin
                bin.push_back(t);
              }

            size_t num_bins = binned.size();

            if(!max_bins || binned.size() <= *max_bins) return { input.first, std::move(binned) };

            std::ostringstream msg;
            msg << "line_collection: too many bins (" << num_bins << ") in collection; maximum allowed is " << *max_bins << ". Excess bins have been discarded.";
            messages.push_back(msg.str());

            using bin_data_type = std::pair< size_t, binned_lines::const_iterator >;
            std::list< bin_data_type > bin_data;

            for(auto t = binned.cbegin(); t != binned.cend(); ++t)
              {
                bin_data.emplace_back(t->second.size(), t);
              }

            struct BinSizeComparator
              {
                bool operator()(const bin_data_type& a, const bin_data_type& b)
                  {
                    return a.first < b.first;
                  }
              };

            bin_data.sort(BinSizeComparator());

            while(bin_data.size() > *max_bins)
              {
                const auto& item = bin_data.back();
                binned.erase(item.second);
                bin_data.pop_back();
              }

            return { input.first, std::move(binned) };
          }


        template <typename number>
        typename derivable_task_set<number>::type line_collection<number>::get_task_dependencies() const
          {
            typename derivable_task_set<number>::type l;
            unsigned int unique_tag = 0;

            // collect task list from each derived_line
            for(const auto& line : this->lines)
              {
                // each line maintains its own list of tasks it depends on, together with any properties required
                // of their content groups.
                // We want to merge all of these together
                const auto& parents = line->get_parent_task_list();

                for(const auto& elt : parents) // TODO: change to std::views::values in C++20 and remove use of .second
                  {
                    // don't insert multiple copies if this task is already present with the same specifiers.
                    // otherwise, we insert a new record. This means we might have records for multiple "versions"
                    // of the same task, with different content group specifiers -- that's expected
                    auto t = std::find_if(l.begin(), l.end(),
                                          [&](const auto& v) { return v.second == elt.second; });

                    if(t == l.end())
                      {
                        auto tag = unique_tag++;
                        l.insert(make_derivable_task_set_element(elt.second.get_task(), elt.second.get_specifier(), tag));
                      }
                  }
              }

            return l;
          }


        template <typename number>
        content_group_name_set line_collection<number>::extract_content_groups(const std::list< data_line<number> >& list) const
          {
            content_group_name_set groups;

            for(const auto& line : list)
              {
                content_group_name_set line_groups = line.get_parent_groups();
                std::merge(groups.begin(), groups.end(),
                           line_groups.begin(), line_groups.end(),
                           std::inserter(groups, groups.begin()));
              }

            return groups;
          }


		    template <typename number>
		    void line_collection<number>::serialize(Json::Value& writer) const
			    {
		        writer[CPPTRANSPORT_NODE_PRODUCT_LINE_COLLECTION_ROOT][CPPTRANSPORT_NODE_PRODUCT_LINE_COLLECTION_LOGX]  = this->log_x;
		        writer[CPPTRANSPORT_NODE_PRODUCT_LINE_COLLECTION_ROOT][CPPTRANSPORT_NODE_PRODUCT_LINE_COLLECTION_LOGY]  = this->log_y;
		        writer[CPPTRANSPORT_NODE_PRODUCT_LINE_COLLECTION_ROOT][CPPTRANSPORT_NODE_PRODUCT_LINE_COLLECTION_ABSY]  = this->abs_y;
		        writer[CPPTRANSPORT_NODE_PRODUCT_LINE_COLLECTION_ROOT][CPPTRANSPORT_NODE_PRODUCT_LINE_COLLECTION_LATEX] = this->use_LaTeX;

		        Json::Value line_array(Json::arrayValue);
            for(const auto& line : this->lines)
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
				    for(auto t = this->lines.begin(); t != this->lines.end(); ++t, ++line_counter)
					    {
						    out << CPPTRANSPORT_PRODUCT_LINE_COLLECTION_LABEL_LINE << " " << line_counter << ":" << '\n';
				        (*t)->write(out);
				        this->wrapper.wrap_newline(out);
					    }
					}


		    void output_value::format_python(std::ostream& out) const
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


				double output_value::format_number(void) const
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
