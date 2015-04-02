//
// Created by David Seery on 04/07/2014.
// Copyright (c) 2014-15 University of Sussex. All rights reserved.
//


#ifndef __zeta_tasks_H_
#define __zeta_tasks_H_


#include <assert.h>
#include <iostream>
#include <iomanip>
#include <array>
#include <vector>
#include <list>
#include <sstream>
#include <stdexcept>
#include <functional>


#include "transport-runtime-api/tasks/task.h"
#include "transport-runtime-api/tasks/derivable_task.h"
#include "transport-runtime-api/tasks/integration_tasks.h"
#include "transport-runtime-api/tasks/task_configurations.h"

#include "transport-runtime-api/derived-products/template_types.h"

#include "transport-runtime-api/messages.h"
#include "transport-runtime-api/defaults.h"


#define __CPP_TRANSPORT_NODE_POSTINTEGRATION_TASK_PARENT     "parent-task"

#define __CPP_TRANSPORT_NODE_FNL_TASK_TEMPLATE               "template"
#define __CPP_TRANSPORT_NODE_FNL_TASK_TEMPLATE_LOCAL         "local"
#define __CPP_TRANSPORT_NODE_FNL_TASK_TEMPLATE_EQUI          "equilateral"
#define __CPP_TRANSPORT_NODE_FNL_TASK_TEMPLATE_ORTHO         "orthogonal"
#define __CPP_TRANSPORT_NODE_FNL_TASK_TEMPLATE_DBI           "DBI"


namespace transport
	{

		// TASK STRUCTURES -- POST-PROCESSING TASKS

		//! A 'postintegration_task' is a specialization of 'task', used to post-process the output of
		//! an integration to produce zeta correlation functions and other derived products.
		//! The more specialized two- and three-pf zeta tasks are derived from it.
		template <typename number>
		class postintegration_task: public derivable_task<number>
			{

				// CONSTRUCTOR, DESTRUCTOR

		  public:

				//! Construct a named postintegration_task with supplied parent integration_task
				postintegration_task(const std::string& nm, const derivable_task<number>& t);

				//! deserialization constructor
				postintegration_task(const std::string& nm, Json::Value& reader, typename repository_finder<number>::task_finder& finder);

				//! override copy constructor to perform a deep copy
				postintegration_task(const postintegration_task<number>& obj);

				//! destroy a postintegration_task
				virtual ~postintegration_task();


		    // INTERFACE - implements a 'derivable task' interface

		  public:

		    //! Get vector of time configurations to store; in a postintegration task, this is inherited from our parent,
				//! which may itself inherit from its parent, and so on
		    virtual const std::vector<time_config>& get_time_config_list() const override { return(this->ptk->get_time_config_list()); }


        // INTERFACE

      public:

        //! Get parent integration task
        derivable_task<number>* get_parent_task() const { return(this->ptk); }


				// SERIALIZATION (implements a 'serializable' interface)

		  public:

				//! serialize this object
				virtual void serialize(Json::Value& writer) const override;


        // WRITE TO STREAM

      public:

        //! write to stream
        void write(std::ostream& out) const;


				// INTERNAL DATA

		  protected:

				//! Parent task, which must be of derivable type
				derivable_task<number>* ptk;

			};


    template <typename number>
    std::ostream& operator<<(std::ostream& out, const postintegration_task<number>& obj)
      {
        obj.write(out);
        return(out);
      }


		template <typename number>
		postintegration_task<number>::postintegration_task(const std::string& nm, const derivable_task<number>& t)
			: derivable_task<number>(nm),
        ptk(dynamic_cast<derivable_task<number>*>(t.clone()))
			{
				assert(ptk != nullptr);
			}


		template <typename number>
		postintegration_task<number>::postintegration_task(const postintegration_task<number>& obj)
			: derivable_task<number>(obj),
			  ptk(dynamic_cast<derivable_task<number>*>(obj.ptk->clone()))
			{
				assert(ptk != nullptr);
			}


		template <typename number>
		postintegration_task<number>::postintegration_task(const std::string& nm, Json::Value& reader, typename repository_finder<number>::task_finder& finder)
			: derivable_task<number>(nm, reader),
				ptk(nullptr)
			{
				// deserialize and reconstruct parent integration task
		    std::string tk_name = reader[__CPP_TRANSPORT_NODE_POSTINTEGRATION_TASK_PARENT].asString();

		    std::unique_ptr< task_record<number> > record(finder(tk_name));
		    assert(record.get() != nullptr);

		    if(record->get_type() != task_record<number>::integration || record->get_type() != task_record<number>::postintegration)
			    throw runtime_exception(runtime_exception::REPOSITORY_ERROR, __CPP_TRANSPORT_REPO_ZETA_TASK_NOT_INTGRTN);

				ptk = dynamic_cast< derivable_task<number>* >(record->get_abstract_task()->clone());
			}


		template <typename number>
		postintegration_task<number>::~postintegration_task()
			{
				delete this->ptk;
			}


		template <typename number>
		void postintegration_task<number>::serialize(Json::Value& writer) const
			{
				// serialize parent integration task
				writer[__CPP_TRANSPORT_NODE_POSTINTEGRATION_TASK_PARENT] = this->ptk->get_name();
				this->derivable_task<number>::serialize(writer);
			}


    template <typename number>
    void postintegration_task<number>::write(std::ostream& out) const
      {
        out << __CPP_TRANSPORT_PARENT_TASK << ": '" << this->ptk->get_name() << "'" << std::endl;
      }


		// ZETA TWOPF LIST TASK -- COMMON ANCESTOR FOR ZETA_TWOPF_TASK AND ZETA_THREEPF_TASK

		template <typename number>
		class zeta_twopf_list_task: public postintegration_task<number>
			{

				// CONSTRUCTOR, DESTRUCTOR

		  public:

				//! construct a zeta_twopf_list task
				zeta_twopf_list_task(const std::string& nm, const twopf_list_task<number>& t);

				//! deserialization constructor
				zeta_twopf_list_task(const std::string& nm, Json::Value& reader, typename repository_finder<number>::task_finder& finder);

				//! destructor is default
				~zeta_twopf_list_task() = default;


		    // INTERFACE

		  public:

		    //! Get flattened list of ks at which we sample the two-point function
		    const std::vector<twopf_kconfig>& get_twopf_kconfig_list() const { return(this->ptk_as_twopf_list->get_twopf_kconfig_list()); }


		    // SERIALIZATION

		  public:

		    virtual void serialize(Json::Value& writer) const override;


				// PRIVATE DATA

		  protected:

				//! cast-up version of parent task
				//! TODO: it would be preferable to avoid this somehow
				twopf_list_task<number>* ptk_as_twopf_list;

			};


		template <typename number>
		zeta_twopf_list_task<number>::zeta_twopf_list_task(const std::string& nm, const twopf_list_task<number>& t)
			: postintegration_task<number>(nm, t)
			{
				ptk_as_twopf_list = dynamic_cast< twopf_list_task<number>* >(this->ptk);
				assert(ptk_as_twopf_list != nullptr);

		    if(ptk_as_twopf_list == nullptr) throw runtime_exception(runtime_exception::RUNTIME_ERROR, __CPP_TRANSPORT_ZETA_TWOPF_LIST_CAST_FAIL);
			}


		template <typename number>
		zeta_twopf_list_task<number>::zeta_twopf_list_task(const std::string& nm, Json::Value& reader, typename repository_finder<number>::task_finder& finder)
			: postintegration_task<number>(nm, reader, finder)
			{
		    ptk_as_twopf_list = dynamic_cast< twopf_list_task<number>* >(this->ptk);
				assert(ptk_as_twopf_list != nullptr);

				if(ptk_as_twopf_list == nullptr) throw runtime_exception(runtime_exception::RUNTIME_ERROR, __CPP_TRANSPORT_ZETA_TWOPF_LIST_CAST_FAIL);
			}


		template <typename number>
		void zeta_twopf_list_task<number>::serialize(Json::Value& writer) const
			{
		    this->postintegration_task<number>::serialize(writer);
			}


		// ZETA TWOPF TASK

		//! A 'zeta_twopf_task' task is a postintegration task which produces the zeta two-point function
		template <typename number>
		class zeta_twopf_task: public zeta_twopf_list_task<number>
			{

				// CONSTRUCTOR, DESTRUCTOR

		  public:

				//! construct a zeta_twopf task
				zeta_twopf_task(const std::string& nm, const twopf_task<number>& t);

				//! deserialization constructor
				zeta_twopf_task(const std::string& nm, Json::Value& reader, typename repository_finder<number>::task_finder& finder);

				//! destructor is default
				virtual ~zeta_twopf_task() = default;


				// SERIALIZATION

		  public:

				virtual void serialize(Json::Value& writer) const override;


				// CLONE

		  public:

				virtual task<number>* clone() const override { return new zeta_twopf_task<number>(static_cast<const zeta_twopf_task<number>&>(*this)); }

			};


    template <typename number>
    zeta_twopf_task<number>::zeta_twopf_task(const std::string& nm, const twopf_task<number>& t)
      : zeta_twopf_list_task<number>(nm, t)
      {
      }


    template <typename number>
    zeta_twopf_task<number>::zeta_twopf_task(const std::string& nm, Json::Value& reader, typename repository_finder<number>::task_finder& finder)
      : zeta_twopf_list_task<number>(nm, reader, finder)
      {
      }


    template <typename number>
    void zeta_twopf_task<number>::serialize(Json::Value& writer) const
      {
        writer[__CPP_TRANSPORT_NODE_TASK_TYPE] = std::string(__CPP_TRANSPORT_NODE_TASK_TYPE_ZETA_TWOPF);

        this->zeta_twopf_list_task<number>::serialize(writer);
      }


		// ZETA THREEPF TASK

		//! A 'zeta_threepf_task' task is a postintegration task which produces the zeta three-point
		//! function and associated derived quantities (the reduced bispectrum at the moment)
		template <typename number>
		class zeta_threepf_task: public zeta_twopf_list_task<number>
			{

				// CONSTRUCTOR, DESTRUCTOR

		  public:

				//! construct a zeta_threepf task
				zeta_threepf_task(const std::string& nm, const threepf_task<number>& t);

				//! deserialization constructor
				zeta_threepf_task(const std::string& nm, Json::Value& reader, typename repository_finder<number>::task_finder& finder);

				//! destructor is default
				virtual ~zeta_threepf_task() = default;


				// INTERFACE

		  public:

		    //! Determine whether this task is integrable; inherited from parent threepf_task
		    bool is_integrable() const { return(this->ptk_as_threepf->is_integrable()); }

		    //! Get list of k-configurations at which this task samples the threepf; inherited from parent threepf_task
		    const std::vector<threepf_kconfig>& get_threepf_kconfig_list() const { return(this->ptk_as_threepf->get_threepf_kconfig_list()); }


				// SERIALIZATION

		  public:

				virtual void serialize(Json::Value& writer) const override;


				// CLONE

		  public:

				virtual task<number>* clone() const override { return new zeta_threepf_task<number>(static_cast<const zeta_threepf_task<number>&>(*this)); }


				// PRIVATE DATA

		  protected:

				//! cast-up version of parent task
				//! TODO: it would be preferable to avoid this somehow
				threepf_task<number>* ptk_as_threepf;

			};


    template <typename number>
    zeta_threepf_task<number>::zeta_threepf_task(const std::string& nm, const threepf_task<number>& t)
      : zeta_twopf_list_task<number>(nm, t)
      {
		    ptk_as_threepf = dynamic_cast< threepf_task<number>* >(this->ptk);
		    assert(ptk_as_threepf != nullptr);

		    if(ptk_as_threepf == nullptr) throw runtime_exception(runtime_exception::RUNTIME_ERROR, __CPP_TRANSPORT_ZETA_THREEPF_CAST_FAIL);
      }


    template <typename number>
    zeta_threepf_task<number>::zeta_threepf_task(const std::string& nm, Json::Value& reader, typename repository_finder<number>::task_finder& finder)
      : zeta_twopf_list_task<number>(nm, reader, finder)
      {
        ptk_as_threepf = dynamic_cast< threepf_task<number>* >(this->ptk);
        assert(ptk_as_threepf != nullptr);

        if(ptk_as_threepf == nullptr) throw runtime_exception(runtime_exception::RUNTIME_ERROR, __CPP_TRANSPORT_ZETA_THREEPF_CAST_FAIL);
      }


    template <typename number>
    void zeta_threepf_task<number>::serialize(Json::Value& writer) const
      {
        writer[__CPP_TRANSPORT_NODE_TASK_TYPE] = std::string(__CPP_TRANSPORT_NODE_TASK_TYPE_ZETA_THREEPF);

        this->zeta_twopf_list_task<number>::serialize(writer);
      }


		// FNL TASK

		//! An 'fNL_task' is a postintegration task which produces an fNL amplitude
		template <typename number>
		class fNL_task: public postintegration_task<number>
			{

				// CONSTRUCTOR, DESTRUCTOR

		  public:

				//! construct an fNL task; this depends on output from a zeta_threepf_task
				fNL_task(const std::string& nm, const zeta_threepf_task<number>& t, derived_data::template_type ty=derived_data::fNL_local_template);

				//! deserialization constructor
				fNL_task(const std::string& nm, Json::Value& reader, typename repository_finder<number>::task_finder& finder);

				//! destructor is default
				virtual ~fNL_task() = default;


        // GET/SET TEMPLATE TYPE

      public:

        //! get current template setting
        derived_data::template_type get_template() const { return(this->type); }

        //! set template setting
        void set_template(derived_data::template_type t) { this->type = t; }


				// SERIALIZATION

		  public:

				virtual void serialize(Json::Value& writer) const override;


				// CLONE

		  public:

				virtual task<number>* clone() const override { return new fNL_task<number>(static_cast<const fNL_task<number>&>(*this)); }


				// INTERNAL DATA

		  protected:

				//! template type
				derived_data::template_type type;

			};


    template <typename number>
    fNL_task<number>::fNL_task(const std::string& nm, const zeta_threepf_task<number>& t, derived_data::template_type ty)
      : postintegration_task<number>(nm, t),
        type(ty)
      {
		    // ensure we are trying to construct this fNL task from an integrable threepf task
        if(!t.is_integrable())
	        {
            std::ostringstream msg;
            msg << __CPP_TRANSPORT_FNL_TASK_NOT_INTEGRABLE << " '" << t.get_name() << "'";
            throw runtime_exception(runtime_exception::DERIVED_PRODUCT_ERROR, msg.str());
	        }
      }


    template <typename number>
    fNL_task<number>::fNL_task(const std::string& nm, Json::Value& reader, typename repository_finder<number>::task_finder& finder)
      : postintegration_task<number>(nm, reader, finder)
      {
        std::string type_str = reader[__CPP_TRANSPORT_NODE_FNL_TASK_TEMPLATE].asString();

        if     (type_str == __CPP_TRANSPORT_NODE_FNL_TASK_TEMPLATE_LOCAL) type = derived_data::fNL_local_template;
        else if(type_str == __CPP_TRANSPORT_NODE_FNL_TASK_TEMPLATE_EQUI)  type = derived_data::fNL_equi_template;
        else if(type_str == __CPP_TRANSPORT_NODE_FNL_TASK_TEMPLATE_ORTHO) type = derived_data::fNL_ortho_template;
        else if(type_str == __CPP_TRANSPORT_NODE_FNL_TASK_TEMPLATE_DBI)   type = derived_data::fNL_DBI_template;
        else
          {
            std::ostringstream msg;
            msg << __CPP_TRANSPORT_FNL_TASK_UNKNOWN_TEMPLATE << " '" << type_str << "'";
            throw runtime_exception(runtime_exception::SERIALIZATION_ERROR, msg.str());
          }
      }


    template <typename number>
    void fNL_task<number>::serialize(Json::Value& writer) const
      {
        writer[__CPP_TRANSPORT_NODE_TASK_TYPE] = std::string(__CPP_TRANSPORT_NODE_TASK_TYPE_FNL);

        switch(this->type)
          {
            case derived_data::fNL_local_template:
              writer[__CPP_TRANSPORT_NODE_FNL_TASK_TEMPLATE] = std::string(__CPP_TRANSPORT_NODE_FNL_TASK_TEMPLATE_LOCAL);
              break;

            case derived_data::fNL_equi_template:
              writer[__CPP_TRANSPORT_NODE_FNL_TASK_TEMPLATE] = std::string(__CPP_TRANSPORT_NODE_FNL_TASK_TEMPLATE_EQUI);
              break;

            case derived_data::fNL_ortho_template:
              writer[__CPP_TRANSPORT_NODE_FNL_TASK_TEMPLATE] = std::string(__CPP_TRANSPORT_NODE_FNL_TASK_TEMPLATE_ORTHO);
              break;

            case derived_data::fNL_DBI_template:
              writer[__CPP_TRANSPORT_NODE_FNL_TASK_TEMPLATE] = std::string(__CPP_TRANSPORT_NODE_FNL_TASK_TEMPLATE_DBI);
              break;

            default:
              assert(false);
              throw runtime_exception(runtime_exception::RUNTIME_ERROR, __CPP_TRANSPORT_FNL_TASK_UNKNOWN_TEMPLATE);
          }

        this->postintegration_task<number>::serialize(writer);
      }


	}   // namespace transport


#endif //__zeta_tasks_H_
