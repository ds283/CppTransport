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

#include "transport-runtime-api/derived-products/line_values.h"


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

#define __CPP_TRANSPORT_NODE_PRODUCT_DERIVED_LINE_T_SERIAL_NUMBERS                          "time-serial-numbers"

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


						// CONSTRUCTOR, DESTRUCTOR

				  public:

						//! Basic user-facing constructor
						derived_line(const derivable_task<number>& tk, axis_type at,
						             unsigned int prec=__CPP_TRANSPORT_DEFAULT_PLOT_PRECISION);

						//! Dummy constructor, should not be used.
						//! Has to be provided as part of our virtual inheritance strategy (mainly to silence
						//! a warning from the Intel compiler), but not intended to be used
						//! to construct real objects.
						derived_line(const task<number>& tk);

				    //! Deserialization constructor
						derived_line(Json::Value& reader, typename repository_finder<number>::task_finder& finder);

						//! Dummy deserialization constructor, should not be used
						//! Has to be provided as part of our virtual inheritance strategy (mainly to silence
						//! a warning from the Intel compiler), but not intended to be used
						//! to construct real objects.
						derived_line(Json::Value& reader);

						// Override default copy constructor to perform a deep copy of the parent task
						derived_line(const derived_line<number>& obj);

						virtual ~derived_line();


				    // ADMIN

				    //! Get parent task
				    derivable_task<number>* get_parent_task() const { return(this->parent_task); }

            //! Get axis type
						axis_type get_axis_type() const { return(this->x_type); }

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
						virtual void serialize(Json::Value& writer) const override;


				    // INTERNAL DATA

          protected:

						//! Axis type
						axis_type x_type;

				    //! record the task which 'owns' this derived content
				    derivable_task<number>* parent_task;

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
				derived_line<number>::derived_line(const derivable_task<number>& tk, axis_type at, unsigned int prec)
					: x_type(at),
            dot_meaning(momenta),
            klabel_meaning(conventional),
            precision(prec),
            parent_task(dynamic_cast< derivable_task<number>* >(tk.clone()))
					{
						assert(parent_task != nullptr);

				    if(parent_task == nullptr)
					    throw runtime_exception(runtime_exception::RUNTIME_ERROR, __CPP_TRANSPORT_PRODUCT_DERIVED_LINE_NOT_DERIVABLE_TASK);
					}


				template <typename number>
				derived_line<number>::derived_line(const task<number>& tk)
					: parent_task(nullptr)
					{
						assert(false);
					}


				template <typename number>
				derived_line<number>::derived_line(Json::Value& reader)
					: parent_task(nullptr)
					{
						assert(false);
					}


				template <typename number>
				derived_line<number>::derived_line(Json::Value& reader, typename repository_finder<number>::task_finder& finder)
					: parent_task(nullptr)
					{
				    precision = reader[__CPP_TRANSPORT_NODE_PRODUCT_DERIVED_LINE_PRECISION].asUInt();

				    std::string parent_task_name = reader[__CPP_TRANSPORT_NODE_PRODUCT_DERIVED_LINE_TASK_NAME].asString();

				    // extract parent task
            std::unique_ptr< task_record<number> > tk_rec(finder(parent_task_name));
            assert(tk_rec.get() != nullptr);

						parent_task = dynamic_cast< derivable_task<number>* >(tk_rec->get_abstract_task()->clone());
						assert(parent_task != nullptr);

						// Deserialize: axis type for this derived line
				    std::string xtype = reader[__CPP_TRANSPORT_NODE_PRODUCT_DERIVED_LINE_XTYPE].asString();
				    if(xtype == __CPP_TRANSPORT_NODE_PRODUCT_DERIVED_LINE_TIME_SERIES)             x_type = time_axis;
				    else if(xtype == __CPP_TRANSPORT_NODE_PRODUCT_DERIVED_LINE_K_SERIES)           x_type = wavenumber_axis;
				    else if(xtype == __CPP_TRANSPORT_NODE_PRODUCT_DERIVED_LINE_ANGLE_SERIES)       x_type = angle_axis;
				    else if(xtype == __CPP_TRANSPORT_NODE_PRODUCT_DERIVED_LINE_SQUEEZING_FRACTION) x_type = squeezing_fraction_axis;
				    else
					    {
				        std::ostringstream msg;
				        msg << __CPP_TRANSPORT_PRODUCT_DERIVED_LINE_UNKNOWN_XTYPE << " '" << xtype << "'";
				        throw runtime_exception(runtime_exception::SERIALIZATION_ERROR, msg.str());
					    }

						// Deserialize: meaning of 'dot' for this derived line
				    std::string dot_meaning_value = reader[__CPP_TRANSPORT_NODE_PRODUCT_DERIVED_LINE_DOT_TYPE].asString();

				    if(dot_meaning_value == __CPP_TRANSPORT_NODE_PRODUCT_DERIVED_LINE_DOT_DERIVATIVE)   dot_meaning = derivatives;
				    else if(dot_meaning_value == __CPP_TRANSPORT_NODE_PRODUCT_DERIVED_LINE_DOT_MOMENTA) dot_meaning = momenta;
				    else
					    {
				        std::ostringstream msg;
				        msg << __CPP_TRANSPORT_PRODUCT_DERIVED_LINE_DOT_TYPE_UNKNOWN << " '" << dot_meaning_value << "'";
				        throw runtime_exception(runtime_exception::SERIALIZATION_ERROR, msg.str());
					    }

						// Deserialize: meaning of k-labels for this derived line
				    std::string label_meaning_value = reader[__CPP_TRANSPORT_NODE_PRODUCT_DERIVED_LINE_KLABEL_TYPE].asString();

				    if(label_meaning_value == __CPP_TRANSPORT_NODE_PRODUCT_DERIVED_LINE_KLABEL_CONVENTIONAL)  klabel_meaning = conventional;
				    else if(label_meaning_value == __CPP_TRANSPORT_NODE_PRODUCT_DERIVED_LINE_KLABEL_COMOVING) klabel_meaning = comoving;
				    else
					    {
				        std::ostringstream msg;
				        msg << __CPP_TRANSPORT_PRODUCT_DERIVED_LINE_KLABEL_TYPE_UNKNOWN << " '" << label_meaning_value << "'";
				        throw runtime_exception(runtime_exception::SERIALIZATION_ERROR, msg.str());
					    }

						// Deserialize: list of time configuration sample points
				    Json::Value& time_sns_list = reader[__CPP_TRANSPORT_NODE_PRODUCT_DERIVED_LINE_T_SERIAL_NUMBERS];
						assert(time_sns_list.isArray());

				    for(Json::Value::iterator t = time_sns_list.begin(); t != time_sns_list.end(); t++)
					    {
				        time_sample_sns.push_back(t->asUInt());
					    }

				    // Deserialize: list of k-configuration sample points
				    Json::Value& kconfig_sns_list = reader[__CPP_TRANSPORT_NODE_PRODUCT_DERIVED_LINE_K_SERIAL_NUMBERS];
						assert(kconfig_sns_list.isArray());

				    for(Json::Value::iterator t = kconfig_sns_list.begin(); t != kconfig_sns_list.end(); t++)
					    {
				        kconfig_sample_sns.push_back(t->asUInt());
					    }
					}


				template <typename number>
				derived_line<number>::derived_line(const derived_line<number>& obj)
					: x_type(obj.x_type),
            dot_meaning(obj.dot_meaning),
					  klabel_meaning(obj.klabel_meaning),
            precision(obj.precision),
            time_sample_sns(obj.time_sample_sns),
            kconfig_sample_sns(obj.kconfig_sample_sns),
		        parent_task(dynamic_cast<derivable_task<number>*>(obj.parent_task->clone()))
					{
						assert(this->parent_task != nullptr);

				    if(parent_task == nullptr)
					    throw runtime_exception(runtime_exception::RUNTIME_ERROR, __CPP_TRANSPORT_PRODUCT_DERIVED_LINE_NOT_DERIVABLE_TASK);
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
		        pipe.attach(this->parent_task, tags);
			    }


		    template <typename number>
		    void derived_line<number>::detach(datapipe<number>& pipe) const
			    {
		        pipe.detach();
			    }


				template <typename number>
				void derived_line<number>::serialize(Json::Value& writer) const
					{
				    writer[__CPP_TRANSPORT_NODE_PRODUCT_DERIVED_LINE_TASK_NAME] = this->parent_task->get_name();
				    writer[__CPP_TRANSPORT_NODE_PRODUCT_DERIVED_LINE_PRECISION] = this->precision;

						// Serialize: axis type of this derived line
				    switch(this->x_type)
							{
						    case time_axis:
							    writer[__CPP_TRANSPORT_NODE_PRODUCT_DERIVED_LINE_XTYPE] = std::string(__CPP_TRANSPORT_NODE_PRODUCT_DERIVED_LINE_TIME_SERIES);
									break;

						    case wavenumber_axis:
							    writer[__CPP_TRANSPORT_NODE_PRODUCT_DERIVED_LINE_XTYPE] = std::string(__CPP_TRANSPORT_NODE_PRODUCT_DERIVED_LINE_K_SERIES);
									break;

				        case angle_axis:
					        writer[__CPP_TRANSPORT_NODE_PRODUCT_DERIVED_LINE_XTYPE] = std::string(__CPP_TRANSPORT_NODE_PRODUCT_DERIVED_LINE_ANGLE_SERIES);
						      break;

				        case squeezing_fraction_axis:
					        writer[__CPP_TRANSPORT_NODE_PRODUCT_DERIVED_LINE_XTYPE] = std::string(__CPP_TRANSPORT_NODE_PRODUCT_DERIVED_LINE_SQUEEZING_FRACTION);
						      break;

						    default:
							    assert(false);
									throw runtime_exception(runtime_exception::RUNTIME_ERROR, __CPP_TRANSPORT_PRODUCT_DERIVED_LINE_UNKNOWN_XTYPE);
							}

						// Serialize: meaning of 'dot' for this derived line
				    switch(this->dot_meaning)
					    {
				        case derivatives:
					        writer[__CPP_TRANSPORT_NODE_PRODUCT_DERIVED_LINE_DOT_TYPE] = std::string(__CPP_TRANSPORT_NODE_PRODUCT_DERIVED_LINE_DOT_DERIVATIVE);
				          break;

				        case momenta:
					        writer[__CPP_TRANSPORT_NODE_PRODUCT_DERIVED_LINE_DOT_TYPE] = std::string(__CPP_TRANSPORT_NODE_PRODUCT_DERIVED_LINE_DOT_MOMENTA);
				          break;

				        default:
					        assert(false);
					        throw runtime_exception(runtime_exception::RUNTIME_ERROR, __CPP_TRANSPORT_PRODUCT_DERIVED_LINE_DOT_TYPE_UNKNOWN);
					    }

						// Serialize: meaning of k-labels for this derived line
				    switch(this->klabel_meaning)
					    {
				        case conventional:
					        writer[__CPP_TRANSPORT_NODE_PRODUCT_DERIVED_LINE_KLABEL_TYPE] = std::string(__CPP_TRANSPORT_NODE_PRODUCT_DERIVED_LINE_KLABEL_CONVENTIONAL);
				          break;

				        case comoving:
					        writer[__CPP_TRANSPORT_NODE_PRODUCT_DERIVED_LINE_KLABEL_TYPE] = std::string(__CPP_TRANSPORT_NODE_PRODUCT_DERIVED_LINE_KLABEL_COMOVING);
				          break;

				        default:
					        assert(false);
					        throw runtime_exception(runtime_exception::RUNTIME_ERROR, __CPP_TRANSPORT_PRODUCT_DERIVED_LINE_KLABEL_TYPE_UNKNOWN);
					    }

						// Serialize: list of time-configuration sample points
				    Json::Value time_sns_list(Json::arrayValue);

				    for(std::vector<unsigned int>::const_iterator t = this->time_sample_sns.begin(); t != this->time_sample_sns.end(); t++)
					    {
				        Json::Value sns_element = *t;
						    time_sns_list.append(sns_element);
					    }
				    writer[__CPP_TRANSPORT_NODE_PRODUCT_DERIVED_LINE_T_SERIAL_NUMBERS] = time_sns_list;

						// Serialize: list of k-configuration sample points
				    Json::Value kconfig_sns_list(Json::arrayValue);

				    for(std::vector<unsigned int>::const_iterator t = this->kconfig_sample_sns.begin(); t != this->kconfig_sample_sns.end(); t++)
					    {
				        Json::Value kconfig_element = *t;
						    kconfig_sns_list.append(kconfig_element);
					    }
				    writer[__CPP_TRANSPORT_NODE_PRODUCT_DERIVED_LINE_K_SERIAL_NUMBERS] = kconfig_sns_list;
					}


				template <typename number>
				void derived_line<number>::write(std::ostream& out)
					{
						out << __CPP_TRANSPORT_PRODUCT_DERIVED_LINE_LABEL << std::endl;
						out << __CPP_TRANSPORT_PRODUCT_DERIVED_LINE_TASK_NAME << " '" << this->parent_task->get_name();

						// output model details if this derived line is directly associated with an integration task
						integration_task<number>* itk = dynamic_cast< integration_task<number>* >(this->parent_task);
						if(itk != nullptr)
							{
						    out << ", " <<__CPP_TRANSPORT_PRODUCT_DERIVED_LINE_MODEL_NAME << " '" << itk->get_model()->get_name()
							    << " [" << itk->get_model()->get_author() << " | " << itk->get_model()->get_tag() << "]" << std::endl;

							}

						out << __CPP_TRANSPORT_PRODUCT_DERIVED_LINE_XTYPE << " ";
						switch(this->x_type)
							{
						    case time_axis:
							    out << __CPP_TRANSPORT_PRODUCT_DERIVED_LINE_TIME_SERIES_LABEL << std::endl;
									break;

						    case wavenumber_axis:
									out << __CPP_TRANSPORT_PRODUCT_DERIVED_LINE_K_SERIES_LABEL << std::endl;
									break;

						    case angle_axis:
							    out << __CPP_TRANSPORT_PRODUCT_DERIVED_LINE_ANGLE_SERIES_LABEL << std::endl;
									break;

						    case squeezing_fraction_axis:
							    out << __CPP_TRANSPORT_PRODUCT_DERIVED_LINE_SQUEEZING_FRACTION_LABEL << std::endl;
									break;

						    default:
							    assert(false);
									throw runtime_exception(runtime_exception::RUNTIME_ERROR, __CPP_TRANSPORT_PRODUCT_DERIVED_LINE_UNKNOWN_XTYPE);
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

			};   // derived_data


	}   // namespace transport


#endif //__derived_line_H_
