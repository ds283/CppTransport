//
// Created by David Seery on 26/05/2014.
// --@@
// Copyright (c) 2016 University of Sussex. All rights reserved.
//
// This file is part of the CppTransport platform.
//
// CppTransport is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 2 of the License, or
// (at your option) any later version.
//
// CppTransport is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with CppTransport.  If not, see <http://www.gnu.org/licenses/>.
//
// @license: GPL-2
// @contributor: David Seery <D.Seery@sussex.ac.uk>
// --@@
//


#ifndef CPPTRANSPORT_DERIVED_LINE_H
#define CPPTRANSPORT_DERIVED_LINE_H


#include <iostream>
#include <sstream>
#include <string>
#include <list>
#include <utility>
#include <vector>
#include <stdexcept>

#include "transport-runtime/serialization/serializable.h"

// forward-declare model class if needed
#include "transport-runtime/models/model_forward_declare.h"

// forward-declare repository records if needed
#include "transport-runtime/repository/records/repository_records_forward_declare.h"

// forward-declare tasks if needed
#include "transport-runtime/tasks/tasks_forward_declare.h"

// get details of datapipe<number>
#include "transport-runtime/data/datapipe/datapipe_decl.h"

#include "transport-runtime/derived-products/line-collections/data_line.h"

#include "transport-runtime/derived-products/utilities/index_selector.h"
#include "transport-runtime/derived-products/utilities/wrapper.h"

#include "transport-runtime/derived-products/line-collections/line_values.h"

#include "transport-runtime/derived-products/derived-content/concepts/derived_line_type.h"


namespace transport
	{

		namespace derived_data
			{

        constexpr auto CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_TASK_NAME                                 = "task-name";

        constexpr auto CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_PARENT_TASK_LIST                          = "parent-tasks";
        constexpr auto CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_PARENT_TASK_LIST_TASKNAME                 = "name";
        constexpr auto CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_PARENT_TASK_LIST_SPECIFIERS               = "content-specifiers";
        constexpr auto CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_PARENT_TASK_LIST_TAG                      = "tag";

        constexpr auto CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_ROOT                                      = "derived-line";

        constexpr auto CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_DOT_TYPE                                  = "threepf-momenta";
        constexpr auto CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_DOT_DERIVATIVE                            = "derivatives";
        constexpr auto CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_DOT_MOMENTA                               = "momenta";

        constexpr auto CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_KLABEL_TYPE                               = "k-label-type";
        constexpr auto CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_KLABEL_CONVENTIONAL                       = "conventional";
        constexpr auto CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_KLABEL_COMOVING                           = "comoving";

        constexpr auto CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_LABEL_SET                                 = "label-set";
        constexpr auto CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_LABEL_LATEX                               = "label-latex";
        constexpr auto CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_LABEL_NONLATEX                            = "label-nonlatex";
        constexpr auto CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_LABEL_USE_TAGS                            = "label-tags";

        constexpr auto CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_PRECISION                                 = "precision";

        constexpr auto CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_X_AXIS_CLASS                              = "axis-class";
        constexpr auto CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_SUPPORTED_X_AXES                          = "supported-axes";
        constexpr auto CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_CURRENT_X_AXIS_VALUE                      = "x-axis-current";


        // derived content types
        constexpr auto CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_TYPE                                      = "derived-line-type";
        constexpr auto CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_BACKGROUND                                = "background";
        constexpr auto CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_BACKGROUND_LINE                           = "background-line";
        constexpr auto CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_U2_LINE                                   = "u2-line";
        constexpr auto CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_U3_LINE                                   = "u3-line";
        constexpr auto CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_LARGEST_U2_LINE                           = "largest-u2-line";
        constexpr auto CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_LARGEST_U3_LINE                           = "largest-u3-line";
        constexpr auto CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_TWOPF_TIME_SERIES                         = "twopf-time-series";
        constexpr auto CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_THREEPF_TIME_SERIES                       = "threepf-time-series";
        constexpr auto CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_TENSOR_TWOPF_TIME_SERIES                  = "tensor-twopf-time-series";
        constexpr auto CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_ZETA_TWOPF_TIME_SERIES                    = "zeta-twopf-time-series";
        constexpr auto CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_ZETA_THREEPF_TIME_SERIES                  = "zeta-threepf-time-series";
        constexpr auto CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_ZETA_REDUCED_BISPECTRUM_TIME_SERIES       = "zeta-reduced-bispectrum-time-series";
        constexpr auto CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_TWOPF_WAVENUMBER_SERIES                   = "twopf-k-series";
        constexpr auto CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_THREEPF_WAVENUMBER_SERIES                 = "threepf-k-series";
        constexpr auto CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_TENSOR_TWOPF_WAVENUMBER_SERIES            = "tensor-twopf-k-series";
        constexpr auto CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_ZETA_TWOPF_WAVENUMBER_SERIES              = "zeta-twopf-k-series";
        constexpr auto CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_ZETA_THREEPF_WAVENUMBER_SERIES            = "zeta-threepf-k-series";
        constexpr auto CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_ZETA_REDUCED_BISPECTRUM_WAVENUMBER_SERIES = "zeta-reduced-bispectrum-k-series";
        constexpr auto CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_FNL_TIME_SERIES                           = "fNL-time-series";
        constexpr auto CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_R_TIME_SERIES                             = "r-time-series";
        constexpr auto CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_R_WAVENUMBER_SERIES                       = "r-k-series";
        constexpr auto CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_INTEGRATION_COST                          = "integration-cost";


        // common serialization groups used by derived products
        constexpr auto CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_K_QUERY                                   = "kconfig-query";

        constexpr auto CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_T_QUERY                                   = "time-query";

        constexpr auto CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_THREEPF_LABEL_KT                          = "use-kt-label";
        constexpr auto CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_THREEPF_LABEL_ALPHA                       = "use-alpha-label";
        constexpr auto CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_THREEPF_LABEL_BETA                        = "use-beta-label";

        // maximum number of serial numbers to output when writing ourselves to
        // a standard stream
        constexpr unsigned int CPPTRANSPORT_PRODUCT_MAX_SN                                              = 15;
        constexpr unsigned int CPPTRANSPORT_PRODUCT_MAX_SUPPORTED_AXES                                  = 5;


				//! content producer
				template <typename number>
				class derived_line: public serializable
					{

				  public:

					  using task_set_type = typename derivable_task_set<number>::type;
            using task_set_element_type = typename derivable_task_set<number>::type::value_type;

					  using owning_task_initializer_list_type = std::initializer_list<task_set_element_type>;


						// CONSTRUCTOR, DESTRUCTOR

				  public:

						//! Simplest user-facing constructor with a single parent task
						derived_line(task_set_element_type tk, axis_class at,
                         std::set<axis_value> sax, unsigned int prec=CPPTRANSPORT_DEFAULT_PLOT_PRECISION);

						derived_line(owning_task_initializer_list_type tks, axis_class at, std::set<axis_value> sax,
                         unsigned int prec=CPPTRANSPORT_DEFAULT_PLOT_PRECISION);

						//! Dummy constructor, should not be used.
						//! Has to be provided as part of our virtual inheritance strategy (mainly to silence
						//! a warning from the Intel compiler), but not intended to be used
						//! to construct real objects.
						explicit derived_line(const task<number>& tk);

				    //! Deserialization constructor
						derived_line(Json::Value& reader, task_finder<number>& finder);

						//! Dummy deserialization constructor, should not be used
						//! Has to be provided as part of our virtual inheritance strategy (mainly to silence
						//! a warning from the Intel compiler), but not intended to be used
						//! to construct real objects.
						explicit derived_line(Json::Value& reader);

						// Override default copy constructor to perform a deep copy of the parent task
						derived_line(const derived_line<number>& obj);

						//! destructor can be defaulted
						~derived_line() override = default;


            // TYPE INTROSPECTION

          public:

            //! get type
            virtual derived_line_type get_line_type() const = 0;


				    // ADMINISTRATION INTERFACE

				  public:

				    //! Get parent task list
				    const task_set_type& get_parent_task_list() const { return this->parent_tasks; }

            //! Get axis type
            axis_class get_x_axis_class() const { return(this->x_class); }

				    //! get dot meaning
				    dot_type get_dot_meaning() const { return(this->dot_meaning); }

				    //! set dot meaning
				    derived_line<number>& set_dot_meaning(dot_type t) { this->dot_meaning = t; return *this; }

				    //! get label meaning
				    klabel_type get_klabel_meaning() const { return(this->klabel_meaning); }

				    //! set label meaning
				    derived_line<number>& set_klabel_meaning(klabel_type t) { this->klabel_meaning = t; return *this; }

						//! get current x-axis value type
						axis_value get_current_x_axis_value() const { return(this->x_type); }

						//! set current x-axis value
						derived_line<number>& set_current_x_axis_value(axis_value v);

						//! set label text
						derived_line<number>& set_label_text(const std::string& latex, const std::string& non_latex);

						//! clear label text
						derived_line<number>& clear_label_text();

						//! has a label been set?
						bool is_label_set() const { return(this->label_set); }

            //! get LaTeX label
            const std::string& get_LaTeX_label() const { return(this->LaTeX_label); }

            //! get non-LaTeX label
            const std::string& get_non_LaTeX_label() const { return(this->non_LaTeX_label); }

						//! use tags?
						bool get_label_tags() const { return(this->use_tags); }

						//! set use tags flag
						derived_line<number>& set_label_tags(bool g) { this->use_tags = g; return *this; }


				    // DATAPIPE HANDLING

				  protected:

				    //! attach datapipe to content group
            std::string attach(datapipe<number>& pipe, const tag_list& tags, unsigned int tag=0) const;

            //! detach datapipe from content group
				    void detach(datapipe<number>& pipe) const;


				    // DERIVE LINES

				  public:

				    //! generate data lines for plotting, output to tables, or whatever other purpose is intended
				    virtual void derive_lines(datapipe<number>& pipe, std::list<data_line<number> >& lines,
				                              const tag_list& tags, slave_message_buffer& messages) const = 0;


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
						void serialize(Json::Value& writer) const override;

					private:

					  // INTERNAL SERIALIZATION API

					  //! serialize parent class list
					  void serialize_parent_task_list(Json::Value& root) const;

					  //! serialize x-axis class
            void serialize_x_axis_class(Json::Value& root) const;
					  
					  //! serialize the list of supported x-axis types
            void serialize_supported_x_axis_types(Json::Value& root) const;

            //! serialize the currently-selected x-axis type
            void serialize_current_x_axis_value(Json::Value& root) const;

            //! serialize the currently-selected meaning for a dot (momentum or time derivative?)
            void serialize_dot_meaning(Json::Value& root) const;

            //! serialize the currently-selected units of k-values
            void serialize_klabel_meaning(Json::Value& root) const;


            // INTERNAL DE-SERIALIZATION API

            //! deserialize parent class list
            task_set_type deserialize_parent_task_list(const Json::Value& root, task_finder<number>& finder) const;

            //! deserialize new-style parent class list in derived_task_list<number>::type format
            task_set_type deserialize_new_parent_task_list(const Json::Value& root, task_finder<number>& finder) const;

            //! deserialize old-style parent task stored in a single field
            task_set_type deserialize_old_parent_task(const Json::Value& root, task_finder<number>& finder) const;

            //! deserialize x-axis class
            axis_class deserialize_x_axis_class(const Json::Value& root) const;

            //! deserialize the list of supported x-axis types
            std::set<axis_value> deserialize_supported_x_axis_types(const Json::Value& root) const;

            //! deserialize the currently-selected x-axis type
            axis_value deserialize_current_x_axis_value(const Json::Value& root) const;

            //! deserialize the currently-selected meaning for a dot (momentum or time derivative?)
            dot_type deserialize_dot_meaning(const Json::Value& root) const;

            //! deserialize the currently-selected units of k-values
            klabel_type deserialize_klabel_meaning(const Json::Value& root) const;



            // INTERNAL DATA

          protected:


					  // ADMIN DATA

            //! Record the tasks that contribute to this derived content. There can be several such tasks,
            //! e.g. for an r-line, which accepts contributions from a zeta correlation function task
            //! *and* a field correlation function task
            task_set_type parent_tasks;

					  // AXIS DATA

						//! x-xis class
						axis_class x_class;

						//! Current x-axis type
						axis_value x_type;

						//! Supported axis values
						std::set< axis_value > supported_x_axes;

						//! LaTeX version of label (optional)
						std::string LaTeX_label;

						//! non-LaTeX version of label (optional)
						std::string non_LaTeX_label;

						//! has an x-label been set?
						bool label_set;


						// LABEL DATA

						//! generate tags when making labels?
						bool use_tags;

				    //! record meaning of momenta - do we interpret them as derivatives?
				    dot_type dot_meaning;

				    //! record meaning of k-labels -- are they conventional or comoving?
				    klabel_type klabel_meaning;


				    // FORMATTING

						//! record default plot precision
						unsigned int precision;

				    //! Wrapped output utility
				    wrapped_output wrapper;

					};


				template <typename number>
				derived_line<number>::derived_line(typename derived_line<number>::task_set_element_type tk, axis_class at,
                                           std::set<axis_value> sax, unsigned int prec)
					: x_class(at),
					  supported_x_axes(std::move(sax)),
            dot_meaning(dot_type::momenta),
            klabel_meaning(klabel_type::conventional),
            label_set(false),
            use_tags(CPPTRANSPORT_DEFAULT_DERIVED_LINE_USE_TAGS),
            precision(prec)
					{
					  parent_tasks.clear();
            parent_tasks.insert(std::move(tk));

				    // check that at least one supportable x-axis type has been specified
						if(supported_x_axes.empty())
						  throw runtime_exception(exception_type::RUNTIME_ERROR,
                                      CPPTRANSPORT_PRODUCT_DERIVED_LINE_NO_AXIS_TYPES);

						// default to first allowed x-axis type
						x_type = *supported_x_axes.cbegin();
					}


        template <typename number>
        derived_line<number>::derived_line(typename derived_line<number>::owning_task_initializer_list_type tks,
                                           axis_class at, std::set<axis_value> sax, unsigned int prec)
          :  x_class(at),
             supported_x_axes(std::move(sax)),
             dot_meaning(dot_type::momenta),
             klabel_meaning(klabel_type::conventional),
             label_set(false),
             use_tags(CPPTRANSPORT_DEFAULT_DERIVED_LINE_USE_TAGS),
             precision(prec)
          {
            parent_tasks.clear();
            for(auto& item : tks)
              {
                parent_tasks.insert(std::move(tks));
              }

            // check that at least one supportable x-axis type has been specified
            if(supported_x_axes.empty())
              throw runtime_exception(exception_type::RUNTIME_ERROR,
                                      CPPTRANSPORT_PRODUCT_DERIVED_LINE_NO_AXIS_TYPES);

            // default to first allowed x-axis type
            x_type = *supported_x_axes.cbegin();
          }


				template <typename number>
				derived_line<number>::derived_line(const task<number>& tk)
					{
					  parent_tasks.clear();
						assert(false);
					}


				template <typename number>
				derived_line<number>::derived_line(Json::Value& reader)
					{
					  parent_tasks.clear();
						assert(false);
					}


        // deserialization constructor
				template <typename number>
				derived_line<number>::derived_line(Json::Value& reader, task_finder<number>& finder)
					{
					  parent_tasks.clear();

					  auto& root = reader[CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_ROOT];

				    precision       = root[CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_PRECISION].asUInt();
				    label_set       = root[CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_LABEL_SET].asBool();
				    LaTeX_label     = root[CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_LABEL_LATEX].asString();
				    non_LaTeX_label = root[CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_LABEL_NONLATEX].asString();
						use_tags        = root[CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_LABEL_USE_TAGS].asBool();

						//! Deserialize: parent task list
						parent_tasks = this->deserialize_parent_task_list(root, finder);

						// Deserialize: axis type for this derived line
						x_class = this->deserialize_x_axis_class(root);

						// Deserialize: supported x-axis values
						supported_x_axes = this->deserialize_supported_x_axis_types(root);

            // Deserialize: current x-axis type
            x_type = this->deserialize_current_x_axis_value(root);

            // check current x-axis type is in supported list
            auto fv = supported_x_axes.find(x_type);    // TODO: change to contains() when we switch to C++20
            if(fv == supported_x_axes.end())
              {
                std::stringstream msg;
                msg << CPPTRANSPORT_PRODUCT_DERIVED_LINE_X_AXIS_NOT_SUPPORTED
                    << " '" << root[CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_CURRENT_X_AXIS_VALUE] << "'";
                throw runtime_exception(exception_type::SERIALIZATION_ERROR, msg.str());
              }

						// Deserialize: meaning of 'dot' for this derived line
						dot_meaning = this->deserialize_dot_meaning(root);

						// Deserialize: meaning of k-labels for this derived line
						klabel_meaning = this->deserialize_klabel_meaning(root);
					}


        // copy constructor
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
            precision(obj.precision)
					{
					  parent_tasks.clear();

					  for(const auto& elt : obj.parent_tasks) // TODO: change to std::views::values in C++20 and remove use of .second
              {
                parent_tasks.insert(std::move(make_derivable_task_set_element(elt.second.get_task(), elt.second.get_specifier(), elt.second.get_tag())));
              }
					}


				template <typename number>
				derived_line<number>& derived_line<number>::set_current_x_axis_value(axis_value v)
					{
				    auto fv = this->supported_x_axes.find(v); // TODO: change to contains() when we switch to C++20

						if(fv == this->supported_x_axes.end())
							{
						    std::ostringstream msg;
								msg << CPPTRANSPORT_PRODUCT_DERIVED_LINE_UNSUPPORTED_X_AXIS_TYPE;
								throw runtime_exception(exception_type::RUNTIME_ERROR, msg.str());
							}

						this->x_type = v;
            return *this;
					}


				template <typename number>
				derived_line<number>& derived_line<number>::set_label_text(const std::string& latex, const std::string& non_latex)
					{
						if(latex.empty() && non_latex.empty())    // treat as an attempt to clear the labels
							{
								this->clear_label_text();
							}
						else
							{
                this->LaTeX_label = latex;
                this->non_LaTeX_label = non_latex;
                this->label_set = true;
							}
            return *this;
					}


				template <typename number>
				derived_line<number>& derived_line<number>::clear_label_text()
					{
						this->LaTeX_label.clear();
						this->non_LaTeX_label.clear();
						this->label_set = false;
            return *this;
					}


		    template <typename number>
        std::string derived_line<number>::attach(datapipe<number>& pipe, const tag_list& tags, unsigned int tag) const
          {
            // find required task from parent task list
            auto t = this->parent_tasks.find(tag);

            if(t == this->parent_tasks.end())
              {
                std::ostringstream msg;
                msg << CPPTRANSPORT_PRODUCT_DERIVED_LINE_DATAPIPE_ATTACH_BAD_TAG << " [tag=" << tag << "]\n";
                throw runtime_exception(exception_type::RUNTIME_ERROR, msg.str());
              }

		        return(pipe.attach(t->second.get_task(), tags));
			    }


        template <typename number>
		    void derived_line<number>::detach(datapipe<number>& pipe) const
			    {
		        pipe.detach();
			    }


				template <typename number>
				void derived_line<number>::serialize(Json::Value& writer) const
					{
            Json::Value& root = writer[CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_ROOT];

            // Serialize: miscellaneous data
            // none of this needs to be packaged up more carefully
            root[CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_PRECISION]      = this->precision;
            root[CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_LABEL_SET]      = this->label_set;
            root[CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_LABEL_LATEX]    = this->LaTeX_label;
            root[CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_LABEL_NONLATEX] = this->non_LaTeX_label;
            root[CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_LABEL_USE_TAGS] = this->use_tags;

            // Serialize: parent task list
            this->serialize_parent_task_list(root);

						// Serialize: x-axis class
            this->serialize_x_axis_class(root);

            // Serialize: supported x-axis types
            this->serialize_supported_x_axis_types(root);

            // Serialize: current x-axis type
            this->serialize_current_x_axis_value(root);

						// Serialize: meaning of 'dot' for this derived line
						this->serialize_dot_meaning(root);

						// Serialize: meaning of k-labels for this derived line
						this->serialize_klabel_meaning(root);
					}


        template <typename number>
        void derived_line<number>::serialize_parent_task_list(Json::Value& root) const
          {
            // prior to 2021.1, a single task name was serialized into the field
            // CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_TASK_NAME

            // in 2021.1, this was replaced by the current use of a derived_task_list<> list,
            // serialized into the field CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_PARENT_TASK_LIST

            // for serialization there is nothing specific arising from this change; we simply serialize
            // the current task list ino CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_PARENT_TASK_LIST.
            Json::Value parents(Json::arrayValue);

            for(const auto& elt : this->parent_tasks)  // TODO: change to std::views::values in C++20 and remove use of .second
              {
                Json::Value tk_block(Json::objectValue);

                const auto& tk = elt.second.get_task();
                const auto& sp = elt.second.get_specifier();
                auto tag = elt.second.get_tag();

                tk_block[CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_PARENT_TASK_LIST_TASKNAME] = tk.get_name();

                Json::Value spec_block(Json::objectValue);
                sp.serialize(spec_block);

                tk_block[CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_PARENT_TASK_LIST_SPECIFIERS] = spec_block;
                tk_block[CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_PARENT_TASK_LIST_TAG] = tag;

                parents.append(tk_block);
              }

            root[CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_PARENT_TASK_LIST] = parents;
          }


        template <typename number>
        typename derived_line<number>::task_set_type
        derived_line<number>::deserialize_parent_task_list(const Json::Value& root, task_finder<number>& finder) const
          {
            // prior to 2021.1, a single task name was serialized into the field
            // CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_TASK_NAME

            // in 2021.1, this was replaced by the current use of a derived_task_list<> list,
            // serialized into the field CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_PARENT_TASK_LIST

            // if no CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_PARENT_TASK_LIST field is present, we should
            // attempt to deserialize CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_TASK_NAME using the old
            // implementation

            if(root.isMember(CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_PARENT_TASK_LIST))
              {
                const Json::Value& new_root = root[CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_PARENT_TASK_LIST];
                return this->deserialize_new_parent_task_list(new_root, finder);
              }

            if(root.isMember(CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_TASK_NAME))
              {
                return this->deserialize_old_parent_task(root, finder);
              }

            throw runtime_exception(exception_type::SERIALIZATION_ERROR,
                                    CPPTRANSPORT_PRODUCT_DERIVED_LINE_NO_PARENT_TASK);
          }


        template <typename number>
        typename derived_line<number>::task_set_type
        derived_line<number>::deserialize_old_parent_task(const Json::Value& root, task_finder<number>& finder) const
          {
            std::string name = root[CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_TASK_NAME].asString();

            // extract parent task (as pointer to task_record instance)
            auto tk_rec = finder(name);
            assert(tk_rec);

            // convert to derivable_task<>
            auto tk = dynamic_cast< derivable_task<number>* >(tk_rec->get_abstract_task());

            task_set_type parents;

            switch(tk_rec->get_type())
              {
                case task_type::integration:
                  {
                    parents.insert(make_derivable_task_set_element(*tk, false, false, false));
                    break;
                  }

                case task_type::postintegration:
                  {
                    precomputed_products p;
                    parents.insert(make_derivable_task_set_element(*tk, p));
                    break;
                  }

                case task_type::output:
                  {
                    throw runtime_exception(exception_type::SERIALIZATION_ERROR,
                                            CPPTRANSPORT_PRODUCT_DERIVED_LINE_BAD_PARENT_TASK_TYPE);
                  }
              }

            return std::move(parents);
          }


        template <typename number>
        typename derived_line<number>::task_set_type
        derived_line<number>::deserialize_new_parent_task_list(const Json::Value& root, task_finder<number>& finder) const
          {
            assert(root.isArray());

            task_set_type parents;
            for(const auto& elt : root)
              {
                // lookup task by name in the repository
                auto name = elt[CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_PARENT_TASK_LIST_TASKNAME].asString();
                auto tk_rec = finder(name);
                assert(tk_rec);

                // convert to derivable_task<>
                auto tk = dynamic_cast< derivable_task<number>* >(tk_rec->get_abstract_task());

                const Json::Value& spec_block = elt[CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_PARENT_TASK_LIST_SPECIFIERS];
                content_group_specifier spec(spec_block);

                auto tag = elt[CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_PARENT_TASK_LIST_TAG].asUInt();

                parents.insert(make_derivable_task_set_element(*tk, spec, tag));
              }

            return std::move(parents);
          }


        template <typename number>
        void derived_line<number>::serialize_supported_x_axis_types(Json::Value& root) const
          {
            Json::Value supported(Json::arrayValue);

            for(auto type : this->supported_x_axes)
              {
                switch(type)
                  {
                    case axis_value::efolds:
                      supported.append(Json::Value(std::string(CPPTRANSPORT_NODE_PRODUCT_AXIS_VALUE_EFOLDS)));
                      break;

                    case axis_value::k:
                      supported.append(Json::Value(std::string(CPPTRANSPORT_NODE_PRODUCT_AXIS_VALUE_WAVENUMBER)));
                      break;

                    case axis_value::efolds_exit:
                      supported.append(Json::Value(std::string(CPPTRANSPORT_NODE_PRODUCT_AXIS_VALUE_EFOLDS_EXIT)));
                      break;

                    case axis_value::alpha:
                      supported.append(Json::Value(std::string(CPPTRANSPORT_NODE_PRODUCT_AXIS_VALUE_ALPHA)));
                      break;

                    case axis_value::beta:
                      supported.append(Json::Value(std::string(CPPTRANSPORT_NODE_PRODUCT_AXIS_VALUE_BETA)));
                      break;

                    case axis_value::squeeze_k1:
                      supported.append(Json::Value(std::string(CPPTRANSPORT_NODE_PRODUCT_AXIS_VALUE_SQUEEZING_FRACTION_K1)));
                      break;

                    case axis_value::squeeze_k2:
                      supported.append(Json::Value(std::string(CPPTRANSPORT_NODE_PRODUCT_AXIS_VALUE_SQUEEZING_FRACTION_K2)));
                      break;

                    case axis_value::squeeze_k3:
                      supported.append(Json::Value(std::string(CPPTRANSPORT_NODE_PRODUCT_AXIS_VALUE_SQUEEZING_FRACTION_K3)));
                      break;

                    default:
                      assert(false);
                      throw runtime_exception(exception_type::RUNTIME_ERROR, CPPTRANSPORT_PRODUCT_DERIVED_LINE_UNKNOWN_X_AXIS_VALUE);
                  }
              }

            root[CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_SUPPORTED_X_AXES] = supported;
          }


        template <typename number>
        std::set<axis_value> derived_line<number>::deserialize_supported_x_axis_types(const Json::Value& root) const
          {
            std::set<axis_value> supported;

            auto& supported_array = root[CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_SUPPORTED_X_AXES];
            assert(supported_array.isArray());

            for(auto& t : supported_array)
              {
                // decode this array element
                std::string value_string = t.asString();

                if(value_string == CPPTRANSPORT_NODE_PRODUCT_AXIS_VALUE_EFOLDS)                     supported.insert(axis_value::efolds);
                else if(value_string == CPPTRANSPORT_NODE_PRODUCT_AXIS_VALUE_WAVENUMBER)            supported.insert(axis_value::k);
                else if(value_string == CPPTRANSPORT_NODE_PRODUCT_AXIS_VALUE_EFOLDS_EXIT)           supported.insert(axis_value::efolds_exit);
                else if(value_string == CPPTRANSPORT_NODE_PRODUCT_AXIS_VALUE_ALPHA)                 supported.insert(axis_value::alpha);
                else if(value_string == CPPTRANSPORT_NODE_PRODUCT_AXIS_VALUE_BETA)                  supported.insert(axis_value::beta);
                else if(value_string == CPPTRANSPORT_NODE_PRODUCT_AXIS_VALUE_SQUEEZING_FRACTION_K1) supported.insert(axis_value::squeeze_k1);
                else if(value_string == CPPTRANSPORT_NODE_PRODUCT_AXIS_VALUE_SQUEEZING_FRACTION_K2) supported.insert(axis_value::squeeze_k2);
                else if(value_string == CPPTRANSPORT_NODE_PRODUCT_AXIS_VALUE_SQUEEZING_FRACTION_K3) supported.insert(axis_value::squeeze_k3);
                else
                  {
                    std::ostringstream msg;
                    msg << CPPTRANSPORT_PRODUCT_DERIVED_LINE_UNKNOWN_X_AXIS_VALUE << " '" << value_string << "'";
                    throw runtime_exception(exception_type::SERIALIZATION_ERROR, msg.str());
                  }
              }

            return std::move(supported);
          }


        template <typename number>
        void derived_line<number>::serialize_x_axis_class(Json::Value& root) const
          {
            switch(this->x_class)
              {
                case axis_class::time:
                  root[CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_X_AXIS_CLASS] = std::string(CPPTRANSPORT_NODE_PRODUCT_AXIS_CLASS_TIME);
                  break;

                case axis_class::wavenumber:
                  root[CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_X_AXIS_CLASS] = std::string(CPPTRANSPORT_NODE_PRODUCT_AXIS_CLASS_WAVENUMBER);
                  break;

                case axis_class::threepf_kconfig:
                  root[CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_X_AXIS_CLASS] = std::string(CPPTRANSPORT_NODE_PRODUCT_AXIS_CLASS_THREEPF_CONFIGURATION);
                  break;
              }
          }


        template <typename number>
        axis_class derived_line<number>::deserialize_x_axis_class(const Json::Value& root) const
          {
            std::string xtype = root[CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_X_AXIS_CLASS].asString();

            if(xtype == CPPTRANSPORT_NODE_PRODUCT_AXIS_CLASS_TIME)
              return axis_class::time;

            if(xtype == CPPTRANSPORT_NODE_PRODUCT_AXIS_CLASS_WAVENUMBER)
              return axis_class::wavenumber;

            if(xtype == CPPTRANSPORT_NODE_PRODUCT_AXIS_CLASS_THREEPF_CONFIGURATION)
              return axis_class::threepf_kconfig;

            std::ostringstream msg;
            msg << CPPTRANSPORT_PRODUCT_DERIVED_LINE_UNKNOWN_X_AXIS_CLASS << " '" << xtype << "'";
            throw runtime_exception(exception_type::SERIALIZATION_ERROR, msg.str());
          }


        template <typename number>
        void derived_line<number>::serialize_current_x_axis_value(Json::Value& root) const
          {
            switch(this->x_type)
              {
                case axis_value::efolds:
                  root[CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_CURRENT_X_AXIS_VALUE] = std::string(CPPTRANSPORT_NODE_PRODUCT_AXIS_VALUE_EFOLDS);
                  break;

                case axis_value::k:
                  root[CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_CURRENT_X_AXIS_VALUE] = std::string(CPPTRANSPORT_NODE_PRODUCT_AXIS_VALUE_WAVENUMBER);
                  break;

                case axis_value::efolds_exit:
                  root[CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_CURRENT_X_AXIS_VALUE] = std::string(CPPTRANSPORT_NODE_PRODUCT_AXIS_VALUE_EFOLDS_EXIT);
                  break;

                case axis_value::alpha:
                  root[CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_CURRENT_X_AXIS_VALUE] = std::string(CPPTRANSPORT_NODE_PRODUCT_AXIS_VALUE_ALPHA);
                  break;

                case axis_value::beta:
                  root[CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_CURRENT_X_AXIS_VALUE] = std::string(CPPTRANSPORT_NODE_PRODUCT_AXIS_VALUE_BETA);
                  break;

                case axis_value::squeeze_k1:
                  root[CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_CURRENT_X_AXIS_VALUE] = std::string(CPPTRANSPORT_NODE_PRODUCT_AXIS_VALUE_SQUEEZING_FRACTION_K1);
                  break;

                case axis_value::squeeze_k2:
                  root[CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_CURRENT_X_AXIS_VALUE] = std::string(CPPTRANSPORT_NODE_PRODUCT_AXIS_VALUE_SQUEEZING_FRACTION_K2);
                  break;

                case axis_value::squeeze_k3:
                  root[CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_CURRENT_X_AXIS_VALUE] = std::string(CPPTRANSPORT_NODE_PRODUCT_AXIS_VALUE_SQUEEZING_FRACTION_K3);
                  break;

                default:
                  assert(false);
                  throw runtime_exception(exception_type::RUNTIME_ERROR, CPPTRANSPORT_PRODUCT_DERIVED_LINE_UNKNOWN_X_AXIS_VALUE);
              }
          }


        template <typename number>
        axis_value derived_line<number>::deserialize_current_x_axis_value(const Json::Value& root) const
          {
            std::string type = root[CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_CURRENT_X_AXIS_VALUE].asString();

            if(type == CPPTRANSPORT_NODE_PRODUCT_AXIS_VALUE_EFOLDS)
              return axis_value::efolds;

            if(type == CPPTRANSPORT_NODE_PRODUCT_AXIS_VALUE_WAVENUMBER)
              return axis_value::k;

            if(type == CPPTRANSPORT_NODE_PRODUCT_AXIS_VALUE_EFOLDS_EXIT)
              return axis_value::efolds_exit;

            if(type == CPPTRANSPORT_NODE_PRODUCT_AXIS_VALUE_ALPHA)
              return axis_value::alpha;

            if(type == CPPTRANSPORT_NODE_PRODUCT_AXIS_VALUE_BETA)
              return axis_value::beta;

            if(type == CPPTRANSPORT_NODE_PRODUCT_AXIS_VALUE_SQUEEZING_FRACTION_K1)
              return axis_value::squeeze_k1;

            if(type == CPPTRANSPORT_NODE_PRODUCT_AXIS_VALUE_SQUEEZING_FRACTION_K2)
              return axis_value::squeeze_k2;

            if(type == CPPTRANSPORT_NODE_PRODUCT_AXIS_VALUE_SQUEEZING_FRACTION_K3)
              return axis_value::squeeze_k3;

            std::ostringstream msg;
            msg << CPPTRANSPORT_PRODUCT_DERIVED_LINE_UNKNOWN_X_AXIS_VALUE << " '" << type << "'";
            throw runtime_exception(exception_type::SERIALIZATION_ERROR, msg.str());
          }


        template <typename number>
        void derived_line<number>::serialize_dot_meaning(Json::Value& root) const
          {
            switch(this->dot_meaning)
              {
                case dot_type::derivatives:
                  root[CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_DOT_TYPE] = std::string(CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_DOT_DERIVATIVE);
                  break;

                case dot_type::momenta:
                  root[CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_DOT_TYPE] = std::string(CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_DOT_MOMENTA);
                  break;
              }
          }


        template <typename number>
        dot_type derived_line<number>::deserialize_dot_meaning(const Json::Value& root) const
          {
            std::string dot_meaning_value = root[CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_DOT_TYPE].asString();

            if(dot_meaning_value == CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_DOT_DERIVATIVE)
              return dot_type::derivatives;

            if(dot_meaning_value == CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_DOT_MOMENTA)
              return dot_type::momenta;

            std::ostringstream msg;
            msg << CPPTRANSPORT_PRODUCT_DERIVED_LINE_DOT_TYPE_UNKNOWN << " '" << dot_meaning_value << "'";
            throw runtime_exception(exception_type::SERIALIZATION_ERROR, msg.str());
          }


        template <typename number>
        void derived_line<number>::serialize_klabel_meaning(Json::Value& root) const
          {
            switch(this->klabel_meaning)
              {
                case klabel_type::conventional:
                  root[CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_KLABEL_TYPE] = std::string(CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_KLABEL_CONVENTIONAL);
                  break;

                case klabel_type::comoving:
                  root[CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_KLABEL_TYPE] = std::string(CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_KLABEL_COMOVING);
                  break;
              }
          }


        template <typename number>
        klabel_type derived_line<number>::deserialize_klabel_meaning(const Json::Value& root) const
          {
            std::string label_meaning_value = root[CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_KLABEL_TYPE].asString();

            if(label_meaning_value == CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_KLABEL_CONVENTIONAL)
              return klabel_type::conventional;

            if(label_meaning_value == CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_KLABEL_COMOVING)
              return klabel_type::comoving;

            std::ostringstream msg;
            msg << CPPTRANSPORT_PRODUCT_DERIVED_LINE_KLABEL_TYPE_UNKNOWN << " '" << label_meaning_value << "'";
            throw runtime_exception(exception_type::SERIALIZATION_ERROR, msg.str());
          }


        template <typename number>
				void derived_line<number>::write(std::ostream& out)
					{
						unsigned int saved_left_margin = this->wrapper.get_left_margin();
						this->wrapper.set_left_margin(2);

//						out << "  " << CPPTRANSPORT_PRODUCT_DERIVED_LINE_LABEL << '\n';
//						out << "  " << CPPTRANSPORT_PRODUCT_DERIVED_LINE_TASK_NAME << " '" << this->parent_task->get_name() << "'" << '\n';

//						// output model details if this derived line is directly associated with an integration task
//						auto db_task = dynamic_cast< integration_task<number>* >(this->parent_task);
//						if(db_task != nullptr)
//							{
//						    out << ", " << CPPTRANSPORT_PRODUCT_DERIVED_LINE_MODEL_NAME << " '" << db_task->get_model()->get_name() << "'" << '\n';
//							}

						out << "  " << CPPTRANSPORT_PRODUCT_DERIVED_LINE_X_AXIS_CLASS << " ";
						switch(this->x_class)
							{
						    case axis_class::time:
							    out << CPPTRANSPORT_PRODUCT_DERIVED_LINE_CLASS_TIME_LABEL << '\n';
									break;

						    case axis_class::wavenumber:
									out << CPPTRANSPORT_PRODUCT_DERIVED_LINE_CLASS_WAVENUMBER_LABEL << '\n';
									break;

						    case axis_class::threepf_kconfig:
							    out << CPPTRANSPORT_PRODUCT_DERIVED_LINE_CLASS_THREEPF_CONFIGURATION_LABEL << '\n';
                  break;
							}

						unsigned int count = 0;

						this->wrapper.wrap_out(out, std::string{"  "} + CPPTRANSPORT_PRODUCT_DERIVED_LINE_X_AXIS_SUPPORTED + " ");  // inefficient?
						for(auto t = this->supported_x_axes.begin(); t != this->supported_x_axes.end() && count < CPPTRANSPORT_PRODUCT_MAX_SUPPORTED_AXES; ++t)
							{
								switch(*t)
									{
								    case axis_value::efolds:
									    this->wrapper.wrap_list_item(out, true, CPPTRANSPORT_PRODUCT_DERIVED_LINE_X_AXIS_VALUE_EFOLDS, count);
											break;

								    case axis_value::k:
									    this->wrapper.wrap_list_item(out, true, CPPTRANSPORT_PRODUCT_DERIVED_LINE_X_AXIS_VALUE_K, count);
											break;

								    case axis_value::efolds_exit:
									    this->wrapper.wrap_list_item(out, true, CPPTRANSPORT_NODE_PRODUCT_AXIS_VALUE_EFOLDS_EXIT, count);
											break;

								    case axis_value::alpha:
									    this->wrapper.wrap_list_item(out, true, CPPTRANSPORT_NODE_PRODUCT_AXIS_VALUE_ALPHA, count);
											break;

								    case axis_value::beta:
									    this->wrapper.wrap_list_item(out, true, CPPTRANSPORT_NODE_PRODUCT_AXIS_VALUE_BETA, count);
											break;

								    case axis_value::squeeze_k1:
									    this->wrapper.wrap_list_item(out, true, CPPTRANSPORT_NODE_PRODUCT_AXIS_VALUE_SQUEEZING_FRACTION_K1, count);
											break;

								    case axis_value::squeeze_k2:
									    this->wrapper.wrap_list_item(out, true, CPPTRANSPORT_NODE_PRODUCT_AXIS_VALUE_SQUEEZING_FRACTION_K2, count);
											break;

								    case axis_value::squeeze_k3:
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
				        case axis_value::efolds:
					        out << CPPTRANSPORT_PRODUCT_DERIVED_LINE_X_AXIS_VALUE_EFOLDS << '\n';
				          break;

				        case axis_value::k:
					        out << CPPTRANSPORT_PRODUCT_DERIVED_LINE_X_AXIS_VALUE_K << '\n';
				          break;

				        case axis_value::efolds_exit:
					        out << CPPTRANSPORT_NODE_PRODUCT_AXIS_VALUE_EFOLDS_EXIT << '\n';
				          break;

				        case axis_value::alpha:
					        out << CPPTRANSPORT_NODE_PRODUCT_AXIS_VALUE_ALPHA << '\n';
				          break;

				        case axis_value::beta:
					        out << CPPTRANSPORT_NODE_PRODUCT_AXIS_VALUE_BETA << '\n';
				          break;

				        case axis_value::squeeze_k1:
					        out << CPPTRANSPORT_NODE_PRODUCT_AXIS_VALUE_SQUEEZING_FRACTION_K1 << '\n';
				          break;

				        case axis_value::squeeze_k2:
					        out << CPPTRANSPORT_NODE_PRODUCT_AXIS_VALUE_SQUEEZING_FRACTION_K2 << '\n';
				          break;

				        case axis_value::squeeze_k3:
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


#endif //CPPTRANSPORT_DERIVED_LINE_H
