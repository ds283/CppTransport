//
// Created by David Seery on 13/05/2014.
// Copyright (c) 2014 University of Sussex. All rights reserved.
//


#ifndef __derived_product_H_
#define __derived_product_H_


#include "transport-runtime-api/serialization/serializable.h"

// need repository in order to get the details of a repository<number>::output_group
// (can't forward-declare because it is a nested class)
#include "transport-runtime-api/manager/repository.h"

// need data_manager in order to get the details of a data_manager<number>>data_pipe
// (can't forward-declare because it is a nested class)
#include "transport-runtime-api/manager/data_manager.h"

#include "transport-runtime-api/derived-products/data_view.h"

#include "transport-runtime-api/defaults.h"
#include "transport-runtime-api/messages.h"
#include "transport-runtime-api/exceptions.h"


#define __CPP_TRANSPORT_NODE_DERIVED_PRODUCT_TYPE                 "derived-product-type"

#define __CPP_TRANSPORT_NODE_DERIVED_PRODUCT_BACKGROUND_TIME_PLOT "background-time-plot"
#define __CPP_TRANSPORT_NODE_DERIVED_PRODUCT_GENERAL_TIME_PLOT    "general-time-plot"

#define __CPP_TRANSPORT_NODE_DERIVED_PRODUCT_FIELD_INDICES        "active-field-indices"

#define __CPP_TRANSPORT_NODE_DERIVED_PRODUCT_FILENAME             "filename"


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

		    //! A derived product represents some particular post-processing
		    //! of the integration data, perhaps to produce a plot,
		    //! extract the data in some suitable format, etc.
		    template <typename number>
		    class derived_product: public serializable
			    {

		      public:

		        typedef std::function<bool(double)> time_filter;

				    typedef std::function<bool(double)> twopf_kconfig_filter;

				    typedef std::function<bool(double,double,double)> threepf_kconfig_filter;

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
