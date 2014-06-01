//
// Created by David Seery on 19/05/2014.
// Copyright (c) 2014 University of Sussex. All rights reserved.
//


#ifndef __time_series_H_
#define __time_series_H_


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

// need data_manager in order to get the details of a data_manager<number>::datapipe
// (can't forward-declare because it is a nested class)
#include "transport-runtime-api/manager/data_manager.h"

#include "transport-runtime-api/derived-products/derived-content/derived_line.h"

#include "transport-runtime-api/derived-products/utilities/index_selector.h"
#include "transport-runtime-api/derived-products/utilities/wrapper.h"
#include "transport-runtime-api/derived-products/utilities/filter.h"


#define __CPP_TRANSPORT_NODE_PRODUCT_TDATA_K_SERIAL_NUMBERS "kconfig-serial-numbers"
#define __CPP_TRANSPORT_NODE_PRODUCT_TDATA_K_SERIAL_NUMBER  "n"

#define __CPP_TRANSPORT_NODE_PRODUCT_TDATA_T_SERIAL_NUMBERS "time-serial-numbers"
#define __CPP_TRANSPORT_NODE_PRODUCT_TDATA_T_SERIAL_NUMBER  "n"

// maximum number of serial numbers to output when writing ourselves to
// a standard stream
#define __CPP_TRANSPORT_PRODUCT_TDATA_MAX_SN (15)



namespace transport
	{

    // forward-declare model class
    template <typename number> class model;

    // forward-declare task classes.
    // task.h includes this header, so we cannot include task.h otherwise we create
    // a circular dependency.
    template <typename number> class task;
    template <typename number> class integration_task;
    template <typename number> class twopf_list_task;


		namespace derived_data
			{

		    //! general time-series content producer, suitable for
		    //! producing content usable in eg. a 2d plot or table.
		    template <typename number>
		    class time_series : public derived_line<number>
			    {

		        // CONSTRUCTOR, DESTRUCTOR

		      public:

		        //! Basic user-facing constructor
		        time_series(const integration_task<number>& tk, model<number>* m, filter::time_filter tfilter,
		                    typename derived_line<number>::value_type vt, unsigned int prec=__CPP_TRANSPORT_DEFAULT_PLOT_PRECISION);

		        //! Deserialization constructor
		        time_series(serialization_reader* reader, typename repository<number>::task_finder finder);

		        virtual ~time_series() = default;


		        // DATAPIPE SERVICES

		      public:

		        //! get serial numbers of sample times
		        const std::vector<unsigned int>& get_time_sample_sns() const { return(this->time_sample_sns); }

		        //! extract axis data, corresponding to our sample times, from datapipe
		        const std::vector<double>& pull_time_axis(typename data_manager<number>::datapipe& pipe) const;


		        // WRITE TO A STREAM

		      public:

		        //! write self-details to a stream
		        virtual void write(std::ostream& out);


		        // SERIALIZATION -- implements a 'serializable' interface

		      public:

		        //! Serialize this object
		        virtual void serialize(serialization_writer& writer) const override;


		        // INTERNAL DATA

		      protected:

		        //! gadget for performing t- or k-sample filtration
		        filter f;

		        //! List of time configuration serial numbers to be used for plotting
		        std::vector<unsigned int> time_sample_sns;

			    };


		    template <typename number>
		    time_series<number>::time_series(const integration_task<number>& tk, model<number>* m,
		                                     filter::time_filter tfilter, typename derived_line<number>::value_type vt,
		                                     unsigned int prec)
			    : derived_line<number>(tk, m, derived_line<number>::time_series, vt, prec)
			    {
		        // set up a list of serial numbers corresponding to the sample times for this plot
		        this->f.filter_time_sample(tfilter, tk.get_sample_times(), time_sample_sns);
			    }


		    template <typename number>
		    time_series<number>::time_series(serialization_reader* reader, typename repository<number>::task_finder finder)
          : derived_line<number>(reader, finder)
			    {
		        assert(reader != nullptr);

		        if(reader == nullptr) throw runtime_exception(runtime_exception::RUNTIME_ERROR, __CPP_TRANSPORT_PRODUCT_TIME_SERIES_NULL_READER);

		        unsigned int sns = reader->start_array(__CPP_TRANSPORT_NODE_PRODUCT_TDATA_T_SERIAL_NUMBERS);

		        for(unsigned int i = 0; i < sns; i++)
			        {
		            reader->start_array_element();

		            unsigned int sn;
		            reader->read_value(__CPP_TRANSPORT_NODE_PRODUCT_TDATA_T_SERIAL_NUMBER, sn);
		            time_sample_sns.push_back(sn);

		            reader->end_array_element();
			        }

		        reader->end_element(__CPP_TRANSPORT_NODE_PRODUCT_TDATA_T_SERIAL_NUMBERS);
			    }


		    template <typename number>
		    const std::vector<double>& time_series<number>::pull_time_axis(typename data_manager<number>::datapipe& pipe) const
			    {
		        if(!pipe.validate()) throw runtime_exception(runtime_exception::DATAPIPE_ERROR, __CPP_TRANSPORT_PRODUCT_TIME_SERIES_NULL_DATAPIPE);

		        // set-up time sample data
				    typename data_manager<number>::datapipe::time_config_handle& handle = pipe.new_time_config_handle(this->time_sample_sns);
				    typename data_manager<number>::datapipe::time_config_tag tag = pipe.new_time_config_tag();

				    return handle.lookup_tag(tag);
			    }


		    template <typename number>
		    void time_series<number>::serialize(serialization_writer& writer) const
			    {
		        this->begin_array(writer, __CPP_TRANSPORT_NODE_PRODUCT_TDATA_T_SERIAL_NUMBERS, this->time_sample_sns.size() == 0);
		        for(std::vector<unsigned int>::const_iterator t = this->time_sample_sns.begin(); t != this->time_sample_sns.end(); t++)
			        {
		            this->begin_node(writer, "arrayelt", false);    // node name ignored for arrays
		            this->write_value_node(writer, __CPP_TRANSPORT_NODE_PRODUCT_TDATA_T_SERIAL_NUMBER, *t);
		            this->end_element(writer, "arrayelt");
			        }
		        this->end_element(writer, __CPP_TRANSPORT_NODE_PRODUCT_TDATA_T_SERIAL_NUMBERS);

				    this->derived_line<number>::serialize(writer);
			    }


		    template <typename number>
		    void time_series<number>::write(std::ostream& out)
			    {
				    this->derived_line<number>::write(out);

		        this->wrapper.wrap_out(out, __CPP_TRANSPORT_PRODUCT_TIME_SERIES_TSAMPLE_SN_LABEL " ");

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


#endif //__time_series_H_
