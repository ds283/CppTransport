//
// Created by David Seery on 13/05/2014.
// Copyright (c) 2014 University of Sussex. All rights reserved.
//


#ifndef __background_H_
#define __background_H_


#include "transport-runtime-api/derived-products/time_plot.h"
#include "transport-runtime-api/derived-products/utilities/index_selector.h"


namespace transport
	{

		namespace derived_data
			{

			  template <typename number>
		    class background: public time_plot<number>
			    {

		      public:

		        // CONSTRUCTOR, DESTRUCTOR

		        background(const std::string& name, const std::string& filename, integration_task<number>& tk,
		                   typename plot2d_product<number>::time_filter filter)
			        : time_plot<number>(name, filename, tk, filter)
			        {
			        }

		        ~background() = default;


				    // APPLY THIS DERIVATION TO PRODUCE OUTPUT -- implements a 'derived_product' interface

						//! Apply this derivation to produce a background plot
						virtual void derive() override;


				    // SERIALIZATION -- implements a 'serializable' interface

		      public:

				    virtual void serialize(serialization_writer& writer) const override;


				    // WRITE SELF TO STANDARD STREAM

		      public:

				    void write(std::ostream& out);

			    };


			    template <typename number>
					void background<number>::derive()
				    {
				    }


					template <typename number>
					void background<number>::serialize(serialization_writer& writer) const
						{

						}


					template <typename number>
					void background<number>::write(std::ostream& out)
						{
							this->time_plot<number>::write(out);
							out << std::endl;
						}


		    }   // namespace derived_data


		//! write a 'background' object to a standard stream
		template <typename number>
		std::ostream& operator<<(std::ostream& out, derived_data::background<number>& obj)
			{
				obj.write(out);
				return(out);
			}

	}   // namespace transport


#endif //__background_H_
