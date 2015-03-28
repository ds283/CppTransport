//
// Created by David Seery on 26/05/2014.
// Copyright (c) 2014-15 University of Sussex. All rights reserved.
//


#ifndef __derived_line_H_
#define __derived_line_H_


#include <iostream>
#include <sstream>
#include <string>
#include <list>
#include <vector>
#include <stdexcept>

#include "transport-runtime-api/serialization/serializable.h"

// forward-declare model class if needed
#include "transport-runtime-api/models/model_forward_declare.h"

// forward-declare repository records if needed
#include "transport-runtime-api/repository/records/repository_records_forward_declare.h"

// forward-declare tasks if needed
#include "transport-runtime-api/tasks/tasks_forward_declare.h"

// get details of datapipe<number>
#include "transport-runtime-api/data/datapipe/datapipe.h"

#include "transport-runtime-api/derived-products/data_line.h"

#include "transport-runtime-api/derived-products/utilities/index_selector.h"
#include "transport-runtime-api/derived-products/utilities/wrapper.h"
#include "transport-runtime-api/derived-products/utilities/filter.h"


#define __CPP_TRANSPORT_NODE_PRODUCT_DERIVED_LINE_TASK_NAME                                 "task-name"

#define __CPP_TRANSPORT_NODE_PRODUCT_DERIVED_LINE_DOT_TYPE                                  "threepf-momenta"
#define __CPP_TRANSPORT_NODE_PRODUCT_DERIVED_LINE_DOT_DERIVATIVE                            "derivatives"
#define __CPP_TRANSPORT_NODE_PRODUCT_DERIVED_LINE_DOT_MOMENTA                               "momenta"

#define __CPP_TRANSPORT_NODE_PRODUCT_DERIVED_LINE_KLABEL_TYPE                               "k-label-type"
#define __CPP_TRANSPORT_NODE_PRODUCT_DERIVED_LINE_KLABEL_CONVENTIONAL                       "conventional"
#define __CPP_TRANSPORT_NODE_PRODUCT_DERIVED_LINE_KLABEL_COMOVING                           "comoving"

#define __CPP_TRANSPORT_NODE_PRODUCT_DERIVED_LINE_PRECISION                                 "precision"

#define __CPP_TRANSPORT_NODE_PRODUCT_DERIVED_LINE_XTYPE                                     "axis-type"
#define __CPP_TRANSPORT_NODE_PRODUCT_DERIVED_LINE_TIME_SERIES                               "time-series"
#define __CPP_TRANSPORT_NODE_PRODUCT_DERIVED_LINE_K_SERIES                                  "k-series"
#define __CPP_TRANSPORT_NODE_PRODUCT_DERIVED_LINE_ANGLE_SERIES                              "angle-series"
#define __CPP_TRANSPORT_NODE_PRODUCT_DERIVED_LINE_SQUEEZING_FRACTION                        "squeezing-fraction"
#define __CPP_TRANSPORT_NODE_PRODUCT_DERIVED_LINE_YTYPE                                     "value-type"
#define __CPP_TRANSPORT_NODE_PRODUCT_DERIVED_LINE_CF                                        "correlation-function"
#define __CPP_TRANSPORT_NODE_PRODUCT_DERIVED_LINE_BGFIELD                                   "background-field"
#define __CPP_TRANSPORT_NODE_PRODUCT_DERIVED_LINE_FNL                                       "fNL"
#define __CPP_TRANSPORT_NODE_PRODUCT_DERIVED_LINE_R                                         "r"
#define __CPP_TRANSPORT_NODE_PRODUCT_DERIVED_LINE_SPECTRAL_INDEX                            "spectral-index"


// derived content types
#define __CPP_TRANSPORT_NODE_PRODUCT_DERIVED_LINE_TYPE                                      "derived-line-type"
#define __CPP_TRANSPORT_NODE_PRODUCT_DERIVED_LINE_BACKGROUND                                "background"
#define __CPP_TRANSPORT_NODE_PRODUCT_DERIVED_LINE_TWOPF_TIME_SERIES                         "twopf-time-series"
#define __CPP_TRANSPORT_NODE_PRODUCT_DERIVED_LINE_THREEPF_TIME_SERIES                       "threepf-time-series"
#define __CPP_TRANSPORT_NODE_PRODUCT_DERIVED_LINE_TENSOR_TWOPF_TIME_SERIES                  "tensor-twopf-time-series"
#define __CPP_TRANSPORT_NODE_PRODUCT_DERIVED_LINE_ZETA_TWOPF_TIME_SERIES                    "zeta-twopf-time-series"
#define __CPP_TRANSPORT_NODE_PRODUCT_DERIVED_LINE_ZETA_THREEPF_TIME_SERIES                  "zeta-threepf-time-series"
#define __CPP_TRANSPORT_NODE_PRODUCT_DERIVED_LINE_ZETA_REDUCED_BISPECTRUM_TIME_SERIES       "zeta-reduced-bispectrum-time-series"
#define __CPP_TRANSPORT_NODE_PRODUCT_DERIVED_LINE_TWOPF_WAVENUMBER_SERIES                   "twopf-k-series"
#define __CPP_TRANSPORT_NODE_PRODUCT_DERIVED_LINE_THREEPF_WAVENUMBER_SERIES                 "threepf-k-series"
#define __CPP_TRANSPORT_NODE_PRODUCT_DERIVED_LINE_TENSOR_TWOPF_WAVENUMBER_SERIES            "tensor-twopf-k-series"
#define __CPP_TRANSPORT_NODE_PRODUCT_DERIVED_LINE_ZETA_TWOPF_WAVENUMBER_SERIES              "zeta-twopf-k-series"
#define __CPP_TRANSPORT_NODE_PRODUCT_DERIVED_LINE_ZETA_THREEPF_WAVENUMBER_SERIES            "zeta-threepf-k-series"
#define __CPP_TRANSPORT_NODE_PRODUCT_DERIVED_LINE_ZETA_REDUCED_BISPECTRUM_WAVENUMBER_SERIES "zeta-reduced-bispectrum-k-series"
#define __CPP_TRANSPORT_NODE_PRODUCT_DERIVED_LINE_FNL_TIME_SERIES                           "fNL-time-series"
#define __CPP_TRANSPORT_NODE_PRODUCT_DERIVED_LINE_R_TIME_SERIES                             "r-time-series"
#define __CPP_TRANSPORT_NODE_PRODUCT_DERIVED_LINE_R_WAVENUMBER_SERIES                       "r-k-series"


// common serialization groups used by derived products
#define __CPP_TRANSPORT_NODE_PRODUCT_DERIVED_LINE_K_SERIAL_NUMBERS                          "kconfig-serial-numbers"
#define __CPP_TRANSPORT_NODE_PRODUCT_DERIVED_LINE_K_SERIAL_NUMBER                           "n"

#define __CPP_TRANSPORT_NODE_PRODUCT_DERIVED_LINE_T_SERIAL_NUMBERS                          "time-serial-numbers"
#define __CPP_TRANSPORT_NODE_PRODUCT_DERIVED_LINE_T_SERIAL_NUMBER                           "n"

#define __CPP_TRANSPORT_NODE_PRODUCT_DERIVED_LINE_THREEPF_LABEL_KT                          "use-kt-label"
#define __CPP_TRANSPORT_NODE_PRODUCT_DERIVED_LINE_THREEPF_LABEL_ALPHA                       "use-alpha-label"
#define __CPP_TRANSPORT_NODE_PRODUCT_DERIVED_LINE_THREEPF_LABEL_BETA                        "use-beta-label"

// maximum number of serial numbers to output when writing ourselves to
// a standard stream
#define __CPP_TRANSPORT_PRODUCT_MAX_SN                                                      (15)


namespace transport
	{

		namespace derived_data
			{

				//! content producer
				template <typename number>
				class derived_line: public serializable
					{

				  public:

				    typedef enum { derivatives, momenta } dot_type;
				    typedef enum { conventional, comoving } klabel_type;

						typedef enum { time_series, wavenumber_series, angle_series, squeezing_fraction } axis_type;
				    typedef enum { background_field, correlation_function, fNL, r, spectral_index } value_type;


						// CONSTRUCTOR, DESTRUCTOR

				  public:

						//! Basic user-facing constructor
						derived_line(const integration_task<number>& tk, axis_type at, value_type vt,
						             unsigned int prec=__CPP_TRANSPORT_DEFAULT_PLOT_PRECISION);

						//! Dummy user-facing constructor, should not be used; called only for incorrectly-constructed instances
						derived_line(const integration_task<number>& tk);

				    //! Deserialization constructor
						derived_line(serialization_reader* reader, typename repository_finder<number>::task_finder& finder);

						//! Dummy deserialization constructor, should not be used; called only for incorrectly-constructed instances
						derived_line(serialization_reader* reader);

						// Override default copy constructor to perform a deep copy of the parent task
						derived_line(const derived_line<number>& obj);

						virtual ~derived_line();


				    // ADMIN

				    //! Get parent task
				    integration_task<number>* get_parent_task() const { return(this->parent_task); }

            //! Get axis type
						axis_type get_axis_type() const { return(this->x_type); }
						//! Get value type
						value_type get_value_type() const { return(this->y_type); }

				    //! get dot meaning
				    dot_type get_dot_meaning() const { return(this->dot_meaning); }
				    //! set dot meaning
				    void set_dot_meaning(dot_type t) { this->dot_meaning = t; }

				    //! get label meaning
				    klabel_type get_klabel_meaning() const { return(this->klabel_meaning); }
				    //! set label meaning
				    void set_klabel_meaning(klabel_type t) { this->klabel_meaning = t; }


				    // DATAPIPE HANDLING

				  public:

				    //! attach datapipe to output group
				    void attach(datapipe<number>& pipe, const std::list<std::string>& tags) const;

				    //! detach datapipe from output group
				    void detach(datapipe<number>& detach) const;


				    // DERIVE LINES

				    //! generate data lines for plotting
				    virtual void derive_lines(datapipe<number>& pipe, std::list<data_line<number> >& lines,
				                              const std::list<std::string>& tags) const = 0;


				    // CLONE

				    //! self-replicate
				    virtual derived_line<number>* clone() const = 0;


				    // WRITE TO A STREAM

				    //! write self-details to a stream
				    virtual void write(std::ostream& out);


						// SERIALIZATION -- implements a 'serializable' interface

				  public:

						//! Serialize this object
						virtual void serialize(serialization_writer& writer) const override;


				    // INTERNAL DATA

          protected:

						//! Axis type
						axis_type x_type;

						//! Value type
						value_type y_type;

				    //! record task which corresponds to this derivation
				    integration_task<number>* parent_task;

				    //! record model used for this derivation
				    model<number>* mdl;

				    //! record meaning of momenta - do we interpret them as derivatives?
				    dot_type dot_meaning;

				    //! record meaning of k-labels -- are they conventional or comoving?
				    klabel_type klabel_meaning;

						//! record default plot precision
						unsigned int precision;

				    //! Wrapped output utility
				    wrapped_output wrapper;

				    //! gadget for performing t- or k-sample filtration
				    filter f;

				    //! List of time configuration serial numbers associated with this derived product
				    std::vector<unsigned int> time_sample_sns;

						//! List of k-configuration serial numbers associated with this derived product
						std::vector<unsigned int> kconfig_sample_sns;

					};


				template <typename number>
				derived_line<number>::derived_line(const integration_task<number>& tk, axis_type at, value_type vt, unsigned int prec)
					: x_type(at),
            y_type(vt),
            dot_meaning(momenta),
            klabel_meaning(conventional),
            precision(prec),
		        mdl(tk.get_model()),
            parent_task(dynamic_cast<integration_task<number>*>(tk.clone()))
					{
						assert(parent_task != nullptr);

				    if(parent_task == nullptr)
					    throw runtime_exception(runtime_exception::RUNTIME_ERROR, __CPP_TRANSPORT_PRODUCT_DERIVED_LINE_NOT_INTEGRATION_TASK);
					}


				template <typename number>
				derived_line<number>::derived_line(const integration_task<number>& tk)
					: parent_task(nullptr), mdl(nullptr)
					{
						assert(false);
					}


				template <typename number>
				derived_line<number>::derived_line(serialization_reader* reader)
					: parent_task(nullptr), mdl(nullptr)
					{
						assert(false);
					}


				template <typename number>
				derived_line<number>::derived_line(serialization_reader* reader, typename repository_finder<number>::task_finder& finder)
					: parent_task(nullptr), mdl(nullptr)
					{
						assert(reader != nullptr);

				    if(reader == nullptr) throw runtime_exception(runtime_exception::RUNTIME_ERROR, __CPP_TRANSPORT_PRODUCT_DERIVED_LINE_NULL_READER);

				    reader->read_value(__CPP_TRANSPORT_NODE_PRODUCT_DERIVED_LINE_PRECISION, precision);

				    std::string parent_task_name;
				    reader->read_value(__CPP_TRANSPORT_NODE_PRODUCT_DERIVED_LINE_TASK_NAME, parent_task_name);

				    // extract parent task and model
            std::unique_ptr< task_record<number> > tk_rec(finder(parent_task_name));
            assert(tk_rec.get() != nullptr);
            
            if(tk_rec->get_type() != task_record<number>::integration)
              throw runtime_exception(runtime_exception::REPOSITORY_ERROR, __CPP_TRANSPORT_REPO_OUTPUT_TASK_NOT_INTGRTN);
            
            integration_task_record<number>* int_rec = dynamic_cast< integration_task_record<number>* >(tk_rec.get());
            assert(int_rec != nullptr);
            
            parent_task = dynamic_cast<integration_task<number>*>(int_rec->get_task()->clone());
            mdl = parent_task->get_model();

						// Deserialize: axis type for this derived line
				    std::string xtype;
						reader->read_value(__CPP_TRANSPORT_NODE_PRODUCT_DERIVED_LINE_XTYPE, xtype);
						if(xtype == __CPP_TRANSPORT_NODE_PRODUCT_DERIVED_LINE_TIME_SERIES) x_type = time_series;
						else if(xtype == __CPP_TRANSPORT_NODE_PRODUCT_DERIVED_LINE_K_SERIES) x_type = wavenumber_series;
						else if(xtype == __CPP_TRANSPORT_NODE_PRODUCT_DERIVED_LINE_ANGLE_SERIES) x_type = angle_series;
						else if(xtype == __CPP_TRANSPORT_NODE_PRODUCT_DERIVED_LINE_SQUEEZING_FRACTION) x_type = squeezing_fraction;
						else
							{
						    std::ostringstream msg;
								msg << __CPP_TRANSPORT_PRODUCT_DERIVED_LINE_UNKNOWN_XTYPE << " '" << xtype << "'";
								throw runtime_exception(runtime_exception::SERIALIZATION_ERROR, msg.str());
							}

						// Deserialize: value type for this derived line
				    std::string ytype;
						reader->read_value(__CPP_TRANSPORT_NODE_PRODUCT_DERIVED_LINE_YTYPE, ytype);
						if(ytype == __CPP_TRANSPORT_NODE_PRODUCT_DERIVED_LINE_CF) y_type = correlation_function;
						else if(ytype == __CPP_TRANSPORT_NODE_PRODUCT_DERIVED_LINE_BGFIELD) y_type = background_field;
						else if(ytype == __CPP_TRANSPORT_NODE_PRODUCT_DERIVED_LINE_FNL) y_type = fNL;
						else if(ytype == __CPP_TRANSPORT_NODE_PRODUCT_DERIVED_LINE_R) y_type = r;
						else if(ytype == __CPP_TRANSPORT_NODE_PRODUCT_DERIVED_LINE_SPECTRAL_INDEX) y_type = spectral_index;
						else
							{
						    std::ostringstream msg;
								msg << __CPP_TRANSPORT_PRODUCT_DERIVED_LINE_UNKNOWN_YTYPE << " '" << ytype << "'";
								throw runtime_exception(runtime_exception::SERIALIZATION_ERROR, msg.str());
							}

						// Deserialize: meaning of 'dot' for this derived line
				    std::string dot_meaning_value;
				    reader->read_value(__CPP_TRANSPORT_NODE_PRODUCT_DERIVED_LINE_DOT_TYPE, dot_meaning_value);

				    if(dot_meaning_value == __CPP_TRANSPORT_NODE_PRODUCT_DERIVED_LINE_DOT_DERIVATIVE) dot_meaning = derivatives;
				    else if(dot_meaning_value == __CPP_TRANSPORT_NODE_PRODUCT_DERIVED_LINE_DOT_MOMENTA) dot_meaning = momenta;
				    else
					    {
				        std::ostringstream msg;
				        msg << __CPP_TRANSPORT_PRODUCT_DERIVED_LINE_DOT_TYPE_UNKNOWN << " '" << dot_meaning_value << "'";
				        throw runtime_exception(runtime_exception::SERIALIZATION_ERROR, msg.str());
					    }

						// Deserialize: meaning of k-labels for this derived line
				    std::string label_meaning_value;
				    reader->read_value(__CPP_TRANSPORT_NODE_PRODUCT_DERIVED_LINE_KLABEL_TYPE, label_meaning_value);

				    if(label_meaning_value == __CPP_TRANSPORT_NODE_PRODUCT_DERIVED_LINE_KLABEL_CONVENTIONAL) klabel_meaning = conventional;
				    else if(label_meaning_value == __CPP_TRANSPORT_NODE_PRODUCT_DERIVED_LINE_KLABEL_COMOVING) klabel_meaning = comoving;
				    else
					    {
				        std::ostringstream msg;
				        msg << __CPP_TRANSPORT_PRODUCT_DERIVED_LINE_KLABEL_TYPE_UNKNOWN << " '" << label_meaning_value << "'";
				        throw runtime_exception(runtime_exception::SERIALIZATION_ERROR, msg.str());
					    }

						// Deserialize: list of time configuration sample points
				    unsigned int time_sns = reader->start_array(__CPP_TRANSPORT_NODE_PRODUCT_DERIVED_LINE_T_SERIAL_NUMBERS);

				    for(unsigned int i = 0; i < time_sns; i++)
					    {
				        reader->start_array_element();

				        unsigned int sn;
				        reader->read_value(__CPP_TRANSPORT_NODE_PRODUCT_DERIVED_LINE_T_SERIAL_NUMBER, sn);
				        time_sample_sns.push_back(sn);

				        reader->end_array_element();
					    }
				    reader->end_element(__CPP_TRANSPORT_NODE_PRODUCT_DERIVED_LINE_T_SERIAL_NUMBERS);

						// Deserialize: list of k-configuration sample points
				    unsigned int kconfig_sns = reader->start_array(__CPP_TRANSPORT_NODE_PRODUCT_DERIVED_LINE_K_SERIAL_NUMBERS);

				    for(unsigned int i = 0; i < kconfig_sns; i++)
					    {
				        reader->start_array_element();

				        unsigned int sn;
				        reader->read_value(__CPP_TRANSPORT_NODE_PRODUCT_DERIVED_LINE_K_SERIAL_NUMBER, sn);
				        kconfig_sample_sns.push_back(sn);

				        reader->end_array_element();
					    }
				    reader->end_element(__CPP_TRANSPORT_NODE_PRODUCT_DERIVED_LINE_K_SERIAL_NUMBERS);
					}


				template <typename number>
				derived_line<number>::derived_line(const derived_line<number>& obj)
					: x_type(obj.x_type), y_type(obj.y_type),
            dot_meaning(obj.dot_meaning), klabel_meaning(obj.klabel_meaning),
            precision(obj.precision),
            time_sample_sns(obj.time_sample_sns),
            kconfig_sample_sns(obj.kconfig_sample_sns),
            mdl(obj.mdl),   // it's OK to shallow copy 'mdl'. Pointers to model instances are managed by the instance_manager
		        parent_task(dynamic_cast<integration_task<number>*>(obj.parent_task->clone()))
					{
						assert(this->parent_task != nullptr);

				    if(parent_task == nullptr)
					    throw runtime_exception(runtime_exception::RUNTIME_ERROR, __CPP_TRANSPORT_PRODUCT_DERIVED_LINE_NOT_INTEGRATION_TASK);
					}


				template <typename number>
				derived_line<number>::~derived_line()
					{
						assert(this->parent_task != nullptr);
						delete this->parent_task;
					}


		    template <typename number>
		    void derived_line<number>::attach(datapipe<number>& pipe, const std::list<std::string>& tags) const
			    {
		        pipe.attach(this->parent_task, this->mdl->get_N_fields(), tags);
			    }


		    template <typename number>
		    void derived_line<number>::detach(datapipe<number>& pipe) const
			    {
		        pipe.detach();
			    }


				template <typename number>
				void derived_line<number>::serialize(serialization_writer& writer) const
					{
				    writer.write_value(__CPP_TRANSPORT_NODE_PRODUCT_DERIVED_LINE_TASK_NAME, this->parent_task->get_name());

				    writer.write_value(__CPP_TRANSPORT_NODE_PRODUCT_DERIVED_LINE_PRECISION, this->precision);

						// Serialize: axis type of this derived line
				    switch(this->x_type)
							{
						    case time_series:
							    writer.write_value(__CPP_TRANSPORT_NODE_PRODUCT_DERIVED_LINE_XTYPE, std::string(__CPP_TRANSPORT_NODE_PRODUCT_DERIVED_LINE_TIME_SERIES));
									break;

						    case wavenumber_series:
							    writer.write_value(__CPP_TRANSPORT_NODE_PRODUCT_DERIVED_LINE_XTYPE, std::string(__CPP_TRANSPORT_NODE_PRODUCT_DERIVED_LINE_K_SERIES));
									break;

				        case angle_series:
					        writer.write_value(__CPP_TRANSPORT_NODE_PRODUCT_DERIVED_LINE_XTYPE, std::string(__CPP_TRANSPORT_NODE_PRODUCT_DERIVED_LINE_ANGLE_SERIES));
						      break;

				        case squeezing_fraction:
					        writer.write_value(__CPP_TRANSPORT_NODE_PRODUCT_DERIVED_LINE_XTYPE, std::string(__CPP_TRANSPORT_NODE_PRODUCT_DERIVED_LINE_SQUEEZING_FRACTION));
						      break;

						    default:
							    assert(false);
									throw runtime_exception(runtime_exception::RUNTIME_ERROR, __CPP_TRANSPORT_PRODUCT_DERIVED_LINE_UNKNOWN_XTYPE);
							}

						// Serialize: value type of this derived line
						switch(this->y_type)
							{
						    case correlation_function:
							    writer.write_value(__CPP_TRANSPORT_NODE_PRODUCT_DERIVED_LINE_YTYPE, std::string(__CPP_TRANSPORT_NODE_PRODUCT_DERIVED_LINE_CF));
									break;

						    case background_field:
							    writer.write_value(__CPP_TRANSPORT_NODE_PRODUCT_DERIVED_LINE_YTYPE, std::string(__CPP_TRANSPORT_NODE_PRODUCT_DERIVED_LINE_BGFIELD));
									break;

						    case fNL:
							    writer.write_value(__CPP_TRANSPORT_NODE_PRODUCT_DERIVED_LINE_YTYPE, std::string(__CPP_TRANSPORT_NODE_PRODUCT_DERIVED_LINE_FNL));
									break;

						    case r:
							    writer.write_value(__CPP_TRANSPORT_NODE_PRODUCT_DERIVED_LINE_YTYPE, std::string(__CPP_TRANSPORT_NODE_PRODUCT_DERIVED_LINE_R));
									break;

						    case spectral_index:
							    writer.write_value(__CPP_TRANSPORT_NODE_PRODUCT_DERIVED_LINE_YTYPE, std::string(__CPP_TRANSPORT_NODE_PRODUCT_DERIVED_LINE_SPECTRAL_INDEX));
									break;

						    default:
							    assert(false);
									throw runtime_exception(runtime_exception::RUNTIME_ERROR, __CPP_TRANSPORT_PRODUCT_DERIVED_LINE_UNKNOWN_YTYPE);
							}

						// Serialize: meaning of 'dot' for this derived line
				    switch(this->dot_meaning)
					    {
				        case derivatives:
					        writer.write_value(__CPP_TRANSPORT_NODE_PRODUCT_DERIVED_LINE_DOT_TYPE, std::string(__CPP_TRANSPORT_NODE_PRODUCT_DERIVED_LINE_DOT_DERIVATIVE));
				        break;

				        case momenta:
					        writer.write_value(__CPP_TRANSPORT_NODE_PRODUCT_DERIVED_LINE_DOT_TYPE, std::string(__CPP_TRANSPORT_NODE_PRODUCT_DERIVED_LINE_DOT_MOMENTA));
				        break;

				        default:
					        assert(false);
					        throw runtime_exception(runtime_exception::RUNTIME_ERROR, __CPP_TRANSPORT_PRODUCT_DERIVED_LINE_DOT_TYPE_UNKNOWN);
					    }

						// Serialize: meaning of k-labels for this derived line
				    switch(this->klabel_meaning)
					    {
				        case conventional:
					        writer.write_value(__CPP_TRANSPORT_NODE_PRODUCT_DERIVED_LINE_KLABEL_TYPE, std::string(__CPP_TRANSPORT_NODE_PRODUCT_DERIVED_LINE_KLABEL_CONVENTIONAL));
				        break;

				        case comoving:
					        writer.write_value(__CPP_TRANSPORT_NODE_PRODUCT_DERIVED_LINE_KLABEL_TYPE, std::string(__CPP_TRANSPORT_NODE_PRODUCT_DERIVED_LINE_KLABEL_COMOVING));
				        break;

				        default:
					        assert(false);
					        throw runtime_exception(runtime_exception::RUNTIME_ERROR, __CPP_TRANSPORT_PRODUCT_DERIVED_LINE_KLABEL_TYPE_UNKNOWN);
					    }

						// Serialize: list of time-configuration sample points
				    writer.start_array(__CPP_TRANSPORT_NODE_PRODUCT_DERIVED_LINE_T_SERIAL_NUMBERS, this->time_sample_sns.size() == 0);
				    for(std::vector<unsigned int>::const_iterator t = this->time_sample_sns.begin(); t != this->time_sample_sns.end(); t++)
					    {
				        writer.start_node("arrayelt", false);    // node name ignored for arrays
				        writer.write_value(__CPP_TRANSPORT_NODE_PRODUCT_DERIVED_LINE_T_SERIAL_NUMBER, *t);
				        writer.end_element("arrayelt");
					    }
				    writer.end_element(__CPP_TRANSPORT_NODE_PRODUCT_DERIVED_LINE_T_SERIAL_NUMBERS);

						// Serialize: list of k-configuration sample points
				    writer.start_array(__CPP_TRANSPORT_NODE_PRODUCT_DERIVED_LINE_K_SERIAL_NUMBERS, this->kconfig_sample_sns.size() == 0);
				    for(std::vector<unsigned int>::const_iterator t = this->kconfig_sample_sns.begin(); t != this->kconfig_sample_sns.end(); t++)
					    {
				        writer.start_node("arrayelt", false);    // node name ignored for arrays
				        writer.write_value(__CPP_TRANSPORT_NODE_PRODUCT_DERIVED_LINE_K_SERIAL_NUMBER, *t);
				        writer.end_element("arrayelt");
					    }
				    writer.end_element(__CPP_TRANSPORT_NODE_PRODUCT_DERIVED_LINE_K_SERIAL_NUMBERS);

					}


				template <typename number>
				void derived_line<number>::write(std::ostream& out)
					{
						out << __CPP_TRANSPORT_PRODUCT_DERIVED_LINE_LABEL << std::endl;
						out << __CPP_TRANSPORT_PRODUCT_DERIVED_LINE_TASK_NAME << " '" << this->parent_task->get_name() << ", ";
						out << __CPP_TRANSPORT_PRODUCT_DERIVED_LINE_MODEL_NAME << " '" << this->mdl->get_name()
								<< " [" << this->mdl->get_author() << " | " << this->mdl->get_tag() << "]" << std::endl;

						out << __CPP_TRANSPORT_PRODUCT_DERIVED_LINE_XTYPE << " ";
						switch(this->x_type)
							{
						    case time_series:
							    out << __CPP_TRANSPORT_PRODUCT_DERIVED_LINE_TIME_SERIES_LABEL << std::endl;
									break;

						    case wavenumber_series:
									out << __CPP_TRANSPORT_PRODUCT_DERIVED_LINE_K_SERIES_LABEL << std::endl;
									break;

						    case angle_series:
							    out << __CPP_TRANSPORT_PRODUCT_DERIVED_LINE_ANGLE_SERIES_LABEL << std::endl;
									break;

						    case squeezing_fraction:
							    out << __CPP_TRANSPORT_PRODUCT_DERIVED_LINE_SQUEEZING_FRACTION_LABEL << std::endl;
									break;

						    default:
							    assert(false);
									throw runtime_exception(runtime_exception::RUNTIME_ERROR, __CPP_TRANSPORT_PRODUCT_DERIVED_LINE_UNKNOWN_XTYPE);
							}

						out << __CPP_TRANSPORT_PRODUCT_DERIVED_LINE_YTYPE << " ";
						switch(this->y_type)
							{
						    case correlation_function:
							    out << __CPP_TRANSPORT_PRODUCT_DERIVED_LINE_CF_LABEL << std::endl;
									break;

						    case background_field:
							    out << __CPP_TRANSPORT_PRODUCT_DERIVED_LINE_BGFIELD_LABEL << std::endl;
									break;

						    case fNL:
							    out << __CPP_TRANSPORT_PRODUCT_DERIVED_LINE_FNL_LABEL << std::endl;
									break;

						    case r:
							    out << __CPP_TRANSPORT_PRODUCT_DERIVED_LINE_R_LABEL << std::endl;
									break;

						    case spectral_index:
							    out << __CPP_TRANSPORT_PRODUCT_DERIVED_LINE_SPECTRAL_INDEX_LABEL << std::endl;
									break;

						    default:
							    assert(false);
									throw runtime_exception(runtime_exception::RUNTIME_ERROR, __CPP_TRANSPORT_PRODUCT_DERIVED_LINE_UNKNOWN_YTYPE);
							}


						out << __CPP_TRANSPORT_PRODUCT_DERIVED_LINE_DOT_MEANING << " ";
						switch(this->dot_meaning)
							{
						    case derivatives:
							    out << __CPP_TRANSPORT_PRODUCT_DERIVED_LINE_DOT_DERIVATIVE;
									break;

						    case momenta:
							    out << __CPP_TRANSPORT_PRODUCT_DERIVED_LINE_DOT_MOMENTA;
									break;

						    default:
							    assert(false);
									throw runtime_exception(runtime_exception::RUNTIME_ERROR, __CPP_TRANSPORT_PRODUCT_DERIVED_LINE_DOT_TYPE_UNKNOWN);
							}

						out << ", " << __CPP_TRANSPORT_PRODUCT_DERIVED_LINE_KLABEL_MEANING << " ";
						switch(this->klabel_meaning)
							{
						    case comoving:
							    out << __CPP_TRANSPORT_PRODUCT_DERIVED_LINE_KLABEL_COMOVING;
									break;

						    case conventional:
							    out << __CPP_TRANSPORT_PRODUCT_DERIVED_LINE_KLABEL_CONVENTIONAL;
									break;

						    default:
							    assert(false);
									throw runtime_exception(runtime_exception::RUNTIME_ERROR, __CPP_TRANSPORT_PRODUCT_DERIVED_LINE_KLABEL_TYPE_UNKNOWN);
							}

				    unsigned int count = 0;

				    this->wrapper.wrap_out(out, __CPP_TRANSPORT_PRODUCT_TIME_SERIES_TSAMPLE_SN_LABEL " ");
				    for(std::vector<unsigned int>::const_iterator t = this->time_sample_sns.begin(); t != this->time_sample_sns.end() && count < __CPP_TRANSPORT_PRODUCT_MAX_SN; t++)
					    {
				        std::ostringstream msg;
				        msg << (*t);

				        this->wrapper.wrap_list_item(out, true, msg.str(), count);
					    }
				    if(count == __CPP_TRANSPORT_PRODUCT_MAX_SN) this->wrapper.wrap_list_item(out, true, "...", count);
				    this->wrapper.wrap_newline(out);

				    count = 0;
				    this->wrapper.wrap_out(out, "  " __CPP_TRANSPORT_PRODUCT_TIME_SERIES_KCONFIG_SN_LABEL " ");
				    for(std::vector<unsigned int>::const_iterator t = this->kconfig_sample_sns.begin(); t != this->kconfig_sample_sns.end() && count < __CPP_TRANSPORT_PRODUCT_MAX_SN; t++)
					    {
				        std::ostringstream msg;
				        msg << (*t);

				        this->wrapper.wrap_list_item(out, true, msg.str(), count);
					    }
				    if(count == __CPP_TRANSPORT_PRODUCT_MAX_SN) this->wrapper.wrap_list_item(out, true, "...", count);
				    this->wrapper.wrap_newline(out);
					}


			}   // derived_data


	}   // namespace transport


#endif //__derived_line_H_
