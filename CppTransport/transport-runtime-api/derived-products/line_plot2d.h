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

						//! Basic user-facing constructor
		        line_plot2d(const std::string& name, const boost::filesystem::path& filename)
		          : line_collection<number>(name, filename, true, 2),
								reverse_x(false), reverse_y(false),
								x_label(false), y_label(false),
								title(false), legend(false), position(top_right),
								typeset_with_LaTeX(false)
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

                this->set_python_path();
			        }

				    //! Deserialization constructor
		        line_plot2d(const std::string& name, serialization_reader* reader, typename repository<number>::task_finder finder);

		        virtual ~line_plot2d() = default;


				    // GENERATE PLOT -- implements a 'derived_product' interface

		      public:

				    //! Generate our derived output
				    virtual void derive(typename data_manager<number>::datapipe& pipe, const std::list<std::string>& tags);

		      protected:

				    //! Make plot
				    bool make_plot(typename data_manager<number>::datapipe& pipe, const std::deque<double>& axis, const typename std::vector< typename line_collection<number>::output_line >& data) const;


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

				    virtual void serialize(serialization_writer& writer) const override;


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


            // OTHER DATA

            //! path to Python interpreter
            boost::filesystem::path python_path;

			    };


				template <typename number>
		    line_plot2d<number>::line_plot2d(const std::string& name, serialization_reader* reader, typename repository<number>::task_finder finder)
			    : line_collection<number>(name, reader, finder)
			    {
		        // extract data from reader
		        assert(reader != nullptr);

		        reader->read_value(__CPP_TRANSPORT_NODE_PRODUCT_LINE_PLOT2D_REVERSEX, reverse_x);
		        reader->read_value(__CPP_TRANSPORT_NODE_PRODUCT_LINE_PLOT2D_REVERSEY, reverse_y);
		        reader->read_value(__CPP_TRANSPORT_NODE_PRODUCT_LINE_PLOT2D_XLABEL, x_label);
		        reader->read_value(__CPP_TRANSPORT_NODE_PRODUCT_LINE_PLOT2D_XLABEL_TEXT, x_label_text);
		        reader->read_value(__CPP_TRANSPORT_NODE_PRODUCT_LINE_PLOT2D_YLABEL, y_label);
		        reader->read_value(__CPP_TRANSPORT_NODE_PRODUCT_LINE_PLOT2D_YLABEL_TEXT, y_label_text);
		        reader->read_value(__CPP_TRANSPORT_NODE_PRODUCT_LINE_PLOT2D_TITLE, title);
		        reader->read_value(__CPP_TRANSPORT_NODE_PRODUCT_LINE_PLOT2D_TITLE_TEXT, title_text);
		        reader->read_value(__CPP_TRANSPORT_NODE_PRODUCT_LINE_PLOT2D_LEGEND, legend);

		        std::string leg_pos;
		        reader->read_value(__CPP_TRANSPORT_NODE_PRODUCT_LINE_PLOT2D_LEGEND_POS, leg_pos);

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

		        reader->read_value(__CPP_TRANSPORT_NODE_PRODUCT_LINE_PLOT2D_TYPESET_LATEX, typeset_with_LaTeX);

            std::string python_str;
            reader->read_value(__CPP_TRANSPORT_NODE_PRODUCT_LINE_PLOT2D_PYTHON_PATH, python_str);
            python_path = python_str;
			    }


				template <typename number>
				void line_plot2d<number>::derive(typename data_manager<number>::datapipe& pipe, const std::list<std::string>& tags)
					{
						// generate output from our constituent lines
				    std::list< data_line<number> > derived_lines;
						this->obtain_output(pipe, tags, derived_lines);

						// merge this output onto a single axis
				    std::deque<double> axis;
				    typename std::vector< typename line_collection<number>::output_line > output_lines;
						this->merge_lines(pipe, derived_lines, axis, output_lines);

						// generate plot
				    bool success = this->make_plot(pipe, axis, output_lines);

						// commit product (even if the plot failed to generate and we are committing the script - the next line
						// will prevent the whole output group being committed)
						pipe.commit(this);

						if(!success) throw runtime_exception(runtime_exception::DERIVED_PRODUCT_ERROR, __CPP_TRANSPORT_PRODUCT_LINE_PLOT2D_PYTHON_FAIL);
					}


				template <typename number>
				bool line_plot2d<number>::make_plot(typename data_manager<number>::datapipe& pipe, const std::deque<double>& axis, const typename std::vector< typename line_collection<number>::output_line >& data) const
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

						if(out.is_open())
							{
						    out << "import numpy as np" << std::endl;
						    out << "import matplotlib.pyplot as plt" << std::endl;

						    if(this->typeset_with_LaTeX) out << "plt.rc('text', usetex=True)" << std::endl;

						    out << "plt.figure()" << std::endl;

						    if(this->log_x) out << "plt.xscale('log')" << std::endl;
						    if(this->log_y) out << "plt.yscale('log')" << std::endl;

						    out << "x = [ ";
						    for(std::deque<double>::const_iterator t = axis.begin(); t != axis.end(); t++)
							    {
						        out << (t != axis.begin() ? ", " : "") << *t;
							    }
						    out << " ]" << std::endl;

						    for(unsigned int i = 0; i < data.size(); i++)
							    {
						        out << "y" << i << "_premask = [ ";

						        const std::deque< typename line_collection<number>::output_value >& line_data = data[i].get_values();
						        assert(line_data.size() == axis.size());

						        for(unsigned int j = 0; j < line_data.size(); j++)
							        {
						            out << (j > 0 ? ", " : "");
						            line_data[j].format_python(out);
							        }
						        out << " ]" << std::endl;

						        out << "y" << i << " = np.ma.array(y" << i << "_premask)" << std::endl;

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
						            throw runtime_exception(runtime_exception::SERIALIZATION_ERROR, __CPP_TRANSPORT_PRODUCT_LINE_PLOT2D_INVALID_LEGEND_POSITION);
							        }
						        out << ")" << std::endl;
							    }
							}
				    else
					    {
				        std::ostringstream msg;
				        msg << __CPP_TRANSPORT_DERIVED_PRODUCT_FAILED << " " << plot_file;
				        throw runtime_exception(runtime_exception::DERIVED_PRODUCT_ERROR, msg.str());
					    }

				    if(this->x_label) out << "plt.xlabel(r'" << this->x_label_text << "')" << std::endl;
				    if(this->y_label) out << "plt.ylabel(r'" << this->y_label_text << "')" << std::endl;
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
		    void line_plot2d<number>::serialize(serialization_writer& writer) const
			    {
		        writer.write_value(__CPP_TRANSPORT_NODE_DERIVED_PRODUCT_TYPE, std::string(__CPP_TRANSPORT_NODE_DERIVED_PRODUCT_LINE_PLOT2D));

		        writer.write_value(__CPP_TRANSPORT_NODE_PRODUCT_LINE_PLOT2D_REVERSEX, this->reverse_x);
		        writer.write_value(__CPP_TRANSPORT_NODE_PRODUCT_LINE_PLOT2D_REVERSEY, this->reverse_y);
		        writer.write_value(__CPP_TRANSPORT_NODE_PRODUCT_LINE_PLOT2D_XLABEL, this->x_label);
		        writer.write_value(__CPP_TRANSPORT_NODE_PRODUCT_LINE_PLOT2D_XLABEL_TEXT, this->x_label_text);
		        writer.write_value(__CPP_TRANSPORT_NODE_PRODUCT_LINE_PLOT2D_YLABEL, this->y_label);
		        writer.write_value(__CPP_TRANSPORT_NODE_PRODUCT_LINE_PLOT2D_YLABEL_TEXT, this->y_label_text);
		        writer.write_value(__CPP_TRANSPORT_NODE_PRODUCT_LINE_PLOT2D_TITLE, this->title);
		        writer.write_value(__CPP_TRANSPORT_NODE_PRODUCT_LINE_PLOT2D_TITLE_TEXT, this->title_text);
		        writer.write_value(__CPP_TRANSPORT_NODE_PRODUCT_LINE_PLOT2D_LEGEND, this->legend);

		        switch(this->position)
			        {
		            case top_left:
			            writer.write_value(__CPP_TRANSPORT_NODE_PRODUCT_LINE_PLOT2D_LEGEND_POS, std::string(__CPP_TRANSPORT_NODE_PRODUCT_LINE_PLOT2D_LEGEND_TL));
		            break;

		            case top_right:
			            writer.write_value(__CPP_TRANSPORT_NODE_PRODUCT_LINE_PLOT2D_LEGEND_POS, std::string(__CPP_TRANSPORT_NODE_PRODUCT_LINE_PLOT2D_LEGEND_TR));
		            break;

		            case bottom_left:
			            writer.write_value(__CPP_TRANSPORT_NODE_PRODUCT_LINE_PLOT2D_LEGEND_POS, std::string(__CPP_TRANSPORT_NODE_PRODUCT_LINE_PLOT2D_LEGEND_BL));
		            break;

		            case bottom_right:
			            writer.write_value(__CPP_TRANSPORT_NODE_PRODUCT_LINE_PLOT2D_LEGEND_POS, std::string(__CPP_TRANSPORT_NODE_PRODUCT_LINE_PLOT2D_LEGEND_BR));
		            break;

		            case right:
			            writer.write_value(__CPP_TRANSPORT_NODE_PRODUCT_LINE_PLOT2D_LEGEND_POS, std::string(__CPP_TRANSPORT_NODE_PRODUCT_LINE_PLOT2D_LEGEND_R));
		            break;

		            case centre_right:
			            writer.write_value(__CPP_TRANSPORT_NODE_PRODUCT_LINE_PLOT2D_LEGEND_POS, std::string(__CPP_TRANSPORT_NODE_PRODUCT_LINE_PLOT2D_LEGEND_CR));
		            break;

		            case centre_left:
			            writer.write_value(__CPP_TRANSPORT_NODE_PRODUCT_LINE_PLOT2D_LEGEND_POS, std::string(__CPP_TRANSPORT_NODE_PRODUCT_LINE_PLOT2D_LEGEND_CL));
		            break;

		            case upper_centre:
			            writer.write_value(__CPP_TRANSPORT_NODE_PRODUCT_LINE_PLOT2D_LEGEND_POS, std::string(__CPP_TRANSPORT_NODE_PRODUCT_LINE_PLOT2D_LEGEND_UC));
		            break;

		            case lower_centre:
			            writer.write_value(__CPP_TRANSPORT_NODE_PRODUCT_LINE_PLOT2D_LEGEND_POS, std::string(__CPP_TRANSPORT_NODE_PRODUCT_LINE_PLOT2D_LEGEND_LC));
		            break;

		            case centre:
			            writer.write_value(__CPP_TRANSPORT_NODE_PRODUCT_LINE_PLOT2D_LEGEND_POS, std::string(__CPP_TRANSPORT_NODE_PRODUCT_LINE_PLOT2D_LEGEND_C));
		            break;

		            default:
			            assert(false);
		            throw runtime_exception(runtime_exception::SERIALIZATION_ERROR, __CPP_TRANSPORT_PRODUCT_LINE_PLOT2D_INVALID_LEGEND_POSITION);
			        }

		        writer.write_value(__CPP_TRANSPORT_NODE_PRODUCT_LINE_PLOT2D_TYPESET_LATEX, this->typeset_with_LaTeX);

            writer.write_value(__CPP_TRANSPORT_NODE_PRODUCT_LINE_PLOT2D_PYTHON_PATH, this->python_path.string());

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
