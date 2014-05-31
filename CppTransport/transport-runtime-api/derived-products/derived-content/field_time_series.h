//
// Created by David Seery on 19/05/2014.
// Copyright (c) 2014 University of Sussex. All rights reserved.
//


#ifndef __field_time_series_H_
#define __field_time_series_H_


#include <iostream>
#include <sstream>
#include <string>
#include <list>
#include <vector>
#include <stdexcept>

#include "transport-runtime-api/derived-products/data_line.h"
#include "transport-runtime-api/derived-products/derived-content/time_series.h"

#include "transport-runtime-api/utilities/latex_output.h"


#define __CPP_TRANSPORT_NODE_PRODUCT_TSERIES_TWOPF_DATA_TYPE      "twopf-components"
#define __CPP_TRANSPORT_NODE_PRODUCT_TSERIES_TWOPF_DATA_REAL      "real"
#define __CPP_TRANSPORT_NODE_PRODUCT_TSERIES_TWOPF_DATA_IMAGINARY "imaginary"

#define __CPP_TRANSPORT_NODE_PRODUCT_TSERIES_THREEPF_LABEL_KT     "use-kt-label"
#define __CPP_TRANSPORT_NODE_PRODUCT_TSERIES_THREEPF_LABEL_ALPHA  "use-alpha-label"
#define __CPP_TRANSPORT_NODE_PRODUCT_TSERIES_THREEPF_LABEL_BETA   "use-beta-label"


namespace transport
	{

		namespace derived_data
			{

		    //! background time data line
		    template <typename number>
		    class background_time_series : public time_series<number>
			    {

		        // CONSTRUCTOR, DESTRUCTOR

		      public:

		        //! construct a background time-data object
		        background_time_series(const integration_task<number>& tk, model<number>* m, index_selector<1>& sel,
		                             filter::time_filter tfilter, unsigned int prec=__CPP_TRANSPORT_DEFAULT_PLOT_PRECISION);

		        //! deserialization constructor.
		        background_time_series(serialization_reader* reader, typename repository<number>::task_finder finder);

		        virtual ~background_time_series() = default;


		        // DERIVE LINES -- implements a 'time_series' interface

		        //! generate data lines for plotting
            virtual void derive_lines(typename data_manager<number>::datapipe& pipe, std::list<data_line<number> >& lines,
                                      const std::list<std::string>& tags) const override;


		        // LABEL GENERATION

		      protected:

				    //! Make LaTeX label for this line
		        std::string make_LaTeX_label(unsigned int i) const;

				    //! Make non-LaTeX label for this line
				    std::string make_non_LaTeX_label(unsigned int i) const;


		        // CLONE

		        //! self-replicate
		        virtual derived_line<number>* clone() const override { return new background_time_series<number>(static_cast<const background_time_series<number>&>(*this)); }


		        // WRITE TO A STREAM

		        //! write self-details to a stream
		        virtual void write(std::ostream& out) override;


		        // SERIALIZATION -- implements a 'serializable' interface

		      public:

		        //! serialize this object
		        virtual void serialize(serialization_writer& writer) const override;

		        // INTERNAL DATA

		      private:

		        //! record which indices are active in this group
		        index_selector<1> active_indices;

			    };


		    template <typename number>
		    background_time_series<number>::background_time_series(const integration_task<number>& tk, model<number>* m, index_selector<1>& sel,
		                                                       filter::time_filter tfilter, unsigned int prec)
			    : active_indices(sel), time_series<number>(tk, m, tfilter, derived_line<number>::background_field, prec)
			    {
		        assert(m != nullptr);

		        if(active_indices.get_number_fields() != this->mdl->get_N_fields())
			        {
		            std::ostringstream msg;
		            msg << __CPP_TRANSPORT_PRODUCT_INDEX_MISMATCH << " ("
			              << __CPP_TRANSPORT_PRODUCT_INDEX_MISMATCH_A << " " << active_indices.get_number_fields() << ", "
			              << __CPP_TRANSPORT_PRODUCT_INDEX_MISMATCH_B << " " << this->mdl->get_N_fields() << ")";
		            throw runtime_exception(runtime_exception::RUNTIME_ERROR, msg.str());
			        }
			    }


		    template <typename number>
		    background_time_series<number>::background_time_series(serialization_reader* reader, typename repository<number>::task_finder finder)
			    : active_indices(reader), time_series<number>(reader, finder)
			    {
		        if(reader == nullptr) throw runtime_exception(runtime_exception::RUNTIME_ERROR, __CPP_TRANSPORT_PRODUCT_TIME_SERIES_NULL_READER);
			    }


		    template <typename number>
		    void background_time_series<number>::write(std::ostream& out)
			    {
				    this->time_series<number>::write(out);
		        out << "  " << __CPP_TRANSPORT_PRODUCT_TIME_SERIES_LABEL_BACKGROUND << std::endl;
		        out << "  " << __CPP_TRANSPORT_PRODUCT_LINE_COLLECTION_LABEL_INDICES << " ";
		        this->active_indices.write(out, this->mdl->get_state_names());
			    }


		    template <typename number>
		    void background_time_series<number>::serialize(serialization_writer& writer) const
			    {
		        this->write_value_node(writer, __CPP_TRANSPORT_NODE_PRODUCT_DERIVED_LINE_TYPE,
		                               std::string(__CPP_TRANSPORT_NODE_PRODUCT_DERIVED_LINE_BACKGROUND));

		        this->active_indices.serialize(writer);

		        this->time_series<number>::serialize(writer);
			    }


        template <typename number>
        void background_time_series<number>::derive_lines(typename data_manager<number>::datapipe& pipe, std::list< data_line<number> >& lines,
                                                        const std::list<std::string>& tags) const
          {
            // attach our datapipe to an output group
            this->attach(pipe, tags);

            const std::vector<double> time_axis = this->pull_time_axis(pipe);

            // loop through all the fields, pulling data from the database for those which are enabled

		        typename data_manager<number>::datapipe::time_data_handle& handle = pipe.new_time_data_handle(this->get_time_sample_sns());

            for(unsigned int m = 0; m < 2 * this->mdl->get_N_fields(); m++)
              {
                std::array<unsigned int, 1> index_set = {m};
                if(this->active_indices.is_on(index_set))
                  {
										typename data_manager<number>::datapipe::background_time_data_tag tag = pipe.new_background_time_data_tag(this->mdl->flatten(m));

                    const std::vector<number>& line_data = handle.lookup_tag(tag);

                    data_line<number> line = data_line<number>(data_line<number>::time_series, data_line<number>::background_field,
                                                               time_axis, line_data,
                                                               this->make_LaTeX_label(m), this->make_non_LaTeX_label(m));

                    lines.push_back(line);
                  }
              }

            // detach pipe from output group
            this->detach(pipe);
          }


		    template <typename number>
		    std::string background_time_series<number>::make_LaTeX_label(unsigned int i) const
			    {
		        std::ostringstream label;

		        unsigned int N_fields = this->mdl->get_N_fields();

		        const std::vector<std::string>& field_names = this->mdl->get_f_latex_names();

		        label << "$";
		        if(this->get_dot_meaning() == derived_line<number>::derivatives)
			        {
		            label << field_names[i % N_fields] << (i >= N_fields ? "^{" __CPP_TRANSPORT_LATEX_PRIME_SYMBOL "}" : "");
			        }
		        else
			        {
		            label << (i >= N_fields ? "p_{" : "") << field_names[i % N_fields] << (i >= N_fields ? "}" : "");
			        }
		        label << "$";

				    return(label.str());
			    }


		    template <typename number>
        std::string background_time_series<number>::make_non_LaTeX_label(unsigned int i) const
	        {
		        std::ostringstream label;

		        unsigned int N_fields = this->mdl->get_N_fields();

	          const std::vector<std::string>& field_names = this->mdl->get_field_names();

	          if(this->get_dot_meaning() == derived_line<number>::derivatives)
	            {
	              label << field_names[i % N_fields] << (i >= N_fields ? __CPP_TRANSPORT_NONLATEX_PRIME_SYMBOL : "");
	            }
	          else
	            {
	              label << (i >= N_fields ? "p_{" : "") << field_names[i % N_fields] << (i >= N_fields ? "}" : "");
	            }

		        return(label.str());
			    }


		    //! twopf time data line
		    template <typename number>
		    class twopf_time_series: public time_series<number>
			    {

		      public:

		        //! which twopf?
		        typedef enum { real, imaginary } twopf_type;

		        // CONSTRUCTOR, DESTRUCTOR

		      public:

		        //! construct a twopf time-data object
		        twopf_time_series(const twopf_list_task<number>& tk, model<number>* m, index_selector<2>& sel,
		                        filter::time_filter tfilter,
		                        filter::twopf_kconfig_filter kfilter,
		                        unsigned int prec=__CPP_TRANSPORT_DEFAULT_PLOT_PRECISION);

		        //! deserialization constuctor.
		        twopf_time_series(serialization_reader* reader, typename repository<number>::task_finder finder);

		        virtual ~twopf_time_series() = default;


		        // MANAGE SETTINGS

		        //! get twopf type setting
		        twopf_type get_type() const { return(this->twopf_meaning); }
		        //! set twopf type setting
		        void set_type(twopf_type m) { this->twopf_meaning = m; }


		        // DERIVE LINES -- implements a 'time_series' interface

		        //! generate data lines for plotting
            virtual void derive_lines(typename data_manager<number>::datapipe& pipe, std::list<data_line<number> >& lines,
                                      const std::list<std::string>& tags) const override;


		        // LABEL GENERATION

		      protected:

				    //! Make LaTeX label for this line
		        std::string make_LaTeX_label(unsigned int m, unsigned int n, const typename data_manager<number>::twopf_configuration& config) const;

				    //! Make non-LaTeX label for this line
				    std::string make_non_LaTeX_label(unsigned int m, unsigned int n, const typename data_manager<number>::twopf_configuration& config) const;


		        // CLONE

		        //! self-replicate
		        virtual derived_line<number>* clone() const override { return new twopf_time_series<number>(static_cast<const twopf_time_series<number>&>(*this)); }


		        // WRITE TO A STREAM

		        //! write self-details to a stream
		        virtual void write(std::ostream& out) override;


		        // SERIALIZATION -- implements a 'serializable' interface

		      public:

		        //! serialize this object
		        virtual void serialize(serialization_writer& writer) const override;


		        // INTERNAL DATA

		      private:

		        //! record which indices are active in this group
		        index_selector<2> active_indices;

		        //! record serial numbers of k-configurations we are using
		        std::vector<unsigned int> kconfig_sample_sns;

		        //! record which type of 2pf we are plotting
		        twopf_type twopf_meaning;
			    };


		    template <typename number>
		    twopf_time_series<number>::twopf_time_series(const twopf_list_task<number>& tk, model<number>* m, index_selector<2>& sel,
		                                             filter::time_filter tfilter, filter::twopf_kconfig_filter kfilter, unsigned int prec)
			    : active_indices(sel), twopf_meaning(real), time_series<number>(tk, m, tfilter, derived_line<number>::correlation_function, prec)
			    {
		        assert(m != nullptr);

		        if(m == nullptr) throw runtime_exception(runtime_exception::RUNTIME_ERROR, __CPP_TRANSPORT_PRODUCT_TIME_SERIES_NULL_MODEL);

		        if(active_indices.get_number_fields() != this->mdl->get_N_fields())
			        {
		            std::ostringstream msg;
		            msg << __CPP_TRANSPORT_PRODUCT_INDEX_MISMATCH << " ("
			            << __CPP_TRANSPORT_PRODUCT_INDEX_MISMATCH_A << " " << active_indices.get_number_fields() << ", "
			            << __CPP_TRANSPORT_PRODUCT_INDEX_MISMATCH_B << " " << this->mdl->get_N_fields() << ")";
		            throw runtime_exception(runtime_exception::RUNTIME_ERROR, msg.str());
			        }

		        this->f.filter_twopf_kconfig_sample(kfilter, tk.get_k_list(), kconfig_sample_sns);
			    }


		    template <typename number>
		    twopf_time_series<number>::twopf_time_series(serialization_reader* reader, typename repository<number>::task_finder finder)
			    : active_indices(reader), time_series<number>(reader, finder)
			    {
		        assert(reader != nullptr);

		        if(reader == nullptr) throw runtime_exception(runtime_exception::RUNTIME_ERROR, __CPP_TRANSPORT_PRODUCT_TIME_SERIES_NULL_READER);

		        std::string tpf_type;
		        reader->read_value(__CPP_TRANSPORT_NODE_PRODUCT_TSERIES_TWOPF_DATA_TYPE, tpf_type);

		        if(tpf_type == __CPP_TRANSPORT_NODE_PRODUCT_TSERIES_TWOPF_DATA_REAL) twopf_meaning = real;
		        else if(tpf_type == __CPP_TRANSPORT_NODE_PRODUCT_TSERIES_TWOPF_DATA_IMAGINARY) twopf_meaning = imaginary;
		        else
			        {
		            std::ostringstream msg;
		            msg << __CPP_TRANSPORT_PRODUCT_DERIVED_LINE_TWOPF_TYPE_UNKNOWN << " '" << tpf_type << "'";
		            throw runtime_exception(runtime_exception::SERIALIZATION_ERROR, msg.str());
			        }

		        unsigned int sns = reader->start_array(__CPP_TRANSPORT_NODE_PRODUCT_TDATA_K_SERIAL_NUMBERS);

		        for(unsigned int i = 0; i < sns; i++)
			        {
		            reader->start_array_element();

		            unsigned int sn;
		            reader->read_value(__CPP_TRANSPORT_NODE_PRODUCT_TDATA_K_SERIAL_NUMBER, sn);

		            kconfig_sample_sns.push_back(sn);

		            reader->end_array_element();
			        }

		        reader->end_element(__CPP_TRANSPORT_NODE_PRODUCT_TDATA_K_SERIAL_NUMBERS);
			    }


        template <typename number>
        void twopf_time_series<number>::derive_lines(typename data_manager<number>::datapipe& pipe, std::list< data_line<number> >& lines,
                                                   const std::list<std::string>& tags) const
			    {
            // attach our datapipe to an output gorup
            this->attach(pipe, tags);

		        // pull time-axis data
            const std::vector<double>& time_axis = this->pull_time_axis(pipe);

		        // set up cache handles
		        typename data_manager<number>::datapipe::twopf_kconfig_handle& k_handle = pipe.new_twopf_kconfig_handle(this->kconfig_sample_sns);
		        typename data_manager<number>::datapipe::time_data_handle& t_handle = pipe.new_time_data_handle(this->get_time_sample_sns());

		        // pull k-configuration information from the database
		        typename data_manager<number>::datapipe::twopf_kconfig_tag k_tag = pipe.new_twopf_kconfig_tag();

		        const typename std::vector< typename data_manager<number>::twopf_configuration >& k_values = k_handle.lookup_tag(k_tag);

		        // loop through all components of the twopf, for each k-configuration we use,
		        // pulling data from the database
		        for(unsigned int i = 0; i < this->kconfig_sample_sns.size(); i++)
			        {
		            for(unsigned int m = 0; m < 2*this->mdl->get_N_fields(); m++)
			            {
		                for(unsigned int n = 0; n < 2*this->mdl->get_N_fields(); n++)
			                {
		                    std::array<unsigned int, 2> index_set = { m, n };
		                    if(this->active_indices.is_on(index_set))
			                    {
				                    typename data_manager<number>::datapipe::cf_time_data_tag tag =
					                                                                              pipe.new_cf_time_data_tag(this->twopf_meaning == real ? data_manager<number>::datapipe::cf_twopf_re : data_manager<number>::datapipe::cf_twopf_im,
					                                                                                                        this->mdl->flatten(m,n), this->kconfig_sample_sns[i]);

		                        const std::vector<number>& line_data = t_handle.lookup_tag(tag);

		                        data_line<number> line = data_line<number>(data_line<number>::time_series, data_line<number>::correlation_function,
		                                                                   time_axis, line_data,
		                                                                   this->make_LaTeX_label(m, n, k_values[i]), this->make_non_LaTeX_label(m, n, k_values[i]));

		                        lines.push_back(line);
			                    }
			                }
			            }
			        }

            // detach pipe from output group
            this->detach(pipe);
			    }


		    template <typename number>
		    std::string twopf_time_series<number>::make_LaTeX_label(unsigned int m, unsigned int n, const typename data_manager<number>::twopf_configuration& config) const
			    {
		        std::ostringstream label;

		        unsigned int N_fields = this->mdl->get_N_fields();

		        label << std::setprecision(this->precision);

		        label << "$" << (this->twopf_meaning == real ? __CPP_TRANSPORT_LATEX_RE_SYMBOL : __CPP_TRANSPORT_LATEX_IM_SYMBOL) << " ";

		        const std::vector<std::string>& field_names = this->mdl->get_f_latex_names();

		        if(this->get_dot_meaning() == derived_line<number>::derivatives)
			        {
		            label << field_names[m % N_fields] << (m >= N_fields ? "^{" __CPP_TRANSPORT_LATEX_PRIME_SYMBOL "}" : "") << " "
			            << field_names[n % N_fields] << (n >= N_fields ? "^{" __CPP_TRANSPORT_LATEX_PRIME_SYMBOL "}" : "");
			        }
		        else
			        {
		            label << (m >= N_fields ? "p_{" : "") << field_names[m % N_fields] << (m >= N_fields ? "}" : "") << " "
			            << (n >= N_fields ? "p_{" : "") << field_names[n % N_fields] << (n >= N_fields ? "}" : "");
			        }

		        label << "\\;"
			        << __CPP_TRANSPORT_LATEX_K_SYMBOL << "=";
		        if(this->get_klabel_meaning() == derived_line<number>::conventional) label << output_latex_number(config.k_conventional, this->precision);
		        else label << output_latex_number(config.k_comoving, this->precision);

		        label << "$";

		        return (label.str());
			    }


				template <typename number>
				std::string twopf_time_series<number>::make_non_LaTeX_label(unsigned int m, unsigned int n, const typename data_manager<number>::twopf_configuration& config) const
					{
				    std::ostringstream label;

				    unsigned int N_fields = this->mdl->get_N_fields();

				    label << std::setprecision(this->precision);

				    label << (this->twopf_meaning == real ? __CPP_TRANSPORT_NONLATEX_RE_SYMBOL : __CPP_TRANSPORT_NONLATEX_IM_SYMBOL) << " ";

				    const std::vector<std::string>& field_names = this->mdl->get_field_names();

				    if(this->get_dot_meaning() == derived_line<number>::derivatives)
					    {
				        label << field_names[m % N_fields] << (m >= N_fields ? __CPP_TRANSPORT_NONLATEX_PRIME_SYMBOL : "") << ", "
					        << field_names[n % N_fields] << (n >= N_fields ? __CPP_TRANSPORT_NONLATEX_PRIME_SYMBOL : "");
					    }
				    else
					    {
				        label << (m >= N_fields ? "p_{" : "") << field_names[m % N_fields] << (m >= N_fields ? "}" : "") << " "
					        << (n >= N_fields ? "p_{" : "") << field_names[n % N_fields] << (n >= N_fields ? "}" : "");
					    }

				    label << " "
					    << __CPP_TRANSPORT_NONLATEX_K_SYMBOL << "=";
				    if(this->get_klabel_meaning() == derived_line<number>::conventional) label << config.k_conventional;
				    else label << config.k_comoving;

		        return(label.str());
			    }


		    template <typename number>
		    void twopf_time_series<number>::write(std::ostream& out)
			    {
		        this->time_series<number>::write(out);

		        out << "  " << __CPP_TRANSPORT_PRODUCT_TIME_SERIES_LABEL_TWOPF << std::endl;
		        out << "  " << __CPP_TRANSPORT_PRODUCT_LINE_COLLECTION_LABEL_INDICES << " ";
		        this->active_indices.write(out, this->mdl->get_state_names());
		        out << std::endl;

		        this->wrapper.wrap_out(out, "  " __CPP_TRANSPORT_PRODUCT_TIME_SERIES_KCONFIG_SN_LABEL " ");

		        unsigned int count = 0;
		        for(std::vector<unsigned int>::const_iterator t = this->kconfig_sample_sns.begin(); t != this->kconfig_sample_sns.end() && count < __CPP_TRANSPORT_PRODUCT_TDATA_MAX_SN; t++)
			        {
		            std::ostringstream msg;
		            msg << (*t);

		            this->wrapper.wrap_list_item(out, true, msg.str(), count);
			        }
		        if(count == __CPP_TRANSPORT_PRODUCT_TDATA_MAX_SN) this->wrapper.wrap_list_item(out, true, "...", count);
		        this->wrapper.wrap_newline(out);
			    }


		    template <typename number>
		    void twopf_time_series<number>::serialize(serialization_writer& writer) const
			    {
		        this->write_value_node(writer, __CPP_TRANSPORT_NODE_PRODUCT_DERIVED_LINE_TYPE,
		                               std::string(__CPP_TRANSPORT_NODE_PRODUCT_DERIVED_LINE_TWOPF_TIME_SERIES));

		        this->active_indices.serialize(writer);

		        switch(this->twopf_meaning)
			        {
		            case real:
			            {
		                this->write_value_node(writer, __CPP_TRANSPORT_NODE_PRODUCT_TSERIES_TWOPF_DATA_TYPE, std::string(__CPP_TRANSPORT_NODE_PRODUCT_TSERIES_TWOPF_DATA_REAL));
		                break;
			            }

		            case imaginary:
			            {
		                this->write_value_node(writer, __CPP_TRANSPORT_NODE_PRODUCT_TSERIES_TWOPF_DATA_TYPE, std::string(__CPP_TRANSPORT_NODE_PRODUCT_TSERIES_TWOPF_DATA_IMAGINARY));
		                break;
			            }

		            default:
			            throw runtime_exception(runtime_exception::RUNTIME_ERROR, __CPP_TRANSPORT_PRODUCT_DERIVED_LINE_TWOPF_TYPE_UNKNOWN);
			        }

		        this->begin_array(writer, __CPP_TRANSPORT_NODE_PRODUCT_TDATA_K_SERIAL_NUMBERS, this->kconfig_sample_sns.size() == 0);
		        for(std::vector<unsigned int>::const_iterator t = this->kconfig_sample_sns.begin(); t != this->kconfig_sample_sns.end(); t++)
			        {
		            this->begin_node(writer, "arrayelt", false);    // node name ignored for arrays
		            this->write_value_node(writer, __CPP_TRANSPORT_NODE_PRODUCT_TDATA_K_SERIAL_NUMBER, *t);
		            this->end_element(writer, "arrayelt");
			        }
		        this->end_element(writer, __CPP_TRANSPORT_NODE_PRODUCT_TDATA_K_SERIAL_NUMBERS);

		        this->time_series<number>::serialize(writer);
			    }


		    //! Utility type for 3pf time lines -- provides shifting function to compute the switch
		    //! from momenta to derivatives
		    template <typename number>
		    class basic_threepf_time_series: public time_series<number>
			    {

		      protected:

		        class extractor
			        {

		          public:

		            extractor(unsigned int n, const typename data_manager<number>::threepf_configuration& c)
			            : num(n), config(c)
			            {
				            if(this->num < 1) this->num = 1;
				            if(this->num > 3) this->num = 3;
			            }

				        ~extractor() = default;

		            double comoving() const
			            {
		                if(this->num == 1) return(this->config.k1_comoving);
		                if(this->num == 2) return(this->config.k2_comoving);
		                if(this->num == 3) return(this->config.k3_comoving);

		                assert(false);
		                return(0.0);
			            }

		            double conventional() const
			            {
		                if(this->num == 1) return(this->config.k1_conventional);
				            if(this->num == 2) return(this->config.k2_conventional);
				            if(this->num == 3) return(this->config.k3_conventional);

				            assert(false);
				            return(0.0);
			            }

				        unsigned int serial() const
					        {
						        if(this->num == 1) return(this->config.k1_serial);
						        if(this->num == 2) return(this->config.k2_serial);
						        if(this->num == 3) return(this->config.k3_serial);

						        assert(false);
						        return(0);
					        }

		          private:

		            unsigned int num;

				        const typename data_manager<number>::threepf_configuration& config;
			        };


		        typedef enum { left, middle, right } operator_position;

		        // CONSTRUCTOR, DESTRUCTOR

		      public:

            //! construct a basic_threepf_time_series object
            basic_threepf_time_series(const integration_task<number>& tk, model<number>* m, filter::time_filter tfilter,
                                      unsigned int prec = __CPP_TRANSPORT_DEFAULT_PLOT_PRECISION,
                                      typename derived_line<number>::value_type type = derived_line<number>::correlation_function)
              : use_kt_label(true), use_alpha_label(false), use_beta_label(false),
                time_series<number>(tk, m, tfilter, type, prec)
              {
                assert(m != nullptr);
              }

		        //! deserialization constructor
		        basic_threepf_time_series(serialization_reader* reader, typename repository<number>::task_finder finder);

		        //! destroy a basic threepf_time_series object
		        virtual ~basic_threepf_time_series() = default;


            // MANAGE LABELS

            //! get k_t label setting
            bool get_use_kt_label() const { return(this->use_kt_label); }
            //! set k_t label setting
            void set_use_kt_label(bool g) { this->use_kt_label = g; }
            //! get alpha label setting
            bool get_use_alpha_label() const { return(this->use_alpha_label); }
            //! set alpha label setting
            void set_use_alpha_label(bool g) { this->use_alpha_label = g; }
            //! get beta label setting
            bool get_use_beta_label() const { return(this->use_beta_label); }
            //! set beta label setting
            void set_use_beta_label(bool g) { this->use_beta_label = g; }


		        // DERIVE LINES

		      protected:

		        //! shift a three-pf timeline for coordinate labels (l,m,n)
		        //! and supplied 2pf k-configuration serial numbers
		        void shift_derivatives(typename data_manager<number>::datapipe& pipe,
                                   const std::vector<unsigned int>& time_sample, std::vector<number>& line_data,
                                   const std::vector<double>& time_axis,
                                   unsigned int l, unsigned int m, unsigned int n,
                                   const typename data_manager<number>::threepf_configuration& config) const;

		        //! apply the derivative shift to a threepf-timeline for a specific
		        //! configuration
		        void compute_derivative_shift(typename data_manager<number>::datapipe& pipe,
                                          const std::vector<unsigned int>& time_sample, std::vector<number>& line_data,
                                          const std::vector<double>& time_axis,
                                          const std::vector< typename std::vector<number> >& background,
		                                      unsigned int p, const extractor& p_config,
		                                      unsigned int q, const extractor& q_config,
		                                      unsigned int r, const extractor& r_config,
		                                      operator_position pos) const;


            // SERIALIZATION -- implements a 'serializable' interface

            //! Serialize this object
            virtual void serialize(serialization_writer& writer) const override;


				    // WRITE SELF TO OUTPUT STREAM

				    //! Write self to output stream
				    void write(std::ostream& out);

            // INTERNAL DATA

          protected:

            //! use k_t on line labels?
            bool use_kt_label;

            //! use alpha on line labels?
            bool use_alpha_label;

            //! use beta on line labels?
            bool use_beta_label;

			    };


        template <typename number>
        basic_threepf_time_series<number>::basic_threepf_time_series(serialization_reader* reader, typename repository<number>::task_finder finder)
          : time_series<number>(reader, finder)
          {
            assert(reader != nullptr);

            if(reader == nullptr) throw runtime_exception(runtime_exception::RUNTIME_ERROR, __CPP_TRANSPORT_PRODUCT_TIME_SERIES_NULL_READER);

            reader->read_value(__CPP_TRANSPORT_NODE_PRODUCT_TSERIES_THREEPF_LABEL_KT, this->use_kt_label);
            reader->read_value(__CPP_TRANSPORT_NODE_PRODUCT_TSERIES_THREEPF_LABEL_ALPHA, this->use_alpha_label);
            reader->read_value(__CPP_TRANSPORT_NODE_PRODUCT_TSERIES_THREEPF_LABEL_BETA, this->use_beta_label);
          }


				template <typename number>
				void basic_threepf_time_series<number>::write(std::ostream& out)
					{
						// FIXME: output label settings
						this->time_series<number>::write(out);
					}


		    template <typename number>
		    void basic_threepf_time_series<number>::shift_derivatives(typename data_manager<number>::datapipe& pipe,
		                                                              const std::vector<unsigned int>& time_sample, std::vector<number>& line_data,
		                                                              const std::vector<double>& time_axis,
		                                                              unsigned int l, unsigned int m, unsigned int n,
		                                                              const typename data_manager<number>::threepf_configuration& config) const
			    {
		        unsigned int N_fields = this->mdl->get_N_fields();

				    typename data_manager<number>::datapipe::time_data_handle& handle = pipe.new_time_data_handle(time_sample);

		        // pull the background time evolution from the database for the time_sample we are using.
            // for future convenience we want this to be a vector of vectors-representing-field-components,
            // ie., for each time step (outer vector) we have a group of field components (inner vector)
		        std::vector< std::vector<number> > background(time_sample.size());

		        for(unsigned int i = 0; i < 2*N_fields; i++)
			        {
				        typename data_manager<number>::datapipe::background_time_data_tag tag = pipe.new_background_time_data_tag(i); // DON'T flatten i, because we want to give the background to the model instance in the order it expects
		            const std::vector<number>& bg_line = handle.lookup_tag(tag);

                assert(bg_line.size() == background.size());
                for(unsigned int j = 0; j < time_sample.size(); j++)
                  {
                    background[j].push_back(bg_line[j]);
                  }
			        }

		        if(l >= N_fields) this->compute_derivative_shift(pipe, time_sample, line_data, time_axis, background, l, extractor(1, config), m, extractor(2, config), n, extractor(3, config), left);
		        if(m >= N_fields) this->compute_derivative_shift(pipe, time_sample, line_data, time_axis, background, m, extractor(2, config), l, extractor(1, config), n, extractor(3, config), middle);
		        if(n >= N_fields) this->compute_derivative_shift(pipe, time_sample, line_data, time_axis, background, n, extractor(3, config), l, extractor(1, config), m, extractor(2, config), right);
			    }


		    template <typename number>
		    void basic_threepf_time_series<number>::compute_derivative_shift(typename data_manager<number>::datapipe& pipe,
		                                                                     const std::vector<unsigned int>& time_sample, std::vector<number>& line_data,
		                                                                     const std::vector<double>& time_axis, const std::vector< std::vector<number> >& background,
		                                                                     unsigned int p, const extractor& p_config,
		                                                                     unsigned int q, const extractor& q_config,
		                                                                     unsigned int r, const extractor& r_config,
		                                                                     operator_position pos) const
			    {
		        assert(time_sample.size() == line_data.size());
		        unsigned int N_fields = this->mdl->get_N_fields();

		        // need multiple components of the 2pf at different k-configurations; allocate space to store them
		        std::vector< std::vector<number> > sigma_q_re(time_sample.size());
		        std::vector< std::vector<number> > sigma_r_re(time_sample.size());
		        std::vector< std::vector<number> > mom_sigma_q_re(time_sample.size());
		        std::vector< std::vector<number> > mom_sigma_r_re(time_sample.size());
            std::vector< std::vector<number> > sigma_q_im(time_sample.size());
            std::vector< std::vector<number> > sigma_r_im(time_sample.size());
		        std::vector< std::vector<number> > mom_sigma_q_im(time_sample.size());
		        std::vector< std::vector<number> > mom_sigma_r_im(time_sample.size());

		        // p is guaranteed to be a momentum label, but we will want to know what field species it corresponds to
		        unsigned int p_species = this->mdl->species(p);

		        typedef enum { first_index, second_index } fixed_index;

		        // we need to know which index on each twopf is fixed, and which is summed over
		        fixed_index q_fixed = first_index;
		        fixed_index r_fixed = first_index;

		        switch(pos)
			        {
		            case left:    // our operator is on the far left-hand side, so is to the left of both the q and r operators
			            q_fixed = second_index;
		              r_fixed = second_index;
		              break;

		            case middle:  // our operator is in the middle, to the right of the q operator but to the left of the r operator
			            q_fixed = first_index;
		              r_fixed = second_index;
		              break;

		            case right:   // our operator is on the right, to the right of both the q and r operators
			            q_fixed = first_index;
		              r_fixed = first_index;
		              break;

		            default:
			            throw runtime_exception(runtime_exception::RUNTIME_ERROR, __CPP_TRANSPORT_PRODUCT_TIME_SERIES_UNKNOWN_OPPOS);
			        }

				    typename data_manager<number>::datapipe::time_data_handle& t_handle = pipe.new_time_data_handle(time_sample);

		        // pull out components of the two-pf that we need
		        for(unsigned int i = 0; i < N_fields; i++)
			        {
		            unsigned int q_id = this->mdl->flatten((q_fixed == first_index ? q : i), (q_fixed == second_index ? q : i));
		            unsigned int r_id = this->mdl->flatten((r_fixed == first_index ? r : i), (r_fixed == second_index ? r : i));
		            unsigned int mom_q_id = this->mdl->flatten((q_fixed == first_index ? q : this->mdl->momentum(i)), (q_fixed == second_index ? q : this->mdl->momentum(i)));
		            unsigned int mom_r_id = this->mdl->flatten((r_fixed == first_index ? r : this->mdl->momentum(i)), (r_fixed == second_index ? r : this->mdl->momentum(i)));

				        typename data_manager<number>::datapipe::cf_time_data_tag q_re_tag     = pipe.new_cf_time_data_tag(data_manager<number>::datapipe::cf_twopf_re, q_id,     q_config.serial());
								typename data_manager<number>::datapipe::cf_time_data_tag q_im_tag     = pipe.new_cf_time_data_tag(data_manager<number>::datapipe::cf_twopf_im, q_id,     q_config.serial());
		            typename data_manager<number>::datapipe::cf_time_data_tag mom_q_re_tag = pipe.new_cf_time_data_tag(data_manager<number>::datapipe::cf_twopf_re, mom_q_id, q_config.serial());
		            typename data_manager<number>::datapipe::cf_time_data_tag mom_q_im_tag = pipe.new_cf_time_data_tag(data_manager<number>::datapipe::cf_twopf_im, mom_q_id, q_config.serial());

		            typename data_manager<number>::datapipe::cf_time_data_tag r_re_tag     = pipe.new_cf_time_data_tag(data_manager<number>::datapipe::cf_twopf_re, r_id,     r_config.serial());
		            typename data_manager<number>::datapipe::cf_time_data_tag r_im_tag     = pipe.new_cf_time_data_tag(data_manager<number>::datapipe::cf_twopf_im, r_id,     r_config.serial());
		            typename data_manager<number>::datapipe::cf_time_data_tag mom_r_re_tag = pipe.new_cf_time_data_tag(data_manager<number>::datapipe::cf_twopf_re, mom_r_id, r_config.serial());
		            typename data_manager<number>::datapipe::cf_time_data_tag mom_r_im_tag = pipe.new_cf_time_data_tag(data_manager<number>::datapipe::cf_twopf_im, mom_r_id, r_config.serial());

		            const std::vector<number>& q_line_re     = t_handle.lookup_tag(q_re_tag);
		            const std::vector<number>& q_line_im     = t_handle.lookup_tag(q_im_tag);
		            const std::vector<number>& mom_q_line_re = t_handle.lookup_tag(mom_q_re_tag);
		            const std::vector<number>& mom_q_line_im = t_handle.lookup_tag(mom_q_im_tag);
		            const std::vector<number>& r_line_re     = t_handle.lookup_tag(r_re_tag);
		            const std::vector<number>& r_line_im     = t_handle.lookup_tag(r_im_tag);
		            const std::vector<number>& mom_r_line_re = t_handle.lookup_tag(mom_r_re_tag);
		            const std::vector<number>& mom_r_line_im = t_handle.lookup_tag(mom_r_im_tag);

                for(unsigned int j = 0; j < line_data.size(); j++)
                  {
                    (sigma_q_re[j]).push_back(q_line_re[j]);
                    (sigma_q_im[j]).push_back(q_line_im[j]);
                    (mom_sigma_q_re[j]).push_back(mom_q_line_re[j]);
                    (mom_sigma_q_im[j]).push_back(mom_q_line_im[j]);
                    (sigma_r_re[j]).push_back(r_line_re[j]);
                    (sigma_r_im[j]).push_back(r_line_im[j]);
                    (mom_sigma_r_re[j]).push_back(mom_r_line_re[j]);
                    (mom_sigma_r_im[j]).push_back(mom_r_line_im[j]);
                  }
			        }

		        // work through the time sample, shifting each value appropriately
		        for(unsigned int i = 0; i < line_data.size(); i++)
			        {
		            std::vector< std::vector< std::vector<number> > > B_qrp;
		            std::vector< std::vector< std::vector<number> > > C_pqr;
                std::vector< std::vector< std::vector<number> > > C_prq;

                // evaluate B and C tensors for this time step
                // B is symmetric on its first two indices, which are the ones we sum over, so we only need a single copy of that.
                // C is symmetric on its first two indices but we sum over the last two. So we need to symmetrize the momenta.
		            this->mdl->B(this->parent_task->get_params(), background[i], q_config.comoving(), r_config.comoving(), p_config.comoving(), time_axis[i], B_qrp);
                this->mdl->C(this->parent_task->get_params(), background[i], p_config.comoving(), q_config.comoving(), r_config.comoving(), time_axis[i], C_pqr);
                this->mdl->C(this->parent_task->get_params(), background[i], p_config.comoving(), r_config.comoving(), q_config.comoving(), time_axis[i], C_prq);

                number shift = 0.0;

                for(unsigned int m = 0; m < N_fields; m++)
                  {
                    for(unsigned int n = 0; n < N_fields; n++)
                      {
                        shift -= B_qrp[m][n][p_species] * ( sigma_q_re[i][m]*sigma_r_re[i][n] - sigma_q_im[i][m]*sigma_r_im[i][n] );

                        shift -= C_pqr[p_species][m][n] * ( mom_sigma_q_re[i][m]*sigma_r_re[i][n] - mom_sigma_q_im[i][m]*sigma_r_im[i][n] );
                        shift -= C_prq[p_species][m][n] * ( sigma_q_re[i][n]*mom_sigma_r_re[i][m] - sigma_q_im[i][n]*mom_sigma_r_im[i][m] );
                      }
                  }

                line_data[i] += shift;
  		        }
			    }


        template <typename number>
        void basic_threepf_time_series<number>::serialize(serialization_writer& writer) const
          {
            this->write_value_node(writer, __CPP_TRANSPORT_NODE_PRODUCT_TSERIES_THREEPF_LABEL_KT, this->use_kt_label);
            this->write_value_node(writer, __CPP_TRANSPORT_NODE_PRODUCT_TSERIES_THREEPF_LABEL_ALPHA, this->use_alpha_label);
            this->write_value_node(writer, __CPP_TRANSPORT_NODE_PRODUCT_TSERIES_THREEPF_LABEL_BETA, this->use_beta_label);

            this->time_series<number>::serialize(writer);
          }


		    //! threepf time data line
		    template <typename number>
		    class threepf_time_series: public basic_threepf_time_series<number>
			    {

		        // CONSTRUCTOR, DESTRUCTOR

		      public:

		        //! construct a threepf time-data object
		        threepf_time_series(const threepf_task<number>& tk, model<number>* m, index_selector<3>& sel,
		                          filter::time_filter tfilter, filter::threepf_kconfig_filter kfilter,
		                          unsigned int prec=__CPP_TRANSPORT_DEFAULT_PLOT_PRECISION);

		        //! deserialization constructor.
		        threepf_time_series(serialization_reader* reader, typename repository<number>::task_finder finder);

		        virtual ~threepf_time_series() = default;


		        // DERIVE LINES -- implements a 'time_series' interface

		        //! generate data lines for plotting
            virtual void derive_lines(typename data_manager<number>::datapipe& pipe, std::list< data_line<number> >& lines,
                                      const std::list<std::string>& tags) const override;


		        // LABEL GENERATION

		      protected:

            //! Make a LaTeX label for a plot line
		        std::string make_LaTeX_label(unsigned int l, unsigned int m, unsigned int n,
		                                     const typename data_manager<number>::threepf_configuration& config) const;

		        //! Make a non-LaTeX label for a plot line
		        std::string make_non_LaTeX_label(unsigned int l, unsigned int m, unsigned int n,
		                                         const typename data_manager<number>::threepf_configuration& config) const;


		        // CLONE

		        //! self-replicate
		        virtual derived_line<number>* clone() const override { return new threepf_time_series<number>(static_cast<const threepf_time_series<number>&>(*this)); }


		        // WRITE TO A STREAM

		        //! write self-details to a stream
		        virtual void write(std::ostream& out) override;


		        // SERIALIZATION -- implements a 'serializable' interface

		      public:

		        //! serialize this object
		        virtual void serialize(serialization_writer& writer) const override;

		        // INTERNAL DATA

		      private:

		        //! record which indices are active in this group
		        index_selector<3> active_indices;

		        //! record serial numbers of k-configurations we are using
		        std::vector<unsigned int> kconfig_sample_sns;

			    };


		    template <typename number>
		    threepf_time_series<number>::threepf_time_series(const threepf_task<number>& tk, model<number>* m, index_selector<3>& sel,
		                                                 filter::time_filter tfilter, filter::threepf_kconfig_filter kfilter,
		                                                 unsigned int prec)
			    : active_indices(sel),
			      basic_threepf_time_series<number>(tk, m, tfilter, prec)
			    {
		        assert(m != nullptr);

		        if(m == nullptr) throw runtime_exception(runtime_exception::RUNTIME_ERROR, __CPP_TRANSPORT_PRODUCT_TIME_SERIES_NULL_MODEL);

		        if(active_indices.get_number_fields() != this->mdl->get_N_fields())
			        {
		            std::ostringstream msg;
		            msg << __CPP_TRANSPORT_PRODUCT_INDEX_MISMATCH << " ("
			            << __CPP_TRANSPORT_PRODUCT_INDEX_MISMATCH_A << " " << active_indices.get_number_fields() << ", "
			            << __CPP_TRANSPORT_PRODUCT_INDEX_MISMATCH_B << " " << this->mdl->get_N_fields() << ")";
		            throw runtime_exception(runtime_exception::RUNTIME_ERROR, msg.str());
			        }

		        // set up a list of serial numbers corresponding to the sample kconfigs for this plot
		        this->f.filter_threepf_kconfig_sample(kfilter, tk.get_sample(), kconfig_sample_sns);
			    }


		    template <typename number>
		    threepf_time_series<number>::threepf_time_series(serialization_reader* reader, typename repository<number>::task_finder finder)
			    : active_indices(reader), basic_threepf_time_series<number>(reader, finder)
			    {
		        assert(reader != nullptr);

		        if(reader == nullptr) throw runtime_exception(runtime_exception::RUNTIME_ERROR, __CPP_TRANSPORT_PRODUCT_TIME_SERIES_NULL_READER);

		        unsigned int sns = reader->start_array(__CPP_TRANSPORT_NODE_PRODUCT_TDATA_K_SERIAL_NUMBERS);

		        for(unsigned int i = 0; i < sns; i++)
			        {
		            reader->start_array_element();

		            unsigned int sn;
		            reader->read_value(__CPP_TRANSPORT_NODE_PRODUCT_TDATA_K_SERIAL_NUMBER, sn);
		            kconfig_sample_sns.push_back(sn);

		            reader->end_array_element();
			        }

            reader->end_element(__CPP_TRANSPORT_NODE_PRODUCT_TDATA_K_SERIAL_NUMBERS);
			    }


        template <typename number>
        void threepf_time_series<number>::derive_lines(typename data_manager<number>::datapipe& pipe, std::list< data_line<number> >& lines,
                                                     const std::list<std::string>& tags) const
			    {
            // attach our datapipe to an output group
            this->attach(pipe, tags);

		        // pull time-axis data
            const std::vector<double>& time_axis = this->pull_time_axis(pipe);

		        // set up cache handles
		        typename data_manager<number>::datapipe::threepf_kconfig_handle& k_handle = pipe.new_threepf_kconfig_handle(this->kconfig_sample_sns);
		        typename data_manager<number>::datapipe::time_data_handle& t_handle = pipe.new_time_data_handle(this->get_time_sample_sns());

		        // pull k-configuration information from the database
		        typename data_manager<number>::datapipe::threepf_kconfig_tag k_tag = pipe.new_threepf_kconfig_tag();

		        const typename std::vector< typename data_manager<number>::threepf_configuration >& k_values = k_handle.lookup_tag(k_tag);

		        // loop through all components of the threepf, for each k-configuration we use,
		        // pulling data from the database

		        for(unsigned int i = 0; i < this->kconfig_sample_sns.size(); i++)
			        {
		            for(unsigned int l = 0; l < 2*this->mdl->get_N_fields(); l++)
			            {
		                for(unsigned int m = 0; m < 2*this->mdl->get_N_fields(); m++)
			                {
		                    for(unsigned int n = 0; n < 2*this->mdl->get_N_fields(); n++)
			                    {
		                        std::array<unsigned int, 3> index_set = { l, m, n };
		                        if(this->active_indices.is_on(index_set))
			                        {
				                        typename data_manager<number>::datapipe::cf_time_data_tag tag = pipe.new_cf_time_data_tag(data_manager<number>::datapipe::cf_threepf, this->mdl->flatten(l,m,n), this->kconfig_sample_sns[i]);

				                        // have to take a copy of the data, rather than use a reference, because we are going to modify it in-place
		                            std::vector<number> line_data = t_handle.lookup_tag(tag);

		                            // the integrator produces correlation functions involving the canonical momenta,
		                            // not the derivatives. If the user wants derivatives then we have to shift.
		                            if(this->get_dot_meaning() == derived_line<number>::derivatives)
			                            this->shift_derivatives(pipe, this->get_time_sample_sns(), line_data, time_axis, l, m, n, k_values[i]);

		                            data_line<number> line = data_line<number>(data_line<number>::time_series, data_line<number>::correlation_function,
		                                                                       time_axis, line_data,
		                                                                       this->make_LaTeX_label(l, m, n, k_values[i]), this->make_non_LaTeX_label(l, m, n, k_values[i]));

		                            lines.push_back(line);
			                        }
			                    }
			                }
			            }
			        }

            // detach pipe frmo output group
            this->detach(pipe);
			    }


		    template <typename number>
		    std::string threepf_time_series<number>::make_LaTeX_label(unsigned int l, unsigned int m, unsigned int n, const typename data_manager<number>::threepf_configuration& config) const
			    {
		        std::ostringstream label;

		        unsigned int N_fields = this->mdl->get_N_fields();

		        label << std::setprecision(this->precision);

		        const std::vector<std::string>& field_names = this->mdl->get_f_latex_names();

		        label << "$";

		        if(this->get_dot_meaning() == derived_line<number>::derivatives)
			        {
		            label << field_names[l % N_fields] << (l >= N_fields ? "^{" __CPP_TRANSPORT_LATEX_PRIME_SYMBOL "}" : "") << " "
			            << field_names[m % N_fields] << (m >= N_fields ? "^{" __CPP_TRANSPORT_LATEX_PRIME_SYMBOL "}" : "") << " "
			            << field_names[n % N_fields] << (n >= N_fields ? "^{" __CPP_TRANSPORT_LATEX_PRIME_SYMBOL "}" : "");
			        }
		        else
			        {
		            label << (l >= N_fields ? "p_{" : "") << field_names[l % N_fields] << (l >= N_fields ? "}" : "") << " "
			            << (m >= N_fields ? "p_{" : "") << field_names[m % N_fields] << (m >= N_fields ? "}" : "") << " "
			            << (n >= N_fields ? "p_{" : "") << field_names[n % N_fields] << (n >= N_fields ? "}" : "");
			        }

		        label << "\\;";
		        unsigned int count = 0;
		        if(this->use_kt_label)
			        {
		            label << (count > 0 ? ",\\, " : "") << __CPP_TRANSPORT_LATEX_KT_SYMBOL << "=";
		            if(this->get_klabel_meaning() == derived_line<number>::conventional) label << output_latex_number(config.kt_conventional, this->precision);
		            else label << output_latex_number(config.kt_comoving, this->precision);
		            count++;
			        }
		        if(this->use_alpha_label)
			        {
		            label << (count > 0 ? ",\\, " : "") << __CPP_TRANSPORT_LATEX_ALPHA_SYMBOL << "=" << output_latex_number(config.alpha, this->precision);
		            count++;
			        }
		        if(this->use_beta_label)
			        {
		            label << (count > 0 ? ",\\, " : "") << __CPP_TRANSPORT_LATEX_BETA_SYMBOL << "=" << output_latex_number(config.beta, this->precision);
		            count++;
			        }

		        label << "$";

		        return (label.str());
			    }


		    template <typename number>
		    std::string threepf_time_series<number>::make_non_LaTeX_label(unsigned int l, unsigned int m, unsigned int n, const typename data_manager<number>::threepf_configuration& config) const
			    {
		        std::ostringstream label;

		        unsigned int N_fields = this->mdl->get_N_fields();

		        label << std::setprecision(this->precision);

		        const std::vector<std::string>& field_names = this->mdl->get_field_names();

		        if(this->get_dot_meaning() == derived_line<number>::derivatives)
			        {
		            label << field_names[l % N_fields] << (l >= N_fields ? __CPP_TRANSPORT_NONLATEX_PRIME_SYMBOL : "") << ", "
			            << field_names[m % N_fields] << (m >= N_fields ? __CPP_TRANSPORT_NONLATEX_PRIME_SYMBOL : "") << ", "
			            << field_names[n % N_fields] << (n >= N_fields ? __CPP_TRANSPORT_NONLATEX_PRIME_SYMBOL : "");
			        }
		        else
			        {
		            label << (l >= N_fields ? "p_{" : "") << field_names[l % N_fields] << (l >= N_fields ? "}" : "") << ", "
			            << (m >= N_fields ? "p_{" : "") << field_names[m % N_fields] << (m >= N_fields ? "}" : "") << ", "
			            << (n >= N_fields ? "p_{" : "") << field_names[n % N_fields] << (n >= N_fields ? "}" : "");
			        }

		        label << " ";
		        unsigned int count = 0;
		        if(this->use_kt_label)
			        {
		            label << (count > 0 ? ", " : "") << __CPP_TRANSPORT_NONLATEX_KT_SYMBOL << "=";
		            if(this->get_klabel_meaning() == derived_line<number>::conventional) label << config.kt_conventional;
		            else label << config.kt_comoving;
		            count++;
			        }
		        if(this->use_alpha_label)
			        {
		            label << (count > 0 ? ", " : "") << __CPP_TRANSPORT_NONLATEX_ALPHA_SYMBOL << "=" << config.alpha;
		            count++;
			        }
		        if(this->use_beta_label)
			        {
		            label << (count > 0 ? ", " : "") << __CPP_TRANSPORT_NONLATEX_BETA_SYMBOL << "=" << config.beta;
		            count++;
			        }

		        return (label.str());
			    }


		    template <typename number>
		    void threepf_time_series<number>::write(std::ostream& out)
			    {
		        this->basic_threepf_time_series <number>::write(out);

		        out << "  " << __CPP_TRANSPORT_PRODUCT_TIME_SERIES_LABEL_THREEPF << std::endl;
		        out << "  " << __CPP_TRANSPORT_PRODUCT_LINE_COLLECTION_LABEL_INDICES << " ";
		        this->active_indices.write(out, this->mdl->get_state_names());
		        out << std::endl;

		        this->wrapper.wrap_out(out, "  " __CPP_TRANSPORT_PRODUCT_TIME_SERIES_KCONFIG_SN_LABEL " ");

		        unsigned int count = 0;
		        for(std::vector<unsigned int>::const_iterator t = this->kconfig_sample_sns.begin(); t != this->kconfig_sample_sns.end() && count < __CPP_TRANSPORT_PRODUCT_TDATA_MAX_SN; t++)
			        {
		            std::ostringstream msg;
		            msg << (*t);

		            this->wrapper.wrap_list_item(out, true, msg.str(), count);
			        }
		        if(count == __CPP_TRANSPORT_PRODUCT_TDATA_MAX_SN) this->wrapper.wrap_list_item(out, true, "...", count);
		        this->wrapper.wrap_newline(out);
			    }


		    template <typename number>
		    void threepf_time_series<number>::serialize(serialization_writer& writer) const
			    {
		        this->write_value_node(writer, __CPP_TRANSPORT_NODE_PRODUCT_DERIVED_LINE_TYPE,
		                               std::string(__CPP_TRANSPORT_NODE_PRODUCT_DERIVED_LINE_THREEPF_TIME_SERIES));

		        this->active_indices.serialize(writer);

		        this->begin_array(writer, __CPP_TRANSPORT_NODE_PRODUCT_TDATA_K_SERIAL_NUMBERS, this->kconfig_sample_sns.size() == 0);
		        for(std::vector<unsigned int>::const_iterator t = this->kconfig_sample_sns.begin(); t != this->kconfig_sample_sns.end(); t++)
			        {
		            this->begin_node(writer, "arrayelt", false);    // node name ignored for arrays
		            this->write_value_node(writer, __CPP_TRANSPORT_NODE_PRODUCT_TDATA_K_SERIAL_NUMBER, *t);
		            this->end_element(writer, "arrayelt");
			        }
		        this->end_element(writer, __CPP_TRANSPORT_NODE_PRODUCT_TDATA_K_SERIAL_NUMBERS);

		        this->basic_threepf_time_series <number>::serialize(writer);
			    }


			}   // namespace derived_data

	}   // namespace transport


#endif // __field_time_series_H_
