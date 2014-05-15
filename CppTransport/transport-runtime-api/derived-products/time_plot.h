//
// Created by David Seery on 13/05/2014.
// Copyright (c) 2014 University of Sussex. All rights reserved.
//


#ifndef __time_plot_H_
#define __time_plot_H_


#include "transport-runtime-api/messages.h"

#include "transport-runtime-api/derived-products/plot2d_product.h"


// maximum number of serial numbers to output when writing ourselves to
// a standard stream
#define __CPP_TRANSPORT_PRODUCT_TIMEPLOT_MAX_SN (15)


#define __CPP_TRANSPORT_NODE_DERIVED_PRODUCT_TIMEPLOT_SNS "time-plot-serial-numbers"
#define __CPP_TRANSPORT_NODE_DERIVED_PRODUCT_TIMEPLOT_SN  "serial-number"


namespace transport
	{

		namespace derived_data
			{

			  //! A time-plot is a specialization of a plot2d-product that produces
		    //! an time-evolution plot of some components (or multiple components)
		    //! of one or more correlation functions.

		    template <typename number>
		    class time_plot: public plot2d_product<number>
			    {

		      public:

				    // CONSTRUCTOR, DESTRUCTOR

				    //! Basic user-facing constructor.
				    time_plot(const std::string& name, const std::string& filename, const integration_task<number>& tk,
				              typename plot2d_product<number>::time_filter filter)
					    : plot2d_product<number>(name, filename, tk)
					    {
				        apply_default_settings();
				        apply_default_labels();

								// set up a list of serial numbers corresponding to the sample times for this plot
				        const std::vector<double>& sample_times = tk.get_sample_times();

						    for(unsigned int i = 0; i < sample_times.size(); i++)
							    {
								    if(filter(sample_times[i])) time_sample_sns.push_back(i);
							    }
					    }

				    //! Deserialization constructor.
				    time_plot(const std::string& name, const integration_task<number>* tk, serialization_reader* reader)
					    : plot2d_product<number>(name, tk, reader)
					    {
						    // extract serial numbers from reader
						    assert(reader != nullptr);

						    unsigned int sns = reader->start_array(__CPP_TRANSPORT_NODE_DERIVED_PRODUCT_TIMEPLOT_SNS);

						    for(unsigned int i = 0; i < sns; i++)
							    {
										reader->start_array_element();

								    unsigned int sn;
								    reader->read_value(__CPP_TRANSPORT_NODE_DERIVED_PRODUCT_TIMEPLOT_SN, sn);
								    time_sample_sns.push_back(sn);

								    reader->end_array_element();
							    }

						    reader->end_element(__CPP_TRANSPORT_NODE_DERIVED_PRODUCT_TIMEPLOT_SNS);
					    }

				    ~time_plot() = default;


		        // SETTING DEFAULTS -- implements a 'plot2d_product' interface

		      public:

		        //! (re-)set a default set of labels; should account for the LaTeX setting if desired
		        virtual void apply_default_labels() override;
		        //! (re-)set a default list of settings
		        virtual void apply_default_settings() override;


				    // EXTRACT TIME SAMPLE

		      public:

				    //! get serial numbers of sample times
				    const std::vector<unsigned int>& get_time_sample_sns() { return(this->time_sample_sns); }


				    // SERIALIZATION -- implements a 'serializable' interface

		      public:

				    virtual void serialize(serialization_writer& writer) const override;


				    // WRITE SELF TO STANDARD STREAM

		      public:

				    void write(std::ostream& out);


				    // INTERNAL DATA

		      protected:

				    std::vector<unsigned int> time_sample_sns;

			    };


				template <typename number>
				void time_plot<number>::apply_default_settings()
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
					}


				template <typename number>
				void time_plot<number>::apply_default_labels()
					{
						// default label set is: no y-axis label, x-axis label is time in e-folds; no title

						if(this->get_use_LaTeX()) this->set_x_label_text(__CPP_TRANSPORT_PRODUCT_TIMEPLOT_X_LABEL_LATEX);
						else                      this->set_x_label_text(__CPP_TRANSPORT_PRODUCT_TIMEPLOT_X_LABEL_NOLATEX);

						this->clear_y_label_text();
						this->clear_title_text();
					}


				template <typename number>
				void time_plot<number>::serialize(serialization_writer& writer) const
					{
						this->begin_array(writer, __CPP_TRANSPORT_NODE_DERIVED_PRODUCT_TIMEPLOT_SNS, this->time_sample_sns.size() == 0);
				    for(unsigned int i = 0; i < this->time_sample_sns.size(); i++)
					    {
				        this->begin_node(writer, "arrayelt", false);    // node name ignored for arrays
				        this->write_value_node(writer, __CPP_TRANSPORT_NODE_DERIVED_PRODUCT_TIMEPLOT_SN, this->time_sample_sns[i]);
				        this->end_element(writer, "arrayelt");
					    }
						this->end_element(writer, __CPP_TRANSPORT_NODE_DERIVED_PRODUCT_TIMEPLOT_SNS);

						this->plot2d_product<number>::serialize(writer);
					}


				template <typename number>
				void time_plot<number>::write(std::ostream& out)
					{
						this->wrap_out(out, __CPP_TRANSPORT_PRODUCT_TIMEPLOT_SN_LABEL " ");

						unsigned int count = 0;
						for(std::vector<unsigned int>::const_iterator t = this->time_sample_sns.begin(); t != time_sample_sns.end() && count < __CPP_TRANSPORT_PRODUCT_TIMEPLOT_MAX_SN; t++)
							{
						    std::ostringstream msg;
								msg << (*t);

						    this->wrap_list_item(out, true, msg.str(), count);
							}
						if(count == __CPP_TRANSPORT_PRODUCT_TIMEPLOT_MAX_SN) this->wrap_list_item(out, true, "...", count);

						this->wrap_newline(out);
						this->plot2d_product<number>::write(out);
					}


			}   // namespace derived_data

	}   // namespace transport


#endif //__time_plot_H_
