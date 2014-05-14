//
// Created by David Seery on 13/05/2014.
// Copyright (c) 2014 University of Sussex. All rights reserved.
//


#ifndef __plot_product_H_
#define __plot_product_H_


#include <functional>

#include "transport-runtime-api/derived-products/derived_product.h"

#include "transport-runtime-api/messages.h"


namespace transport
	{

		namespace derived_data
			{

			  //! A plot2d-line is a data line suitable for inclusion in a plot2d-product

		    template <typename number>
		    class plot2d_line
			    {

			    };

		    //! A plot2d-product is a specialization of a derived-product that
		    //! produces a plot of something against time.

		    template <typename number>
		    class plot2d_product: public derived_product<number>
			    {

		      public:

				    typedef enum { top_left, top_right, bottom_left, bottom_right } legend_pos;

				    typedef std::function<bool(double)> time_filter;

		      public:

		        // CONSTRUCTOR, DESTRUCTOR

		        plot2d_product(const std::string& name, const std::string& filename, integration_task<number>& tk,
		                       time_filter tf)
		          : filter(tf), derived_product<number>(name, filename, tk)
			        {
			        }

		        virtual ~plot2d_product() = default;


		        // SETTING DEFAULTS

		      public:

		        //! (re-)set a default set of labels; should account for the LaTeX setting if desired
		        virtual void apply_default_labels() = 0;
		        //! (re-)set a default list of settings
		        virtual void apply_default_settings() = 0;


		        // GET AND SET BASIC PLOT LABELS

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
				    bool get_legend() { return(this->legend); }
				    //! set legend setting
				    void set_legend(bool g) { this->legend = g; }

				    //! get legend position
				    legend_pos get_legend_position() { return(this->position); }
				    //! set legend position
				    void set_legend_position(legend_pos pos) { this->position = pos; }


				    // WRITE SELF TO A STANDARD STREAM

		      public:

				    void write(std::ostream& out);


		        // INTERNAL DATA

		      protected:

				    //! time filter function: used to decide which time serial numbers should be used
				    //! when producing a plot
				    time_filter filter;

				    // PLOT DETAILS

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

			    };


				template <typename number>
				void plot2d_product<number>::write(std::ostream& out)
					{
						unsigned int count = 0;

				    this->wrap_option(out, this->log_x, __CPP_TRANSPORT_PRODUCT_PLOT2D_LABEL_LOGX, count);
				    this->wrap_option(out, this->log_y, __CPP_TRANSPORT_PRODUCT_PLOT2D_LABEL_LOGY, count);
				    this->wrap_option(out, this->abs_y, __CPP_TRANSPORT_PRODUCT_PLOT2D_LABEL_ABSY, count);
				    this->wrap_option(out, this->reverse_x, __CPP_TRANSPORT_PRODUCT_PLOT2D_LABEL_REVERSEX, count);
				    this->wrap_option(out, this->reverse_y, __CPP_TRANSPORT_PRODUCT_PLOT2D_LABEL_REVERSEY, count);
				    this->wrap_option(out, this->use_LaTeX, __CPP_TRANSPORT_PRODUCT_PLOT2D_LABEL_LATEX, count);
				    this->wrap_option(out, this->x_label, __CPP_TRANSPORT_PRODUCT_PLOT2D_LABEL_XLABEL, count);
						if(this->x_label)
							this->wrap_value(out, this->x_label_text, __CPP_TRANSPORT_PRODUCT_PLOT2D_LABEL_LABEL, count);
				    this->wrap_option(out, this->y_label, __CPP_TRANSPORT_PRODUCT_PLOT2D_LABEL_YLABEL, count);
						if(this->y_label)
							this->wrap_value(out, this->y_label_text, __CPP_TRANSPORT_PRODUCT_PLOT2D_LABEL_LABEL, count);
				    this->wrap_option(out, this->title, __CPP_TRANSPORT_PRODUCT_PLOT2D_LABEL_TITLE, count);
						if(this->title)
							this->wrap_value(out, this->title_text, __CPP_TRANSPORT_PRODUCT_PLOT2D_LABEL_LABEL, count);
				    this->wrap_option(out, this->legend, __CPP_TRANSPORT_PRODUCT_PLOT2D_LABEL_LEGEND, count);

						if(this->legend)
							{
								if(count > 0) out << ", ";
								switch(this->position)
									{
								    case top_left:
									    out << __CPP_TRANSPORT_PRODUCT_PLOT2D_LABEL_LEGEND_TL;
											break;
								    case top_right:
									    out << __CPP_TRANSPORT_PRODUCT_PLOT2D_LABEL_LEGEND_TR;
											break;
								    case bottom_left:
									    out << __CPP_TRANSPORT_PRODUCT_PLOT2D_LABEL_LEGEND_BL;
											break;
								    case bottom_right:
									    out << __CPP_TRANSPORT_PRODUCT_PLOT2D_LABEL_LEGEND_BR;
											break;
								    default:
									    assert(false);
									}
							}
					}

			}   // namespace derived_data

	}   // namespace transport


#endif //__plot_product_H_
