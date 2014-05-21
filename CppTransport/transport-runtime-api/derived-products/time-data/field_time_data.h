//
// Created by David Seery on 19/05/2014.
// Copyright (c) 2014 University of Sussex. All rights reserved.
//


#ifndef __field_time_data_H_
#define __field_time_data_H_


#include <iostream>
#include <sstream>
#include <string>
#include <list>
#include <vector>
#include <stdexcept>

#include "transport-runtime-api/derived-products/time-data/general_time_data.h"


#define __CPP_TRANSPORT_NODE_PRODUCT_TDATA_TWOPF_DATA_TYPE      "twopf-components"
#define __CPP_TRANSPORT_NODE_PRODUCT_TDATA_TWOPF_DATA_REAL      "real"
#define __CPP_TRANSPORT_NODE_PRODUCT_TDATA_TWOPF_DATA_IMAGINARY "imaginary"

#define __CPP_TRANSPORT_NODE_PRODUCT_TDATA_THREEPF_LABEL_KT     "use-kt-label"
#define __CPP_TRANSPORT_NODE_PRODUCT_TDATA_THREEPF_LABEL_ALPHA  "use-alpha-label"
#define __CPP_TRANSPORT_NODE_PRODUCT_TDATA_THREEPF_LABEL_BETA   "use-beta-label"


namespace transport
	{

		namespace derived_data
			{

		    //! background time data line
		    template <typename number>
		    class background_time_data: public general_time_data<number>
			    {

		        // CONSTRUCTOR, DESTRUCTOR

		      public:

		        //! construct a background time-data object
		        background_time_data(const integration_task<number>& tk, model<number>* m, index_selector<1>& sel, filter::time_filter tfilter);

		        //! deserialization constructor.
		        background_time_data(serialization_reader* reader, typename repository<number>::task_finder finder);

		        virtual ~background_time_data() = default;


		        // DERIVE LINES -- implements a 'general_time_data' interface

		        //! generate data lines for plotting
		        virtual void derive_lines(typename data_manager<number>::datapipe& pipe, const std::vector<unsigned int>& time_sample,
		                                  plot2d_product<number>& plot, std::list< time_data_line<number> >& lines) const override;


		        // LABEL GENERATION

		      protected:

		        std::string make_label(unsigned int i, plot2d_product<number>& plot, model<number>* m) const;


		        // CLONE

		        //! self-replicate
		        virtual general_time_data<number>* clone() const override { return new background_time_data<number>(static_cast<const background_time_data<number>&>(*this)); }


		        // WRITE TO A STREAM

		        //! write self-details to a stream
		        virtual void write(std::ostream& out, wrapped_output& wrapper) override;


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
		    background_time_data<number>::background_time_data(const integration_task<number>& tk, model<number>* m, index_selector<1>& sel,
		                                                       filter::time_filter tfilter)
			    : active_indices(sel), general_time_data<number>(tk, m, tfilter)
			    {
		        assert(m != nullptr);

		        if(active_indices.get_number_fields() != mdl->get_N_fields())
			        {
		            std::ostringstream msg;
		            msg << __CPP_TRANSPORT_PRODUCT_GENERAL_TPLOT_INDEX_MISMATCH << " ("
			              << __CPP_TRANSPORT_PRODUCT_GENERAL_TPLOT_INDEX_MISMATCH_A << " " << active_indices.get_number_fields() << ", "
			              << __CPP_TRANSPORT_PRODUCT_GENERAL_TPLOT_INDEX_MISMATCH_B << " " << mdl->get_N_fields() << ")";
		            throw runtime_exception(runtime_exception::RUNTIME_ERROR, msg.str());
			        }
			    }


		    template <typename number>
		    background_time_data<number>::background_time_data(serialization_reader* reader, typename repository<number>::task_finder finder)
			    : active_indices(reader), general_time_data<number>(reader)
			    {
		        if(reader == nullptr) throw runtime_exception(runtime_exception::RUNTIME_ERROR, __CPP_TRANSPORT_PRODUCT_GENERAL_TPLOT_NULL_READER);
			    }


		    template <typename number>
		    void background_time_data<number>::write(std::ostream& out, wrapped_output& wrapper)
			    {
		        out << "  " << __CPP_TRANSPORT_PRODUCT_GENERAL_TPLOT_LABEL_BACKGROUND << std::endl;
		        out << "  " << __CPP_TRANSPORT_PRODUCT_GENERAL_TPLOT_LABEL_INDICES << " ";
		        this->active_indices.write(out, mdl->get_state_names());
			    }


		    template <typename number>
		    void background_time_data<number>::serialize(serialization_writer& writer) const
			    {
		        this->write_value_node(writer, __CPP_TRANSPORT_NODE_PRODUCT_TDATA_TYPE,
		                               std::string(__CPP_TRANSPORT_NODE_PRODUCT_TDATA_BACKGROUND));

		        this->active_indices.serialize(writer);

		        this->general_time_data<number>::serialize(writer);
			    }


		    template <typename number>
		    void background_time_data<number>::derive_lines(typename data_manager<number>::datapipe& pipe,  const std::vector<unsigned int>& time_sample,
		                                                    plot2d_product<number>& plot, std::list< time_data_line<number> >& lines) const
			    {
		        // loop through all the fields, pulling data from the database for those which are enabled

		        for(unsigned int m = 0; m < 2*this->mdl->get_N_fields(); m++)
			        {
		            std::array<unsigned int, 1> index_set = { m };
		            if(this->active_indices.is_on(index_set))
			            {
		                std::vector<number> line_data;

		                pipe.pull_background_time_sample(this->mdl->flatten(m), time_sample, line_data);

		                time_data_line<number> line = time_data_line<number>(line_data, this->make_label(m, plot, this->mdl));

		                lines.push_back(line);
			            }
			        }
			    }


		    template <typename number>
		    std::string background_time_data<number>::make_label(unsigned int i, plot2d_product<number>& plot, model<number>* m) const
			    {
		        std::ostringstream label;

		        assert(m != nullptr);
		        if(m == nullptr) throw runtime_exception(runtime_exception::RUNTIME_ERROR, __CPP_TRANSPORT_PRODUCT_PLOT2D_NULL_MODEL);

		        unsigned int N_fields = m->get_N_fields();

		        if(plot.get_use_LaTeX())
			        {
		            const std::vector<std::string>& field_names = m->get_f_latex_names();

		            label << "$";
		            if(this->get_dot_meaning() == general_time_data<number>::derivatives)
			            {
		                label << field_names[i % N_fields] << (i >= N_fields ? "^{" __CPP_TRANSPORT_LATEX_PRIME_SYMBOL "}" : "");
			            }
		            else
			            {
		                label << (i >= N_fields ? "p_{" : "") << field_names[i % N_fields] << (i >= N_fields ? "}" : "");
			            }
		            label << "$";
			        }
		        else
			        {
		            const std::vector<std::string>& field_names = m->get_field_names();

		            if(this->get_dot_meaning() == general_time_data<number>::derivatives)
			            {
		                label << field_names[i % N_fields] << (i >= N_fields ? __CPP_TRANSPORT_NONLATEX_PRIME_SYMBOL : "");
			            }
		            else
			            {
		                label << (i >= N_fields ? "p_{" : "") << field_names[i % N_fields] << (i >= N_fields ? "}" : "");
			            }
			        }

		        return(label.str());
			    }


		    //! twopf time data line
		    template <typename number>
		    class twopf_time_data: public general_time_data<number>
			    {

		      public:

		        //! which twopf?
		        typedef enum { real, imaginary } twopf_type;

		        // CONSTRUCTOR, DESTRUCTOR

		      public:

		        //! construct a twopf time-data object
		        twopf_time_data(const twopf_list_task<number>& tk, model<number>* m, index_selector<2>& sel,
		                        filter::time_filter tfilter,
		                        filter::twopf_kconfig_filter kfilter,
		                        unsigned int prec = __CPP_TRANSPORT_DEFAULT_PLOT_PRECISION);

		        //! deserialization constuctor.
		        twopf_time_data(serialization_reader* reader, typename repository<number>::task_finder finder);

		        virtual ~twopf_time_data() = default;


		        // MANAGE SETTINGS

		        //! get twopf type setting
		        twopf_type get_type() const { return(this->twopf_meaning); }
		        //! set twopf type setting
		        void set_type(twopf_type m) { this->twopf_meaning = m; }

		        // DERIVE LINES -- implements a 'general_time_data' interface

		        //! generate data lines for plotting
		        virtual void derive_lines(typename data_manager<number>::datapipe& pipe, const std::vector<unsigned int>& time_sample,
		                                  plot2d_product<number>& plot, std::list< time_data_line<number> >& lines) const override;

		        // LABEL GENERATION

		      protected:

		        std::string make_label(unsigned int m, unsigned int n, plot2d_product<number>& plot,
		                               typename data_manager<number>::twopf_configuration& config, model<number>* mdl) const;


		        // CLONE

		        //! self-replicate
		        virtual general_time_data<number>* clone() const override { return new twopf_time_data<number>(static_cast<const twopf_time_data<number>&>(*this)); }


		        // WRITE TO A STREAM

		        //! write self-details to a stream
		        virtual void write(std::ostream& out, wrapped_output& wrapper) override;


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

		        //! record precision of labels
		        unsigned int precision;
			    };


		    template <typename number>
		    twopf_time_data<number>::twopf_time_data(const twopf_list_task<number>& tk, model<number>* m, index_selector<2>& sel,
		                                             filter::time_filter tfilter, filter::twopf_kconfig_filter kfilter, unsigned int prec)
			    : active_indices(sel), twopf_meaning(real), precision(prec), general_time_data<number>(tk, m, tfilter)
			    {
		        assert(m != nullptr);

		        if(m == nullptr) throw runtime_exception(runtime_exception::RUNTIME_ERROR, __CPP_TRANSPORT_PRODUCT_GENERAL_TPLOT_NULL_MODEL);

		        if(active_indices.get_number_fields() != mdl->get_N_fields())
			        {
		            std::ostringstream msg;
		            msg << __CPP_TRANSPORT_PRODUCT_GENERAL_TPLOT_INDEX_MISMATCH << " ("
			            << __CPP_TRANSPORT_PRODUCT_GENERAL_TPLOT_INDEX_MISMATCH_A << " " << active_indices.get_number_fields() << ", "
			            << __CPP_TRANSPORT_PRODUCT_GENERAL_TPLOT_INDEX_MISMATCH_B << " " << mdl->get_N_fields() << ")";
		            throw runtime_exception(runtime_exception::RUNTIME_ERROR, msg.str());
			        }

		        this->f.filter_twopf_kconfig_sample(kfilter, tk.get_k_list(), kconfig_sample_sns);
			    }


		    template <typename number>
		    twopf_time_data<number>::twopf_time_data(serialization_reader* reader, typename repository<number>::task_finder finder)
			    : active_indices(reader), general_time_data<number>(reader)
			    {
		        assert(reader != nullptr);

		        if(reader == nullptr) throw runtime_exception(runtime_exception::RUNTIME_ERROR, __CPP_TRANSPORT_PRODUCT_GENERAL_TPLOT_NULL_READER);

		        reader->read_value(__CPP_TRANSPORT_NODE_PRODUCT_TDATA_LABEL_PRECISION, precision);

		        std::string tpf_type;
		        reader->read_value(__CPP_TRANSPORT_NODE_PRODUCT_TDATA_TWOPF_DATA_TYPE, tpf_type);

		        if(tpf_type == __CPP_TRANSPORT_NODE_PRODUCT_TDATA_TWOPF_DATA_REAL) twopf_meaning = real;
		        else if(tpf_type == __CPP_TRANSPORT_NODE_PRODUCT_TDATA_TWOPF_DATA_IMAGINARY) twopf_meaning = imaginary;
		        else
			        {
		            std::ostringstream msg;
		            msg << __CPP_TRANSPORT_PRODUCT_GENERAL_TPLOT_TWOPF_DATA_UNKNOWN << " '" << tpf_type << "'";
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
		    void twopf_time_data<number>::derive_lines(typename data_manager<number>::datapipe& pipe, const std::vector<unsigned int>& time_sample,
		                                               plot2d_product<number>& plot, std::list<time_data_line<number> >& lines) const
			    {
		        // pull k-configuration information from the database
		        typename std::vector< typename data_manager<number>::twopf_configuration > k_values;
		        pipe.pull_twopf_kconfig_sample(this->kconfig_sample_sns, k_values);

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
		                        std::vector<number> line_data;

		                        pipe.pull_twopf_time_sample(this->mdl->flatten(m, n), time_sample, this->kconfig_sample_sns[i], line_data,
		                                                    (this->twopf_meaning == real ? data_manager<number>::twopf_real : data_manager<number>::twopf_imag));

		                        time_data_line<number> line = time_data_line<number>(line_data, this->make_label(m, n, plot, k_values[i], this->mdl));

		                        lines.push_back(line);
			                    }
			                }
			            }
			        }
			    }


		    template <typename number>
		    std::string twopf_time_data<number>::make_label(unsigned int m, unsigned int n, plot2d_product<number>& plot,
		                                                    typename data_manager<number>::twopf_configuration& config, model<number>* mdl) const
			    {
		        std::ostringstream label;

		        assert(mdl != nullptr);
		        if(mdl == nullptr) throw runtime_exception(runtime_exception::RUNTIME_ERROR, __CPP_TRANSPORT_PRODUCT_PLOT2D_NULL_MODEL);

		        unsigned int N_fields = mdl->get_N_fields();

		        label << std::setprecision(this->precision);

		        if(plot.get_use_LaTeX())
			        {
		            label << "$" << (this->twopf_meaning == real ? __CPP_TRANSPORT_LATEX_RE_SYMBOL : __CPP_TRANSPORT_LATEX_IM_SYMBOL) << " ";

		            const std::vector<std::string>& field_names = mdl->get_f_latex_names();

		            if(this->get_dot_meaning() == general_time_data<number>::derivatives)
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
		            if(this->get_klabel_meaning() == general_time_data<number>::conventional) label << config.conventional;
		            else                                                                      label << config.comoving;

		            label << "$";
			        }
		        else
			        {
		            label << (this->twopf_meaning == real ? __CPP_TRANSPORT_NONLATEX_RE_SYMBOL : __CPP_TRANSPORT_NONLATEX_IM_SYMBOL) << " ";

		            const std::vector<std::string>& field_names = mdl->get_field_names();

		            if(this->get_dot_meaning() == general_time_data<number>::derivatives)
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
		            if(this->get_klabel_meaning() == general_time_data<number>::conventional) label << config.conventional;
		            else                                                                      label << config.comoving;
			        }

		        return(label.str());
			    }


		    template <typename number>
		    void twopf_time_data<number>::write(std::ostream& out, wrapped_output& wrapper)
			    {
		        out << "  " << __CPP_TRANSPORT_PRODUCT_GENERAL_TPLOT_LABEL_TWOPF << std::endl;
		        out << "  " << __CPP_TRANSPORT_PRODUCT_GENERAL_TPLOT_LABEL_INDICES << " ";
		        this->active_indices.write(out, mdl->get_state_names());
		        out << std::endl;

		        wrapper.wrap_out(out, "  " __CPP_TRANSPORT_PRODUCT_GENERAL_TPLOT_KCONFIG_SN_LABEL " ");

		        unsigned int count = 0;
		        for(std::vector<unsigned int>::const_iterator t = this->kconfig_sample_sns.begin(); t != this->kconfig_sample_sns.end() && count < __CPP_TRANSPORT_PRODUCT_TIMEPLOT_MAX_SN; t++)
			        {
		            std::ostringstream msg;
		            msg << (*t);

		            wrapper.wrap_list_item(out, true, msg.str(), count);
			        }
		        if(count == __CPP_TRANSPORT_PRODUCT_TIMEPLOT_MAX_SN) wrapper.wrap_list_item(out, true, "...", count);
		        wrapper.wrap_newline(out);
			    }


		    template <typename number>
		    void twopf_time_data<number>::serialize(serialization_writer& writer) const
			    {
		        this->write_value_node(writer, __CPP_TRANSPORT_NODE_PRODUCT_TDATA_TYPE,
		                               std::string(__CPP_TRANSPORT_NODE_PRODUCT_TDATA_TWOPF));

		        this->write_value_node(writer, __CPP_TRANSPORT_NODE_PRODUCT_TDATA_LABEL_PRECISION, this->precision);

		        this->active_indices.serialize(writer);

		        switch(this->twopf_meaning)
			        {
		            case real:
			            {
		                this->write_value_node(writer, __CPP_TRANSPORT_NODE_PRODUCT_TDATA_TWOPF_DATA_TYPE, std::string(__CPP_TRANSPORT_NODE_PRODUCT_TDATA_TWOPF_DATA_REAL));
		                break;
			            }

		            case imaginary:
			            {
		                this->write_value_node(writer, __CPP_TRANSPORT_NODE_PRODUCT_TDATA_TWOPF_DATA_TYPE, std::string(__CPP_TRANSPORT_NODE_PRODUCT_TDATA_TWOPF_DATA_IMAGINARY));
		                break;
			            }

		            default:
			            throw runtime_exception(runtime_exception::RUNTIME_ERROR, __CPP_TRANSPORT_PRODUCT_GENERAL_TPLOT_TWOPF_DATA_UNKNOWN);
			        }

		        this->begin_array(writer, __CPP_TRANSPORT_NODE_PRODUCT_TDATA_K_SERIAL_NUMBERS, this->kconfig_sample_sns.size() == 0);
		        for(std::vector<unsigned int>::const_iterator t = this->kconfig_sample_sns.begin(); t != this->kconfig_sample_sns.end(); t++)
			        {
		            this->begin_node(writer, "arrayelt", false);    // node name ignored for arrays
		            this->write_value_node(writer, __CPP_TRANSPORT_NODE_PRODUCT_TDATA_K_SERIAL_NUMBER, *t);
		            this->end_element(writer, "arrayelt");
			        }
		        this->end_element(writer, __CPP_TRANSPORT_NODE_PRODUCT_TDATA_K_SERIAL_NUMBERS);

		        this->general_time_data<number>::serialize(writer);
			    }


		    //! Utility type for 3pf time lines -- provides shifting function to compute the switch
		    //! from momenta to derivatives
		    template <typename number>
		    class basic_threepf_time_data : public general_time_data<number>
			    {

		        typedef enum { left, middle, right } operator_position;

		        // CONSTRUCTOR, DESTRUCTOR

		      public:

		        //! construct a basic_threepf_time_data object
		        basic_threepf_time_data(const integration_task<number>& tk, model<number>* m, filter::time_filter tfilter)
			        : general_time_data<number>(tk, m, tfilter)
			        {
				        assert(m != nullptr);
			        }

		        //! deserialization constructor
		        basic_threepf_time_data(serialization_reader* reader, typename repository<number>::task_finder finder)
			        : general_time_data<number>(reader)
			        {
			        }

		        //! destroy a basic threepf_time_data object
		        virtual ~threepf_time_data() = default;


		        // DERIVE LINES

		      protected:

		        //! shift a three-pf timeline for coordinate labels (l,m,n)
		        //! and supplied 2pf k-configuration serial numbers
		        void shift_derivatives(typename data_manager<number>::datapipe& pipe, const std::vector<unsigned int>& time_sample,
		                               std::vector<number>& line_data, unsigned int l, unsigned int m, unsigned int n,
		                               const typename data_manager<number>::twopf_configuration& l_config,
		                               const typename data_manager<number>::twopf_configuration& m_config,
		                               const typename data_manager<number>::twopf_configuration& n_config) const;

		        //! apply the derivative shift to a threepf-timeline for a specific
		        //! configuration
		        void compute_derivative_shift(typename data_manager<number>::datapipe& pipe, const std::vector<unsigned int>& time_sample,
		                                      std::vector<number>& line_data, const std::vector< typename std::vector<number> >& background,
		                                      unsigned int p, const typename data_manager<number>::twopf_configuration& p_config,
		                                      unsigned int q, const typename data_manager<number>::twopf_configuration& q_config,
		                                      unsigned int r, const typename data_manager<number>::twopf_configuration& r_config,
		                                      operator_position pos) const;

			    };


		    template <typename number>
		    void basic_threepf_time_data<number>::shift_derivatives(typename data_manager<number>::datapipe& pipe, const std::vector<unsigned int>& time_sample,
		                                                            std::vector<number>& line_data, unsigned int l, unsigned int m, unsigned int n,
		                                                            const typename data_manager<number>::twopf_configuration& l_config,
		                                                            const typename data_manager<number>::twopf_configuration& m_config,
		                                                            const typename data_manager<number>::twopf_configuration& n_config) const
			    {
		        unsigned int N_fields = this->mdl->get_N_fields();

		        // pull the background time evolution from the database for the time_sample we are using
		        std::vector< std::vector<number> > background;

		        for(unsigned int i = 0; i < N_fields; i++)
			        {
		            std::vector<number> bg_line;
		            pipe.pull_background_time_sample(i, time_sample, bg_line);    // DON'T flatten, because we want to give the background to the model instance in the order it expects
		            background.push_back(bg_line);
			        }

		        if(l > N_fields) this->compute_derivative_shift(pipe, time_sample, line_data, background, l, l_config, m, m_config, n, n_config, left);
		        if(m > N_fields) this->compute_derivative_shift(pipe, time_sample, line_data, background, m, m_config, l, l_config, n, n_config, middle);
		        if(n > N_fields) this->compute_derivative_shift(pipe, time_sample, line_data, background, n, n_config, l, l_config, m, m_config, right);
			    }


		    template <typename number>
		    void basic_threepf_time_data<number>::compute_derivative_shift(typename data_manager<number>::datapipe& pipe, const std::vector<unsigned int>& time_sample,
		                                                                   std::vector<number>& line_data, const std::vector<typename std::vector<number> >& background,
		                                                                   unsigned int p, const typename data_manager<number>::twopf_configuration& p_config,
		                                                                   unsigned int q, const typename data_manager<number>::twopf_configuration& q_config,
		                                                                   unsigned int r, const typename data_manager<number>::twopf_configuration& r_config,
		                                                                   operator_position pos) const
			    {
		        assert(time_sample.size() == line_data().size());
		        unsigned int N_fields = this->mdl->get_N_fields();

		        // need multiple components of the 2pf at different k-configurations:
		        std::vector< std::vector<number> > sigma_q;
		        std::vector< std::vector<number> > sigma_r;

		        // p is guaranteed to be a momentum label, but we will want to know what field species it corresponds to
		        unsigned int p_species = p % N_fields;

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
			            throw runtime_exception(runtime_exception::RUNTIME_ERROR, __CPP_TRANSPORT_PRODUCT_GENERAL_TPLOT_UNKNOWN_OPPOS);
			        }

		        // pull out components of the two-pf that we need
		        for(unsigned int i = 0; i < N_fields; i++)
			        {
		            std::vector<number> q_line_re;
		            std::vector<number> q_line_im;
		            std::vector<number> r_line_re;
		            std::vector<number> r_line_im;

		            unsigned int q_id = this->mdl->flatten((q_fixed == first_index ? q : i), (q_fixed == second_index ? q : i));
		            unsigned int r_id = this->mdl->flatten((r_fixed == first_index ? r : i), (r_fixed == second_index ? r : i));

		            pipe.pull_twopf_time_sample(q_id, time_sample, q_config.serial, q_line_re, data_manager<number>::twopf_real);
		            pipe.pull_twopf_time_sample(q_id, time_sample, q_config.serial, q_line_im, data_manager<number>::twopf_imag);

		            pipe.pull_twopf_time_sample(r_id, time_sample, r_config.serial, r_line_re, data_manager<number>::twopf_real);
		            pipe.pull_twopf_time_sample(r_id, time_sample, r_config.serial, r_line_im, data_manager<number>::twopf_imag);
			        }

		        // work through the time sample
		        for(unsigned int i = 0; i < line_data.size(); i++)
			        {
		            std::vector< std::vector< std::vector<number> > > B_qrp;
		            std::vector< std::vector< std::vector<number> > > C_pqr;

		            this->mdl->B(this->)
			        }
			    }


		    //! threepf time data line
		    template <typename number>
		    class threepf_time_data: public basic_threepf_time_data<number>
			    {

		      public:

		        // CONSTRUCTOR, DESTRUCTOR

		      public:

		        //! construct a threepf time-data object
		        threepf_time_data(const threepf_task<number>& tk, model<number>* m, index_selector<3>& sel,
		                          filter::time_filter tfilter, filter::threepf_kconfig_filter kfilter,
		                          unsigned int prec = __CPP_TRANSPORT_DEFAULT_PLOT_PRECISION);

		        //! deserialization constructor.
		        threepf_time_data(serialization_reader* reader, typename repository<number>::task_finder finder);

		        virtual ~threepf_time_data() = default;


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

		        // DERIVE LINES -- implements a 'general_time_data' interface

		        //! generate data lines for plotting
		        virtual void derive_lines(typename data_manager<number>::datapipe& pipe, const std::vector<unsigned int>& time_sample,
		                                  plot2d_product<number>& plot, std::list< time_data_line<number> >& lines) const override;


		        // LABEL GENERATION

		      protected:

		        std::string make_label(unsigned int l, unsigned int m, unsigned int n, plot2d_product<number>& plot,
		                               typename data_manager<number>::threepf_configuration& config, model<number>* mdl) const;


		        // CLONE

		        //! self-replicate
		        virtual general_time_data<number>* clone() const override { return new threepf_time_data<number>(static_cast<const threepf_time_data<number>&>(*this)); }


		        // WRITE TO A STREAM

		        //! write self-details to a stream
		        virtual void write(std::ostream& out, wrapped_output& wrapper) override;


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

		        //! precision to use for labels
		        unsigned int precision;

		        //! use k_t on line labels?
		        bool use_kt_label;

		        //! use alpha on line labels?
		        bool use_alpha_label;

		        //! use beta on line labels?
		        bool use_beta_label;
			    };


		    template <typename number>
		    threepf_time_data<number>::threepf_time_data(const threepf_task<number>& tk, model<number>* m, index_selector<3>& sel,
		                                                 filter::time_filter tfilter, filter::threepf_kconfig_filter kfilter,
		                                                 unsigned int prec)
			    : active_indices(sel), precision(prec), use_kt_label(true), use_alpha_label(false), use_alpha_label(false),
			      basic_threepf_time_data<number>(tk, m, tfilter)
			    {
		        assert(m != nullptr);

		        if(m == nullptr) throw runtime_exception(runtime_exception::RUNTIME_ERROR, __CPP_TRANSPORT_PRODUCT_GENERAL_TPLOT_NULL_MODEL);

		        if(active_indices.get_number_fields() != mdl->get_N_fields())
			        {
		            std::ostringstream msg;
		            msg << __CPP_TRANSPORT_PRODUCT_GENERAL_TPLOT_INDEX_MISMATCH << " ("
			            << __CPP_TRANSPORT_PRODUCT_GENERAL_TPLOT_INDEX_MISMATCH_A << " " << active_indices.get_number_fields() << ", "
			            << __CPP_TRANSPORT_PRODUCT_GENERAL_TPLOT_INDEX_MISMATCH_B << " " << mdl->get_N_fields() << ")";
		            throw runtime_exception(runtime_exception::RUNTIME_ERROR, msg.str());
			        }

		        // set up a list of serial numbers corresponding to the sample kconfigs for this plot
		        this->f.filter_threepf_kconfig_sample(kfilter, tk.get_sample(), kconfig_sample_sns);
			    }


		    template <typename number>
		    threepf_time_data<number>::threepf_time_data(serialization_reader* reader)
			    : active_indices(reader), basic_threepf_time_data<number>(reader)
			    {
		        assert(reader != nullptr);

		        if(reader == nullptr) throw runtime_exception(runtime_exception::RUNTIME_ERROR, __CPP_TRANSPORT_PRODUCT_GENERAL_TPLOT_NULL_READER);

		        reader->read_value(__CPP_TRANSPORT_NODE_PRODUCT_TDATA_LABEL_PRECISION, precision);

		        unsigned int sns = reader->start_array(__CPP_TRANSPORT_NODE_PRODUCT_TDATA_K_SERIAL_NUMBERS);

		        for(unsigned int i = 0; i < sns; i++)
			        {
		            reader->start_array_element();

		            unsigned int sn;
		            reader->read_value(__CPP_TRANSPORT_NODE_PRODUCT_TDATA_K_SERIAL_NUMBER, sn);
		            kconfig_sample_sns.push_back(sn);

		            reader->end_array_element();
			        }

		        reader->read_value(__CPP_TRANSPORT_NODE_PRODUCT_TDATA_THREEPF_LABEL_KT, this->use_kt_label);
		        reader->read_value(__CPP_TRANSPORT_NODE_PRODUCT_TDATA_THREEPF_LABEL_ALPHA, this->use_alpha_label);
		        reader->read_value(__CPP_TRANSPORT_NODE_PRODUCT_TDATA_THREEPF_LABEL_BETA, this->use_beta_label);

		        reader->end_element(__CPP_TRANSPORT_NODE_PRODUCT_TDATA_K_SERIAL_NUMBERS);
			    }


		    template <typename number>
		    void threepf_time_data<number>::derive_lines(typename data_manager<number>::datapipe& pipe, const std::vector<unsigned int>& time_sample,
		                                                 plot2d_product<number>& plot, std::list<time_data_line<number> >& lines) const
			    {
		        // pull k-configuration information from the database
		        typename std::vector< typename data_manager<number>::threepf_configuration > k_values;
		        std::vector<unsigned int> k1_serials;
		        std::vector<unsigned int> k2_serials;
		        std::vector<unsigned int> k3_serials;
		        pipe.pull_threepf_kconfig_sample(this->kconfig_sample_sns, k_values, k1_serials, k2_serials, k3_serials);

		        assert(k1_serials.size() == this->kconfig_sample_sns.size());
		        assert(k2_serials.size() == this->kconfig_sample_sns.size());
		        assert(k3_serials.size() == this->kconfig_sample_sns.size());
		        assert(k_values.size()   == this->kconfig_sample_sns.size());

		        typename std::vector< typename data_manager<number>::twopf_configuration > k1_values;
		        typename std::vector< typename data_manager<number>::twopf_configuration > k2_values;
		        typename std::vector< typename data_manager<number>::twopf_configuration > k3_values;
		        pipe.pull_twopf_kconfig_sample(k1_serials, k1_values);
		        pipe.pull_twopf_kconfig_sample(k2_serials, k2_values);
		        pipe.pull_twopf_kconfig_sample(k3_serials, k3_values);

		        assert(k1_values.size() == this->kconfig_sample_sns.size());
		        assert(k2_values.size() == this->kconfig_sample_sns.size());
		        assert(k3_values.size() == this->kconfig_sample_sns.size());

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
		                            std::vector<number> line_data;

		                            pipe.pull_threepf_time_sample(this->mdl->flatten(l, m, n), time_sample, this->kconfig_sample_sns[i], line_data);

		                            // the integrator produces correlation functions involving the canonical momenta,
		                            // not the derivatives. If the user wants derivatives then we have to shift.
		                            if(this->get_dot_meaning() == general_time_data<number>::derivatives)
			                            this->shift_derivatives(pipe, time_sample, line_data, l, m, n, k1_values[i], k2_values[i], k3_values[i]);

		                            time_data_line<number> line = time_data_line<number>(line_data, this->make_label(l, m, n, plot, k_values[i], this->mdl));

		                            lines.push_back(line);
			                        }
			                    }
			                }
			            }
			        }
			    }


		    template <typename number>
		    std::string threepf_time_data<number>::make_label(unsigned int l, unsigned int m, unsigned int n, plot2d_product<number>& plot,
		                                                      typename data_manager<number>::threepf_configuration& config, model<number>* mdl) const
			    {
		        std::ostringstream label;

		        assert(mdl != nullptr);
		        if(mdl == nullptr) throw runtime_exception(runtime_exception::RUNTIME_ERROR, __CPP_TRANSPORT_PRODUCT_PLOT2D_NULL_MODEL);

		        unsigned int N_fields = mdl->get_N_fields();

		        label << std::setprecision(this->precision);

		        if(plot.get_use_LaTeX())
			        {
		            const std::vector<std::string>& field_names = mdl->get_f_latex_names();

		            label << "$";

		            if(this->get_dot_meaning() == general_time_data<number>::derivatives)
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

		            // FIXME: allow alpha, beta, etc. in labels too?
		            label << "\\;"
		            unsigned int count=0;
		            if(this->use_kt_label)
			            {
		                label << (count > 0 ? ", " : "") << __CPP_TRANSPORT_LATEX_KT_SYMBOL << "=";
		                if(this->get_klabel_meaning() == general_time_data<number>::conventional) label << config.kt_conventional;
		                else label << config.kt_comoving;
		                count++;
			            }
		            if(this->use_alpha_label)
			            {
		                label << (count > 0 ? ", " : "") << __CPP_TRANSPORT_LATEX_ALPHA_SYMBOL << "=" << config.alpha;
		                count++;
			            }
		            if(this->use_beta_label)
			            {
		                label << (count > 0, ", " : "") << __CPP_TRANSPORT_LATEX_BETA_SYMBOL << "=" << config.beta;
		                count++
			            }

		            label << "$";
			        }
		        else
			        {
		            const std::vector<std::string>& field_names = mdl->get_field_names();

		            if(this->get_dot_meaning() == general_time_data<number>::derivatives)
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
		            unsigned int count=0;
		            if(this->use_kt_label)
			            {
		                label << (count > 0 ? ", " : "") << __CPP_TRANSPORT_NONLATEX_KT_SYMBOL << "=";
		                if(this->get_klabel_meaning() == general_time_data<number>::conventional) label << config.kt_conventional;
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
		                label << (count > 0, ", " : "") << __CPP_TRANSPORT_NONLATEX_BETA_SYMBOL << "=" << config.beta;
		                count++;
			            }

			        }

		        return(label.str());
			    }


		    template <typename number>
		    void threepf_time_data<number>::write(std::ostream& out, wrapped_output& wrapper)
			    {
		        out << "  " << __CPP_TRANSPORT_PRODUCT_GENERAL_TPLOT_LABEL_THREEPF << std::endl;
		        out << "  " << __CPP_TRANSPORT_PRODUCT_GENERAL_TPLOT_LABEL_INDICES << " ";
		        this->active_indices.write(out, mdl->get_state_names());
		        out << std::endl;

		        wrapper.wrap_out(out, "  " __CPP_TRANSPORT_PRODUCT_GENERAL_TPLOT_KCONFIG_SN_LABEL " ");

		        unsigned int count = 0;
		        for(std::vector<unsigned int>::const_iterator t = this->kconfig_sample_sns.begin(); t != this->kconfig_sample_sns.end() && count < __CPP_TRANSPORT_PRODUCT_TIMEPLOT_MAX_SN; t++)
			        {
		            std::ostringstream msg;
		            msg << (*t);

		            wrapper.wrap_list_item(out, true, msg.str(), count);
			        }
		        if(count == __CPP_TRANSPORT_PRODUCT_TIMEPLOT_MAX_SN) wrapper.wrap_list_item(out, true, "...", count);
		        wrapper.wrap_newline(out);
			    }


		    template <typename number>
		    void threepf_time_data<number>::serialize(serialization_writer& writer) const
			    {
		        this->write_value_node(writer, __CPP_TRANSPORT_NODE_PRODUCT_TDATA_TYPE,
		                               std::string(__CPP_TRANSPORT_NODE_PRODUCT_TDATA_THREEPF));

		        this->write_value_node(writer, __CPP_TRANSPORT_NODE_PRODUCT_TDATA_LABEL_PRECISION, this->precision);

		        this->active_indices.serialize(writer);

		        this->begin_array(writer, __CPP_TRANSPORT_NODE_PRODUCT_TDATA_K_SERIAL_NUMBERS, this->kconfig_sample_sns.size() == 0);
		        for(std::vector<unsigned int>::const_iterator t = this->kconfig_sample_sns.begin(); t != this->kconfig_sample_sns.end(); t++)
			        {
		            this->begin_node(writer, "arrayelt", false);    // node name ignored for arrays
		            this->write_value_node(writer, __CPP_TRANSPORT_NODE_PRODUCT_TDATA_K_SERIAL_NUMBER, *t);
		            this->end_element(writer, "arrayelt");
			        }
		        this->end_element(writer, __CPP_TRANSPORT_NODE_PRODUCT_TDATA_K_SERIAL_NUMBERS);

		        this->write_value_node(writer, __CPP_TRANSPORT_NODE_GENERAL_TPLOT_LABEL_KT, this->use_kt_label);
		        this->write_value_node(writer, __CPP_TRANSPORT_NODE_GENERAL_TPLOT_LABEL_ALPHA, this->use_alpha_label);
		        this->write_value_node(writer, __CPP_TRANSPORT_NODE_GENERAL_TPLOT_LABEL_BETA, this->use_beta_label);

		        this->general_time_data<number>::serialize(writer);
			    }


			}   // namespace derived_data

	}   // namespace transport


#endif //__field_time_data_H_
