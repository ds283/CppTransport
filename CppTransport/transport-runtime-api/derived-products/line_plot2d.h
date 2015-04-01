//
// Created by David Seery on 13/05/2014.
// Copyright (c) 2014-15 University of Sussex. All rights reserved.
//


#ifndef __CPP_TRANSPORT_PRODUCT_LINE_PLOT2D_H_
#define __CPP_TRANSPORT_PRODUCT_LINE_PLOT2D_H_


#include <iostream>
#include <fstream>
#include <functional>

#include <boost/algorithm/string.hpp>

#include "transport-runtime-api/derived-products/line_collection.h"
#include "transport-runtime-api/derived-products/data_line.h"

#include "transport-runtime-api/messages.h"
#include "transport-runtime-api/exceptions.h"

#include "boost/filesystem/operations.hpp"


#define __CPP_TRANSPORT_NODE_PRODUCT_LINE_PLOT2D_REVERSEX      "reverse-x"
#define __CPP_TRANSPORT_NODE_PRODUCT_LINE_PLOT2D_REVERSEY      "reverse-y"
#define __CPP_TRANSPORT_NODE_PRODUCT_LINE_PLOT2D_XLABEL        "x-label"
#define __CPP_TRANSPORT_NODE_PRODUCT_LINE_PLOT2D_XLABEL_TEXT   "x-label-text"
#define __CPP_TRANSPORT_NODE_PRODUCT_LINE_PLOT2D_YLABEL        "y-label"
#define __CPP_TRANSPORT_NODE_PRODUCT_LINE_PLOT2D_YLABEL_TEXT   "y-label-text"
#define __CPP_TRANSPORT_NODE_PRODUCT_LINE_PLOT2D_TITLE         "title"
#define __CPP_TRANSPORT_NODE_PRODUCT_LINE_PLOT2D_TITLE_TEXT    "title-text"
#define __CPP_TRANSPORT_NODE_PRODUCT_LINE_PLOT2D_LEGEND        "legend"
#define __CPP_TRANSPORT_NODE_PRODUCT_LINE_PLOT2D_LEGEND_POS    "legend-position"
#define __CPP_TRANSPORT_NODE_PRODUCT_LINE_PLOT2D_DASH_SECOND   "dash-second-axis"
#define __CPP_TRANSPORT_NODE_PRODUCT_LINE_PLOT2D_LEGEND_TR     "top-right"
#define __CPP_TRANSPORT_NODE_PRODUCT_LINE_PLOT2D_LEGEND_BR     "bottom-right"
#define __CPP_TRANSPORT_NODE_PRODUCT_LINE_PLOT2D_LEGEND_BL     "bottom-left"
#define __CPP_TRANSPORT_NODE_PRODUCT_LINE_PLOT2D_LEGEND_TL     "top-left"
#define __CPP_TRANSPORT_NODE_PRODUCT_LINE_PLOT2D_LEGEND_R      "right"
#define __CPP_TRANSPORT_NODE_PRODUCT_LINE_PLOT2D_LEGEND_CR     "centre-right"
#define __CPP_TRANSPORT_NODE_PRODUCT_LINE_PLOT2D_LEGEND_CL     "centre-left"
#define __CPP_TRANSPORT_NODE_PRODUCT_LINE_PLOT2D_LEGEND_UC     "upper-centre"
#define __CPP_TRANSPORT_NODE_PRODUCT_LINE_PLOT2D_LEGEND_LC     "lower-centre"
#define __CPP_TRANSPORT_NODE_PRODUCT_LINE_PLOT2D_LEGEND_C      "centre"
#define __CPP_TRANSPORT_NODE_PRODUCT_LINE_PLOT2D_TYPESET_LATEX "typeset-with-latex"
#define __CPP_TRANSPORT_NODE_PRODUCT_LINE_PLOT2D_PYTHON_PATH   "python-path"


namespace transport
	{


		namespace derived_data
			{

        //! A line_plot2d is a specialization of a line_collection that
		    //! produces a plot of derived data lines against an axis.

		    template <typename number>
		    class line_plot2d: public line_collection<number>
			    {

		      public:

				    typedef enum { top_left, top_right, bottom_left, bottom_right,
					                 right, centre_right, centre_left,
					                 upper_centre, lower_centre, centre} legend_pos;


		        // LINE_PLOT: CONSTRUCTOR, DESTRUCTOR

		      public:

						//! Basic user-facing constructor.
				    //! A line_plot2d is an instance of a line_collection; we limit
				    //! the maximum number of value types in the collection to two,
				    //! because that's as many different y-axes as we can have on the plot.
		        line_plot2d(const std::string& name, const boost::filesystem::path& filename)
		          : line_collection<number>(name, filename, true, 2),
								reverse_x(false),
								reverse_y(false),
								x_label(false),
								y_label(false),
								title(false),
								legend(false),
								position(top_right),
								typeset_with_LaTeX(false),
								dash_second_axis(false)
			        {
								if(this->filename.extension().string() != ".pdf" &&
									 this->filename.extension().string() != ".png" &&
									 this->filename.extension().string() != ".ps" &&
									 this->filename.extension().string() != ".eps" &&
									 this->filename.extension().string() != ".svg" &&
									 this->filename.extension().string() != ".svgz")
									{
								    std::ostringstream msg;
										msg << __CPP_TRANSPORT_PRODUCT_LINE_PLOT2D_UNSUPPORTED_FORMAT << " " << filename.extension();
										throw runtime_exception(runtime_exception::DERIVED_PRODUCT_ERROR, msg.str());
									}

								// find location of Python interpreter
                this->set_python_path();
			        }

				    //! Deserialization constructor
		        line_plot2d(const std::string& name, Json::Value& reader, typename repository_finder<number>::task_finder finder);

		        virtual ~line_plot2d() = default;


				    // GENERATE PLOT -- implements a 'derived_product' interface

		      public:

				    //! Generate our derived output
				    virtual void derive(datapipe<number>& pipe, const std::list<std::string>& tags);

		      protected:

				    //! Bin lines according to their value type
				    void bin_lines(std::vector< typename line_collection<number>::output_line >& input,
				                   std::vector< std::vector< typename line_collection<number>::output_line > >& output,
				                   std::vector< value_type >& bin_types);

				    //! Make plot
				    bool make_plot(datapipe<number>& pipe, const std::deque<double>& axis,
				                   const typename std::vector< std::vector< typename line_collection<number>::output_line > >& data_bins,
				                   const std::vector< value_type >& bin_types) const;


		        // GET AND SET BASIC PLOT ATTRIBUTES

		      public:

		        //! get reverse x-axis setting
		        bool get_reverse_x() const { return(this->reverse_x); }
		        //! set reverse x-axis setting
		        void set_reverse_x(bool g) { this->reverse_x = g; }

		        //! get reverse y-axis setting
		        bool get_reverse_y() const { return(this->reverse_y); }
		        //! set reverse y-axis setting
		        void set_reverse_y(bool g) { this->reverse_y = g; }

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

				    //! get dash second axis
				    bool get_dash_second_axis() const { return(this->dash_second_axis); }
				    //! set dash second axis
				    void set_dash_second_axis(bool g) { this->dash_second_axis = g; }


            // GET AND SET RUNTIME ENVIRONMENT

          public:

            //! get path to Python interpreter
            const boost::filesystem::path& get_python_interpreter() const { return(this->python_path); }

            //! set path to Python interpreter
            void set_python_interpreter(const boost::filesystem::path& p);

          protected:

            //! auto-detect path to Python interpreter
            void set_python_path();


				    // CLONE

		      public:

				    //! Copy this object
				    virtual derived_product<number>* clone() const { return new line_plot2d<number>(static_cast<const line_plot2d<number>&>(*this)); }

				    // SERIALIZATION -- implements a 'serializable' interface

		      public:

				    virtual void serialize(Json::Value& writer) const override;


				    // WRITE SELF TO A STANDARD STREAM

		      public:

				    void write(std::ostream& out);


		        // INTERNAL DATA

		      protected:

				    // STYLE ATTRIBUTES

				    //! reverse x-axis?
				    bool reverse_x;

				    //! reverse y-axis?
				    bool reverse_y;

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

				    //! uses dashes for lines plotted on second y-axis?
				    bool dash_second_axis;


            // OTHER DATA

            //! path to Python interpreter
            boost::filesystem::path python_path;

			    };


				template <typename number>
		    line_plot2d<number>::line_plot2d(const std::string& name, Json::Value& reader, typename repository_finder<number>::task_finder finder)
			    : line_collection<number>(name, reader, finder)
			    {
				    reverse_x        = reader[__CPP_TRANSPORT_NODE_PRODUCT_LINE_PLOT2D_REVERSEX].asBool();
				    reverse_y        = reader[__CPP_TRANSPORT_NODE_PRODUCT_LINE_PLOT2D_REVERSEY].asBool();
				    x_label          = reader[__CPP_TRANSPORT_NODE_PRODUCT_LINE_PLOT2D_XLABEL].asBool();
				    x_label_text     = reader[__CPP_TRANSPORT_NODE_PRODUCT_LINE_PLOT2D_XLABEL_TEXT].asString();
				    y_label          = reader[__CPP_TRANSPORT_NODE_PRODUCT_LINE_PLOT2D_YLABEL].asBool();
				    y_label_text     = reader[__CPP_TRANSPORT_NODE_PRODUCT_LINE_PLOT2D_YLABEL_TEXT].asString();
				    title            = reader[__CPP_TRANSPORT_NODE_PRODUCT_LINE_PLOT2D_TITLE].asBool();
				    title_text       = reader[__CPP_TRANSPORT_NODE_PRODUCT_LINE_PLOT2D_TITLE_TEXT].asString();
				    legend           = reader[__CPP_TRANSPORT_NODE_PRODUCT_LINE_PLOT2D_LEGEND].asBool();
				    dash_second_axis = reader[__CPP_TRANSPORT_NODE_PRODUCT_LINE_PLOT2D_DASH_SECOND].asBool();

		        std::string leg_pos = reader[__CPP_TRANSPORT_NODE_PRODUCT_LINE_PLOT2D_LEGEND_POS].asString();

		        if     (leg_pos == __CPP_TRANSPORT_NODE_PRODUCT_LINE_PLOT2D_LEGEND_TL) position = top_left;
		        else if(leg_pos == __CPP_TRANSPORT_NODE_PRODUCT_LINE_PLOT2D_LEGEND_TR) position = top_right;
		        else if(leg_pos == __CPP_TRANSPORT_NODE_PRODUCT_LINE_PLOT2D_LEGEND_BL) position = bottom_left;
		        else if(leg_pos == __CPP_TRANSPORT_NODE_PRODUCT_LINE_PLOT2D_LEGEND_BR) position = bottom_right;
		        else if(leg_pos == __CPP_TRANSPORT_NODE_PRODUCT_LINE_PLOT2D_LEGEND_R)  position = right;
		        else if(leg_pos == __CPP_TRANSPORT_NODE_PRODUCT_LINE_PLOT2D_LEGEND_CR) position = centre_left;
		        else if(leg_pos == __CPP_TRANSPORT_NODE_PRODUCT_LINE_PLOT2D_LEGEND_CL) position = centre_right;
		        else if(leg_pos == __CPP_TRANSPORT_NODE_PRODUCT_LINE_PLOT2D_LEGEND_UC) position = lower_centre;
		        else if(leg_pos == __CPP_TRANSPORT_NODE_PRODUCT_LINE_PLOT2D_LEGEND_LC) position = upper_centre;
		        else if(leg_pos == __CPP_TRANSPORT_NODE_PRODUCT_LINE_PLOT2D_LEGEND_C)  position = centre;
		        else
			        {
		            std::ostringstream msg;
		            msg << __CPP_TRANSPORT_PRODUCT_LINE_PLOT2D_UNKNOWN_LEG_POS << " '" << leg_pos << "'";
		            throw runtime_exception(runtime_exception::SERIALIZATION_ERROR, msg.str());
			        }

		        typeset_with_LaTeX = reader[__CPP_TRANSPORT_NODE_PRODUCT_LINE_PLOT2D_TYPESET_LATEX].asBool();
            python_path = reader[__CPP_TRANSPORT_NODE_PRODUCT_LINE_PLOT2D_PYTHON_PATH].asString();
			    }


				template <typename number>
				void line_plot2d<number>::derive(datapipe<number>& pipe, const std::list<std::string>& tags)
					{
						// generate output from our constituent lines
				    std::list< data_line<number> > derived_lines;
						this->obtain_output(pipe, tags, derived_lines);

						// merge this output onto a single axis
						// this turns out collection of data_lines into a collection of output_lines
				    std::deque<double> axis;
				    typename std::vector< typename line_collection<number>::output_line > output_lines;
						this->merge_lines(pipe, derived_lines, axis, output_lines);

						// bin these lines into groups of the same value type
				    std::vector< std::vector< typename line_collection<number>::output_line > > binned_lines;
				    std::vector< value_type > bin_types;
						this->bin_lines(output_lines, binned_lines, bin_types);

						// generate plot
				    bool success = this->make_plot(pipe, axis, binned_lines, bin_types);

						// commit product (even if the plot failed to generate and we are committing the script - the next line
						// will prevent the whole output group being committed)
						pipe.commit(this);

						if(!success) throw runtime_exception(runtime_exception::DERIVED_PRODUCT_ERROR, __CPP_TRANSPORT_PRODUCT_LINE_PLOT2D_PYTHON_FAIL);
					}


		    template <typename number>
		    void line_plot2d<number>::bin_lines(std::vector< typename line_collection<number>::output_line >& input,
		                                        std::vector< std::vector< typename line_collection<number>::output_line > >& output,
		                                        std::vector< value_type >& bin_types)
			    {
						while(input.size() > 0)
							{
								// add a new bin to output
								output.resize(output.size()+1);

								// get a pointer to the new bin
						    typename std::vector< std::vector< typename line_collection<number>::output_line > >::iterator bin = --output.end();

						    typename std::vector< typename line_collection<number>::output_line >::iterator cursor = input.begin();

								// move first element of input to the new bin
								bin->push_back(*cursor);
								bin_types.push_back(cursor->get_value_type());

								value_type this_value = cursor->get_value_type();
						    cursor++;

								for(; cursor != input.end(); cursor++)
									{
										if(cursor->get_value_type() == this_value)
											{
												bin->push_back(*cursor);
											}
									}

								// now erase all elements from input
								class RemovePredicate
									{
								  public:
										RemovePredicate(value_type t)
											: value(t)
											{
											}

										bool operator()(const typename line_collection<number>::output_line& A)
											{
												return(A.get_value_type() == this->value);
											}

								  private:
										value_type value;
									};

						    input.erase(std::remove_if(input.begin(), input.end(), RemovePredicate(this_value)), input.end());
							}
			    }


				// data is a set of bins, with all lines in a bin sharing a common value type
				template <typename number>
				bool line_plot2d<number>::make_plot(datapipe<number>& pipe, const std::deque<double>& axis,
				                                    const typename std::vector< std::vector< typename line_collection<number>::output_line > >& data_bins,
																						const std::vector< value_type >& bin_types) const
					{
						// extract paths from the datapipe
            boost::filesystem::path temp_root = pipe.get_abs_tempdir_path();

						// obtain path for Python script output
				    boost::filesystem::path script_file = temp_root / this->filename;
						script_file.replace_extension(".py");

						// obtain path for plot output
				    boost::filesystem::path plot_file = temp_root / this->filename;

				    std::ofstream out;
						out.open(script_file.string().c_str(), std::ios_base::trunc | std::ios_base::out);

						if(!out.is_open() || out.fail())
					    {
						    std::ostringstream msg;
						    msg << __CPP_TRANSPORT_DERIVED_PRODUCT_FAILED << " " << plot_file;
						    throw runtime_exception(runtime_exception::DERIVED_PRODUCT_ERROR, msg.str());
					    }

				    out << "import numpy as np" << std::endl;
				    out << "import matplotlib.pyplot as plt" << std::endl;

				    if(this->typeset_with_LaTeX)
					    {
				        out << "plt.rc('text', usetex=True)" << std::endl;
				        out << "plt.rc('font',**{'family':'serif','serif':['Computer Modern Roman']})" << std::endl;
						    out << "plt.rc('font',**{'family':'sans-serif','sans-serif':['Computer Modern Sans serif']})" << std::endl;
					    }

				    out << "plt.figure()" << std::endl;

				    out << "x = [ ";
				    for(std::deque<double>::const_iterator t = axis.begin(); t != axis.end(); t++)
					    {
				        out << (t != axis.begin() ? ", " : "") << *t;
					    }
				    out << " ]" << std::endl;

						unsigned int current_id = 0;
				    std::vector< std::vector<unsigned int> > line_ids;
				    std::vector< std::vector<std::string> >  line_labels;

						// loop through all bins, writing out arrays
						// representing the values in each line, and recording
						// which arrays correspond to which bin
						for(typename std::vector< std::vector< typename line_collection<number>::output_line > >::const_iterator t = data_bins.begin(); t != data_bins.end(); t++)
							{
								line_ids.resize(line_ids.size()+1);
						    std::vector< std::vector<unsigned int> >::iterator current_id_bin = --line_ids.end();

								line_labels.resize(line_labels.size()+1);
						    std::vector< std::vector<std::string> >::iterator current_label_bin = --line_labels.end();

								// loop through all lines in this bin
								for(typename std::vector< typename line_collection<number>::output_line >::const_iterator u = t->begin(); u != t->end(); u++)
									{
									  current_id_bin->push_back(current_id);
										current_label_bin->push_back(u->get_label());

								    out << "y" << current_id << "_premask = [ ";

								    const std::deque< typename line_collection<number>::output_value >& line_data = u->get_values();
								    assert(line_data.size() == axis.size());

										for(typename std::deque< typename line_collection<number>::output_value >::const_iterator w = line_data.begin(); w != line_data.end(); w++)
											{
												out << (w > line_data.begin() ? ", " : "");
												w->format_python(out);
											}

								    out << " ]" << std::endl;

								    out << "y" << current_id << " = np.ma.array(y" << current_id << "_premask)" << std::endl;

										current_id++;
									}
							}

						// now allocate lines to axes
						unsigned int bin_count = 1;
				    std::vector< std::vector<unsigned int> >::const_iterator t = line_ids.begin();
				    std::vector< std::vector<std::string> >::const_iterator  u = line_labels.begin();
						for(; t != line_ids.end() && u != line_labels.end(); t++, u++, bin_count++)
							{
								if(bin_count == 1) out << "ax1 = plt.gca()" << std::endl;
								if(bin_count == 2) out << "ax2 = ax1.twinx()" << std::endl;

						    std::vector<unsigned int>::const_iterator tt = t->begin();
						    std::vector<std::string>::const_iterator  uu = u->begin();

								for(; tt != t->end() && uu != u->end(); tt++, uu++)
									{
								    out << "ln" << *tt << " = ax" << bin_count << ".errorbar(x, y" << *tt << ", label=r'" << *uu << "'";
										if(bin_count == 2 && this->dash_second_axis)
											{
												out << ", linestyle='--'";
											}
								    out << ")" << std::endl;
									}
							}

				    if(this->reverse_x) out << "ax1.set_xlim(ax.get_xlim()[::-1])" << std::endl;
				    if(this->reverse_y)
					    {
						    for(unsigned int i = 1; i < bin_count; i++)
							    {
						        out << "ax" << i << ".set_ylim(ax.get_ylim()[::-1])" << std::endl;
							    }
					    }
				    if(this->log_x) out << "ax1.set_xscale('log')" << std::endl;
				    if(this->log_y)
					    {
				        for(unsigned int i = 1; i < bin_count; i++)
					        {
				            out << "ax" << i << ".set_yscale('log')" << std::endl;
					        }
					    }

				    if(this->legend)
					    {
						    for(unsigned int i = 1; i < bin_count; i++)
							    {
						        out << "handles" << i << ", labels" << i << " = ax" << i << ".get_legend_handles_labels()" << std::endl;

								    out << "handles = ";
								    if(i > 1) out << "handles + ";
								    out << "handles" << i << std::endl;

								    out << "labels = ";
								    if(i > 1) out << "labels + ";
								    out << "labels" << i << std::endl;
							    }

//						        out << "y_labels = [ ";
//								    unsigned int count = 0;
//								    for(std::vector< std::vector<std::string> >::const_iterator t = line_labels.begin(); t != line_labels.end(); t++)
//									    {
//										    for(std::vector<std::string>::const_iterator u = t->begin(); u != t->end(); u++)
//											    {
//												    out << (count > 0 ? ", " : "") << "r'" << *u << "'";
//											    }
//									    }
//						        out << " ]" << std::endl;

				        out << "ax" << bin_count-1 << ".legend(handles, labels, frameon=False, loc=";
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
				            throw runtime_exception(runtime_exception::SERIALIZATION_ERROR, __CPP_TRANSPORT_PRODUCT_LINE_PLOT2D_INVALID_LEGEND_POSITION);
					        }
				        out << ")" << std::endl;
					    }

				    if(this->x_label) out << "ax1.set_xlabel(r'" << this->x_label_text << "')" << std::endl;

				    if(this->y_label)
					    {
				        // if there is user-set text, apply this to the first axis;
				        // otherwise, use default labels for each value axis
				        if(this->y_label_text.length() > 0)
					        {
				            out << "ax1.set_ylabel(r'" << this->y_label_text << "')" << std::endl;
					        }
				        else
					        {
						        unsigned int i = 1;
				            for(std::vector<value_type>::const_iterator w = bin_types.begin(); w != bin_types.end() && i < bin_count; w++, i++)
					            {
				                std::string label = this->typeset_with_LaTeX ? value_type_to_string_LaTeX(*w) : value_type_to_string_non_LaTeX(*w);
						            out << "ax" << i << ".set_ylabel(r'" << label << "')" << std::endl;
					            }
					        }
					    }

				    if(this->title)   out << "plt.title(r'"  << this->title_text   << "')" << std::endl;

				    out << "plt.savefig('" << plot_file.string() << "')" << std::endl;
				    out << "plt.close()" << std::endl;

				    out.close();

				    std::ostringstream command;
						command << "source ~/.profile; " << this->python_path.string() << " \"" << script_file.string() << "\"";
				    int rc = system(command.str().c_str());

						bool rval = true;

						// remove python script if worked ok, otherwise move script to destination and throw an exception
						if(rc == 0)
							{
						    boost::filesystem::remove(script_file);
							}
						else
							{
								if(boost::filesystem::exists(plot_file)) boost::filesystem::remove(plot_file);
						    boost::filesystem::rename(script_file, plot_file);
								rval = false;
							}

						return(rval);
					}


        template <typename number>
        void line_plot2d<number>::set_python_interpreter(const boost::filesystem::path& p)
          {
            if(!boost::filesystem::exists(p))
              {
                std::ostringstream msg;
                msg << __CPP_TRANSPORT_PRODUCT_LINE_PLOT2D_INTERPRETER_NOT_FOUND << " " << p;
                throw runtime_exception(runtime_exception::DERIVED_PRODUCT_ERROR, msg.str());
              }

            this->python_path = p;
          }


        template <typename number>
        void line_plot2d<number>::set_python_path()
          {
            FILE* f = popen("which python", "r");

            if(!f)
              {
                this->python_path = __CPP_TRANSPORT_DEFAULT_PYTHON_PATH;
              }
            else
              {
                char buffer[1024];
                char* line = fgets(buffer, sizeof(buffer), f);
                pclose(f);

                std::string path(line);
                boost::algorithm::trim_right(path);
                this->python_path = path;
              }
          }


		    template <typename number>
		    void line_plot2d<number>::serialize(Json::Value& writer) const
			    {
		        writer[__CPP_TRANSPORT_NODE_DERIVED_PRODUCT_TYPE] = std::string(__CPP_TRANSPORT_NODE_DERIVED_PRODUCT_LINE_PLOT2D);

		        writer[__CPP_TRANSPORT_NODE_PRODUCT_LINE_PLOT2D_REVERSEX]    = this->reverse_x;
		        writer[__CPP_TRANSPORT_NODE_PRODUCT_LINE_PLOT2D_REVERSEY]    = this->reverse_y;
		        writer[__CPP_TRANSPORT_NODE_PRODUCT_LINE_PLOT2D_XLABEL]      = this->x_label;
		        writer[__CPP_TRANSPORT_NODE_PRODUCT_LINE_PLOT2D_XLABEL_TEXT] = this->x_label_text;
		        writer[__CPP_TRANSPORT_NODE_PRODUCT_LINE_PLOT2D_YLABEL]      = this->y_label;
		        writer[__CPP_TRANSPORT_NODE_PRODUCT_LINE_PLOT2D_YLABEL_TEXT] = this->y_label_text;
		        writer[__CPP_TRANSPORT_NODE_PRODUCT_LINE_PLOT2D_TITLE]       = this->title;
		        writer[__CPP_TRANSPORT_NODE_PRODUCT_LINE_PLOT2D_TITLE_TEXT]  = this->title_text;
		        writer[__CPP_TRANSPORT_NODE_PRODUCT_LINE_PLOT2D_LEGEND]      = this->legend;
		        writer[__CPP_TRANSPORT_NODE_PRODUCT_LINE_PLOT2D_DASH_SECOND] = this->dash_second_axis;

		        switch(this->position)
			        {
		            case top_left:
			            writer[__CPP_TRANSPORT_NODE_PRODUCT_LINE_PLOT2D_LEGEND_POS] = std::string(__CPP_TRANSPORT_NODE_PRODUCT_LINE_PLOT2D_LEGEND_TL);
		              break;

		            case top_right:
			            writer[__CPP_TRANSPORT_NODE_PRODUCT_LINE_PLOT2D_LEGEND_POS] = std::string(__CPP_TRANSPORT_NODE_PRODUCT_LINE_PLOT2D_LEGEND_TR);
		              break;

		            case bottom_left:
			            writer[__CPP_TRANSPORT_NODE_PRODUCT_LINE_PLOT2D_LEGEND_POS] = std::string(__CPP_TRANSPORT_NODE_PRODUCT_LINE_PLOT2D_LEGEND_BL);
		              break;

		            case bottom_right:
			            writer[__CPP_TRANSPORT_NODE_PRODUCT_LINE_PLOT2D_LEGEND_POS] = std::string(__CPP_TRANSPORT_NODE_PRODUCT_LINE_PLOT2D_LEGEND_BR);
		              break;

		            case right:
			            writer[__CPP_TRANSPORT_NODE_PRODUCT_LINE_PLOT2D_LEGEND_POS] = std::string(__CPP_TRANSPORT_NODE_PRODUCT_LINE_PLOT2D_LEGEND_R);
		              break;

		            case centre_right:
			            writer[__CPP_TRANSPORT_NODE_PRODUCT_LINE_PLOT2D_LEGEND_POS] = std::string(__CPP_TRANSPORT_NODE_PRODUCT_LINE_PLOT2D_LEGEND_CR);
		              break;

		            case centre_left:
			            writer[__CPP_TRANSPORT_NODE_PRODUCT_LINE_PLOT2D_LEGEND_POS] = std::string(__CPP_TRANSPORT_NODE_PRODUCT_LINE_PLOT2D_LEGEND_CL);
		              break;

		            case upper_centre:
			            writer[__CPP_TRANSPORT_NODE_PRODUCT_LINE_PLOT2D_LEGEND_POS] = std::string(__CPP_TRANSPORT_NODE_PRODUCT_LINE_PLOT2D_LEGEND_UC);
		              break;

		            case lower_centre:
			            writer[__CPP_TRANSPORT_NODE_PRODUCT_LINE_PLOT2D_LEGEND_POS] = std::string(__CPP_TRANSPORT_NODE_PRODUCT_LINE_PLOT2D_LEGEND_LC);
		              break;

		            case centre:
			            writer[__CPP_TRANSPORT_NODE_PRODUCT_LINE_PLOT2D_LEGEND_POS] = std::string(__CPP_TRANSPORT_NODE_PRODUCT_LINE_PLOT2D_LEGEND_C);
		              break;

		            default:
			            assert(false);
		              throw runtime_exception(runtime_exception::SERIALIZATION_ERROR, __CPP_TRANSPORT_PRODUCT_LINE_PLOT2D_INVALID_LEGEND_POSITION);
			        }

		        writer[__CPP_TRANSPORT_NODE_PRODUCT_LINE_PLOT2D_TYPESET_LATEX] = this->typeset_with_LaTeX;
            writer[__CPP_TRANSPORT_NODE_PRODUCT_LINE_PLOT2D_PYTHON_PATH] = this->python_path.string();

		        // call next serialization
		        this->line_collection<number>::serialize(writer);
			    }


		    template <typename number>
		    void line_plot2d<number>::write(std::ostream& out)
			    {
		        // call next writer
		        this->line_collection<number>::write(out);

		        unsigned int count = 0;

		        this->wrapper.wrap_newline(out);
		        this->wrapper.wrap_list_item(out, this->reverse_x, __CPP_TRANSPORT_PRODUCT_LINE_PLOT2D_LABEL_REVERSEX, count);
		        this->wrapper.wrap_list_item(out, this->reverse_y, __CPP_TRANSPORT_PRODUCT_LINE_PLOT2D_LABEL_REVERSEY, count);
		        this->wrapper.wrap_list_item(out, this->x_label, __CPP_TRANSPORT_PRODUCT_LINE_PLOT2D_LABEL_XLABEL, count);
		        if(this->x_label)
			        this->wrapper.wrap_value(out, this->x_label_text, __CPP_TRANSPORT_PRODUCT_LINE_PLOT2D_LABEL_LABEL, count);
		        this->wrapper.wrap_list_item(out, this->y_label, __CPP_TRANSPORT_PRODUCT_LINE_PLOT2D_LABEL_YLABEL, count);
		        if(this->y_label)
			        this->wrapper.wrap_value(out, this->y_label_text, __CPP_TRANSPORT_PRODUCT_LINE_PLOT2D_LABEL_LABEL, count);
		        this->wrapper.wrap_list_item(out, this->title, __CPP_TRANSPORT_PRODUCT_LINE_PLOT2D_LABEL_TITLE, count);
		        if(this->title)
			        this->wrapper.wrap_value(out, this->title_text, __CPP_TRANSPORT_PRODUCT_LINE_PLOT2D_LABEL_LABEL, count);
		        this->wrapper.wrap_list_item(out, this->legend, __CPP_TRANSPORT_PRODUCT_LINE_PLOT2D_LABEL_LEGEND, count);

		        if(this->legend)
			        {
		            if(count > 0) out << ", ";
		            switch(this->position)
			            {
		                case top_left:
			                out << __CPP_TRANSPORT_NODE_PRODUCT_LINE_PLOT2D_LEGEND_TL;
		                break;
		                case top_right:
			                out << __CPP_TRANSPORT_NODE_PRODUCT_LINE_PLOT2D_LEGEND_TR;
		                break;
		                case bottom_left:
			                out << __CPP_TRANSPORT_NODE_PRODUCT_LINE_PLOT2D_LEGEND_BL;
		                break;
		                case bottom_right:
			                out << __CPP_TRANSPORT_NODE_PRODUCT_LINE_PLOT2D_LEGEND_BR;
		                break;
		                case right:
			                out << __CPP_TRANSPORT_NODE_PRODUCT_LINE_PLOT2D_LEGEND_R;
		                break;
		                case centre_right:
			                out << __CPP_TRANSPORT_NODE_PRODUCT_LINE_PLOT2D_LEGEND_CR;
		                break;
		                case centre_left:
			                out << __CPP_TRANSPORT_NODE_PRODUCT_LINE_PLOT2D_LEGEND_CL;
		                break;
		                case upper_centre:
			                out << __CPP_TRANSPORT_NODE_PRODUCT_LINE_PLOT2D_LEGEND_LC;
		                break;
		                case lower_centre:
			                out << __CPP_TRANSPORT_NODE_PRODUCT_LINE_PLOT2D_LEGEND_UC;
		                break;
		                case centre:
			                out << __CPP_TRANSPORT_NODE_PRODUCT_LINE_PLOT2D_LEGEND_C;
		                break;
		                default:
			                assert(false);
			            }
			        }
			    }


		    template <typename number>
		    std::ostream& operator<<(std::ostream& out, line_plot2d<number>& obj)
			    {
		        obj.write(out);
		        return(out);
			    }


			}   // namespace derived_data

	}   // namespace transport


#endif // __CPP_TRANSPORT_PRODUCT_LINE_PLOT2D_H_
