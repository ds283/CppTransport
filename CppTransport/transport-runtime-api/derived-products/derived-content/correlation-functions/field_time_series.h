//
// Created by David Seery on 19/05/2014.
// Copyright (c) 2014-2016 University of Sussex. All rights reserved.
//


#ifndef CPPTRANSPORT_FIELD_TIME_SERIES_H
#define CPPTRANSPORT_FIELD_TIME_SERIES_H


#include <iostream>
#include <sstream>
#include <string>
#include <list>
#include <vector>
#include <array>
#include <stdexcept>

#include "transport-runtime-api/derived-products/line-collections/data_line.h"
#include "transport-runtime-api/derived-products/derived-content/concepts/series/time_series.h"
#include "transport-runtime-api/derived-products/derived-content/concepts/lines/twopf_line.h"
#include "transport-runtime-api/derived-products/derived-content/concepts/lines/threepf_line.h"

#include "transport-runtime-api/derived-products/derived-content/SQL_query/SQL_query.h"


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
            background_time_series(const twopf_db_task<number>& tk, index_selector<1>& sel,
                                   SQL_time_config_query tq, unsigned int prec = CPPTRANSPORT_DEFAULT_PLOT_PRECISION);

		        //! deserialization constructor.
		        background_time_series(Json::Value& reader, task_finder<number>& finder);

		        virtual ~background_time_series() = default;


            // TYPE INTROSPECTION

          public:

            //! get type
            virtual derived_line_type get_line_type() const override { return derived_line_type::background; }


            // EXTRACT QUERIES

          public:

            //! get time query
            const SQL_time_config_query& get_time_query() const { return(this->tquery); }


		        // DERIVE LINES -- implements a 'time_series' interface

          public:

		        //! generate data lines for plotting
            virtual void derive_lines(datapipe<number>& pipe, std::list<data_line<number> >& lines,
                                      const std::list<std::string>& tags) const override;


		        // LABEL GENERATION

		      protected:

				    //! Make LaTeX label for this line
		        std::string make_LaTeX_label(unsigned int i) const;

				    //! Make non-LaTeX label for this line
				    std::string make_non_LaTeX_label(unsigned int i) const;


		        // CLONE

		        //! self-replicate
		        virtual background_time_series<number>* clone() const override { return new background_time_series<number>(static_cast<const background_time_series<number>&>(*this)); }


		        // WRITE TO A STREAM

		        //! write self-details to a stream
		        virtual void write(std::ostream& out) override;


		        // SERIALIZATION -- implements a 'serializable' interface

		      public:

		        //! serialize this object
		        virtual void serialize(Json::Value& writer) const override;

		        // INTERNAL DATA

		      protected:

				    //! gadget for extracting details about parent task
				    integration_task_gadget<number> gadget;

		        //! record which indices are active in this group
		        index_selector<1> active_indices;

		        //! query representing x-axis
		        SQL_time_config_query tquery;

			    };


				// note that because time_series<> inherits virtually from derived_line<>, the constructor for
				// derived_line<> is *not* called from time_series<>. We have to call it ourselves.
		    template <typename number>
		    background_time_series<number>::background_time_series(const twopf_db_task<number>& tk, index_selector<1>& sel,
		                                                           SQL_time_config_query tq, unsigned int prec)
			    : derived_line<number>(tk, axis_class::time_axis, std::list<axis_value>{ axis_value::efolds_axis }, prec),
			      time_series<number>(tk),
			      gadget(tk),
			      active_indices(sel),
			      tquery(tq)
			    {
		        if(active_indices.get_number_fields() != gadget.get_N_fields())
			        {
		            std::ostringstream msg;
		            msg << CPPTRANSPORT_PRODUCT_INDEX_MISMATCH << " ("
			              << CPPTRANSPORT_PRODUCT_INDEX_MISMATCH_A << " " << active_indices.get_number_fields() << ", "
			              << CPPTRANSPORT_PRODUCT_INDEX_MISMATCH_B << " " << gadget.get_N_fields() << ")";
		            throw runtime_exception(exception_type::RUNTIME_ERROR, msg.str());
			        }
			    }

		    // note that because time_series<> inherits virtually from derived_line<>, the constructor for
		    // derived_line<> is *not* called from time_series<>. We have to call it ourselves.
		    template <typename number>
		    background_time_series<number>::background_time_series(Json::Value& reader, task_finder<number>& finder)
			    : derived_line<number>(reader, finder),
			      time_series<number>(reader),
			      gadget(),
			      active_indices(reader),
			      tquery(reader[CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_T_QUERY])
			    {
				    assert(this->parent_task != nullptr);
		        gadget.set_task(this->parent_task, finder);
			    }

		    // note that because time_series<> inherits virtually from derived_line<>, the write method for
		    // derived_line<> is *not* called from time_series<>. We have to call it ourselves.
		    template <typename number>
		    void background_time_series<number>::write(std::ostream& out)
			    {
		        out << "  " << CPPTRANSPORT_PRODUCT_TIME_SERIES_LABEL_BACKGROUND << '\n';
		        out << "  " << CPPTRANSPORT_PRODUCT_LINE_COLLECTION_LABEL_INDICES << " ";

		        integration_task<number>* itk = dynamic_cast< integration_task<number>* >(this->get_parent_task());
		        this->active_indices.write(out, itk->get_model()->get_state_names());

		        this->time_series<number>::write(out);
		        this->derived_line<number>::write(out);
			    }


		    // note that because time_series<> inherits virtually from derived_line<>, the serialize method for
		    // derived_line<> is *not* called from time_series<>. We have to call it ourselves.
		    template <typename number>
		    void background_time_series<number>::serialize(Json::Value& writer) const
			    {
		        writer[CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_TYPE] = std::string(CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_BACKGROUND);

		        this->active_indices.serialize(writer);
		        this->tquery.serialize(writer[CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_T_QUERY]);

		        this->time_series<number>::serialize(writer);
				    this->derived_line<number>::serialize(writer);
			    }


		    template <typename number>
		    void background_time_series<number>::derive_lines(datapipe<number>& pipe, std::list<data_line<number> >& lines,
		                                                      const std::list<std::string>& tags) const
          {
            // attach our datapipe to an output group
            std::string group = this->attach(pipe, tags);

            const std::vector<double> t_axis = this->pull_time_axis(pipe, this->tquery);

            // loop through all the fields, pulling data from the database for those which are enabled

		        typename datapipe<number>::time_data_handle& handle = pipe.new_time_data_handle(this->tquery);

            for(unsigned int m = 0; m < 2 * this->gadget.get_N_fields(); ++m)
              {
                std::array<unsigned int, 1> index_set = {m};
                if(this->active_indices.is_on(index_set))
                  {
										background_time_data_tag<number> tag = pipe.new_background_time_data_tag(this->gadget.get_model()->flatten(m));

                    // it's safe to take a reference here to avoid a copy; we don't need the cache data to survive over multiple calls to lookup_tag()
                    const std::vector<number>& line_data = handle.lookup_tag(tag);

                    data_line<number> line = data_line<number>(group, this->x_type, this->gadget.get_model()->is_field(m) ? value_type::field_value :  value_type::momentum_value, t_axis, line_data,
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

				    if(this->label_set)
					    {
						    label << this->LaTeX_label;
					    }
				    else
					    {
				        unsigned int N_fields = this->gadget.get_N_fields();

				        const std::vector<std::string>& field_names = this->gadget.get_model()->get_f_latex_names();

				        label << "$";
                switch(this->get_dot_meaning())
                  {
                    case dot_type::derivatives:
                      label << field_names[i % N_fields] << (i >= N_fields ? "^{" CPPTRANSPORT_LATEX_PRIME_SYMBOL "}" : "");
                      break;

                    case dot_type::momenta:
                      label << (i >= N_fields ? "p_{" : "") << field_names[i % N_fields] << (i >= N_fields ? "}" : "");
                      break;
                  }
				        label << "$";
					    }

				    return(label.str());
			    }


		    template <typename number>
        std::string background_time_series<number>::make_non_LaTeX_label(unsigned int i) const
	        {
		        std::ostringstream label;

				    if(this->label_set)
					    {
						    label << this->non_LaTeX_label;
					    }
				    else
					    {
				        unsigned int N_fields = this->gadget.get_N_fields();

				        const std::vector<std::string>& field_names = this->gadget.get_model()->get_field_names();

                switch(this->get_dot_meaning())
                  {
                    case dot_type::derivatives:
                      label << field_names[i % N_fields] << (i >= N_fields ? CPPTRANSPORT_NONLATEX_PRIME_SYMBOL : "");
                      break;

                    case dot_type::momenta:
                      label << (i >= N_fields ? "p_{" : "") << field_names[i % N_fields] << (i >= N_fields ? "}" : "");
                      break;
                  }
					    }

		        return(label.str());
			    }


		    //! twopf time data line
		    template <typename number>
		    class twopf_time_series: public time_series<number>, public twopf_line<number>
			    {

		        // CONSTRUCTOR, DESTRUCTOR

		      public:

		        //! construct a twopf time-series object
		        twopf_time_series(const twopf_db_task<number>& tk, index_selector<2>& sel,
		                          SQL_time_config_query tq, SQL_twopf_kconfig_query kq,
		                          unsigned int prec = CPPTRANSPORT_DEFAULT_PLOT_PRECISION);

		        //! deserialization constuctor.
		        twopf_time_series(Json::Value& reader, task_finder<number>& finder);

		        virtual ~twopf_time_series() = default;


            // TYPE INTROSPECTION

          public:

            //! get type
            virtual derived_line_type get_line_type() const override { return derived_line_type::twopf_time; }


            // EXTRACT QUERIES

          public:

            //! get time query
            const SQL_time_config_query& get_time_query() const { return(this->tquery); }

            //! get wavenumber query
            const SQL_twopf_kconfig_query& get_k_query() const { return(this->kquery); }


		        // DERIVE LINES -- implements a 'time_series' interface

          public:

		        //! generate data lines for plotting
            virtual void derive_lines(datapipe<number>& pipe, std::list<data_line<number> >& lines,
                                      const std::list<std::string>& tags) const override;

		        //! generate a LaTeX label
		        std::string get_LaTeX_label(unsigned int m, unsigned int n, const twopf_kconfig& k) const;

		        //! generate a non-LaTeX label
		        std::string get_non_LaTeX_label(unsigned int m, unsigned int n, const twopf_kconfig& k) const;


		        // CLONE

		        //! self-replicate
		        virtual twopf_time_series<number>* clone() const override { return new twopf_time_series<number>(static_cast<const twopf_time_series<number>&>(*this)); }


		        // WRITE TO A STREAM

		        //! write self-details to a stream
		        virtual void write(std::ostream& out) override;


		        // SERIALIZATION -- implements a 'serializable' interface

		      public:

		        //! serialize this object
		        virtual void serialize(Json::Value& writer) const override;


		        // INTERNAL DATA

		        protected:

		        //! SQL query representing x-axis
		        SQL_time_config_query tquery;

		        //! SQL query representing different lines
		        SQL_twopf_kconfig_query kquery;

			    };


		    // note that because time_series<> inherits virtually from derived_line<>, the constructor for
		    // derived_line<> is *not* called from time_series<>. We have to call it ourselves.
		    template <typename number>
		    twopf_time_series<number>::twopf_time_series(const twopf_db_task<number>& tk, index_selector<2>& sel,
		                                                 SQL_time_config_query tq, SQL_twopf_kconfig_query kq, unsigned int prec)
			    : derived_line<number>(tk, axis_class::time_axis, std::list<axis_value>{ axis_value::efolds_axis }, prec),
			      twopf_line<number>(tk, sel),
			      time_series<number>(tk),
			      tquery(tq),
			      kquery(kq)
			    {
			    }


		    // note that because time_series<> inherits virtually from derived_line<>, the constructor for
		    // derived_line<> is *not* called from time_series<>. We have to call it ourselves.
		    template <typename number>
		    twopf_time_series<number>::twopf_time_series(Json::Value& reader, task_finder<number>& finder)
			    : derived_line<number>(reader, finder),
			      twopf_line<number>(reader, finder),
			      time_series<number>(reader),
			      tquery(reader[CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_T_QUERY]),
			      kquery(reader[CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_K_QUERY])
			    {
			    }


		    template <typename number>
		    void twopf_time_series<number>::derive_lines(datapipe<number>& pipe, std::list< data_line<number> >& lines,
		                                                 const std::list<std::string>& tags) const
			    {
            // attach our datapipe to an output group
            std::string group = this->attach(pipe, tags);

		        // pull time-axis data
            const std::vector<double> t_axis = this->pull_time_axis(pipe, this->tquery);

		        // set up cache handles
		        typename datapipe<number>::twopf_kconfig_handle& k_handle = pipe.new_twopf_kconfig_handle(this->kquery);
		        typename datapipe<number>::time_data_handle& t_handle = pipe.new_time_data_handle(this->tquery);

		        // pull k-configuration information from the database
		        twopf_kconfig_tag<number> k_tag = pipe.new_twopf_kconfig_tag();

		        const typename std::vector< twopf_kconfig > k_values = k_handle.lookup_tag(k_tag);

		        // loop through all components of the twopf, for each k-configuration we use,
		        // pulling data from the database
		        for(std::vector<twopf_kconfig>::const_iterator t = k_values.begin(); t != k_values.end(); ++t)
			        {
		            for(unsigned int m = 0; m < 2*this->gadget.get_N_fields(); ++m)
			            {
		                for(unsigned int n = 0; n < 2*this->gadget.get_N_fields(); ++n)
			                {
		                    std::array<unsigned int, 2> index_set = { m, n };
		                    if(this->active_indices.is_on(index_set))
			                    {
		                        cf_time_data_tag<number> tag =
			                                                 pipe.new_cf_time_data_tag(this->is_real_twopf() ? cf_data_type::cf_twopf_re : cf_data_type::cf_twopf_im,
			                                                                           this->gadget.get_model()->flatten(m, n), t->serial);

		                        std::vector<number> line_data = t_handle.lookup_tag(tag);
                            assert(line_data.size() == t_axis.size());

                            value_type value;
		                        if(this->dimensionless)
			                        {
		                            for(unsigned int j = 0; j < line_data.size(); ++j)
			                            {
		                                line_data[j] *= 1.0 / (2.0*M_PI*M_PI);
			                            }
                                value = value_type::dimensionless_value;
			                        }
                            else
                              {
                                double k_cube = t->k_comoving * t->k_comoving * t->k_comoving;
                                for(unsigned int j = 0; j < line_data.size(); ++j)
                                  {
                                    line_data[j] *= 1.0 / k_cube;
                                  }
                                value = value_type::correlation_function_value;
                              }

                            data_line<number> line = data_line<number>(group, this->x_type, value, t_axis, line_data,
                                                                       this->get_LaTeX_label(m,n,*t), this->get_non_LaTeX_label(m,n,*t));
                            lines.push_back(line);
			                    }
			                }
			            }
			        }

            // detach pipe from output group
            this->detach(pipe);
			    }


		    template <typename number>
		    std::string twopf_time_series<number>::get_LaTeX_label(unsigned int m, unsigned int n, const twopf_kconfig& k) const
			    {
		        std::string tag = this->make_LaTeX_tag(k);
		        std::string label;

		        if(this->label_set)
			        {
		            label = this->LaTeX_label;
			        }
		        else
			        {
		            label = "$" + this->make_LaTeX_label(m,n) + "$";
			        }

				    if(this->use_tags) label += " $" + tag + "$";
				    return(label);
			    }


		    template <typename number>
		    std::string twopf_time_series<number>::get_non_LaTeX_label(unsigned int m, unsigned int n, const twopf_kconfig& k) const
			    {
		        std::string tag = this->make_non_LaTeX_tag(k);
		        std::string label;

		        if(this->label_set)
			        {
		            label = this->non_LaTeX_label;
			        }
		        else
			        {
		            label = this->make_non_LaTeX_label(m,n);
			        }

				    if(this->use_tags) label += " " + tag;
				    return(label);
			    }


		    // note that because time_series<> inherits virtually from derived_line<>, the write method for
		    // derived_line<> is *not* called from time_series<>. We have to call it ourselves.
		    template <typename number>
		    void twopf_time_series<number>::write(std::ostream& out)
			    {
		        this->twopf_line<number>::write(out);
		        this->time_series<number>::write(out);
				    this->derived_line<number>::write(out);
			    }


		    // note that because time_series<> inherits virtually from derived_line<>, the serialize method for
		    // derived_line<> is *not* called from time_series<>. We have to call it ourselves.
		    template <typename number>
		    void twopf_time_series<number>::serialize(Json::Value& writer) const
			    {
		        writer[CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_TYPE] = std::string(CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_TWOPF_TIME_SERIES);

		        this->tquery.serialize(writer[CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_T_QUERY]);
		        this->kquery.serialize(writer[CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_K_QUERY]);

				    this->derived_line<number>::serialize(writer);
				    this->twopf_line<number>::serialize(writer);
		        this->time_series<number>::serialize(writer);
			    }


		    //! threepf time data line
		    template <typename number>
		    class threepf_time_series: public time_series<number>, public threepf_line<number>
			    {

		        // CONSTRUCTOR, DESTRUCTOR

		      public:

            //! construct a threepf time-data object
            threepf_time_series(const threepf_task<number>& tk, index_selector<3>& sel,
                                SQL_time_config_query tq, SQL_threepf_kconfig_query kq,
                                unsigned int prec = CPPTRANSPORT_DEFAULT_PLOT_PRECISION);

		        //! deserialization constructor.
		        threepf_time_series(Json::Value& reader, task_finder<number>& finder);

		        virtual ~threepf_time_series() = default;


            // TYPE INTROSPECTION

          public:

            //! get type
            virtual derived_line_type get_line_type() const override { return derived_line_type::threepf_time; }


            // EXTRACT QUERIES

          public:

            //! get time query
            const SQL_time_config_query& get_time_query() const { return(this->tquery); }

            //! get wavenumber query
            const SQL_threepf_kconfig_query& get_k_query() const { return(this->kquery); }


		        // DERIVE LINES -- implements a 'time_series' interface

		      public:

		        //! generate data lines for plotting
            virtual void derive_lines(datapipe<number>& pipe, std::list< data_line<number> >& lines,
                                      const std::list<std::string>& tags) const override;

		        //! generate a LaTeX label
		        std::string get_LaTeX_label(unsigned int l, unsigned int m, unsigned int n, const threepf_kconfig& k) const;

		        //! generate a non-LaTeX label
		        std::string get_non_LaTeX_label(unsigned int l, unsigned int m, unsigned int n, const threepf_kconfig& k) const;


		        // CLONE

		      public:

		        //! self-replicate
		        virtual threepf_time_series<number>* clone() const override { return new threepf_time_series<number>(static_cast<const threepf_time_series<number>&>(*this)); }


		        // WRITE TO A STREAM

		      public:

		        //! write self-details to a stream
		        virtual void write(std::ostream& out) override;


		        // SERIALIZATION -- implements a 'serializable' interface

		      public:

		        //! serialize this object
		        virtual void serialize(Json::Value& writer) const override;


            // INTERNAL DATA

          protected:

            //! SQL query representing x-axis
            SQL_time_config_query tquery;

            //! SQL query representing different lines
            SQL_threepf_kconfig_query kquery;

			    };


		    // note that because time_series<> inherits virtually from derived_line<>, the constructor for
		    // derived_line<> is *not* called from time_series<>. We have to call it ourselves.
		    template <typename number>
		    threepf_time_series<number>::threepf_time_series(const threepf_task<number>& tk, index_selector<3>& sel,
		                                                     SQL_time_config_query tq, SQL_threepf_kconfig_query kq,
		                                                     unsigned int prec)
			    : derived_line<number>(tk, axis_class::time_axis, std::list<axis_value>{ axis_value::efolds_axis }, prec),
			      threepf_line<number>(tk, sel),
			      time_series<number>(tk),
			      tquery(tq),
			      kquery(kq)
			    {
			    }


		    // note that because time_series<> inherits virtually from derived_line<>, the constructor for
		    // derived_line<> is *not* called from time_series<>. We have to call it ourselves.
		    template <typename number>
		    threepf_time_series<number>::threepf_time_series(Json::Value& reader, task_finder<number>& finder)
			    : derived_line<number>(reader, finder),
		        threepf_line<number>(reader, finder),
		        time_series<number>(reader),
		        tquery(reader[CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_T_QUERY]),
		        kquery(reader[CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_K_QUERY])
			    {
			    }


        template <typename number>
        void threepf_time_series<number>::derive_lines(datapipe<number>& pipe, std::list< data_line<number> >& lines,
                                                     const std::list<std::string>& tags) const
			    {
            // attach our datapipe to an output group
            std::string group = this->attach(pipe, tags);

		        // pull time-axis data
            const std::vector<double> t_axis = this->pull_time_axis(pipe, this->tquery);

		        // set up cache handles
            typename datapipe<number>::threepf_kconfig_handle& kc_handle = pipe.new_threepf_kconfig_handle(this->kquery);
            typename datapipe<number>::time_data_handle&       t_handle  = pipe.new_time_data_handle(this->tquery);

		        // pull time- and k-configuration information from the database
		        threepf_kconfig_tag<number> kc_tag = pipe.new_threepf_kconfig_tag();

		        const std::vector< threepf_kconfig > k_values = kc_handle.lookup_tag(kc_tag);

		        // loop through all components of the threepf, for each k-configuration we use,
		        // pulling data from the database

		        for(std::vector<threepf_kconfig>::const_iterator t = k_values.begin(); t != k_values.end(); ++t)
			        {
		            for(unsigned int l = 0; l < 2*this->gadget.get_N_fields(); ++l)
			            {
		                for(unsigned int m = 0; m < 2*this->gadget.get_N_fields(); ++m)
			                {
		                    for(unsigned int n = 0; n < 2*this->gadget.get_N_fields(); ++n)
			                    {
		                        std::array<unsigned int, 3> index_set = { l, m, n };
		                        if(this->active_indices.is_on(index_set))
			                        {
				                        cf_time_data_tag<number> tag = pipe.new_cf_time_data_tag(this->get_dot_meaning() == dot_type::derivatives ? cf_data_type::cf_threepf_Nderiv : cf_data_type::cf_threepf_momentum,
                                                                                         this->gadget.get_model()->flatten(l,m,n), t->serial);

		                            std::vector<number> line_data = t_handle.lookup_tag(tag);
                                assert(line_data.size() == t_axis.size());

                                value_type value;
                                if(this->dimensionless)
                                  {
                                    value = value_type::dimensionless_value;
                                  }
                                else
                                  {
                                    double shape = t->k1_comoving*t->k1_comoving * t->k2_comoving*t->k2_comoving * t->k3_comoving*t->k3_comoving;
                                    for(unsigned int j = 0; j < line_data.size(); ++j)
                                      {
                                        line_data[j] *= 1.0/shape;
                                      }
                                    value = value_type::correlation_function_value;
                                  }

                                data_line<number> line = data_line<number>(group, this->x_type, value, t_axis, line_data,
                                                                           this->get_LaTeX_label(l, m, n, *t), this->get_non_LaTeX_label(l, m, n, *t));
                                lines.push_back(line);
			                        }
			                    }
			                }
			            }
			        }

            // detach pipe from output group
            this->detach(pipe);
			    }


		    template <typename number>
		    std::string threepf_time_series<number>::get_LaTeX_label(unsigned int l, unsigned int m, unsigned int n, const threepf_kconfig& k) const
			    {
		        std::string tag = this->make_LaTeX_tag(k, this->use_kt_label, this->use_alpha_label, this->use_beta_label);
		        std::string label;

		        if(this->label_set)
			        {
		            label = this->LaTeX_label;
			        }
		        else
			        {
		            label = "$" + this->make_LaTeX_label(l,m,n) + "$";
			        }

		        if(this->use_tags) label += " $" + tag + "$";
		        return(label);
			    }


		    template <typename number>
		    std::string threepf_time_series<number>::get_non_LaTeX_label(unsigned int l, unsigned int m, unsigned int n, const threepf_kconfig& k) const
			    {
		        std::string tag = this->make_non_LaTeX_tag(k, this->use_kt_label, this->use_alpha_label, this->use_beta_label);
		        std::string label;

		        if(this->label_set)
			        {
		            label = this->non_LaTeX_label;
			        }
		        else
			        {
		            label = this->make_non_LaTeX_label(l,m,n);
			        }

		        if(this->use_tags) label += " " + tag;
		        return(label);
			    }


		    // note that because time_series<> inherits virtually from derived_line<>, the write method for
		    // derived_line<> is *not* called from time_series<>. We have to call it ourselves.
		    template <typename number>
		    void threepf_time_series<number>::write(std::ostream& out)
			    {
		        this->threepf_line<number>::write(out);
		        this->time_series<number>::write(out);
				    this->derived_line<number>::write(out);
			    }


		    // note that because time_series<> inherits virtually from derived_line<>, the serialize method for
		    // derived_line<> is *not* called from time_series<>. We have to call it ourselves.
		    template <typename number>
		    void threepf_time_series<number>::serialize(Json::Value& writer) const
			    {
		        writer[CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_TYPE] = std::string(CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_THREEPF_TIME_SERIES);

		        this->tquery.serialize(writer[CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_T_QUERY]);
		        this->kquery.serialize(writer[CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_K_QUERY]);

		        this->derived_line<number>::serialize(writer);
				    this->threepf_line<number>::serialize(writer);
				    this->time_series<number>::serialize(writer);
			    }


			}   // namespace derived_data

	}   // namespace transport


#endif // CPPTRANSPORT_FIELD_TIME_SERIES_H
