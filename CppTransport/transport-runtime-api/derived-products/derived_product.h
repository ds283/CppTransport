//
// Created by David Seery on 13/05/2014.
// Copyright (c) 2014 University of Sussex. All rights reserved.
//


#ifndef __derived_product_H_
#define __derived_product_H_


#include <float.h>


#include "transport-runtime-api/serialization/serializable.h"

// need repository in order to get the details of a repository<number>::output_group
// (can't forward-declare because it is a nested class)
#include "transport-runtime-api/manager/repository.h"

// need data_manager in order to get the details of a data_manager<number>>data_pipe
// (can't forward-declare because it is a nested class)
#include "transport-runtime-api/manager/data_manager.h"

#include "transport-runtime-api/tasks/task_k_configurations.h"

#include "transport-runtime-api/derived-products/data_view.h"

#include "transport-runtime-api/defaults.h"
#include "transport-runtime-api/messages.h"
#include "transport-runtime-api/exceptions.h"


#define __CPP_TRANSPORT_NODE_DERIVED_PRODUCT_TYPE              "derived-product-type"

#define __CPP_TRANSPORT_NODE_DERIVED_PRODUCT_GENERAL_TIME_PLOT "general-time-plot"

#define __CPP_TRANSPORT_NODE_DERIVED_PRODUCT_FILENAME          "filename"


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

				//! Filters for time and k-configurations
				class filter
					{

				  public:

				    class time_filter_data
					    {
				      public:
				        bool max;
				        bool min;
				        double time;
					    };

				    class twopf_kconfig_filter_data
					    {
				      public:
				        bool max;
				        bool min;
				        double k;
					    };

				    class threepf_kconfig_filter_data
					    {
				      public:
				        double kt;
				        double alpha;
				        double beta;
				        double k1;
				        double k2;
				        double k3;
				        bool kt_max;
				        bool kt_min;
				        bool alpha_max;
				        bool alpha_min;
				        bool beta_max;
				        bool beta_min;
				        bool k1_max;
				        bool k1_min;
				        bool k2_max;
				        bool k2_min;
				        bool k3_max;
				        bool k3_min;
					    };

				  public:

				    typedef std::function<bool(const time_filter_data&)> time_filter;

				    typedef std::function<bool(const twopf_kconfig_filter_data&)> twopf_kconfig_filter;

				    typedef std::function<bool(const threepf_kconfig_filter_data&)> threepf_kconfig_filter;


				    // FILTERS

				  protected:

				    //! time filter: filter out the serial numbers we want from a list
				    void filter_time_sample(time_filter t_filter, const std::vector<double>& t_samples, std::vector<unsigned int>& t_serials) const;

						//! 2pf k-configuration filter
						void filter_twopf_kconfig_sample(twopf_kconfig_filter k_filter, const std::vector<double>& k_samples,
						                                 std::vector<unsigned int>& k_serials) const;

						//! 3pf k-configuration filter
						void filter_threepf_kconfig_sample(threepf_kconfig_filter k_filter, const std::vector<threepf_kconfig>& k_samples,
						                                   std::vector<unsigned int>& k_serials) const;

					};


		    void filter::filter_time_sample(time_filter t_filter, const std::vector<double>& t_samples, std::vector<unsigned int>& t_serials) const
			    {
		        int min_pos = -1;
		        double t_min = DBL_MAX;
		        int max_pos = -1;
		        double t_max = DBL_MIN;

		        // scan through to find min and max values of time
		        for(unsigned int i = 0; i < t_samples.size(); i++)
			        {
		            if(t_samples[i] > t_min) { t_min = t_samples[i]; min_pos = i; }
		            if(t_samples[i] < t_max) { t_max = t_samples[i]; max_pos = i; }
			        }

		        // ask filter to decide which values it wants
		        t_serials.clear();
		        for(unsigned int i = 0; i < t_samples.size(); i++)
			        {
		            time_filter_data data;

		            data.max = (i == max_pos ? true : false);
		            data.min = (i == min_pos ? true : false);
		            data.time = t_samples[i];

		            if(t_filter(data)) t_serials.push_back(i);
			        }
			    }


		    void filter::filter_twopf_kconfig_sample(twopf_kconfig_filter k_filter, const std::vector<double>& k_samples,
		                                             std::vector<unsigned int>& k_serials) const
			    {
		        int min_pos = -1;
		        double k_min = DBL_MAX;
		        int max_pos = -1;
		        double k_max = DBL_MIN;

		        // scan through to find min and max values of time
		        for(unsigned int i = 0; i < k_samples.size(); i++)
			        {
		            if(k_samples[i] < k_min) { k_min = k_samples[i]; min_pos = i; }
		            if(k_samples[i] > k_max) { k_max = k_samples[i]; max_pos = i; }
			        }

		        // ask filter to decide which values it wants
		        k_serials.clear();
		        for(unsigned int i = 0; i < k_samples.size(); i++)
			        {
		            twopf_kconfig_filter_data data;

		            data.max = (i == max_pos ? true : false);
		            data.min = (i == min_pos ? true : false);
		            data.k = k_samples[i];

		            if(k_filter(data)) k_serials.push_back(i);
			        }
			    }


		    void filter::filter_threepf_kconfig_sample(threepf_kconfig_filter k_filter, const std::vector<threepf_kconfig>& k_samples,
		                                               std::vector<unsigned int>& k_serials) const
			    {
		        int kt_min_pos = -1;
		        double kt_min = DBL_MAX;
		        int kt_max_pos = -1;
		        double kt_max = DBL_MIN;

				    int alpha_min_pos = -1;
				    double alpha_min = DBL_MAX;
				    int alpha_max_pos = -1;
				    double alpha_max = DBL_MIN;

				    int beta_min_pos = -1;
				    double beta_min = DBL_MAX;
				    int beta_max_pos = -1;
				    double beta_max = DBL_MIN;

				    int k1_min_pos = -1;
				    double k1_min = DBL_MAX;
				    int k1_max_pos = -1;
				    double k1_max = DBL_MIN;

				    int k2_min_pos = -1;
				    double k2_min = DBL_MAX;
				    int k2_max_pos = -1;
				    double k2_max = DBL_MIN;

				    int k3_min_pos = -1;
				    double k3_min = DBL_MAX;
				    int k3_max_pos = -1;
				    double k3_max = DBL_MIN;

		        // scan through to find min and max values of time
		        for(unsigned int i = 0; i < k_samples.size(); i++)
			        {
		            if(k_samples[i].k_t_conventional < kt_min) { kt_min = k_samples[i].k_t_conventional; kt_min_pos = i; }
		            if(k_samples[i].k_t_conventional > kt_max) { kt_max = k_samples[i].k_t_conventional; kt_max_pos = i; }

		            if(k_samples[i].alpha < alpha_min) { alpha_min = k_samples[i].alpha; alpha_min_pos = i; }
		            if(k_samples[i].alpha > alpha_max) { alpha_max = k_samples[i].alpha; alpha_max_pos = i; }

		            if(k_samples[i].beta < beta_min) { beta_min = k_samples[i].beta; beta_min_pos = i; }
		            if(k_samples[i].beta > beta_max) { beta_max = k_samples[i].beta; beta_max_pos = i; }

		            if(k_samples[i].k1 < k1_min) { k1_min = k_samples[i].k1; k1_min_pos = i; }
		            if(k_samples[i].k1 > k1_max) { k1_max = k_samples[i].k1; k1_max_pos = i; }

		            if(k_samples[i].k2 < k2_min) { k2_min = k_samples[i].k2; k2_min_pos = i; }
		            if(k_samples[i].k2 > k2_max) { k2_max = k_samples[i].k2; k2_max_pos = i; }

		            if(k_samples[i].k3 < k3_min) { k3_min = k_samples[i].k3; k3_min_pos = i; }
		            if(k_samples[i].k3 > k3_max) { k3_max = k_samples[i].k3; k3_max_pos = i; }
			        }

		        // ask filter to decide which values it wants
		        k_serials.clear();
		        for(unsigned int i = 0; i < k_samples.size(); i++)
			        {
		            threepf_kconfig_filter_data data;

		            data.kt_max = (i == kt_max_pos ? true : false);
		            data.kt_min = (i == kt_min_pos ? true : false);
		            data.kt = k_samples[i].k_t_conventional;

		            data.alpha_max = (i == alpha_max_pos ? true : false);
		            data.alpha_min = (i == alpha_min_pos ? true : false);
		            data.alpha = k_samples[i].alpha;

		            data.beta_max = (i == beta_max_pos ? true : false);
		            data.beta_min = (i == beta_min_pos ? true : false);
		            data.beta = k_samples[i].beta;

		            data.k1_max = (i == k1_max_pos ? true : false);
		            data.k1_min = (i == k1_min_pos ? true : false);
		            data.k1 = k_samples[i].k1;

		            data.k2_max = (i == k2_max_pos ? true : false);
		            data.k2_min = (i == k2_min_pos ? true : false);
		            data.k2 = k_samples[i].k2;

		            data.k3_max = (i == k3_max_pos ? true : false);
		            data.k3_min = (i == k3_min_pos ? true : false);
		            data.k3 = k_samples[i].k3;

		            if(k_filter(data)) k_serials.push_back(i);
			        }
			    }


		    //! A derived product represents some particular post-processing
		    //! of the integration data, perhaps to produce a plot,
		    //! extract the data in some suitable format, etc.
		    template <typename number>
		    class derived_product: public serializable
			    {

		      public:

		        // CONSTRUCTOR, DESTRUCTOR

				    //! Construct a derived product object.
		        derived_product(const std::string& nm, const std::string& fnam, const task<number>& tk)
		          : name(nm), filename(fnam), parent(tk.clone()),
		            wrap_width(__CPP_TRANSPORT_DEFAULT_WRAP_WIDTH), cpos(0)
			        {
						    assert(parent != nullptr);
			        }

				    //! Override default copy constructor to generate a deep copy of the task object
				    derived_product(const derived_product<number>& obj)
				      : name(obj.name), filename(obj.filename), parent(obj.parent->clone())
					    {
						    assert(parent != nullptr);
					    }

				    //! Deserialization constructor
				    derived_product(const std::string& nm, const task<number>* tk, serialization_reader* reader)
				      : name(nm), parent(tk->clone()), wrap_width(__CPP_TRANSPORT_DEFAULT_WRAP_WIDTH), cpos(0)
					    {
						    assert(reader != nullptr);
						    assert(parent != nullptr);

						    reader->read_value(__CPP_TRANSPORT_NODE_DERIVED_PRODUCT_FILENAME, filename);
					    }

		        virtual ~derived_product()
			        {
								assert(this->parent != nullptr);
				        delete this->parent;
			        }


		        // DERIVED PRODUCT INTERFACE

		      public:

				    //! Get name of this derived data product
				    const std::string& get_name() const { return(this->name); }

				    //! Get filename associated with this derived data product
				    const std::string& get_filename() { return(this->name); }

				    //! Get parent task
				    const task<number>* get_parent_task() const { return(this->parent); }

		        //! Apply the analysis represented by this derived product to a given
		        //! output group
		        virtual void derive(typename data_manager<number>::datapipe& pipe) = 0;


				    // WRAPPED OUTPUT

		      protected:

				    //! Get wrap width
				    unsigned int get_wrap_width() const { return(this->wrap_width); }

				    //! Set wrap width
				    void set_wrap_width(unsigned int w) { if(w > 0) this->wrap_width = w; }

				    //! Output a string to a stream
				    void wrap_out(std::ostream& out, const std::string& text);

				    //! Output an option to a stream
				    void wrap_list_item(std::ostream& out, bool value, const std::string& label, unsigned int& count);

				    //! Output a string value to a stream
				    void wrap_value(std::ostream& out, const std::string& value, const std::string& label, unsigned int& count);

				    //! Output a new line
				    void wrap_newline(std::ostream& out) { this->cpos = 0; out << std::endl; }


				    // SERIALIZATION -- implements a 'serializable' interface

		      public:

				    virtual void serialize(serialization_writer& writer) const override;


				    // CLONE

		      public:

				    virtual derived_product<number>* clone() const = 0;

		        // INTERNAL DATA

		      protected:

		        //! Name of this derived product
		        const std::string name;

		        //! Standardized filename
		        std::string filename;

		        //! Parent task which which this derived data-product is associated.
				    //! We store our own copy.
		        task<number>* parent;

				    // WRAPPED OUTPUT

				    //! Width at which to wrap output
				    unsigned int wrap_width;

				    //! Current line position
				    unsigned int cpos;
			    };


				template <typename number>
				void derived_product<number>::wrap_out(std::ostream& out, const std::string& text)
					{
						if(this->cpos + text.length() >= this->wrap_width) this->wrap_newline(out);
						out << text;
						this->cpos += text.length();
					}


				template <typename number>
				void derived_product<number>::wrap_list_item(std::ostream& out, bool value, const std::string& label, unsigned int& count)
					{
						if(value)
							{
								if(this->cpos + label.length() + 2 >= this->wrap_width) this->wrap_newline(out);
						    if(count > 0) out << ", ";
						    out << label;
						    count++;
								this->cpos += label.length() + 2;
							}
					}


				template <typename number>
				void derived_product<number>::wrap_value(std::ostream& out, const std::string& value, const std::string& label, unsigned int& count)
					{
						if(this->cpos + value.length() + label.length() + 5 >= this->wrap_width) this->wrap_newline(out);
						if(count > 0) out << ", ";
						out << label << " = \"" << value << "\"";
						count++;
						this->cpos += value.length() + label.length() + 5;
					}


				template <typename number>
				void derived_product<number>::serialize(serialization_writer& writer) const
					{
						this->write_value_node(writer, __CPP_TRANSPORT_NODE_DERIVED_PRODUCT_FILENAME, this->filename);
					}


			}   // namespace derived_data

	}   // namespace transport


#endif //__derived_product_H_
