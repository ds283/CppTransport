//
// Created by David Seery on 13/05/2014.
// Copyright (c) 2014 University of Sussex. All rights reserved.
//


#ifndef __background_H_
#define __background_H_


#include "transport-runtime-api/derived-products/time_plot.h"
#include "transport-runtime-api/derived-products/utilities/index_selector.h"

#include "transport-runtime-api/exceptions.h"


namespace transport
	{

		template <typename number> class model;

		namespace derived_data
			{

			  template <typename number>
		    class background: public time_plot<number>
			    {

		      public:

		        // CONSTRUCTOR, DESTRUCTOR

				    //! Basic user-facing constructor. Accepts a filename, integration task,
				    //! filter function which determines which times are plotted,
				    //! and an index_selector<1> which determines which fields are plotted.
				    //! Also needs a model reference.
		        background(const std::string& name, const std::string& filename, const integration_task<number>& tk,
		                   typename plot2d_product<number>::time_filter filter,
		                   index_selector<1>& sel, model<number>* m)
			        : active_indices(sel), mdl(m), time_plot<number>(name, filename, tk, filter)
			        {
				        assert(m != nullptr);

				        if(active_indices.get_number_fields() != mdl->get_N_fields())
					        {
				            std::ostringstream msg;
						        msg << __CPP_TRANSPORT_PRODUCT_BACKGROUND_INDEX_MISMATCH   << " ("
							          << __CPP_TRANSPORT_PRODUCT_BACKGROUND_INDEX_MISMATCH_A << " " << active_indices.get_number_fields() << ", "
							          << __CPP_TRANSPORT_PRODUCT_BACKGROUND_INDEX_MISMATCH_B << " " << mdl->get_N_fields() << ")";
				            throw runtime_exception(runtime_exception::RUNTIME_ERROR, msg.str());
					        }
			        }

				    //! Deserialization constructor.
				    //! Accepts a serialization_reader and an index_selector, together with a model reference.
				    //! The remaining items are extracted from the reader.
				    background(const std::string& name, const integration_task<number>* tk, index_selector<1>* sel, model<number>* m,
				               serialization_reader* reader)
				      : active_indices(*sel), mdl(m), time_plot<number>(name, tk, reader)
					    {
				        assert(m != nullptr);

				        if(active_indices.get_number_fields() != mdl->get_N_fields())
					        {
				            std::ostringstream msg;
				            msg << __CPP_TRANSPORT_PRODUCT_BACKGROUND_INDEX_MISMATCH   << " ("
					            << __CPP_TRANSPORT_PRODUCT_BACKGROUND_INDEX_MISMATCH_A << " " << active_indices.get_number_fields() << ", "
					            << __CPP_TRANSPORT_PRODUCT_BACKGROUND_INDEX_MISMATCH_B << " " << mdl->get_N_fields() << ")";
				            throw runtime_exception(runtime_exception::RUNTIME_ERROR, msg.str());
					        }
					    }

		        ~background() = default;


				    // APPLY THIS DERIVATION TO PRODUCE OUTPUT -- implements a 'derived_product' interface

						//! Apply this derivation to produce a background plot
						virtual void derive(typename data_manager<number>::datapipe& pipe) override;


				    // SERIALIZATION -- implements a 'serializable' interface

		      public:

				    virtual void serialize(serialization_writer& writer) const override;


				    // WRITE SELF TO STANDARD STREAM

		      public:

				    void write(std::ostream& out);


		        // CLONE

		      public:

		        //! Virtual copy
		        virtual derived_product<number>* clone() const override { return new background<number>(static_cast<const background<number>&>(*this)); }


				    // INTERNAL DATA

		      private:

				    //! record which indices are being used
				    index_selector<1> active_indices;

				    //! record model object with which we are associated
				    //! (although this is a pointer we don't care if it is shallow-copied,
				    //! because all model<> instances are handled by the instance_manager)
				    model<number>* mdl;

			    };


		    template <typename number>
				void background<number>::derive(typename data_manager<number>::datapipe& pipe)
			    {
			    }


				template <typename number>
				void background<number>::serialize(serialization_writer& writer) const
					{
						this->write_value_node(writer, __CPP_TRANSPORT_NODE_DERIVED_PRODUCT_TYPE, std::string(__CPP_TRANSPORT_NODE_DERIVED_PRODUCT_BACKGROUND_TIME_PLOT));

						this->begin_node(writer, __CPP_TRANSPORT_NODE_DERIVED_PRODUCT_FIELD_INDICES, false);
						this->active_indices.serialize(writer);
						this->end_element(writer, __CPP_TRANSPORT_NODE_DERIVED_PRODUCT_FIELD_INDICES);

						// call next serialization
						this->time_plot<number>::serialize(writer);
					}


				namespace
					{

						namespace background_helper
							{

								template <typename number>
								derived_data::background<number>* deserialize(const std::string& name, serialization_reader* reader, integration_task<number>* tk, model<number>* m)
									{
										assert(reader != nullptr);
										assert(m != nullptr);

										reader->start_node(__CPP_TRANSPORT_NODE_DERIVED_PRODUCT_FIELD_INDICES);
								    transport::index_selector<1>* bg_sel = transport::index_selector_helper::deserialize<1>(reader);
										reader->end_element(__CPP_TRANSPORT_NODE_DERIVED_PRODUCT_FIELD_INDICES);

								    derived_data::background<number>* rval = new derived_data::background<number>(name, tk, bg_sel, m, reader);

										delete bg_sel;

										return(rval);
									}

							}   // namespace background_helper

					}   // unnamed namespace


				template <typename number>
				void background<number>::write(std::ostream& out)
					{
						assert(this->mdl != nullptr);

						// FIXME: inconsistent output techniques; index_selector won't respect the same wrap settings. Need a better way to do this.
						out << __CPP_TRANSPORT_PRODUCT_BACKGROUND_LABEL_INDICES << ": ";
						this->active_indices.write(out, this->mdl->get_state_names());
						out << std::endl;
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
