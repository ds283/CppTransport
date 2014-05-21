//
// Created by David Seery on 19/05/2014.
// Copyright (c) 2014 University of Sussex. All rights reserved.
//


#ifndef __general_time_data_H_
#define __general_time_data_H_


#include <iostream>
#include <sstream>
#include <string>
#include <list>
#include <vector>
#include <stdexcept>

#include "transport-runtime-api/serialization/serializable.h"
// need repository in order to get the details of a repository<number>::output_group
// (can't forward-declare because it is a nested class)
#include "transport-runtime-api/manager/repository.h"

// need data_manager in order to get the details of a data_manager<number>>data_pipe
// (can't forward-declare because it is a nested class)
#include "transport-runtime-api/manager/data_manager.h"

#include "transport-runtime-api/derived-products/utilities/index_selector.h"
#include "transport-runtime-api/derived-products/utilities/wrapper.h"
#include "transport-runtime-api/derived-products/utilities/filter.h"

#include "transport-runtime-api/derived-products/plot2d_product.h"


#define __CPP_TRANSPORT_NODE_PRODUCT_TDATA_TYPE                 "time-data-group-type"
#define __CPP_TRANSPORT_NODE_PRODUCT_TDATA_BACKGROUND           "background-group"
#define __CPP_TRANSPORT_NODE_PRODUCT_TDATA_TWOPF                "twopf-group"
#define __CPP_TRANSPORT_NODE_PRODUCT_TDATA_THREEPF              "threepf-group"

#define __CPP_TRANSPORT_NODE_PRODUCT_TDATA_TASK_NAME            "task-name"
#define __CPP_TRANSPORT_NODE_PRODUCT_TDATA_LABEL_PRECISION      "precision"

#define __CPP_TRANSPORT_NODE_PRODUCT_TDATA_DOT_TYPE             "threepf-momenta"
#define __CPP_TRANSPORT_NODE_PRODUCT_TDATA_DOT_DERIVATIVE       "derivatives"
#define __CPP_TRANSPORT_NODE_PRODUCT_TDATA_DOT_MOMENTA          "momenta"

#define __CPP_TRANSPORT_NODE_PRODUCT_TDATA_KLABEL_TYPE          "k-label-type"
#define __CPP_TRANSPORT_NODE_PRODUCT_TDATA_KLABEL_CONVENTIONAL  "conventional"
#define __CPP_TRANSPORT_NODE_PRODUCT_TDATA_KLABEL_COMOVING      "comoving"

#define __CPP_TRANSPORT_NODE_PRODUCT_TDATA_K_SERIAL_NUMBERS     "kconfig-serial-numbers"
#define __CPP_TRANSPORT_NODE_PRODUCT_TDATA_K_SERIAL_NUMBER      "sn"

#define __CPP_TRANSPORT_NODE_PRODUCT_TDATA_TIME_SNS             "time-plot-serial-numbers"
#define __CPP_TRANSPORT_NODE_PRODUCT_TDATA_TIME_SN              "sn"

// maximum number of serial numbers to output when writing ourselves to
// a standard stream
#define __CPP_TRANSPORT_PRODUCT_TDATA_MAX_SN                    (15)



namespace transport
	{

    // forward-declare model class
    template <typename number> class model;

    // forward-declare class task.
    // task.h includes this header, so we cannot include task.h otherwise we create
    // a circular dependency.
    template <typename number> class task;
    template <typename number> class integration_task;
    template <typename number> class twopf_list_task;


		namespace derived_data
			{

		    //! general time data content producer, suitable for
		    //! producing content usable in eg. a general time plot
		    template <typename number>
		    class general_time_data: public serializable
			    {

		      public:

		        typedef enum { derivatives, momenta } dot_type;
		        typedef enum { conventional, comoving } klabel_type;

		        // CONSTRUCTOR, DESTRUCTOR

		      public:

		        //! Basic user-facing constructor
		        general_time_data(integration_task<number>& tk, model<number>* m, filter::time_filter tfilter);

		        //! Deserialization constructor
		        general_time_data(serialization_reader* reader, typename repository<number>::task_finder finder);

				    //! Override default copy constructor to perform a deep copy of the parent task
				    general_time_data(const general_time_data<number>& obj);

		        virtual ~general_time_data() = default;


		        // ADMIN

		        //! Get parent task
		        const task<number>* get_parent_task() const { return(this->parent_task); }

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
            void attach(typename data_manager<number>::datapipe& pipe, const std::list<std::string>& tags) const;

            //! detach datapipe from output group
            void detach(typename data_manager<number>::datapipe& detach) const;

		        //! get serial numbers of sample times
		        const std::vector<unsigned int>& get_time_sample_sns() const { return(this->time_sample_sns); }

		        //! extract axis data, corresponding to our sample times, from datapipe
		        void pull_time_axis(typename data_manager<number>::datapipe& pipe, std::vector<double>& axis) const;


		        // DERIVE LINES

		        //! generate data lines for plotting
            virtual void derive_lines(typename data_manager<number>::datapipe& pipe,
                                      plot2d_product<number> &plot, std::list<time_data_line<number> >& lines,
                                      const std::list<std::string>& tags) const = 0;

		        // CLONE

		        //! self-replicate
		        virtual general_time_data<number>* clone() const = 0;


		        // WRITE TO A STREAM

		        //! write self-details to a stream
		        virtual void write(std::ostream& out) = 0;

		        // SERIALIZATION -- implements a 'serializable' interface

		      public:

		        //! Serialize this object
		        virtual void serialize(serialization_writer& writer) const override;


		        // INTERNAL DATA

		      protected:

		        //! gadget for performing t- or k-sample filtration
		        filter f;

		        //! record task which corresponds to this derivation
		        integration_task<number>* parent_task;

				    //! record model used for this derivation
				    model<number>* mdl;

		        //! List of time serial numbers to be used for plotting
		        std::vector<unsigned int> time_sample_sns;

		        //! record meaning of momenta - do we interpret them as derivatives?
		        dot_type dot_meaning;

		        //! record meaning of k-labels -- are they conventional or comoving?
		        klabel_type klabel_meaning;

		        //! Wrapped output utility
		        wrapped_output wrapper;
			    };


		    template <typename number>
		    general_time_data<number>::general_time_data(integration_task<number>& tk, model<number>* m, filter::time_filter tfilter)
			    : parent_task(dynamic_cast<integration_task<number>*>(tk.clone())), mdl(m), dot_meaning(momenta), klabel_meaning(conventional)
			    {
		        assert(parent_task != nullptr);

            if(parent_task == nullptr)
              throw runtime_exception(runtime_exception::RUNTIME_ERROR, __CPP_TRANSPORT_PRODUCT_GENERAL_NOT_INTEGRATION_TASK);

		        // set up a list of serial numbers corresponding to the sample times for this plot
		        this->f.filter_time_sample(tfilter, tk.get_sample_times(), time_sample_sns);
			    }


		    template <typename number>
		    general_time_data<number>::general_time_data(serialization_reader* reader, typename repository<number>::task_finder finder)
          : parent_task(nullptr), mdl(nullptr)
			    {
		        assert(reader != nullptr);

		        if(reader == nullptr) throw runtime_exception(runtime_exception::RUNTIME_ERROR, __CPP_TRANSPORT_PRODUCT_GENERAL_TPLOT_NULL_READER);

            std::string parent_task_name;
            reader->read_value(__CPP_TRANSPORT_NODE_PRODUCT_TDATA_TASK_NAME, parent_task_name);

            // extract parent task and model
            task<number>* tk = finder(parent_task_name, &mdl);
            if((parent_task = dynamic_cast< integration_task<number>* >(tk)) == nullptr)
              throw runtime_exception(runtime_exception::REPOSITORY_ERROR, __CPP_TRANSPORT_REPO_OUTPUT_TASK_NOT_INTGRTN);

		        unsigned int sns = reader->start_array(__CPP_TRANSPORT_NODE_PRODUCT_TDATA_TIME_SNS);

		        for(unsigned int i = 0; i < sns; i++)
			        {
		            reader->start_array_element();

		            unsigned int sn;
		            reader->read_value(__CPP_TRANSPORT_NODE_PRODUCT_TDATA_TIME_SN, sn);
		            time_sample_sns.push_back(sn);

		            reader->end_array_element();
			        }

		        reader->end_element(__CPP_TRANSPORT_NODE_PRODUCT_TDATA_TIME_SNS);

		        std::string dot_meaning_value;
		        reader->read_value(__CPP_TRANSPORT_NODE_PRODUCT_TDATA_DOT_TYPE, dot_meaning_value);

		        if(dot_meaning_value == __CPP_TRANSPORT_NODE_PRODUCT_TDATA_DOT_DERIVATIVE) dot_meaning = derivatives;
		        else if(dot_meaning_value == __CPP_TRANSPORT_NODE_PRODUCT_TDATA_DOT_MOMENTA) dot_meaning = momenta;
		        else
			        {
		            std::ostringstream msg;
		            msg << __CPP_TRANSPORT_PRODUCT_GENERAL_TPLOT_GENERAL_DOT_UNKNOWN << " '" << dot_meaning_value << "'";
		            throw runtime_exception(runtime_exception::SERIALIZATION_ERROR, msg.str());
			        }

		        std::string label_meaning_value;
		        reader->read_value(__CPP_TRANSPORT_NODE_PRODUCT_TDATA_KLABEL_TYPE, label_meaning_value);

		        if(label_meaning_value == __CPP_TRANSPORT_NODE_PRODUCT_TDATA_KLABEL_CONVENTIONAL) klabel_meaning = conventional;
		        else if(label_meaning_value == __CPP_TRANSPORT_NODE_PRODUCT_TDATA_KLABEL_COMOVING) klabel_meaning = comoving;
		        else
			        {
		            std::ostringstream msg;
		            msg << __CPP_TRANSPORT_PRODUCT_GENERAL_TPLOT_GENERAL_KLABEL_UNKNOWN << " '" << label_meaning_value << "'";
		            throw runtime_exception(runtime_exception::SERIALIZATION_ERROR, msg.str());
			        }
			    }


				template <typename number>
				general_time_data<number>::general_time_data(const general_time_data<number>& obj)
					: parent_task(dynamic_cast<integration_task<number>*>(obj.parent_task->clone())), mdl(obj.mdl),   // OK to shallow-copy model; pointers to model instances are managed by the instance_manager
					  dot_meaning(obj.dot_meaning), klabel_meaning(obj.klabel_meaning),
						time_sample_sns(obj.time_sample_sns)
					{
				    assert(this->parent_task != nullptr);

            if(this->parent_task == nullptr)
              throw runtime_exception(runtime_exception::RUNTIME_ERROR, __CPP_TRANSPORT_PRODUCT_GENERAL_NOT_INTEGRATION_TASK);
					}


				template <typename number>
				general_time_data<number>::~general_time_data()
					{
						assert(this->parent_task != nullptr);
						delete this->parent_task;
					}


        template <typename number>
        void general_time_data<number>::attach(typename data_manager<number>::datapipe& pipe, const std::list<std::string>& tags) const
          {
            pipe.attach(this->parent_task, tags);
          }


        template <typename number>
        void general_time_data<number>::detach(typename data_manager<number>::datapipe& pipe) const
          {
            pipe.detach();
          }


		    template <typename number>
		    void general_time_data<number>::pull_time_axis(typename data_manager<number>::datapipe& pipe, std::vector<double>& axis) const
			    {
		        if(!pipe.validate()) throw runtime_exception(runtime_exception::DATAPIPE_ERROR, __CPP_TRANSPORT_PRODUCT_GENERAL_TPLOT_NULL_DATAPIPE);

		        // set-up time sample data
		        pipe.pull_time_sample(this->time_sample_sns, axis);
			    }


		    template <typename number>
		    void general_time_data<number>::serialize(serialization_writer& writer) const
			    {
            this->write_value_node(writer, __CPP_TRANSPORT_NODE_PRODUCT_TDATA_TASK_NAME, this->parent_task->get_name());

		        this->begin_array(writer, __CPP_TRANSPORT_NODE_PRODUCT_TDATA_TIME_SNS, this->time_sample_sns.size() == 0);
		        for(std::vector<unsigned int>::const_iterator t = this->time_sample_sns.begin(); t != this->time_sample_sns.end(); t++)
			        {
		            this->begin_node(writer, "arrayelt", false);    // node name ignored for arrays
		            this->write_value_node(writer, __CPP_TRANSPORT_NODE_PRODUCT_TDATA_TIME_SN, *t);
		            this->end_element(writer, "arrayelt");
			        }
		        this->end_element(writer, __CPP_TRANSPORT_NODE_PRODUCT_TDATA_TIME_SNS);

		        switch(this->dot_meaning)
			        {
		            case derivatives:
			            this->write_value_node(writer, __CPP_TRANSPORT_NODE_PRODUCT_TDATA_DOT_TYPE, std::string(__CPP_TRANSPORT_NODE_PRODUCT_TDATA_DOT_DERIVATIVE));
		            break;

		            case momenta:
			            this->write_value_node(writer, __CPP_TRANSPORT_NODE_PRODUCT_TDATA_DOT_TYPE, std::string(__CPP_TRANSPORT_NODE_PRODUCT_TDATA_DOT_MOMENTA));
		            break;

		            default:
			            throw runtime_exception(runtime_exception::RUNTIME_ERROR, __CPP_TRANSPORT_PRODUCT_GENERAL_TPLOT_GENERAL_DOT_UNKNOWN);
			        }

		        switch(this->klabel_meaning)
			        {
		            case conventional:
			            this->write_value_node(writer, __CPP_TRANSPORT_NODE_PRODUCT_TDATA_KLABEL_TYPE, std::string(__CPP_TRANSPORT_NODE_PRODUCT_TDATA_KLABEL_CONVENTIONAL));
		            break;

		            case comoving:
			            this->write_value_node(writer, __CPP_TRANSPORT_NODE_PRODUCT_TDATA_KLABEL_TYPE, std::string(__CPP_TRANSPORT_NODE_PRODUCT_TDATA_KLABEL_COMOVING));
		            break;

		            default:
			            throw runtime_exception(runtime_exception::RUNTIME_ERROR, __CPP_TRANSPORT_PRODUCT_GENERAL_TPLOT_GENERAL_KLABEL_UNKNOWN);
			        }
			    }


		    template <typename number>
		    void general_time_data<number>::write(std::ostream& out)
			    {
		        this->wrapper.wrap_out(out, __CPP_TRANSPORT_PRODUCT_GENERAL_TPLOT_TSAMPLE_SN_LABEL " ");

		        unsigned int count = 0;
		        for(std::vector<unsigned int>::const_iterator t = this->time_sample_sns.begin(); t != this->time_sample_sns.end() && count < __CPP_TRANSPORT_PRODUCT_TDATA_MAX_SN; t++)
			        {
		            std::ostringstream msg;
		            msg << (*t);

		            this->wrapper.wrap_list_item(out, true, msg.str(), count);
			        }
		        if(count == __CPP_TRANSPORT_PRODUCT_TDATA_MAX_SN) this->wrapper.wrap_list_item(out, true, "...", count);

		        this->wrapper.wrap_newline(out);
			    }


			}   // namespace derived_data

	}   // namespace transport


#endif //__general_time_data_H_
