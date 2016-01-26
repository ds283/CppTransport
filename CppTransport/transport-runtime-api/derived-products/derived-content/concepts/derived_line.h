//
// Created by David Seery on 26/05/2014.
// Copyright (c) 2014-2016 University of Sussex. All rights reserved.
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
#include "transport-runtime-api/data/datapipe/datapipe_decl.h"

#include "transport-runtime-api/derived-products/line-collections/data_line.h"

#include "transport-runtime-api/derived-products/utilities/index_selector.h"
#include "transport-runtime-api/derived-products/utilities/wrapper.h"

#include "transport-runtime-api/derived-products/line-collections/line_values.h"


#define CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_TASK_NAME                                 "task-name"

#define CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_ROOT                                      "derived-line"

#define CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_DOT_TYPE                                  "threepf-momenta"
#define CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_DOT_DERIVATIVE                            "derivatives"
#define CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_DOT_MOMENTA                               "momenta"

#define CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_KLABEL_TYPE                               "k-label-type"
#define CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_KLABEL_CONVENTIONAL                       "conventional"
#define CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_KLABEL_COMOVING                           "comoving"

#define CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_LABEL_SET                                 "label-set"
#define CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_LABEL_LATEX                               "label-latex"
#define CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_LABEL_NONLATEX                            "label-nonlatex"
#define CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_LABEL_USE_TAGS                            "label-tags"

#define CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_PRECISION                                 "precision"

#define CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_X_AXIS_CLASS                              "axis-class"
#define CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_SUPPORTED_X_AXES                          "supported-axes"
#define CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_CURRENT_X_AXIS_VALUE                      "x-axis-current"


// derived content types
#define CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_TYPE                                      "derived-line-type"
#define CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_BACKGROUND                                "background"
#define CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_BACKGROUND_LINE                           "background-line"
#define CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_U2_LINE                                   "u2-line"
#define CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_U3_LINE                                   "u3-line"
#define CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_LARGEST_U2_LINE                           "largest-u2-line"
#define CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_LARGEST_U3_LINE                           "largest-u3-line"
#define CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_TWOPF_TIME_SERIES                         "twopf-time-series"
#define CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_THREEPF_TIME_SERIES                       "threepf-time-series"
#define CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_TENSOR_TWOPF_TIME_SERIES                  "tensor-twopf-time-series"
#define CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_ZETA_TWOPF_TIME_SERIES                    "zeta-twopf-time-series"
#define CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_ZETA_THREEPF_TIME_SERIES                  "zeta-threepf-time-series"
#define CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_ZETA_REDUCED_BISPECTRUM_TIME_SERIES       "zeta-reduced-bispectrum-time-series"
#define CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_TWOPF_WAVENUMBER_SERIES                   "twopf-k-series"
#define CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_THREEPF_WAVENUMBER_SERIES                 "threepf-k-series"
#define CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_TENSOR_TWOPF_WAVENUMBER_SERIES            "tensor-twopf-k-series"
#define CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_ZETA_TWOPF_WAVENUMBER_SERIES              "zeta-twopf-k-series"
#define CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_ZETA_THREEPF_WAVENUMBER_SERIES            "zeta-threepf-k-series"
#define CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_ZETA_REDUCED_BISPECTRUM_WAVENUMBER_SERIES "zeta-reduced-bispectrum-k-series"
#define CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_FNL_TIME_SERIES                           "fNL-time-series"
#define CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_R_TIME_SERIES                             "r-time-series"
#define CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_R_WAVENUMBER_SERIES                       "r-k-series"
#define CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_INTEGRATION_COST                          "integration-cost"


// common serialization groups used by derived products
#define CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_K_QUERY                                   "kconfig-query"

#define CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_T_QUERY                                   "time-query"

#define CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_THREEPF_LABEL_KT                          "use-kt-label"
#define CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_THREEPF_LABEL_ALPHA                       "use-alpha-label"
#define CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_THREEPF_LABEL_BETA                        "use-beta-label"

// maximum number of serial numbers to output when writing ourselves to
// a standard stream
#define CPPTRANSPORT_PRODUCT_MAX_SN                                                      (15)
#define CPPTRANSPORT_PRODUCT_MAX_SUPPORTED_AXES                                          (5)


namespace transport
	{

		namespace derived_data
			{

				//! content producer
				template <typename number>
				class derived_line: public serializable
					{

				  public:


						// CONSTRUCTOR, DESTRUCTOR

				  public:

						//! Basic user-facing constructor
						derived_line(const derivable_task<number>& tk, axis_class at, std::list< axis_value > sax,
						             unsigned int prec=CPPTRANSPORT_DEFAULT_PLOT_PRECISION);

						//! Dummy constructor, should not be used.
						//! Has to be provided as part of our virtual inheritance strategy (mainly to silence
						//! a warning from the Intel compiler), but not intended to be used
						//! to construct real objects.
						derived_line(const task<number>& tk);

				    //! Deserialization constructor
						derived_line(Json::Value& reader, task_finder<number>& finder);

						//! Dummy deserialization constructor, should not be used
						//! Has to be provided as part of our virtual inheritance strategy (mainly to silence
						//! a warning from the Intel compiler), but not intended to be used
						//! to construct real objects.
						derived_line(Json::Value& reader);

						// Override default copy constructor to perform a deep copy of the parent task
						derived_line(const derived_line<number>& obj);

						virtual ~derived_line();


				    // ADMIN

				  public:

				    //! Get parent task
				    derivable_task<number>* get_parent_task() const { return(this->parent_task); }

            //! Get axis type
            axis_class get_x_axis_class() const { return(this->x_class); }

				    //! get dot meaning
				    dot_type get_dot_meaning() const { return(this->dot_meaning); }

				    //! set dot meaning
				    void set_dot_meaning(dot_type t) { this->dot_meaning = t; }

				    //! get label meaning
				    klabel_type get_klabel_meaning() const { return(this->klabel_meaning); }

				    //! set label meaning
				    void set_klabel_meaning(klabel_type t) { this->klabel_meaning = t; }

						//! get current x-axis value type
						axis_value get_current_x_axis_value() const { return(this->x_type); }

						//! set current x-axis value
						void set_current_x_axis_value(axis_value v);

						//! set label text
						void set_label_text(const std::string& latex, const std::string& non_latex);

						//! clear label text
						void clear_label_text();

						//! has a label been set?
						bool is_label_set() const { return(this->label_set); }

						//! use tags?
						bool get_label_tags() const { return(this->use_tags); }

						//! set use tags flag
						void set_label_tags(bool g) { this->use_tags = g; }


				    // DATAPIPE HANDLING

				  protected:

				    //! attach datapipe to output group
            std::string attach(datapipe<number>& pipe, const std::list<std::string>& tags) const ;

						//! attach datapipe to output group, using specified task
            std::string attach(datapipe<number>& pipe, const std::list<std::string>& tags, derivable_task<number>* tk) const;

				    //! detach datapipe from output group
				    void detach(datapipe<number>& detach) const;


				    // DERIVE LINES

				  public:

				    //! generate data lines for plotting
				    virtual void derive_lines(datapipe<number>& pipe, std::list<data_line<number> >& lines,
				                              const std::list<std::string>& tags) const = 0;


				    // CLONE

				  public:

				    //! self-replicate
				    virtual derived_line<number>* clone() const = 0;


				    // WRITE TO A STREAM

				  public:

				    //! write self-details to a stream
				    virtual void write(std::ostream& out);


						// SERIALIZATION -- implements a 'serializable' interface

				  public:

						//! Serialize this object
						virtual void serialize(Json::Value& writer) const override;


				    // INTERNAL DATA

          protected:

						//! x-xis class
						axis_class x_class;

						//! Current x-axis type
						axis_value x_type;

						//! Supported axis values
						std::list< axis_value > supported_x_axes;

						//! LaTeX version of label (optional)
						std::string LaTeX_label;

						//! non-LaTeX version of label (optional)
						std::string non_LaTeX_label;

						//! has an x-label been set?
						bool label_set;

						//! generate tags when making labels?
						bool use_tags;

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

//				    //! List of time configuration serial numbers associated with this derived product
//				    std::vector<unsigned int> time_sample_sns;
//
//						//! List of k-configuration serial numbers associated with this derived product
//						std::vector<unsigned int> kconfig_sample_sns;

					};


				template <typename number>
				derived_line<number>::derived_line(const derivable_task<number>& tk, axis_class at, std::list< axis_value > sax, unsigned int prec)
					: x_class(at),
					  supported_x_axes(sax),
            dot_meaning(dot_type::momenta),
            klabel_meaning(klabel_type::conventional),
            label_set(false),
            use_tags(true),
            precision(prec),
            parent_task(dynamic_cast< derivable_task<number>* >(tk.clone()))
					{
						assert(parent_task != nullptr);

				    if(parent_task == nullptr) throw runtime_exception(exception_type::RUNTIME_ERROR, CPPTRANSPORT_PRODUCT_DERIVED_LINE_NOT_DERIVABLE_TASK);
						if(supported_x_axes.size() == 0) throw runtime_exception(exception_type::RUNTIME_ERROR, CPPTRANSPORT_PRODUCT_DERIVED_LINE_NO_AXIS_TYPES);

						x_type = supported_x_axes.front();
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
				derived_line<number>::derived_line(Json::Value& reader, task_finder<number>& finder)
					: parent_task(nullptr)
					{
				    precision       = reader[CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_ROOT][CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_PRECISION].asUInt();
				    label_set       = reader[CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_ROOT][CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_LABEL_SET].asBool();
				    LaTeX_label     = reader[CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_ROOT][CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_LABEL_LATEX].asString();
				    non_LaTeX_label = reader[CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_ROOT][CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_LABEL_NONLATEX].asString();
						use_tags        = reader[CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_ROOT][CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_LABEL_USE_TAGS].asBool();

				    std::string parent_task_name = reader[CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_ROOT][CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_TASK_NAME].asString();

				    // extract parent task
            std::unique_ptr< task_record<number> > tk_rec = finder(parent_task_name);
            assert(tk_rec.get() != nullptr);

						parent_task = dynamic_cast< derivable_task<number>* >(tk_rec->get_abstract_task()->clone());
						assert(parent_task != nullptr);

						// Deserialize: axis type for this derived line
				    std::string xtype = reader[CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_ROOT][CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_X_AXIS_CLASS].asString();
				    if(xtype == CPPTRANSPORT_NODE_PRODUCT_AXIS_CLASS_TIME)                       x_class = axis_class::time_axis;
				    else if(xtype == CPPTRANSPORT_NODE_PRODUCT_AXIS_CLASS_WAVENUMBER)            x_class = axis_class::wavenumber_axis;
				    else if(xtype == CPPTRANSPORT_NODE_PRODUCT_AXIS_CLASS_THREEPF_CONFIGURATION) x_class = axis_class::threepf_kconfig_axis;
				    else
					    {
				        std::ostringstream msg;
				        msg << CPPTRANSPORT_PRODUCT_DERIVED_LINE_UNKNOWN_X_AXIS_CLASS << " '" << xtype << "'";
				        throw runtime_exception(exception_type::SERIALIZATION_ERROR, msg.str());
					    }

						// Deserialize: supported x-axis values
				    Json::Value supported_array = reader[CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_ROOT][CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_SUPPORTED_X_AXES];
						supported_x_axes.clear();
						assert(supported_array.isArray());
						for(Json::Value::iterator t = supported_array.begin(); t != supported_array.end(); ++t)
							{
								// decode this array element
						    std::string value_string = t->asString();
								axis_value value = axis_value::efolds_axis;
								if(value_string == CPPTRANSPORT_NODE_PRODUCT_AXIS_VALUE_EFOLDS)                     value = axis_value::efolds_axis;
								else if(value_string == CPPTRANSPORT_NODE_PRODUCT_AXIS_VALUE_WAVENUMBER)            value = axis_value::k_axis;
								else if(value_string == CPPTRANSPORT_NODE_PRODUCT_AXIS_VALUE_EFOLDS_EXIT)           value = axis_value::efolds_exit_axis;
								else if(value_string == CPPTRANSPORT_NODE_PRODUCT_AXIS_VALUE_ALPHA)                 value = axis_value::alpha_axis;
								else if(value_string == CPPTRANSPORT_NODE_PRODUCT_AXIS_VALUE_BETA)                  value = axis_value::beta_axis;
								else if(value_string == CPPTRANSPORT_NODE_PRODUCT_AXIS_VALUE_SQUEEZING_FRACTION_K1) value = axis_value::squeezing_fraction_k1_axis;
								else if(value_string == CPPTRANSPORT_NODE_PRODUCT_AXIS_VALUE_SQUEEZING_FRACTION_K2) value = axis_value::squeezing_fraction_k2_axis;
								else if(value_string == CPPTRANSPORT_NODE_PRODUCT_AXIS_VALUE_SQUEEZING_FRACTION_K3) value = axis_value::squeezing_fraction_k3_axis;
								else
									{
								    std::ostringstream msg;
										msg << CPPTRANSPORT_PRODUCT_DERIVED_LINE_UNKNOWN_X_AXIS_VALUE << " '" << value_string << "'";
								    throw runtime_exception(exception_type::SERIALIZATION_ERROR, msg.str());
									}

								// add this value to the list of supported axes, if it is not already present
						    std::list< axis_value >::iterator u = std::find(supported_x_axes.begin(), supported_x_axes.end(), value);
								if(u == supported_x_axes.end()) supported_x_axes.push_back(value);
							}

						// Deserialize: current x-axis type
				    std::string x_type_string = reader[CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_ROOT][CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_CURRENT_X_AXIS_VALUE].asString();
				    if(x_type_string == CPPTRANSPORT_NODE_PRODUCT_AXIS_VALUE_EFOLDS)                     x_type = axis_value::efolds_axis;
				    else if(x_type_string == CPPTRANSPORT_NODE_PRODUCT_AXIS_VALUE_WAVENUMBER)            x_type = axis_value::k_axis;
				    else if(x_type_string == CPPTRANSPORT_NODE_PRODUCT_AXIS_VALUE_EFOLDS_EXIT)           x_type = axis_value::efolds_exit_axis;
				    else if(x_type_string == CPPTRANSPORT_NODE_PRODUCT_AXIS_VALUE_ALPHA)                 x_type = axis_value::alpha_axis;
				    else if(x_type_string == CPPTRANSPORT_NODE_PRODUCT_AXIS_VALUE_BETA)                  x_type = axis_value::beta_axis;
				    else if(x_type_string == CPPTRANSPORT_NODE_PRODUCT_AXIS_VALUE_SQUEEZING_FRACTION_K1) x_type = axis_value::squeezing_fraction_k1_axis;
				    else if(x_type_string == CPPTRANSPORT_NODE_PRODUCT_AXIS_VALUE_SQUEEZING_FRACTION_K2) x_type = axis_value::squeezing_fraction_k2_axis;
				    else if(x_type_string == CPPTRANSPORT_NODE_PRODUCT_AXIS_VALUE_SQUEEZING_FRACTION_K3) x_type = axis_value::squeezing_fraction_k3_axis;
				    else
					    {
				        std::ostringstream msg;
				        msg << CPPTRANSPORT_PRODUCT_DERIVED_LINE_UNKNOWN_X_AXIS_VALUE << " '" << x_type_string << "'";
				        throw runtime_exception(exception_type::SERIALIZATION_ERROR, msg.str());
					    }
				    std::list< axis_value >::iterator fv = std::find(supported_x_axes.begin(), supported_x_axes.end(), x_type);
						if(fv == supported_x_axes.end())
							{
						    std::stringstream msg;
								msg << CPPTRANSPORT_PRODUCT_DERIVED_LINE_X_AXIS_NOT_SUPPORTED << " '" << x_type_string << "'";
								throw runtime_exception(exception_type::SERIALIZATION_ERROR, msg.str());
							}

						// Deserialize: meaning of 'dot' for this derived line
				    std::string dot_meaning_value = reader[CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_ROOT][CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_DOT_TYPE].asString();

				    if(dot_meaning_value == CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_DOT_DERIVATIVE)   dot_meaning = dot_type::derivatives;
				    else if(dot_meaning_value == CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_DOT_MOMENTA) dot_meaning = dot_type::momenta;
				    else
					    {
				        std::ostringstream msg;
				        msg << CPPTRANSPORT_PRODUCT_DERIVED_LINE_DOT_TYPE_UNKNOWN << " '" << dot_meaning_value << "'";
				        throw runtime_exception(exception_type::SERIALIZATION_ERROR, msg.str());
					    }

						// Deserialize: meaning of k-labels for this derived line
				    std::string label_meaning_value = reader[CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_ROOT][CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_KLABEL_TYPE].asString();

				    if(label_meaning_value == CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_KLABEL_CONVENTIONAL)  klabel_meaning = klabel_type::conventional;
				    else if(label_meaning_value == CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_KLABEL_COMOVING) klabel_meaning = klabel_type::comoving;
				    else
					    {
				        std::ostringstream msg;
				        msg << CPPTRANSPORT_PRODUCT_DERIVED_LINE_KLABEL_TYPE_UNKNOWN << " '" << label_meaning_value << "'";
				        throw runtime_exception(exception_type::SERIALIZATION_ERROR, msg.str());
					    }
					}


				template <typename number>
				derived_line<number>::derived_line(const derived_line<number>& obj)
					: x_class(obj.x_class),
					  x_type(obj.x_type),
					  supported_x_axes(obj.supported_x_axes),
            dot_meaning(obj.dot_meaning),
					  klabel_meaning(obj.klabel_meaning),
					  label_set(obj.label_set),
					  LaTeX_label(obj.LaTeX_label),
					  non_LaTeX_label(obj.non_LaTeX_label),
					  use_tags(obj.use_tags),
            precision(obj.precision),
		        parent_task(dynamic_cast<derivable_task<number>*>(obj.parent_task->clone()))
					{
						assert(this->parent_task != nullptr);

				    if(parent_task == nullptr)
					    throw runtime_exception(exception_type::RUNTIME_ERROR, CPPTRANSPORT_PRODUCT_DERIVED_LINE_NOT_DERIVABLE_TASK);
					}


				template <typename number>
				derived_line<number>::~derived_line()
					{
						assert(this->parent_task != nullptr);
						delete this->parent_task;
					}


				template <typename number>
				void derived_line<number>::set_current_x_axis_value(axis_value v)
					{
				    std::list< axis_value >::iterator fv = std::find(this->supported_x_axes.begin(), this->supported_x_axes.end(), v);

						if(fv == this->supported_x_axes.end())
							{
						    std::ostringstream msg;
								msg << CPPTRANSPORT_PRODUCT_DERIVED_LINE_UNSUPPORTED_X_AXIS_TYPE;
								throw runtime_exception(exception_type::RUNTIME_ERROR, msg.str());
							}

						this->x_type = v;
					}


				template <typename number>
				void derived_line<number>::set_label_text(const std::string& latex, const std::string& non_latex)
					{
						if(latex == "" && non_latex == "")    // treat as an attempt to clear the labels
							{
								this->clear_label_text();
							}
						else
							{
						    this->LaTeX_label     = latex;
						    this->non_LaTeX_label = non_latex;
						    this->label_set       = true;
							}
					}


				template <typename number>
				void derived_line<number>::clear_label_text()
					{
						this->LaTeX_label.clear();
						this->non_LaTeX_label.clear();
						this->label_set = false;
					}


		    template <typename number>
        std::string derived_line<number>::attach(datapipe<number>& pipe, const std::list<std::string>& tags) const
          {
		        return(pipe.attach(this->parent_task, tags));
			    }


		    template <typename number>
        std::string derived_line<number>::attach(datapipe<number>& pipe, const std::list<std::string>& tags, derivable_task<number>* tk) const
        {
				    assert(tk != nullptr);
		        return(pipe.attach(tk, tags));
        }

		    template <typename number>
		    void derived_line<number>::detach(datapipe<number>& pipe) const
			    {
		        pipe.detach();
			    }


				template <typename number>
				void derived_line<number>::serialize(Json::Value& writer) const
					{
				    writer[CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_ROOT][CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_TASK_NAME]      = this->parent_task->get_name();
				    writer[CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_ROOT][CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_PRECISION]      = this->precision;
				    writer[CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_ROOT][CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_LABEL_SET]      = this->label_set;
				    writer[CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_ROOT][CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_LABEL_LATEX]    = this->LaTeX_label;
				    writer[CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_ROOT][CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_LABEL_NONLATEX] = this->non_LaTeX_label;
						writer[CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_ROOT][CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_LABEL_USE_TAGS] = this->use_tags;

						// Serialize: axis type of this derived line
				    switch(this->x_class)
							{
						    case axis_class::time_axis:
							    writer[CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_ROOT][CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_X_AXIS_CLASS] = std::string(CPPTRANSPORT_NODE_PRODUCT_AXIS_CLASS_TIME);
									break;

						    case axis_class::wavenumber_axis:
							    writer[CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_ROOT][CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_X_AXIS_CLASS] = std::string(CPPTRANSPORT_NODE_PRODUCT_AXIS_CLASS_WAVENUMBER);
									break;

				        case axis_class::threepf_kconfig_axis:
					        writer[CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_ROOT][CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_X_AXIS_CLASS] = std::string(CPPTRANSPORT_NODE_PRODUCT_AXIS_CLASS_THREEPF_CONFIGURATION);
						      break;
							}

						// Serialize: supported x-axis types
				    Json::Value supported_array(Json::arrayValue);
						for(std::list< axis_value >::const_iterator t = this->supported_x_axes.begin(); t != this->supported_x_axes.end(); ++t)
							{
								switch(*t)
									{
								    case axis_value::efolds_axis:
									    supported_array.append(Json::Value(std::string(CPPTRANSPORT_NODE_PRODUCT_AXIS_VALUE_EFOLDS)));
											break;

								    case axis_value::k_axis:
									    supported_array.append(Json::Value(std::string(CPPTRANSPORT_NODE_PRODUCT_AXIS_VALUE_WAVENUMBER)));
											break;

								    case axis_value::efolds_exit_axis:
									    supported_array.append(Json::Value(std::string(CPPTRANSPORT_NODE_PRODUCT_AXIS_VALUE_EFOLDS_EXIT)));
											break;

								    case axis_value::alpha_axis:
									    supported_array.append(Json::Value(std::string(CPPTRANSPORT_NODE_PRODUCT_AXIS_VALUE_ALPHA)));
											break;

								    case axis_value::beta_axis:
									    supported_array.append(Json::Value(std::string(CPPTRANSPORT_NODE_PRODUCT_AXIS_VALUE_BETA)));
											break;

								    case axis_value::squeezing_fraction_k1_axis:
									    supported_array.append(Json::Value(std::string(CPPTRANSPORT_NODE_PRODUCT_AXIS_VALUE_SQUEEZING_FRACTION_K1)));
											break;

								    case axis_value::squeezing_fraction_k2_axis:
									    supported_array.append(Json::Value(std::string(CPPTRANSPORT_NODE_PRODUCT_AXIS_VALUE_SQUEEZING_FRACTION_K2)));
									    break;

								    case axis_value::squeezing_fraction_k3_axis:
									    supported_array.append(Json::Value(std::string(CPPTRANSPORT_NODE_PRODUCT_AXIS_VALUE_SQUEEZING_FRACTION_K3)));
									    break;

								    default:
									    assert(false);
											throw runtime_exception(exception_type::RUNTIME_ERROR, CPPTRANSPORT_PRODUCT_DERIVED_LINE_UNKNOWN_X_AXIS_VALUE);
									}
							}
						writer[CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_ROOT][CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_SUPPORTED_X_AXES] = supported_array;

						// Serialize: current x-axis type
				    switch(this->x_type)
					    {
				        case axis_value::efolds_axis:
					        writer[CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_ROOT][CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_CURRENT_X_AXIS_VALUE] = std::string(CPPTRANSPORT_NODE_PRODUCT_AXIS_VALUE_EFOLDS);
				          break;

				        case axis_value::k_axis:
					        writer[CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_ROOT][CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_CURRENT_X_AXIS_VALUE] = std::string(CPPTRANSPORT_NODE_PRODUCT_AXIS_VALUE_WAVENUMBER);
				          break;

				        case axis_value::efolds_exit_axis:
					        writer[CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_ROOT][CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_CURRENT_X_AXIS_VALUE] = std::string(CPPTRANSPORT_NODE_PRODUCT_AXIS_VALUE_EFOLDS_EXIT);
				          break;

				        case axis_value::alpha_axis:
					        writer[CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_ROOT][CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_CURRENT_X_AXIS_VALUE] = std::string(CPPTRANSPORT_NODE_PRODUCT_AXIS_VALUE_ALPHA);
				          break;

				        case axis_value::beta_axis:
					       writer[CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_ROOT][CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_CURRENT_X_AXIS_VALUE] = std::string(CPPTRANSPORT_NODE_PRODUCT_AXIS_VALUE_BETA);
				          break;

				        case axis_value::squeezing_fraction_k1_axis:
					        writer[CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_ROOT][CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_CURRENT_X_AXIS_VALUE] = std::string(CPPTRANSPORT_NODE_PRODUCT_AXIS_VALUE_SQUEEZING_FRACTION_K1);
				          break;

				        case axis_value::squeezing_fraction_k2_axis:
					        writer[CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_ROOT][CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_CURRENT_X_AXIS_VALUE] = std::string(CPPTRANSPORT_NODE_PRODUCT_AXIS_VALUE_SQUEEZING_FRACTION_K2);
				          break;

				        case axis_value::squeezing_fraction_k3_axis:
					        writer[CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_ROOT][CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_CURRENT_X_AXIS_VALUE] = std::string(CPPTRANSPORT_NODE_PRODUCT_AXIS_VALUE_SQUEEZING_FRACTION_K3);
				          break;

				        default:
					        assert(false);
				          throw runtime_exception(exception_type::RUNTIME_ERROR, CPPTRANSPORT_PRODUCT_DERIVED_LINE_UNKNOWN_X_AXIS_VALUE);
					    }

						// Serialize: meaning of 'dot' for this derived line
				    switch(this->dot_meaning)
					    {
				        case dot_type::derivatives:
					        writer[CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_ROOT][CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_DOT_TYPE] = std::string(CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_DOT_DERIVATIVE);
				          break;

                case dot_type::momenta:
					        writer[CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_ROOT][CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_DOT_TYPE] = std::string(CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_DOT_MOMENTA);
				          break;
					    }

						// Serialize: meaning of k-labels for this derived line
				    switch(this->klabel_meaning)
					    {
				        case klabel_type::conventional:
					        writer[CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_ROOT][CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_KLABEL_TYPE] = std::string(CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_KLABEL_CONVENTIONAL);
				          break;

                case klabel_type::comoving:
					        writer[CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_ROOT][CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_KLABEL_TYPE] = std::string(CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_KLABEL_COMOVING);
				          break;
					    }
					}


				template <typename number>
				void derived_line<number>::write(std::ostream& out)
					{
						unsigned int saved_left_margin = this->wrapper.get_left_margin();
						this->wrapper.set_left_margin(2);

//						out << "  " << CPPTRANSPORT_PRODUCT_DERIVED_LINE_LABEL << '\n';
						out << "  " << CPPTRANSPORT_PRODUCT_DERIVED_LINE_TASK_NAME << " '" << this->parent_task->get_name() << "'" << '\n';

						// output model details if this derived line is directly associated with an integration task
						integration_task<number>* itk = dynamic_cast< integration_task<number>* >(this->parent_task);
						if(itk != nullptr)
							{
						    out << ", " <<CPPTRANSPORT_PRODUCT_DERIVED_LINE_MODEL_NAME << " '" << itk->get_model()->get_name()
							    << " [" << itk->get_model()->get_author() << " | " << itk->get_model()->get_tag() << "]" << '\n';

							}

						out << "  " << CPPTRANSPORT_PRODUCT_DERIVED_LINE_X_AXIS_CLASS << " ";
						switch(this->x_class)
							{
						    case axis_class::time_axis:
							    out << CPPTRANSPORT_PRODUCT_DERIVED_LINE_CLASS_TIME_LABEL << '\n';
									break;

						    case axis_class::wavenumber_axis:
									out << CPPTRANSPORT_PRODUCT_DERIVED_LINE_CLASS_WAVENUMBER_LABEL << '\n';
									break;

						    case axis_class::threepf_kconfig_axis:
							    out << CPPTRANSPORT_PRODUCT_DERIVED_LINE_CLASS_THREEPF_CONFIGURATION_LABEL << '\n';
                  break;
							}

						unsigned int count = 0;

						this->wrapper.wrap_out(out, "  " CPPTRANSPORT_PRODUCT_DERIVED_LINE_X_AXIS_SUPPORTED " ");
						for(std::list< axis_value >::const_iterator t = this->supported_x_axes.begin(); t != this->supported_x_axes.end() && count < CPPTRANSPORT_PRODUCT_MAX_SUPPORTED_AXES; ++t)
							{
								switch(*t)
									{
								    case axis_value::efolds_axis:
									    this->wrapper.wrap_list_item(out, true, CPPTRANSPORT_PRODUCT_DERIVED_LINE_X_AXIS_VALUE_EFOLDS, count);
											break;

								    case axis_value::k_axis:
									    this->wrapper.wrap_list_item(out, true, CPPTRANSPORT_PRODUCT_DERIVED_LINE_X_AXIS_VALUE_K, count);
											break;

								    case axis_value::efolds_exit_axis:
									    this->wrapper.wrap_list_item(out, true, CPPTRANSPORT_NODE_PRODUCT_AXIS_VALUE_EFOLDS_EXIT, count);
											break;

								    case axis_value::alpha_axis:
									    this->wrapper.wrap_list_item(out, true, CPPTRANSPORT_NODE_PRODUCT_AXIS_VALUE_ALPHA, count);
											break;

								    case axis_value::beta_axis:
									    this->wrapper.wrap_list_item(out, true, CPPTRANSPORT_NODE_PRODUCT_AXIS_VALUE_BETA, count);
											break;

								    case axis_value::squeezing_fraction_k1_axis:
									    this->wrapper.wrap_list_item(out, true, CPPTRANSPORT_NODE_PRODUCT_AXIS_VALUE_SQUEEZING_FRACTION_K1, count);
											break;

								    case axis_value::squeezing_fraction_k2_axis:
									    this->wrapper.wrap_list_item(out, true, CPPTRANSPORT_NODE_PRODUCT_AXIS_VALUE_SQUEEZING_FRACTION_K2, count);
											break;

								    case axis_value::squeezing_fraction_k3_axis:
									    this->wrapper.wrap_list_item(out, true, CPPTRANSPORT_NODE_PRODUCT_AXIS_VALUE_SQUEEZING_FRACTION_K3, count);
											break;

								    default:
									    assert(false);
											throw runtime_exception(exception_type::RUNTIME_ERROR, CPPTRANSPORT_PRODUCT_DERIVED_LINE_UNKNOWN_X_AXIS_VALUE);
									}
							}
						if(count == CPPTRANSPORT_PRODUCT_MAX_SUPPORTED_AXES) this->wrapper.wrap_list_item(out, true, "...", count);
				    this->wrapper.wrap_newline(out);

						out << CPPTRANSPORT_PRODUCT_DERIVED_LINE_CURRENT_X_AXIS << " ";
				    switch(this->x_type)
					    {
				        case axis_value::efolds_axis:
					        out << CPPTRANSPORT_PRODUCT_DERIVED_LINE_X_AXIS_VALUE_EFOLDS << '\n';
				          break;

				        case axis_value::k_axis:
					        out << CPPTRANSPORT_PRODUCT_DERIVED_LINE_X_AXIS_VALUE_K << '\n';
				          break;

				        case axis_value::efolds_exit_axis:
					        out << CPPTRANSPORT_NODE_PRODUCT_AXIS_VALUE_EFOLDS_EXIT << '\n';
				          break;

				        case axis_value::alpha_axis:
					        out << CPPTRANSPORT_NODE_PRODUCT_AXIS_VALUE_ALPHA << '\n';
				          break;

				        case axis_value::beta_axis:
					        out << CPPTRANSPORT_NODE_PRODUCT_AXIS_VALUE_BETA << '\n';
				          break;

				        case axis_value::squeezing_fraction_k1_axis:
					        out << CPPTRANSPORT_NODE_PRODUCT_AXIS_VALUE_SQUEEZING_FRACTION_K1 << '\n';
				          break;

				        case axis_value::squeezing_fraction_k2_axis:
					        out << CPPTRANSPORT_NODE_PRODUCT_AXIS_VALUE_SQUEEZING_FRACTION_K2 << '\n';
				          break;

				        case axis_value::squeezing_fraction_k3_axis:
					        out << CPPTRANSPORT_NODE_PRODUCT_AXIS_VALUE_SQUEEZING_FRACTION_K3 << '\n';
				          break;

				        default:
					        assert(false);
				          throw runtime_exception(exception_type::RUNTIME_ERROR, CPPTRANSPORT_PRODUCT_DERIVED_LINE_UNKNOWN_X_AXIS_VALUE);
					    }

						out << "  " << CPPTRANSPORT_PRODUCT_DERIVED_LINE_DOT_MEANING << " ";
						switch(this->dot_meaning)
							{
						    case dot_type::derivatives:
							    out << CPPTRANSPORT_PRODUCT_DERIVED_LINE_DOT_DERIVATIVE;
									break;

                case dot_type::momenta:
							    out << CPPTRANSPORT_PRODUCT_DERIVED_LINE_DOT_MOMENTA;
									break;
							}

						out << ", " << CPPTRANSPORT_PRODUCT_DERIVED_LINE_KLABEL_MEANING << " ";
						switch(this->klabel_meaning)
							{
						    case klabel_type::comoving:
							    out << CPPTRANSPORT_PRODUCT_DERIVED_LINE_KLABEL_COMOVING;
									break;

                case klabel_type::conventional:
							    out << CPPTRANSPORT_PRODUCT_DERIVED_LINE_KLABEL_CONVENTIONAL;
									break;
							}

				    count = 0;

						this->wrapper.wrap_newline(out);
//				    this->wrapper.wrap_out(out, CPPTRANSPORT_PRODUCT_TIME_SERIES_TSAMPLE_SN_LABEL " ");
//				    for(std::vector<unsigned int>::const_iterator t = this->time_sample_sns.begin(); t != this->time_sample_sns.end() && count < CPPTRANSPORT_PRODUCT_MAX_SN; ++t)
//					    {
//				        std::ostringstream msg;
//				        msg << (*t);
//
//				        this->wrapper.wrap_list_item(out, true, msg.str(), count);
//					    }
//				    if(count == CPPTRANSPORT_PRODUCT_MAX_SN) this->wrapper.wrap_list_item(out, true, "...", count);
//				    this->wrapper.wrap_newline(out);
//
//				    count = 0;
//				    this->wrapper.wrap_out(out, CPPTRANSPORT_PRODUCT_TIME_SERIES_KCONFIG_SN_LABEL " ");
//				    for(std::vector<unsigned int>::const_iterator t = this->kconfig_sample_sns.begin(); t != this->kconfig_sample_sns.end() && count < CPPTRANSPORT_PRODUCT_MAX_SN; ++t)
//					    {
//				        std::ostringstream msg;
//				        msg << (*t);
//
//				        this->wrapper.wrap_list_item(out, true, msg.str(), count);
//					    }
//				    if(count == CPPTRANSPORT_PRODUCT_MAX_SN) this->wrapper.wrap_list_item(out, true, "...", count);
//				    this->wrapper.wrap_newline(out);

						this->wrapper.set_left_margin(saved_left_margin);
					}

			};   // derived_data


	}   // namespace transport


#endif //__derived_line_H_
