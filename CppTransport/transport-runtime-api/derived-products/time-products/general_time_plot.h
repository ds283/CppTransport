//
// Created by David Seery on 17/05/2014.
// Copyright (c) 2014 University of Sussex. All rights reserved.
//


#ifndef __general_time_plot_H_
#define __general_time_plot_H_


#include "transport-runtime-api/serialization/serializable.h"

#include "transport-runtime-api/derived-products/plot2d_product.h"
#include "transport-runtime-api/derived-products/time-data/field_time_data.h"
#include "transport-runtime-api/derived-products/time-data/zeta_time_data.h"
#include "transport-runtime-api/derived-products/time-data/general_time_data_helper.h"

#include "transport-runtime-api/derived-products/utilities/index_selector.h"
#include "transport-runtime-api/derived-products/utilities/filter.h"
#include "transport-runtime-api/derived-products/utilities/wrapper.h"

#include "transport-runtime-api/defaults.h"
#include "transport-runtime-api/messages.h"


#define __CPP_TRANSPORT_NODE_PRODUCT_TPLOT_LINE_ARRAY "line-array"


namespace transport
	{

		namespace derived_data
			{

		    template <typename number>
		    class general_time_plot : public plot2d_product<number>
			    {

		      public:

		        // CONSTRUCTOR, DESTRUCTOR

		        //! Basic user-facing constructor. Accepts a filename, integration task,
		        //! filter function which determines which times are plotted.
		        //! Also requires a model reference.
		        general_time_plot(const std::string& name, const std::string& filename);

		        //! Deserialization constructor.
		        //! Accepts a serialization_reader and a model reference.
		        //! Remaining data is extracted from the reader.
		        general_time_plot(const std::string& name, serialization_reader* reader, typename repository<number>::task_finder finder);

				    //! Copy constructor. Required to perform a deep copy on the general_time_data objects we own
				    general_time_plot(const general_time_plot<number>& obj);

				    ~general_time_plot();


		        // SETTING DEFAULTS -- implements a 'plot2d_product' interface

		      public:

		        //! (re-)set a default set of labels; should account for the LaTeX setting if desired
		        virtual void apply_default_labels() override;

		        //! (re-)set a default list of settings
		        virtual void apply_default_settings() override;


				    // ADD LINES TO THE PLOT

		      public:

		        // FIXME: should check that the line uses a task which is compatible with the task used to set up this plot
				    //! Add a group of general_time_data lines to the plot.
				    //! Hides the underlying add_line provided by plot2d_product
				    void add_line(const general_time_data<number>& line);

		        // APPLY THIS DERIVATION TO PRODUCE OUTPUT -- implements a 'derived_product' interface

		      public:

		        //! Apply this derivation to produce a plot
		        virtual void derive(typename data_manager<number>::datapipe& pipe, const std::list<std::string>& tags) override;


		        // SERIALIZATION -- implements a 'serializable' interface

		      public:

		        virtual void serialize(serialization_writer& writer) const override;


		        // WRITE SELF TO STANDARD STREAM

		      public:

		        void write(std::ostream& out);


		        // CLONE

		      public:

		        //! Virtual copy
		        virtual derived_product<number>* clone() const override { return new general_time_plot<number>(static_cast<const general_time_plot<number>&>(*this)); }


		        // INTERNAL DATA

		      private:

		        //! data lines included on the plot
		        std::list< general_time_data<number>* > data_lines;

		        //! record model object with which we are associated
		        //! (although this is a pointer, we don't care if it is shallow-copied
		        //! because all model<> instances are handled by the instance_manager)
		        model<number>* mdl;

			    };


		    template <typename number>
		    general_time_plot<number>::general_time_plot(const std::string& name, const std::string& filename)
			    : plot2d_product<number>(name, filename)
			    {
		        apply_default_settings();
		        apply_default_labels();
			    }


		    template <typename number>
		    general_time_plot<number>::general_time_plot(const std::string& name, serialization_reader* reader, typename repository<number>::task_finder finder)
			    : plot2d_product<number>(name, reader, finder)
			    {
		        assert(reader != nullptr);

		        if(reader == nullptr) throw runtime_exception(runtime_exception::RUNTIME_ERROR, __CPP_TRANSPORT_PRODUCT_GENERAL_TPLOT_NULL_READER);

		        unsigned int lines = reader->start_array(__CPP_TRANSPORT_NODE_PRODUCT_TPLOT_LINE_ARRAY);

		        data_lines.clear();
		        for(unsigned int i = 0; i < lines; i++)
			        {
		            reader->start_array_element();

		            general_time_data<number>* data = general_time_data_helper::deserialize<number>(reader, finder);
		            data_lines.push_back(data);

		            reader->end_array_element();
			        }

		        reader->end_element(__CPP_TRANSPORT_NODE_PRODUCT_TPLOT_LINE_ARRAY);
			    }


				template <typename number>
				general_time_plot<number>::general_time_plot(const general_time_plot<number>& obj)
					: mdl(obj.mdl), plot2d_product<number>(obj)
					{
						data_lines.clear();

						for(typename std::list< general_time_data<number>* >::const_iterator t = obj.data_lines.begin(); t != obj.data_lines.end(); t++)
							{
								data_lines.push_back((*t)->clone());
							}
					}


				template <typename number>
				general_time_plot<number>::~general_time_plot()
					{
						for(typename std::list< general_time_data<number>* >::const_iterator t = this->data_lines.begin(); t != this->data_lines.end(); t++)
							{
								delete *t;
							}
					}


				template <typename number>
				void general_time_plot<number>::add_line(const general_time_data<number>& line)
					{
						this->data_lines.push_back(line.clone());
					}


				template <typename number>
				void general_time_plot<number>::derive(typename data_manager<number>::datapipe& pipe, const std::list<std::string>& tags)
					{
            // reset the plot, then pull data from the datapipe to regenerate ourselves
            this->reset_plot();

						// loop through all data lines, adding their components to the plot
						for(typename std::list< general_time_data<number>* >::const_iterator t = this->data_lines.begin(); t != this->data_lines.end(); t++)
							{
						    std::list< time_data_line<number> > lines;

						    (*t)->derive_lines(pipe, *this, lines, tags);

								for(typename std::list< time_data_line<number> >::const_iterator u = lines.begin(); u != lines.end(); u++)
									{
										this->plot2d_product<number>::add_line(*u);
									}
							}

						this->make_plot(pipe);
					}


		    template <typename number>
		    void general_time_plot<number>::apply_default_settings()
			    {
		        // default settings are: logarithmic y axis, absolute y values, linear x axis, no reversals, x-axis label only, use LaTeX, legend

		        this->set_log_x(false);
		        this->set_log_y(true);
		        this->set_reverse_x(false);
		        this->set_reverse_y(false);
		        this->set_abs_y(true);
		        this->set_use_LaTeX(true);
		        this->set_x_label(true);
		        this->set_y_label(false);
		        this->set_title(false);
		        this->set_legend(true);
		        this->set_legend_position(plot2d_product<number>::top_right);
		        this->set_typeset_with_LaTeX(false);
			    }


		    template <typename number>
		    void general_time_plot<number>::apply_default_labels()
			    {
		        // default label set is: no y-axis label, x-axis label is time in e-folds; no title

		        if(this->get_use_LaTeX()) this->set_x_label_text(__CPP_TRANSPORT_PRODUCT_TIMEPLOT_X_LABEL_LATEX);
		        else                      this->set_x_label_text(__CPP_TRANSPORT_PRODUCT_TIMEPLOT_X_LABEL_NOLATEX);

		        this->clear_y_label_text();
		        this->clear_title_text();
			    }


				template <typename number>
				void general_time_plot<number>::serialize(serialization_writer& writer) const
					{
						this->write_value_node(writer, __CPP_TRANSPORT_NODE_DERIVED_PRODUCT_TYPE, std::string(__CPP_TRANSPORT_NODE_DERIVED_PRODUCT_GENERAL_TIME_PLOT));

						this->begin_array(writer, __CPP_TRANSPORT_NODE_PRODUCT_TPLOT_LINE_ARRAY, this->data_lines.size() == 0);
						for(typename std::list< general_time_data<number>* >::const_iterator t = this->data_lines.begin(); t != this->data_lines.end(); t++)
							{
								this->begin_node(writer, "arrayelt", false);    // node name ignored in array
						    (*t)->serialize(writer);
								this->end_element(writer, "arrayelt");
							}
						this->end_element(writer, __CPP_TRANSPORT_NODE_PRODUCT_TPLOT_LINE_ARRAY);

						// call next serialization
						this->plot2d_product<number>::serialize(writer);
					}


				template <typename number>
				void general_time_plot<number>::write(std::ostream& out)
					{
						out << __CPP_TRANSPORT_PRODUCT_GENERAL_TPLOT_LABEL_TITLE_A << " '" << this->get_name() << "', " << __CPP_TRANSPORT_PRODUCT_GENERAL_TPLOT_LABEL_TITLE_B << std::endl;

						for(typename std::list< general_time_data<number>* >::iterator t = this->data_lines.begin(); t != this->data_lines.end(); t++)
							{
						    (*t)->write(out);
						    this->wrapper.wrap_newline(out);
							}
					}


				template <typename number>
				std::ostream& operator<<(std::ostream& out, general_time_plot<number>& obj)
					{
						obj.write(out);
						return(out);
					}


			}   // namespace derived_data


	}   // namespace transport


#endif //__general_time_plot_H_
