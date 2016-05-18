//
// Created by David Seery on 02/04/15.
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


#ifndef CPPTRANSPORT_INTEGRATION_TASK_GADGET_H
#define CPPTRANSPORT_INTEGRATION_TASK_GADGET_H


#include "transport-runtime/exceptions.h"
#include "transport-runtime/messages.h"

#include "transport-runtime/tasks/tasks_forward_declare.h"
#include "transport-runtime/repository/records/repository_records_forward_declare.h"


namespace transport
	{

		namespace derived_data
			{

				template <typename number>
				class integration_task_gadget
					{

				  public:

						//! construct gadget from direct integration_task
						integration_task_gadget(const twopf_db_task<number>& tk);

						//! null constructor, used when deserializing
						integration_task_gadget();

						//! override copy constructor to perform a deep copy
						integration_task_gadget(const integration_task_gadget<number>& obj);

						//! destructor
						~integration_task_gadget();


						// ADMIN

				  public:

						//! initialize after default constructor
						void set_task(derivable_task<number>* tk, task_finder<number>& finder);

						//! get pointer to model instance
						model<number>* get_model() const { return(this->mdl); }

						//! get pointer to integration_task<> instance
						twopf_db_task<number>* get_integration_task() const { return(this->itk); }

						//! quick access to get number of fields
						unsigned int get_N_fields() const { return(this->N_fields); }


						// PRIVATE DATA

				  private:

						//! pointer to integration task
						twopf_db_task<number>* itk;

				    //! pointer to model.
				    //! It's OK for this to be 'shallow' copied, rather than 'deep' copied, because model instances are managed
				    //! by the top-level instance manager. This means our pointer won't be deallocated during the runtime of
				    //! this task.
						model<number>* mdl;

						//! store number of fields
						unsigned int N_fields;

					};


				template <typename number>
				integration_task_gadget<number>::integration_task_gadget(const twopf_db_task<number>& tk)
					: mdl(tk.get_model()),
					  itk(dynamic_cast< twopf_db_task<number>* >(tk.clone())),
						N_fields(tk.get_model()->get_N_fields())
					{
					}


				template <typename number>
				integration_task_gadget<number>::integration_task_gadget()
					: mdl(nullptr),
					  itk(nullptr),
					  N_fields(0)
					{
					}


				template <typename number>
				integration_task_gadget<number>::~integration_task_gadget()
					{
						delete this->itk;
					}


				template <typename number>
				void integration_task_gadget<number>::set_task(derivable_task<number>* tk, task_finder<number>& finder)
					{
						assert(tk != nullptr);

				    std::unique_ptr< task_record<number> > tk_rec = finder(tk->get_name());
				    assert(tk_rec.get() != nullptr);

				    if(tk_rec->get_type() != task_type::integration)
					    throw runtime_exception(exception_type::REPOSITORY_ERROR, CPPTRANSPORT_REPO_TASK_NOT_INTEGRATION);

				    integration_task_record<number>* int_rec = dynamic_cast< integration_task_record<number>* >(tk_rec.get());
				    assert(int_rec != nullptr);

				    this->itk = dynamic_cast< twopf_db_task<number>* >(int_rec->get_task()->clone());
						assert(this->itk != nullptr);

				    this->mdl = this->itk->get_model();

						this->N_fields = this->mdl->get_N_fields();
					}


				template <typename number>
				integration_task_gadget<number>::integration_task_gadget(const integration_task_gadget<number>& obj)
					: itk(dynamic_cast< twopf_db_task<number>* >(obj.itk->clone())),
		        mdl(obj.mdl),
		        N_fields(obj.N_fields)
					{
					}


			}   // namespace derived_data

	}   // namespace transport


#endif //CPPTRANSPORT_INTEGRATION_TASK_GADGET_H
