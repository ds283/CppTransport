//
// Created by David Seery on 17/05/2014.
// Copyright (c) 2014 University of Sussex. All rights reserved.
//


#ifndef __general_time_plot_H_
#define __general_time_plot_H_


#include "transport-runtime-api/serialization/serializable.h"

#include "transport-runtime-api/derived-products/time_plot.h"
#include "transport-runtime-api/derived-products/utilities/index_selector.h"


#define __CPP_TRANSPORT_NODE_GENERAL_TPLOT_DATA_TYPE            "time-data-group-type"
#define __CPP_TRANSPORT_NODE_GENERAL_TPLOT_DATA_BACKGROUND      "background-group"
#define __CPP_TRANSPORT_NODE_GENERAL_TPLOT_DATA_TWOPF           "twopf-group"
#define __CPP_TRANSPORT_NODE_GENERAL_TPLOT_DATA_THREEPF         "threepf-group"

#define __CPP_TRANSPORT_NODE_GENERAL_TPLOT_TWOPF_DATA_TYPE      "twopf-selector"
#define __CPP_TRANSPORT_NODE_GENERAL_TPLOT_TWOPF_DATA_REAL      "real"
#define __CPP_TRANSPORT_NODE_GENERAL_TPLOT_TWOPF_DATA_IMAGINARY "imaginary"

#define __CPP_TRANSPORT_NODE_GENERAL_TPLOT_K_SERIAL_NUMBERS     "kconfig-serial-numbers"
#define __CPP_TRANSPORT_NODE_GENERAL_TPLOT_K_SERIAL_NUMBER      "sn"

#define __CPP_TRANSPORT_NODE_GENERAL_TPLOT_LINE_ARRAY           "line-array"

namespace transport
	{

		namespace derived_data
			{

				//! general time data line, suitable for inclusion in a general time plot
				template <typename number>
				class general_time_data: public serializable
					{

						// CONSTRUCTOR, DESTRUCTOR

				  public:

						general_time_data()
							{
							}

						general_time_data(serialization_reader* reader)
							{
							}


						// DERIVE LINES

						//! generate data lines for plotting
						virtual void derive_lines(typename data_manager<number>::datapipe& pipe, const std::vector<unsigned int>& time_sample,
						                          plot2d_product<number>& plot, std::list< plot2d_line<number> >& lines) const = 0;


						// CLONE

						//! self-replicate
						virtual general_time_data<number>* clone() const = 0;


						// SERIALIZATION -- implements a 'serializable' interface

				  public:

						virtual void serialize(serialization_writer& writer) const override
							{
							}

				  public:

						virtual ~general_time_data() = default;

					};


				//! background time data line
				template <typename number>
				class background_time_data: public general_time_data<number>
					{

						// CONSTRUCTOR, DESTRUCTOR

				  public:

						//! construct a background time-data object
						background_time_data(index_selector<1>& sel, model<number>* m);

						//! deserialization constructor.
						background_time_data(serialization_reader* reader, model<number>* m);

						virtual ~background_time_data() = default;


						// DERIVE LINES -- implements a 'general_time_data' interface

				    //! generate data lines for plotting
						virtual void derive_lines(typename data_manager<number>::datapipe& pipe, const std::vector<unsigned int>& time_sample,
						                          plot2d_product<number>& plot, std::list< plot2d_line<number> >& lines) const override;

				    // LABEL GENERATION

				  protected:

				    std::string make_label(unsigned int i, plot2d_product<number>& plot, model<number>* m) const;


				    // CLONE

				    //! self-replicate
				    virtual general_time_data<number>* clone() const override { return new background_time_data<number>(static_cast<const background_time_data<number>&>(*this)); }


				    // SERIALIZATION -- implements a 'serializable' interface

				  public:

				    //! serialize this object
				    virtual void serialize(serialization_writer& writer) const override;

						// INTERNAL DATA

				  private:

						//! record which indices are active in this group
						index_selector<1> active_indices;

						//! record model
						model<number>* mdl;

					};


		    template <typename number>
		    background_time_data<number>::background_time_data(index_selector<1>& sel, model<number>* m)
			    : active_indices(sel), mdl(m), general_time_data<number>()
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
				background_time_data<number>::background_time_data(serialization_reader* reader, model<number>* m)
					: mdl(m), active_indices(reader), general_time_data<number>(reader)
					{
						assert(m != nullptr);

				    if(m == nullptr) throw runtime_exception(runtime_exception::RUNTIME_ERROR, __CPP_TRANSPORT_PRODUCT_GENERAL_TPLOT_NULL_MODEL);
				    if(reader == nullptr) throw runtime_exception(runtime_exception::RUNTIME_ERROR, __CPP_TRANSPORT_PRODUCT_GENERAL_TPLOT_NULL_READER);
					}


				template <typename number>
				void background_time_data<number>::serialize(serialization_writer& writer) const
					{
						this->write_value_node(writer, __CPP_TRANSPORT_NODE_GENERAL_TPLOT_DATA_TYPE,
						                       std::string(__CPP_TRANSPORT_NODE_GENERAL_TPLOT_DATA_BACKGROUND));

						this->active_indices.serialize(writer);

						this->general_time_data<number>::serialize(writer);
					}


				template <typename number>
				void background_time_data<number>::derive_lines(typename data_manager<number>::datapipe& pipe,  const std::vector<unsigned int>& time_sample,
				                                                plot2d_product<number>& plot, std::list< plot2d_line<number> >& lines) const
					{
						// loop through all the fields, pulling data from the database for those which are enabled

						for(unsigned int m = 0; m < 2*this->mdl->get_N_fields(); m++)
							{
						    std::array<unsigned int, 1> index_set = { m };
						    if(this->active_indices.is_on(index_set))
							    {
						        std::vector<number> line_data;

						        pipe.pull_background_time_sample(this->mdl->flatten(m), time_sample, line_data);

						        plot2d_line<number> line = plot2d_line<number>(line_data, this->make_label(m, plot, this->mdl));

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
		            label << "$" << field_names[i % N_fields] << (i >= N_fields ? "^{" __CPP_TRANSPORT_LATEX_PRIME_SYMBOL " }" : "") << "$";
			        }
		        else
			        {
		            const std::vector<std::string>& field_names = m->get_field_names();
		            label << field_names[i % N_fields] << (i >= N_fields ? __CPP_TRANSPORT_NONLATEX_PRIME_SYMBOL : "");
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
						twopf_time_data(const twopf_list_task<number>& tk, index_selector<2>& sel, twopf_type tp,
						                typename derived_product<number>::twopf_kconfig_filter filter, model<number>* m);

						//! deserialization constuctor.
						twopf_time_data(serialization_reader* reader, model<number>* m);

						virtual ~twopf_time_data() = default;


				    // DERIVE LINES -- implements a 'general_time_data' interface

				    //! generate data lines for plotting
				    virtual void derive_lines(typename data_manager<number>::datapipe& pipe, const std::vector<unsigned int>& time_sample,
				                              plot2d_product<number>& plot, std::list< plot2d_line<number> >& lines) const override;


				    // CLONE

				    //! self-replicate
				    virtual general_time_data<number>* clone() const override { return new twopf_time_data<number>(static_cast<const twopf_time_data<number>&>(*this)); }


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
						twopf_type type;

						//! record model
						model<number>* mdl;
					};


		    template <typename number>
		    twopf_time_data<number>::twopf_time_data(const twopf_list_task<number>& tk, index_selector<2>& sel, twopf_type tp,
		                                             typename derived_product<number>::twopf_kconfig_filter filter, model<number>* m)
			    : active_indices(sel), mdl(m), type(tp), general_time_data<number>()
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
				    const std::vector<double>& sample_ks = tk.get_k_list();

				    unsigned int i = 0;
				    for(std::vector<double>::const_iterator t = sample_ks.begin(); t != sample_ks.end(); t++)
					    {
						    if(filter(*t)) kconfig_sample_sns.push_back(i);
						    i++;
					    }
			    }


		    template <typename number>
		    twopf_time_data<number>::twopf_time_data(serialization_reader* reader, model<number>* m)
			    : mdl(m), active_indices(reader), general_time_data<number>()
			    {
		        assert(m != nullptr);
				    assert(reader != nullptr);

		        if(m == nullptr) throw runtime_exception(runtime_exception::RUNTIME_ERROR, __CPP_TRANSPORT_PRODUCT_GENERAL_TPLOT_NULL_MODEL);
		        if(reader == nullptr) throw runtime_exception(runtime_exception::RUNTIME_ERROR, __CPP_TRANSPORT_PRODUCT_GENERAL_TPLOT_NULL_READER);

		        std::string tpf_type;
						reader->read_value(__CPP_TRANSPORT_NODE_GENERAL_TPLOT_TWOPF_DATA_TYPE, tpf_type);

				    if(tpf_type == __CPP_TRANSPORT_NODE_GENERAL_TPLOT_TWOPF_DATA_REAL) type = real;
				    else if(tpf_type == __CPP_TRANSPORT_NODE_GENERAL_TPLOT_TWOPF_DATA_IMAGINARY) type = imaginary;
				    else
					    {
				        std::ostringstream msg;
						    msg << __CPP_TRANSPORT_PRODUCT_GENERAL_TPLOT_TWOPF_DATA_UNKNOWN << " '" << tpf_type << "'";
						    throw runtime_exception(runtime_exception::SERIALIZATION_ERROR, msg.str());
					    }

						unsigned int sns = reader->start_array(__CPP_TRANSPORT_NODE_GENERAL_TPLOT_K_SERIAL_NUMBERS);

				    for(unsigned int i = 0; i < sns; i++)
					    {
						    reader->start_array_element();

						    unsigned int sn;
						    reader->read_value(__CPP_TRANSPORT_NODE_GENERAL_TPLOT_K_SERIAL_NUMBER, sn);
						    kconfig_sample_sns.push_back(sn);

						    reader->end_array_element();
					    }

				    reader->end_element(__CPP_TRANSPORT_NODE_GENERAL_TPLOT_K_SERIAL_NUMBERS);
			    }


		    template <typename number>
		    void twopf_time_data<number>::derive_lines(typename data_manager<number>::datapipe& pipe, const std::vector<unsigned int>& time_sample,
		                                               plot2d_product<number>& plot, std::list<plot2d_line<number> >& lines) const
			    {
			    }


				template <typename number>
				void twopf_time_data<number>::serialize(serialization_writer& writer) const
					{
				    this->write_value_node(writer, __CPP_TRANSPORT_NODE_GENERAL_TPLOT_DATA_TYPE,
				                           std::string(__CPP_TRANSPORT_NODE_GENERAL_TPLOT_DATA_TWOPF));

				    this->active_indices.serialize(writer);

						switch(this->type)
							{
						    case real:
									{
								    this->write_value_node(writer, __CPP_TRANSPORT_NODE_GENERAL_TPLOT_TWOPF_DATA_TYPE, __CPP_TRANSPORT_NODE_GENERAL_TPLOT_TWOPF_DATA_REAL);
								    break;
									}

						    case imaginary:
							    {
						        this->write_value_node(writer, __CPP_TRANSPORT_NODE_GENERAL_TPLOT_TWOPF_DATA_TYPE, __CPP_TRANSPORT_NODE_GENERAL_TPLOT_TWOPF_DATA_IMAGINARY);
						        break;
							    }

						    default:
							    {
						        throw runtime_exception(runtime_exception::RUNTIME_ERROR, __CPP_TRANSPORT_PRODUCT_GENERAL_TPLOT_TWOPF_DATA_UNKNOWN);
							    }
							}

						this->begin_array(writer, __CPP_TRANSPORT_NODE_GENERAL_TPLOT_K_SERIAL_NUMBERS, this->kconfig_sample_sns.size() == 0);
						for(std::vector<unsigned int>::const_iterator t = this->kconfig_sample_sns.begin(); t != this->kconfig_sample_sns.end(); t++)
							{
								this->begin_node(writer, "arrayelt", false);    // node name ignored for arrays
								this->write_value_node(writer, __CPP_TRANSPORT_NODE_GENERAL_TPLOT_K_SERIAL_NUMBER, *t);
								this->end_element(writer, "arrayelt");
							}
						this->end_element(writer, __CPP_TRANSPORT_NODE_GENERAL_TPLOT_K_SERIAL_NUMBERS);

				    this->general_time_data<number>::serialize(writer);
					}


		    //! threepf time data line
		    template <typename number>
		    class threepf_time_data: public general_time_data<number>
			    {

		      public:

		        // CONSTRUCTOR, DESTRUCTOR

		      public:

		        //! construct a threepf time-data object
		        threepf_time_data(const threepf_task<number>& tk, index_selector<3>& sel,
		                          typename derived_product<number>::threepf_kconfig_filter filter, model<number>* m);

		        //! deserialization constructor.
		        threepf_time_data(serialization_reader* reader, model<number> *m);

		        virtual ~threepf_time_data() = default;


		        // DERIVE LINES -- implements a 'general_time_data' interface

		        //! generate data lines for plotting
		        virtual void derive_lines(typename data_manager<number>::datapipe& pipe, const std::vector<unsigned int>& time_sample,
		                                  plot2d_product<number>& plot, std::list< plot2d_line<number> >& lines) const override;


		        // CLONE

		        //! self-replicate
		        virtual general_time_data<number>* clone() const override { return new threepf_time_data<number>(static_cast<const threepf_time_data<number>&>(*this)); }


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

		        //! record model
		        model<number>* mdl;
			    };


		    template <typename number>
		    threepf_time_data<number>::threepf_time_data(const threepf_task<number>& tk, index_selector<3>& sel,
		                                                 typename derived_product<number>::threepf_kconfig_filter filter, model<number>* m)
			    : active_indices(sel), mdl(m), general_time_data<number>()
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
		        const std::vector<threepf_kconfig>& sample_ks = tk.get_sample();

				    unsigned int i = 0;
				    for(std::vector<threepf_kconfig>::const_iterator t = sample_ks.begin(); t != sample_ks.end(); t++)
					    {
						    if(filter((*t).k_t_conventional, (*t).alpha, (*t).beta)) kconfig_sample_sns.push_back(i);
						    i++;
					    }
			    }


				template <typename number>
				threepf_time_data<number>::threepf_time_data(serialization_reader* reader, model<number>* m)
					: mdl(m), active_indices(reader), general_time_data<number>()
					{
						assert(m != nullptr);
						assert(reader != nullptr);

				    if(m == nullptr) throw runtime_exception(runtime_exception::RUNTIME_ERROR, __CPP_TRANSPORT_PRODUCT_GENERAL_TPLOT_NULL_MODEL);
				    if(reader == nullptr) throw runtime_exception(runtime_exception::RUNTIME_ERROR, __CPP_TRANSPORT_PRODUCT_GENERAL_TPLOT_NULL_READER);

				    unsigned int sns = reader->start_array(__CPP_TRANSPORT_NODE_GENERAL_TPLOT_K_SERIAL_NUMBERS);

				    for(unsigned int i = 0; i < sns; i++)
					    {
				        reader->start_array_element();

				        unsigned int sn;
				        reader->read_value(__CPP_TRANSPORT_NODE_GENERAL_TPLOT_K_SERIAL_NUMBER, sn);
				        kconfig_sample_sns.push_back(sn);

				        reader->end_array_element();
					    }

				    reader->end_element(__CPP_TRANSPORT_NODE_GENERAL_TPLOT_K_SERIAL_NUMBERS);
					}


		    template <typename number>
		    void threepf_time_data<number>::derive_lines(typename data_manager<number>::datapipe& pipe, const std::vector<unsigned int>& time_sample,
		                                                 plot2d_product<number>& plot, std::list<plot2d_line<number> >& lines) const
			    {
			    }


				template <typename number>
				void threepf_time_data<number>::serialize(serialization_writer& writer) const
					{
						this->write_value_node(writer, __CPP_TRANSPORT_NODE_GENERAL_TPLOT_DATA_TYPE,
																   std::string(__CPP_TRANSPORT_NODE_GENERAL_TPLOT_DATA_THREEPF));

						this->active_indices.serialize(writer);

				    this->begin_array(writer, __CPP_TRANSPORT_NODE_GENERAL_TPLOT_K_SERIAL_NUMBERS, this->kconfig_sample_sns.size() == 0);
				    for(std::vector<unsigned int>::const_iterator t = this->kconfig_sample_sns.begin(); t != this->kconfig_sample_sns.end(); t++)
					    {
				        this->begin_node(writer, "arrayelt", false);    // node name ignored for arrays
				        this->write_value_node(writer, __CPP_TRANSPORT_NODE_GENERAL_TPLOT_K_SERIAL_NUMBER, *t);
				        this->end_element(writer, "arrayelt");
					    }
				    this->end_element(writer, __CPP_TRANSPORT_NODE_GENERAL_TPLOT_K_SERIAL_NUMBERS);

				    this->general_time_data<number>::serialize(writer);
					}


				namespace
					{

						namespace general_time_data_helper
							{

								template <typename number>
								general_time_data<number>* deserialize(serialization_reader* reader, model<number>* m)
									{
										assert(m != nullptr);
										assert(reader != nullptr);

										if(m == nullptr) throw runtime_exception(runtime_exception::RUNTIME_ERROR, __CPP_TRANSPORT_PRODUCT_GENERAL_TPLOT_NULL_MODEL);
								    if(reader == nullptr) throw runtime_exception(runtime_exception::RUNTIME_ERROR, __CPP_TRANSPORT_PRODUCT_GENERAL_TPLOT_NULL_READER);

										general_time_data<number>* rval = nullptr;

								    std::string type;
										reader->read_value(__CPP_TRANSPORT_NODE_GENERAL_TPLOT_DATA_TYPE, type);

										if      (type == __CPP_TRANSPORT_NODE_GENERAL_TPLOT_DATA_BACKGROUND) rval = new background_time_data<number>(reader, m);
										else if (type == __CPP_TRANSPORT_NODE_GENERAL_TPLOT_DATA_TWOPF)      rval = new twopf_time_data<number>(reader, m);
										else if (type == __CPP_TRANSPORT_NODE_GENERAL_TPLOT_DATA_THREEPF)    rval = new threepf_time_data<number>(reader, m);
										else
											{
										    std::ostringstream msg;
												msg << __CPP_TRANSPORT_PRODUCT_GENERAL_TPLOT_LINE_TYPE_UNKNOWN << " '" << type << "'";
												throw runtime_exception(runtime_exception::SERIALIZATION_ERROR, msg.str());
											}

										return(rval);
									}

							}   // namespace general_time_data_helper

					}   // unnamed namespace


		    template <typename number>
		    class general_time_plot : public time_plot<number>
			    {

		      public:

		        // CONSTRUCTOR, DESTRUCTOR

		        //! Basic user-facing constructor. Accepts a filename, integration task,
		        //! filter function which determines which times are plotted.
		        //! Also requires a model reference.
		        general_time_plot(const std::string& name, const std::string& filename, const integration_task<number>& tk,
		                          typename time_plot<number>::time_filter filter, model<number>* m);

		        //! Deserialization constructor.
		        //! Accepts a serialization_reader and a model reference.
		        //! Remaining data is extracted from the reader.
		        general_time_plot(const std::string& name, const integration_task<number>* tk, model<number>* m,
		                          serialization_reader* reader);

				    //! Copy constructor. Required to perform a deep copy on the general_time_data objects we own
				    general_time_plot(const general_time_plot<number>& obj);

				    ~general_time_plot();


				    // ADD LINES TO THE PLOT

		      public:

				    //! Add a group of general_time_data lines to the plot.
				    //! Hides the underlying add_line provided by plot2d_product
				    void add_line(const general_time_data<number>& line);

		        // APPLY THIS DERIVATION TO PRODUCE OUTPUT -- implements a 'derived_product' interface

		      public:

		        //! Apply this derivation to produce a plot
		        virtual void derive(typename data_manager<number>::datapipe& pipe) override;


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
		    general_time_plot<number>::general_time_plot(const std::string& name, const std::string& filename, const integration_task<number>& tk,
		                                                 typename time_plot<number>::time_filter filter, model<number>* m)
			    : mdl(m), time_plot<number>(name, filename, tk, filter)
			    {
		        assert(m != nullptr);

		        if(m == nullptr) throw runtime_exception(runtime_exception::RUNTIME_ERROR, __CPP_TRANSPORT_PRODUCT_GENERAL_TPLOT_NULL_MODEL);
			    }


		    template <typename number>
		    general_time_plot<number>::general_time_plot(const std::string& name, const integration_task<number>* tk, model<number>* m,
		                                                 serialization_reader* reader)
			    : mdl(m), time_plot<number>(name, tk, reader)
			    {
		        assert(m != nullptr);
		        assert(reader != nullptr);

		        if(m == nullptr) throw runtime_exception(runtime_exception::RUNTIME_ERROR, __CPP_TRANSPORT_PRODUCT_GENERAL_TPLOT_NULL_MODEL);
		        if(reader == nullptr) throw runtime_exception(runtime_exception::RUNTIME_ERROR, __CPP_TRANSPORT_PRODUCT_GENERAL_TPLOT_NULL_READER);

		        unsigned int lines = reader->start_array(__CPP_TRANSPORT_NODE_GENERAL_TPLOT_LINE_ARRAY);

		        data_lines.clear();
		        for(unsigned int i = 0; i < lines; i++)
			        {
		            reader->start_array_element();

		            general_time_data<number>* data = general_time_data_helper::deserialize(reader, m);
		            data_lines.push_back(data);

		            reader->end_array_element();
			        }

		        reader->end_element(__CPP_TRANSPORT_NODE_GENERAL_TPLOT_LINE_ARRAY);
			    }


				template <typename number>
				general_time_plot<number>::general_time_plot(const general_time_plot<number>& obj)
					: mdl(obj.mdl), time_plot<number>(obj)
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
				void general_time_plot<number>::derive(typename data_manager<number>::datapipe& pipe)
					{
				    // ensure that the supplied pipe is attached to a data container
				    if(!pipe.validate()) throw runtime_exception(runtime_exception::DATAPIPE_ERROR, __CPP_TRANSPORT_PRODUCT_GENERAL_TPLOT_NULL_DATAPIPE);

				    // reset the plot, then pull data from the datapipe to regenerate ourselves
				    this->reset_plot();

				    // set time axis data
				    this->set_time_axis(pipe);

						// loop through all data lines, adding their components to the plot
						for(typename std::list< general_time_data<number>* >::const_iterator t = this->data_lines.begin(); t != this->data_lines.end(); t++)
							{
						    std::list< plot2d_line<number> > lines;

						    (*t)->derive_lines(pipe, this->get_time_sample_sns(), *this, lines);

								for(typename std::list< plot2d_line<number> >::const_iterator u = lines.begin(); u != lines.end(); u++)
									{
										this->plot2d_product<number>::add_line(*u);
									}
							}

						this->make_plot(pipe);
					}


				template <typename number>
				void general_time_plot<number>::serialize(serialization_writer& writer) const
					{
						this->write_value_node(writer, __CPP_TRANSPORT_NODE_DERIVED_PRODUCT_TYPE, std::string(__CPP_TRANSPORT_NODE_DERIVED_PRODUCT_GENERAL_TIME_PLOT));

						this->begin_array(writer, __CPP_TRANSPORT_NODE_GENERAL_TPLOT_LINE_ARRAY, this->data_lines.size() == 0);
						for(typename std::list< general_time_data<number>* >::const_iterator t = this->data_lines.begin(); t != this->data_lines.end(); t++)
							{
								this->begin_node(writer, "arrayelt", false);    // node name ignored in array
						    (*t)->serialize(writer);
								this->end_element(writer, "arrayelt");
							}
						this->end_element(writer, __CPP_TRANSPORT_NODE_GENERAL_TPLOT_LINE_ARRAY);

						// call next serialization
						this->time_plot<number>::serialize(writer);
					}


			}

	}


#endif //__general_time_plot_H_
