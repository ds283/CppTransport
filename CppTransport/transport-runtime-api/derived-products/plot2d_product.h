//
// Created by David Seery on 13/05/2014.
// Copyright (c) 2014 University of Sussex. All rights reserved.
//


#ifndef __plot_product_H_
#define __plot_product_H_


#include <functional>

#include "transport-runtime-api/derived-products/derived_product.h"
#include "transport-runtime-api/derived-products/time-data/time_data_line.h"

#include "transport-runtime-api/messages.h"
#include "transport-runtime-api/exceptions.h"
#include "boost/filesystem/convenience.hpp"


#define __CPP_TRANSPORT_NODE_PRODUCT_PLOT2D_LOGX          "log-x"
#define __CPP_TRANSPORT_NODE_PRODUCT_PLOT2D_LOGY          "log-y"
#define __CPP_TRANSPORT_NODE_PRODUCT_PLOT2D_ABSY          "abs-y"
#define __CPP_TRANSPORT_NODE_PRODUCT_PLOT2D_REVERSEX      "reverse-x"
#define __CPP_TRANSPORT_NODE_PRODUCT_PLOT2D_REVERSEY      "reverse-y"
#define __CPP_TRANSPORT_NODE_PRODUCT_PLOT2D_LATEX         "latex"
#define __CPP_TRANSPORT_NODE_PRODUCT_PLOT2D_XLABEL        "x-label"
#define __CPP_TRANSPORT_NODE_PRODUCT_PLOT2D_XLABEL_TEXT   "x-label-text"
#define __CPP_TRANSPORT_NODE_PRODUCT_PLOT2D_YLABEL        "y-label"
#define __CPP_TRANSPORT_NODE_PRODUCT_PLOT2D_YLABEL_TEXT   "y-label-text"
#define __CPP_TRANSPORT_NODE_PRODUCT_PLOT2D_TITLE         "title"
#define __CPP_TRANSPORT_NODE_PRODUCT_PLOT2D_TITLE_TEXT    "title-text"
#define __CPP_TRANSPORT_NODE_PRODUCT_PLOT2D_LEGEND        "legend"
#define __CPP_TRANSPORT_NODE_PRODUCT_PLOT2D_LEGEND_POS    "legend-position"
#define __CPP_TRANSPORT_NODE_PRODUCT_PLOT2D_LEGEND_TR     "top-right"
#define __CPP_TRANSPORT_NODE_PRODUCT_PLOT2D_LEGEND_BR     "bottom-right"
#define __CPP_TRANSPORT_NODE_PRODUCT_PLOT2D_LEGEND_BL     "bottom-left"
#define __CPP_TRANSPORT_NODE_PRODUCT_PLOT2D_LEGEND_TL     "top-left"
#define __CPP_TRANSPORT_NODE_PRODUCT_PLOT2D_LEGEND_R      "right"
#define __CPP_TRANSPORT_NODE_PRODUCT_PLOT2D_LEGEND_CR     "centre-right"
#define __CPP_TRANSPORT_NODE_PRODUCT_PLOT2D_LEGEND_CL     "centre-left"
#define __CPP_TRANSPORT_NODE_PRODUCT_PLOT2D_LEGEND_UC     "upper-centre"
#define __CPP_TRANSPORT_NODE_PRODUCT_PLOT2D_LEGEND_LC     "lower-centre"
#define __CPP_TRANSPORT_NODE_PRODUCT_PLOT2D_LEGEND_C      "centre"
#define __CPP_TRANSPORT_NODE_PRODUCT_PLOT2D_TYPESET_LATEX "typeset-with-latex"


namespace transport
	{

		// forward-declare model
		template <typename number> class model;


		namespace derived_data
			{

        //! A plot2d-product is a specialization of a derived-product that
		    //! produces a plot of something against time.

		    template <typename number>
		    class plot2d_product: public derived_product<number>
			    {

		      public:

				    typedef enum { top_left, top_right, bottom_left, bottom_right,
					                 right, centre_right, centre_left,
					                 upper_centre, lower_centre, centre} legend_pos;


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

                void format_python(std::ostream& out) const;


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
                std::deque<output_value>& get_values() const { return(this->values); }

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

		      public:

		        // CONSTRUCTOR, DESTRUCTOR

						//! Basic user-facing constructor
		        plot2d_product(const std::string& name, const std::string& filename)
		          : derived_product<number>(name, filename)
			        {
			        }

				    //! Deserialization constructor
		        plot2d_product(const std::string& name, serialization_reader* reader, typename repository<number>::task_finder finder);

		        virtual ~plot2d_product() = default;


		        // SETTING DEFAULTS

		      protected:

		        //! (re-)set a default set of labels; should account for the LaTeX setting if desired
		        virtual void apply_default_labels() = 0;

		        //! (re-)set a default list of settings
		        virtual void apply_default_settings() = 0;

				    //! reset all plot data
				    void reset_plot()
					    {
						    this->axis.clear();
						    this->lines.clear();
					    }


				    // PLOT DATA

		      protected:

				    //! Set the axis data
				    void set_axis(const std::vector<double>& axis_points);

				    //! Add a line to the plot
				    void add_line(const time_data_line<number>& line);


				    // GENERATE PLOT

		      protected:

				    //! Make plot
				    void make_plot(typename data_manager<number>::datapipe& pipe) const;

          protected:

            //! Merge lines
            void merge_lines(typename data_manager<number>::datapipe& pipe, std::deque<double>& axis, std::vector<output_line>& data);


		        // GET AND SET BASIC PLOT DATA

		      public:

		        //! get logarithmic x-axis setting
		        bool get_log_x() const { return(this->log_x); }
		        //! set logarithmic x-axis setting
		        void set_log_x(bool g) { this->log_x = g; }

		        //! get logarithmic y-axis setting
		        bool get_log_y() const { return(this->log_y); }
		        //! set logarithmic y-axis setting
		        void set_log_y(bool g) { this->log_y = g; }

		        //! get reverse x-axis setting
		        bool get_reverse_x() const { return(this->reverse_x); }
		        //! set reverse x-axis setting
		        void set_reverse_x(bool g) { this->reverse_x = g; }

		        //! get reverse y-axis setting
		        bool get_reverse_y() const { return(this->reverse_y); }
		        //! set reverse y-axis setting
		        void set_reverse_y(bool g) { this->reverse_y = g; }

		        //! get abs-y-axis setting
		        bool get_abs_y() const { return(this->abs_y); }
		        //! set abs-y-axis setting
		        void set_abs_y(bool g) { this->abs_y = g; }

		        //! get default LaTeX labels setting
		        bool get_use_LaTeX() const { return(this->use_LaTeX); }
		        //! set default LaTeX labels setting
		        void set_use_LaTeX(bool g) { this->use_LaTeX = g; }

		        //! get use x-axis label setting
		        bool get_x_label() const { return(this->x_label); }
		        //! set use x-axis label setting
		        void set_x_label(bool g) { this->x_label = g; }
		        //! get x-axis label text
		        const std::string& get_x_label_text() const { return(this->x_label_text); }
		        //! set x-axis label text
		        void set_x_label_text(const std::string& text)
			        {
		            if(text.empty())  // assume intended to suppress x-axis label
			            {
		                this->x_label_text.clear();
		                this->x_label = false;
			            }
		            else
			            {
		                this->x_label_text = text;
		                this->x_label = true;
			            }
			        }
		        //! clear x-axis label text
		        void clear_x_label_text() { this->x_label_text.clear(); }

		        //! get use y-axis label setting
		        bool get_y_label() const { return(this->y_label); }
		        //! set use y-axis label setting
		        void set_y_label(bool g) { this->y_label = g; }
		        //! get y-axis label text
		        const std::string& get_y_label_text() const { return(this->y_label_text); }
		        //! set y-axis label text
		        void set_y_label_text(const std::string& text)
			        {
		            if(text.empty())  // assume intended to suppress y-axis label
			            {
		                this->y_label_text.clear();
		                this->y_label = false;
			            }
		            else
			            {
		                this->y_label_text = text;
		                this->y_label = true;
			            }
			        }
		        //! clear y-axis label text
		        void clear_y_label_text() { this->y_label_text.clear(); }

		        //! get use title setting
		        bool get_title() const { return(this->title); }
		        //! set use title setting
		        void set_title(bool g) { this->title = g; }
		        //! get title text
		        const std::string& get_title_text() const { return(this->title_text); }
		        //! set title text
		        void set_title_text(const std::string& text)
			        {
		            if(text.empty())    // assume intended to suppress the title
			            {
		                this->title_text.clear();
		                this->title = false;
			            }
		            else
			            {
		                this->title_text = text;
		                this->title = true;
			            }
			        }
				    //! clear title text
				    void clear_title_text() { this->title_text.clear(); }

				    //! get legend setting
				    bool get_legend() const { return(this->legend); }
				    //! set legend setting
				    void set_legend(bool g) { this->legend = g; }

				    //! get legend position
				    legend_pos get_legend_position() const { return(this->position); }
				    //! set legend position
				    void set_legend_position(legend_pos pos) { this->position = pos; }

				    //! get typeset with LaTeX setting
				    bool get_typeset_with_LaTeX() const { return(this->typeset_with_LaTeX); }
				    //! set typeset with LaTeX setting
				    void set_typeset_with_LaTeX(bool g) { this->typeset_with_LaTeX = g; }

				    // SERIALIZATION -- implements a 'serializable' interface

		      public:

				    virtual void serialize(serialization_writer& writer) const override;


				    // WRITE SELF TO A STANDARD STREAM

		      public:

				    void write(std::ostream& out);


		        // INTERNAL DATA

		      protected:

				    // PLOT DATA

		        //! List of axis values to be used for plotting.
		        std::vector<double> axis;

		        //! List of time_2dline objects to be plotted on the graph.
		        std::list< time_data_line<number> > lines;

				    // STYLE ATTRIBUTES

		        //! logarithmic x-axis?
		        bool log_x;

		        //! logarithmic y-axis?
		        bool log_y;

				    //! reverse x-axis?
				    bool reverse_x;

				    //! reverse y-axis?
				    bool reverse_y;

		        //! take absolute value on y-axis? (mostly useful with log-y plots)
		        bool abs_y;

		        //! use LaTeX default labels
		        bool use_LaTeX;

		        //! generate an x-axis label?
		        bool x_label;
		        std::string x_label_text;

		        //! generate a y-axis label?
		        bool y_label;
		        std::string y_label_text;

		        //! generate a title?
		        bool title;
		        std::string title_text;

		        //! add a legend?
		        bool legend;

				    //! location of legend
				    legend_pos position;

		        //! use LaTeX to typeset labels?
		        bool typeset_with_LaTeX;

			    };


				template <typename number>
		    plot2d_product::plot2d_product(const std::string& name, serialization_reader* reader, typename repository<number>::task_finder finder)
			    : derived_product<number>(name, reader, finder)
			    {
		        // extract data from reader;
		        assert(reader != nullptr);

		        reader->read_value(__CPP_TRANSPORT_NODE_PRODUCT_PLOT2D_LOGX, log_x);
		        reader->read_value(__CPP_TRANSPORT_NODE_PRODUCT_PLOT2D_LOGY, log_y);
		        reader->read_value(__CPP_TRANSPORT_NODE_PRODUCT_PLOT2D_ABSY, abs_y);
		        reader->read_value(__CPP_TRANSPORT_NODE_PRODUCT_PLOT2D_REVERSEX, reverse_x);
		        reader->read_value(__CPP_TRANSPORT_NODE_PRODUCT_PLOT2D_REVERSEY, reverse_y);
		        reader->read_value(__CPP_TRANSPORT_NODE_PRODUCT_PLOT2D_LATEX, use_LaTeX);
		        reader->read_value(__CPP_TRANSPORT_NODE_PRODUCT_PLOT2D_XLABEL, x_label);
		        reader->read_value(__CPP_TRANSPORT_NODE_PRODUCT_PLOT2D_XLABEL_TEXT, x_label_text);
		        reader->read_value(__CPP_TRANSPORT_NODE_PRODUCT_PLOT2D_YLABEL, y_label);
		        reader->read_value(__CPP_TRANSPORT_NODE_PRODUCT_PLOT2D_YLABEL_TEXT, y_label_text);
		        reader->read_value(__CPP_TRANSPORT_NODE_PRODUCT_PLOT2D_TITLE, title);
		        reader->read_value(__CPP_TRANSPORT_NODE_PRODUCT_PLOT2D_TITLE_TEXT, title_text);
		        reader->read_value(__CPP_TRANSPORT_NODE_PRODUCT_PLOT2D_LEGEND, legend);

		        std::string leg_pos;
		        reader->read_value(__CPP_TRANSPORT_NODE_PRODUCT_PLOT2D_LEGEND_POS, leg_pos);

		        if     (leg_pos == __CPP_TRANSPORT_NODE_PRODUCT_PLOT2D_LEGEND_TL) position = top_left;
		        else if(leg_pos == __CPP_TRANSPORT_NODE_PRODUCT_PLOT2D_LEGEND_TR) position = top_right;
		        else if(leg_pos == __CPP_TRANSPORT_NODE_PRODUCT_PLOT2D_LEGEND_BL) position = bottom_left;
		        else if(leg_pos == __CPP_TRANSPORT_NODE_PRODUCT_PLOT2D_LEGEND_BR) position = bottom_right;
		        else if(leg_pos == __CPP_TRANSPORT_NODE_PRODUCT_PLOT2D_LEGEND_R)  position = right;
		        else if(leg_pos == __CPP_TRANSPORT_NODE_PRODUCT_PLOT2D_LEGEND_CR) position = centre_left;
		        else if(leg_pos == __CPP_TRANSPORT_NODE_PRODUCT_PLOT2D_LEGEND_CL) position = centre_right;
		        else if(leg_pos == __CPP_TRANSPORT_NODE_PRODUCT_PLOT2D_LEGEND_UC) position = lower_centre;
		        else if(leg_pos == __CPP_TRANSPORT_NODE_PRODUCT_PLOT2D_LEGEND_LC) position = upper_centre;
		        else if(leg_pos == __CPP_TRANSPORT_NODE_PRODUCT_PLOT2D_LEGEND_C)  position = centre;
		        else
			        {
		            std::ostringstream msg;
		            msg << __CPP_TRANSPORT_PRODUCT_PLOT2D_UNKNOWN_LEG_POS << " '" << leg_pos << "'";
		            throw runtime_exception(runtime_exception::SERIALIZATION_ERROR, msg.str());
			        }

		        reader->read_value(__CPP_TRANSPORT_NODE_PRODUCT_PLOT2D_TYPESET_LATEX, typeset_with_LaTeX);
			    }


		    template <typename number>
				void plot2d_product<number>::set_axis(const std::vector<double>& axis_points)
					{
						// if there are already lines on the plot, ensure that the number of points is compatible
						if(this->lines.size() > 0)
							{
								// only need to check front object; they are all guaranteed to have the same size
								if(this->lines.front().get_sample_points() != axis_points.size())
									throw runtime_exception(runtime_exception::DERIVED_PRODUCT_ERROR, __CPP_TRANSPORT_PRODUCT_PLOT2D_INCOMPATIBLE_AXIS_SIZE);
							}

						// if we are logging the x-axis, check all points are strictly positive
						bool ok = true;
						if(this->log_x)
							{
								for(std::vector<double>::const_iterator t = axis_points.begin(); ok && t != axis_points.end(); t++)
									{
										if((*t) <= 0.0) ok = false;
									}
							}
						if(!ok) throw runtime_exception(runtime_exception::DERIVED_PRODUCT_ERROR, __CPP_TRANSPORT_PRODUCT_PLOT2D_NEGATIVE_AXIS_POINT);

						this->axis = axis_points;
					}


				template <typename number>
				void plot2d_product<number>::add_line(const time_data_line<number>& line)
					{
//						// check that an axis has already been set
//						if(this->axis.size() == 0)
//							throw runtime_exception(runtime_exception::DERIVED_PRODUCT_ERROR, __CPP_TRANSPORT_PRODUCT_PLOT2D_ADD_LINE_UNSET_AXIS);
//
//						// check that the number of points in this line is compatible with the axis
//						if(line.get_sample_points() != this->axis.size())
//							throw runtime_exception(runtime_exception::DERIVED_PRODUCT_ERROR, __CPP_TRANSPORT_PRODUCT_PLOT2D_ADD_LINE_INCOMPATIBLE_SIZE);
//
						this->lines.push_back(line);
					}


				template <typename number>
				void plot2d_product<number>::make_plot(typename data_manager<number>::datapipe& pipe) const
					{
				    // ensure that the supplied pipe is attached to a data container
				    if(!pipe.validate()) throw runtime_exception(runtime_exception::DATAPIPE_ERROR, __CPP_TRANSPORT_PRODUCT_PLOT2D_NULL_DATAPIPE);

						// extract paths from the datapipe and output group.
						// note that paths from the datapipe are absolute.
            boost::filesystem::path temp_root = pipe.get_temporary_files_path();

						// obtain path for Python script output
				    boost::filesystem::path script_file = temp_root / this->filename;
						script_file.replace_extension(".py");

						// obtain path for plot output
				    boost::filesystem::path plot_file = temp_root / this->filename;

				    std::ofstream out;
						out.open(script_file.string().c_str(), std::ios_base::trunc | std::ios_base::out);

						out << "import matplotlib.pyplot as plt" << std::endl;

						if(this->typeset_with_LaTeX) out << "plt.rc('text', usetex=True)" << std::endl;

						out << "plt.figure()" << std::endl;

						if(this->log_x) out << "plt.xscale('log')" << std::endl;
						if(this->log_y) out << "plt.yscale('log')" << std::endl;

            std::deque<double> axis;
            std::vector<output_line> data;
            this->merge_lines(pipe, axis, data);

            out << "x = [ ";
            for(std::deque<std::vector>::const_iterator t = axis.begin(); t != axis.end(); t++)
              {
                out << (t != axis.begin() ? ", " : "") << *t;
              }
            out << " ]" << std::endl;

            for(unsigned int i = 0; i < data.size(); i++)
              {
                out << "y" << i << " = [ ";

                std::deque<output_value>& line_data = data[i].get_values();
                assert(line_data.size() == axis.size());

                for(unsigned int j = 0; j < line_data.size(); j++)
                  {
                    out << (j > 0 ? ", " : "");
                    line_data[j].format_python(out);
                  }
                out << " ]" << std::endl;

                out << "plt.errorbar(x, y" << i << ", label=r'" << data[i].get_label() << "')" << std::endl;
              }

				    out << "ax = plt.gca()" << std::endl;

				    if(this->reverse_x) out << "ax.set_xlim(ax.get_xlim()[::-1])";
				    if(this->reverse_y) out << "ax.set_ylim(ax.get_ylim()[::-1])";

				    if(this->legend)
					    {
				        out << "handles, labels = ax.get_legend_handles_labels()" << std::endl;

				        out << "y_labels = [ ";
                for(unsigned int i = 0; i < data.size(); i++)
                  {
                    out << (i > 0 ? ", " : "") << "r'" << data[i].get_label() << "'";
                  }
				        out << " ]" << std::endl;

				        out << "plt.legend(handles, y_labels, frameon=False, loc=";
					      switch(this->position)
						      {
					          case top_right:    out << "1"; break;
					          case top_left:     out << "2"; break;
					          case bottom_left:  out << "3"; break;
					          case bottom_right: out << "4"; break;
					          case right:        out << "5"; break;
					          case centre_left:  out << "6"; break;
					          case centre_right: out << "7"; break;
					          case upper_centre: out << "8"; break;
					          case lower_centre: out << "9"; break;
					          case centre:       out << "10"; break;
					          default:
						          assert(false);
						          throw runtime_exception(runtime_exception::SERIALIZATION_ERROR, __CPP_TRANSPORT_PRODUCT_PLOT2D_INVALID_LEGEND_POSITION);
						      }
				        out << ")" << std::endl;
					    }

				    if(this->x_label) out << "plt.xlabel(r'" << this->x_label_text << "')" << std::endl;
				    if(this->y_label) out << "plt.ylabel(r'" << this->y_label_text << "')" << std::endl;
				    if(this->title)   out << "plt.title(r'"  << this->title_text   << "')" << std::endl;

				    out << "plt.savefig('" << plot_file.string() << "')" << std::endl;
				    out << "plt.close()" << std::endl;

				    out.close();
//				    system(("source ~/.profile; /opt/local/bin/python2.7 " + script_file.string()).c_str());
					}


        template <typename number>
        void plot2d_product<number>::merge_lines(typename data_manager<number>::datapipe& pipe, std::deque<double>& axis, std::vector<output_line>& data)
          {
            // step through our plot lines, merging axis data and excluding any lines which are unplottable

            // FIRST, build a list of plottable lines in 'data' (and work out whether we need to take the absolute value)
            data.clear();

            std::vector< std::vector<double> > axis_data;
            std::vector< std::vector<number> > plot_data;
            std::vector< bool >                data_absy;

            for(typename std::list< time_data_line<number> >::const_iterator t = this->lines.begin(); t != this->lines.end(); t++)
              {
                const std::vector<number>& line_data = (*t).get_data_points();

                bool need_abs_y = false;
                bool nonzero_values = false;

                if(this->log_y)
                  {
                    for(typename std::vector<number>::const_iterator u = line_data.begin(); (!need_abs_y || !nonzero_values) && u != line_data.end(); u++)
                      {
                        if((*u) <= 0.0) need_abs_y = true;
                        if((*u) > 0.0 || (*u) < 0.0) nonzero_values = true;
                      }

                    // warn if absolute values are needed, but suppress warning if the line won't plot - would be confusing
                    if(need_abs_y && !this->abs_y && nonzero_values)
                      BOOST_LOG_SEV(pipe.get_log(), data_manager<number>::normal) << ":: Warning: data line '" << (*t).get_label() << "' contains negative or zero values; plotting absolute values instead because of logarithmic y-axis";
                  }

                // we can't plot the line if it is logarithmic but has no nonzero values, so check
                if(!this->log_y || (this->log_y && nonzero_values))   // can plot the line
                  {
                    data.push_back(output_line((*t).get_label()));
                    data_absy.push_back(this->abs_y || need_abs_y);

                    axis_data.push_back((*t).get_axis_points());
                    plot_data.push_back((*t).get_data_points());
                  }
                else    // can't plot the line
                  BOOST_LOG_SEV(pipe.get_log(), data_manager<number>::normal) << ":: Warning: data line '" << (*t).get_label() << "' contains no nonzero values and can't be plotted on a logarithmic axis -- skipping this line";
              }

            // SECOND work through each axis, populating the single merged axis
            axis.clear();
            bool finished = false;

            while(!finished)
              {
                finished = true;

                // any work left to do?
                for(unsigned int i = 0; finished && i < data.size(); i++)
                  {
                    if(axis_data[i].size() > 0) finished = false;
                  }

                if(!finished)
                  {
                    // find next point to add to merged x-axis (we work from the far right because std::vector can only pop from the end)
                    double next_axis_point = DBL_MIN;
                    for(unsigned int i = 0; i < data.size(); i++)
                      {
                        if(axis_data[i].size() > 0)
                          {
                            if(axis_data[i].back() > next_axis_point) next_axis_point = axis_data[i].back();
                          }
                      }

                    // push point to merged axis
                    axis.push_front(next_axis_point);

                    // find data points on each line, if they exist, corresponding to this axis point
                    for(unsigned int i = 0; i < data.size(); i++)
                      {
                        if(axis_data[i].size() > 0)
                          {
                            if(axis_data[i].back() == next_axis_point)   // yes, this line has a match
                              {
                                data[i].push_front(output_value(data_absy[i] ? fabs(plot_data[i].back()) : plot_data[i].back()));

                                // remove point from this line
                                axis_data[i].pop_back();
                                plot_data[i].pop_back();
                              }
                            else
                              {
                                data[i].push_front(output_value());
                              }
                          }
                        else  // no match, add an empty component
                          {
                            data[i].push_front(output_value());
                          }
                      }
                  }
              }
          }


		    template <typename number>
		    void plot2d_product<number>::serialize(serialization_writer& writer) const
			    {
		        this->write_value_node(writer, __CPP_TRANSPORT_NODE_PRODUCT_PLOT2D_LOGX, this->log_x);
		        this->write_value_node(writer, __CPP_TRANSPORT_NODE_PRODUCT_PLOT2D_LOGY, this->log_y);
		        this->write_value_node(writer, __CPP_TRANSPORT_NODE_PRODUCT_PLOT2D_ABSY, this->abs_y);
		        this->write_value_node(writer, __CPP_TRANSPORT_NODE_PRODUCT_PLOT2D_REVERSEX, this->reverse_x);
		        this->write_value_node(writer, __CPP_TRANSPORT_NODE_PRODUCT_PLOT2D_REVERSEY, this->reverse_y);
		        this->write_value_node(writer, __CPP_TRANSPORT_NODE_PRODUCT_PLOT2D_LATEX, this->use_LaTeX);
		        this->write_value_node(writer, __CPP_TRANSPORT_NODE_PRODUCT_PLOT2D_XLABEL, this->x_label);
		        this->write_value_node(writer, __CPP_TRANSPORT_NODE_PRODUCT_PLOT2D_XLABEL_TEXT, this->x_label_text);
		        this->write_value_node(writer, __CPP_TRANSPORT_NODE_PRODUCT_PLOT2D_YLABEL, this->y_label);
		        this->write_value_node(writer, __CPP_TRANSPORT_NODE_PRODUCT_PLOT2D_YLABEL_TEXT, this->y_label_text);
		        this->write_value_node(writer, __CPP_TRANSPORT_NODE_PRODUCT_PLOT2D_TITLE, this->title);
		        this->write_value_node(writer, __CPP_TRANSPORT_NODE_PRODUCT_PLOT2D_TITLE_TEXT, this->title_text);
		        this->write_value_node(writer, __CPP_TRANSPORT_NODE_PRODUCT_PLOT2D_LEGEND, this->legend);

		        switch(this->position)
			        {
		            case top_left:
			            this->write_value_node(writer, __CPP_TRANSPORT_NODE_PRODUCT_PLOT2D_LEGEND_POS, std::string(__CPP_TRANSPORT_NODE_PRODUCT_PLOT2D_LEGEND_TL));
		              break;

		            case top_right:
			            this->write_value_node(writer, __CPP_TRANSPORT_NODE_PRODUCT_PLOT2D_LEGEND_POS, std::string(__CPP_TRANSPORT_NODE_PRODUCT_PLOT2D_LEGEND_TR));
		              break;

		            case bottom_left:
			            this->write_value_node(writer, __CPP_TRANSPORT_NODE_PRODUCT_PLOT2D_LEGEND_POS, std::string(__CPP_TRANSPORT_NODE_PRODUCT_PLOT2D_LEGEND_BL));
		              break;

		            case bottom_right:
			            this->write_value_node(writer, __CPP_TRANSPORT_NODE_PRODUCT_PLOT2D_LEGEND_POS, std::string(__CPP_TRANSPORT_NODE_PRODUCT_PLOT2D_LEGEND_BR));
		              break;

		            case right:
			            this->write_value_node(writer, __CPP_TRANSPORT_NODE_PRODUCT_PLOT2D_LEGEND_POS, std::string(__CPP_TRANSPORT_NODE_PRODUCT_PLOT2D_LEGEND_R));
		              break;

		            case centre_right:
			            this->write_value_node(writer, __CPP_TRANSPORT_NODE_PRODUCT_PLOT2D_LEGEND_POS, std::string(__CPP_TRANSPORT_NODE_PRODUCT_PLOT2D_LEGEND_CR));
		              break;

		            case centre_left:
			            this->write_value_node(writer, __CPP_TRANSPORT_NODE_PRODUCT_PLOT2D_LEGEND_POS, std::string(__CPP_TRANSPORT_NODE_PRODUCT_PLOT2D_LEGEND_CL));
		              break;

		            case upper_centre:
			            this->write_value_node(writer, __CPP_TRANSPORT_NODE_PRODUCT_PLOT2D_LEGEND_POS, std::string(__CPP_TRANSPORT_NODE_PRODUCT_PLOT2D_LEGEND_UC));
		              break;

		            case lower_centre:
			            this->write_value_node(writer, __CPP_TRANSPORT_NODE_PRODUCT_PLOT2D_LEGEND_POS, std::string(__CPP_TRANSPORT_NODE_PRODUCT_PLOT2D_LEGEND_LC));
		              break;

		            case centre:
			            this->write_value_node(writer, __CPP_TRANSPORT_NODE_PRODUCT_PLOT2D_LEGEND_POS, std::string(__CPP_TRANSPORT_NODE_PRODUCT_PLOT2D_LEGEND_C));
		              break;

		            default:
			            assert(false);
			            throw runtime_exception(runtime_exception::SERIALIZATION_ERROR, __CPP_TRANSPORT_PRODUCT_PLOT2D_INVALID_LEGEND_POSITION);
			        }

		        this->write_value_node(writer, __CPP_TRANSPORT_NODE_PRODUCT_PLOT2D_TYPESET_LATEX, this->typeset_with_LaTeX);

		        this->derived_product<number>::serialize(writer);
			    }


		    template <typename number>
		    void plot2d_product<number>::write(std::ostream& out)
			    {
		        unsigned int count = 0;

		        this->wrapper.wrap_list_item(out, this->log_x, __CPP_TRANSPORT_PRODUCT_PLOT2D_LABEL_LOGX, count);
		        this->wrapper.wrap_list_item(out, this->log_y, __CPP_TRANSPORT_PRODUCT_PLOT2D_LABEL_LOGY, count);
		        this->wrapper.wrap_list_item(out, this->abs_y, __CPP_TRANSPORT_PRODUCT_PLOT2D_LABEL_ABSY, count);
		        this->wrapper.wrap_list_item(out, this->reverse_x, __CPP_TRANSPORT_PRODUCT_PLOT2D_LABEL_REVERSEX, count);
		        this->wrapper.wrap_list_item(out, this->reverse_y, __CPP_TRANSPORT_PRODUCT_PLOT2D_LABEL_REVERSEY, count);
		        this->wrapper.wrap_list_item(out, this->use_LaTeX, __CPP_TRANSPORT_PRODUCT_PLOT2D_LABEL_LATEX, count);
		        this->wrapper.wrap_list_item(out, this->x_label, __CPP_TRANSPORT_PRODUCT_PLOT2D_LABEL_XLABEL, count);
		        if(this->x_label)
			        this->wrapper.wrap_value(out, this->x_label_text, __CPP_TRANSPORT_PRODUCT_PLOT2D_LABEL_LABEL, count);
		        this->wrapper.wrap_list_item(out, this->y_label, __CPP_TRANSPORT_PRODUCT_PLOT2D_LABEL_YLABEL, count);
		        if(this->y_label)
			        this->wrapper.wrap_value(out, this->y_label_text, __CPP_TRANSPORT_PRODUCT_PLOT2D_LABEL_LABEL, count);
		        this->wrapper.wrap_list_item(out, this->title, __CPP_TRANSPORT_PRODUCT_PLOT2D_LABEL_TITLE, count);
		        if(this->title)
			        this->wrapper.wrap_value(out, this->title_text, __CPP_TRANSPORT_PRODUCT_PLOT2D_LABEL_LABEL, count);
		        this->wrapper.wrap_list_item(out, this->legend, __CPP_TRANSPORT_PRODUCT_PLOT2D_LABEL_LEGEND, count);

		        if(this->legend)
			        {
		            if(count > 0) out << ", ";
		            switch(this->position)
			            {
		                case top_left:
			                out << __CPP_TRANSPORT_NODE_PRODUCT_PLOT2D_LEGEND_TL;
		                  break;
		                case top_right:
			                out << __CPP_TRANSPORT_NODE_PRODUCT_PLOT2D_LEGEND_TR;
		                  break;
		                case bottom_left:
			                out << __CPP_TRANSPORT_NODE_PRODUCT_PLOT2D_LEGEND_BL;
		                  break;
		                case bottom_right:
			                out << __CPP_TRANSPORT_NODE_PRODUCT_PLOT2D_LEGEND_BR;
		                  break;
		                case right:
			                out << __CPP_TRANSPORT_NODE_PRODUCT_PLOT2D_LEGEND_R;
		                  break;
		                case centre_right:
			                out << __CPP_TRANSPORT_NODE_PRODUCT_PLOT2D_LEGEND_CR;
		                  break;
		                case centre_left:
			                out << __CPP_TRANSPORT_NODE_PRODUCT_PLOT2D_LEGEND_CL;
		                  break;
		                case upper_centre:
			                out << __CPP_TRANSPORT_NODE_PRODUCT_PLOT2D_LEGEND_LC;
		                  break;
		                case lower_centre:
			                out << __CPP_TRANSPORT_NODE_PRODUCT_PLOT2D_LEGEND_UC;
		                  break;
		                case centre:
			                out << __CPP_TRANSPORT_NODE_PRODUCT_PLOT2D_LEGEND_C;
		                  break;
		                default:
			                assert(false);
			            }
			        }
			    }


        template <typename number>
        void plot2d_product<number>::output_value::format_python(std::ostream& out) const
          {
            switch(this->exists)
              {
                case true:
                  out << this->value;
                  break;

                case false:
                  out << "np.nan";
                  break;
              }
          }

			}   // namespace derived_data

	}   // namespace transport


#endif //__plot_product_H_
