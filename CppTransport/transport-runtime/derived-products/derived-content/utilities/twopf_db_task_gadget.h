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


#ifndef CPPTRANSPORT_TWOPF_DB_TASK_GADGET_H
#define CPPTRANSPORT_TWOPF_DB_TASK_GADGET_H


#include "transport-runtime/exceptions.h"
#include "transport-runtime/messages.h"

#include "transport-runtime/tasks/tasks_forward_declare.h"
#include "transport-runtime/repository/records/repository_records_forward_declare.h"


namespace transport
	{

		namespace derived_data
			{

				template <typename number>
				class twopf_db_task_gadget
					{

				  public:

						//! construct gadget directly from a suitable twopf_db_task instance
						twopf_db_task_gadget(const twopf_db_task<number>& tk);

						//! construct gadget from a derivable_task_set<> instance, assumed to contain a suitable
						//! twopf_db_task instance
            twopf_db_task_gadget(const typename derivable_task_set<number>::type& task_list);

						//! override copy constructor to perform a deep copy
						twopf_db_task_gadget(const twopf_db_task_gadget<number>& obj);

						//! destructor
						~twopf_db_task_gadget() = default;


						// ADMIN

				  public:

						//! get pointer to model instance
						model<number>* get_model() const { return this->mdl; }

						//! get pointer to twopf_db_task<> instance
						twopf_db_task<number>* get_twopf_db_task() const { return this->db_task.get(); }

						//! quick access to get number of fields
						unsigned int get_N_fields() const { return(this->N_fields); }


						// PRIVATE DATA

				  private:

						//! pointer to integration task
						std::unique_ptr< twopf_db_task<number> > db_task;

				    //! pointer to model.
				    //! It's OK for this to be 'shallow' copied, rather than 'deep' copied, because model instances are managed
				    //! by the top-level instance manager. This means our pointer won't be deallocated during the runtime of
				    //! this task.
						model<number>* mdl;

						//! store number of fields
						unsigned int N_fields;

					};


				template <typename number>
				twopf_db_task_gadget<number>::twopf_db_task_gadget(const twopf_db_task<number>& tk)
					: mdl(tk.get_model()),
            db_task(dynamic_cast< twopf_db_task<number>* >(tk.clone())),
            N_fields(tk.get_model()->get_N_fields())
					{
					}


				template <typename number>
				twopf_db_task_gadget<number>::twopf_db_task_gadget(const typename derivable_task_set<number>::type& task_list)
					: mdl(nullptr),
            db_task(nullptr),
            N_fields(0)
					{
					  // search through task_list to find a suitable twopf_db_task<> instance
					  for(const auto& elt : task_list)
              {
                const derivable_task<number>& tk = elt.second.get_task();

                if(tk.get_type() == task_type::integration)
                  {
                    db_task.reset(dynamic_cast< twopf_db_task<number>* >(tk.clone()));
                    mdl = db_task->get_model();
                    N_fields = mdl->get_N_fields();
                    break;
                  }
              }
					}


        template <typename number>
        twopf_db_task_gadget<number>::twopf_db_task_gadget(const twopf_db_task_gadget<number>& obj)
          : db_task(dynamic_cast< twopf_db_task<number>* >(obj.db_task->clone())),
            mdl(obj.mdl),
            N_fields(obj.N_fields)
          {
          }


			}   // namespace derived_data

	}   // namespace transport


#endif //CPPTRANSPORT_TWOPF_DB_TASK_GADGET_H
