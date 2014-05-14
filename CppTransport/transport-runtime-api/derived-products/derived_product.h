//
// Created by David Seery on 13/05/2014.
// Copyright (c) 2014 University of Sussex. All rights reserved.
//


#ifndef __derived_product_H_
#define __derived_product_H_

#include "transport-runtime-api/defaults.h"

#include "transport-runtime-api/serialization/serializable.h"

#include "transport-runtime-api/derived-products/data_view.h"


namespace transport
	{

		// forward-declare class task.
		// task.h includes this header, so we cannot include task.h otherwise we create
		// a circular dependency.
		template <typename number> class task;
		template <typename number> class integration_task;

		namespace derived_data
			{

		    //! A derived product represents some particular post-processing
		    //! of the integration data, perhaps to produce a plot,
		    //! extract the data in some suitable format, etc.
		    template <typename number>
		    class derived_product: public serializable
			    {

		      public:

		        // CONSTRUCTOR, DESTRUCTOR

		        derived_product(const std::string& nm, const std::string& fnam, task<number>& tk)
		          : name(nm), filename(fnam), parent(tk),
		            wrap_width(__CPP_TRANSPORT_DEFAULT_WRAP_WIDTH), cpos(0)
			        {
			        }

		        virtual ~derived_product() = default;


		        // DERIVED PRODUCT INTERFACE

		      public:

				    //! Get name of this derived data product
				    const std::string& get_name() { return(this->name); }

				    //! Get filename associated with this derived data product
				    const std::string& get_filename() { return(this->name); }

				    //! Get parent task
				    task<number>& get_parent_task() { return(this->tk); }

		        //! Apply the analysis represented by this derived product to a given
		        //! output group
		        virtual void derive() = 0;


				    // WRAPPED OUTPUT

		      protected:

				    //! Get wrap width
				    unsigned int get_wrap_width() const { return(this->wrap_width); }

				    //! Set wrap width
				    void set_wrap_width(unsigned int w) { if(w > 0) this->wrap_width = w; }

				    //! Output a string to a stream
				    void wrap_out(std::ostream& out, const std::string& text);

				    //! Output an option to a stream
				    void wrap_option(std::ostream& out, bool value, const std::string& label, unsigned int& count);

				    //! Output a string value to a stream
				    void wrap_value(std::ostream& out, const std::string& value, const std::string& label, unsigned int& count);

				    //! Output a new line
				    void wrap_newline(std::ostream& out) { this->cpos = 0; out << std::endl; }

		        // INTERNAL DATA

		      protected:

		        //! Name of this derived product
		        const std::string name;

		        //! Standardized filename
		        const std::string filename;

		        //! Parent task which which this derived data-product is associated
		        task<number>& parent;

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
				void derived_product<number>::wrap_option(std::ostream& out, bool value, const std::string& label, unsigned int& count)
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

			}   // namespace derived_data

	}   // namespace transport


#endif //__derived_product_H_
