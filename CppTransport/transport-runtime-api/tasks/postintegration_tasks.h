//
// Created by David Seery on 04/07/2014.
// Copyright (c) 2014 University of Sussex. All rights reserved.
//


#ifndef __zeta_tasks_H_
#define __zeta_tasks_H_


#include "transport-runtime-api/tasks/task.h"
#include "transport-runtime-api/tasks/integration_tasks.h"
#include "transport-runtime-api/tasks/task_configurations.h"

#include "transport-runtime-api/derived-products/template_types.h"

#include "transport-runtime-api/messages.h"
#include "transport-runtime-api/defaults.h"


#define __CPP_TRANSPORT_NODE_POSTINTEGRATION_TASK_PARENT "parent-task"


namespace transport
	{

		// TASK STRUCTURES -- POST-PROCESSING TASKS

		//! A 'postintegration_task' is a specialization of 'task', used to post-process the output of
		//! an integration to produce zeta correlation functions and other derived products.
		//! The more specialized two- and three-pf zeta tasks are derived from it.
		template <typename number>
		class postintegration_task : public task<number>
			{

				// CONSTRUCTOR, DESTRUCTOR

		  public:

				//! Construct a named postintegration_task with supplied parent integration_task
				postintegration_task(const std::string& nm, const integration_task<number>& t);

				//! deserialization constructor
				postintegration_task(const std::string& nm, serialization_reader* reader, typename repository<number>::task_finder& finder);

				//! override copy constructor to perform a deep copy
				postintegration_task(const postintegration_task<number>& obj);

				//! destroy a postintegration_task
				virtual ~postintegration_task();


				// SERIALIZATION (implements a 'serializable' interface)

		  public:

				//! serialize this object
				virtual void serialize(serialization_writer& writer) const override;


				// INTERNAL DATA

		  protected:

				//! Parent integration task
				integration_task<number>* tk;

			};


		template <typename number>
		postintegration_task<number>::postintegration_task(const std::string& nm, const integration_task<number>& t)
			: task<number>(nm),
        tk(dynamic_cast<integration_task<number>*>(t.clone()))
			{
				assert(tk != nullptr);
			}


		template <typename number>
		postintegration_task<number>::postintegration_task(const postintegration_task<number>& obj)
			: task<number>(obj),
			  tk(dynamic_cast<integration_task<number>*>(obj.tk->clone()))
			{
				assert(tk != nullptr);
			}


		template <typename number>
		postintegration_task<number>::postintegration_task(const std::string& nm, serialization_reader* reader, typename repository<number>::task_finder& finder)
			: task<number>(nm, reader),
				tk(nullptr)
			{
				assert(reader != nullptr);

				// deserialize and reconstruct parent integration task
		    std::string tk_name;
				reader->read_value(__CPP_TRANSPORT_NODE_POSTINTEGRATION_TASK_PARENT, tk_name);

		    std::unique_ptr<typename repository<number>::task_record> record(finder(tk_name));
		    assert(record.get() != nullptr);

		    if(record->get_type() != repository<number>::task_record::integration)
			    throw runtime_exception(runtime_exception::REPOSITORY_ERROR, __CPP_TRANSPORT_REPO_ZETA_TASK_NOT_INTGRTN);

				typename repository<number>::integration_task_record* int_rec = dynamic_cast<typename repository<number>::integration_task_record*>(record.get());
				assert(int_rec != nullptr);

				tk = dynamic_cast<integration_task<number>*>(int_rec->get_task()->clone());
			}


		template <typename number>
		postintegration_task<number>::~postintegration_task()
			{
				delete this->tk;
			}


		template <typename number>
		void postintegration_task<number>::serialize(serialization_writer& writer) const
			{
				// serialize parent integration task
				writer.write_value(__CPP_TRANSPORT_NODE_POSTINTEGRATION_TASK_PARENT, this->tk->get_name());
			}


		// ZETA TWOPF TASK

		//! A 'zeta_twopf_task' task is a postintegration task which produces the zeta two-point function
		template <typename number>
		class zeta_twopf_task: public postintegration_task<number>
			{

				// CONSTRUCTOR, DESTRUCTOR

		  public:

				//! construct a zeta_twopf task
				zeta_twopf_task(const std::string& nm, const integration_task<number>& t);

				//! deserialization constructor
				zeta_twopf_task(const std::string& nm, serialization_reader* reader);

				//! destructor is default
				virtual ~zeta_twopf_task() = default;


				// SERIALIZATION

		  public:

				virtual void serialize(serialization_writer& writer) const override;


				// CLONE

		  public:

				virtual task<number>* clone() const override { return new zeta_twopf_task<number>(static_cast<const zeta_twopf_task<number>&>(*this)); }

			};


		// ZETA THREEPF TASK

		//! A 'zeta_threepf_task' task is a postintegration task which produces the zeta three-point
		//! function and associated derived quantities (the reduced bispectrum at the moment)
		template <typename number>
		class zeta_threepf_task: public postintegration_task<number>
			{

				// CONSTRUCTOR, DESTRUCTOR

		  public:

				//! construct a zeta_threepf task
				zeta_threepf_task(const std::string& nm, const integration_task<number>& t);

				//! deserialization constructor
				zeta_threepf_task(const std::string& nm, serialization_reader* reader);

				//! destructor is default
				virtual ~zeta_threepf_task() = default;


				// SERIALIZATION

		  public:

				virtual void serialize(serialization_writer& writer) const override;


				// CLONE

		  public:

				virtual task<number>* clone() const override { return new zeta_threepf_task<number>(static_cast<const zeta_threepf_task<number>&>(*this)); }

			};


		// FNL TASK

		//! An 'fNL_task' is a postintegration task which produces an fNL amplitude
		template <typename number>
		class fNL_task: public postintegration_task<number>
			{

				// CONSTRUCTOR, DESTRUCTOR

		  public:

				//! construct an fNL task
				fNL_task(const std::string& nm, const integration_task<number>& t);

				//! deserialization constructor
				fNL_task(const std::string& nm, serialization_reader* reader);

				//! destructor is default
				virtual ~fNL_task() = default;


				// SERIALIZATION

		  public:

				virtual void serialize(serialization_writer& writer) const override;


				// CLONE

		  public:

				virtual task<number>* clone() const override { return new fNL_task<number>(static_cast<const fNL_task<number>&>(*this)); }


				// INTERNAL DATA

		  protected:

				derived_data::template_type type;

			};

	}   // namespace transport


#endif //__zeta_tasks_H_
