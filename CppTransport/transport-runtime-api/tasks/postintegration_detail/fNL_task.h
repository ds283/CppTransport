//
// Created by David Seery on 15/04/15.
// Copyright (c) 2015 University of Sussex. All rights reserved.
//


#ifndef __fNL_task_H_
#define __fNL_task_H_


#include "transport-runtime-api/tasks/postintegration_detail/common.h"
#include "transport-runtime-api/tasks/postintegration_detail/abstract.h"
#include "transport-runtime-api/tasks/postintegration_detail/zeta_twopf_list_task.h"
#include "transport-runtime-api/tasks/postintegration_detail/zeta_threepf_task.h"


#define CPPTRANSPORT_NODE_FNL_TASK_TEMPLATE               "template"
#define CPPTRANSPORT_NODE_FNL_TASK_TEMPLATE_LOCAL         "local"
#define CPPTRANSPORT_NODE_FNL_TASK_TEMPLATE_EQUI          "equilateral"
#define CPPTRANSPORT_NODE_FNL_TASK_TEMPLATE_ORTHO         "orthogonal"
#define CPPTRANSPORT_NODE_FNL_TASK_TEMPLATE_DBI           "DBI"


namespace transport
	{

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

        virtual fNL_task<number>* clone() const override { return new fNL_task<number>(static_cast<const fNL_task<number>&>(*this)); }


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
            msg << CPPTRANSPORT_FNL_TASK_NOT_INTEGRABLE << " '" << t.get_name() << "'";
            throw runtime_exception(runtime_exception::DERIVED_PRODUCT_ERROR, msg.str());
	        }
	    }


    template <typename number>
    fNL_task<number>::fNL_task(const std::string& nm, Json::Value& reader, typename repository_finder<number>::task_finder& finder)
	    : postintegration_task<number>(nm, reader, finder)
	    {
        std::string type_str = reader[CPPTRANSPORT_NODE_FNL_TASK_TEMPLATE].asString();

        if     (type_str == CPPTRANSPORT_NODE_FNL_TASK_TEMPLATE_LOCAL) type = derived_data::fNL_local_template;
        else if(type_str == CPPTRANSPORT_NODE_FNL_TASK_TEMPLATE_EQUI)  type = derived_data::fNL_equi_template;
        else if(type_str == CPPTRANSPORT_NODE_FNL_TASK_TEMPLATE_ORTHO) type = derived_data::fNL_ortho_template;
        else if(type_str == CPPTRANSPORT_NODE_FNL_TASK_TEMPLATE_DBI)   type = derived_data::fNL_DBI_template;
        else
	        {
            std::ostringstream msg;
            msg << CPPTRANSPORT_FNL_TASK_UNKNOWN_TEMPLATE << " '" << type_str << "'";
            throw runtime_exception(runtime_exception::SERIALIZATION_ERROR, msg.str());
	        }
	    }


    template <typename number>
    void fNL_task<number>::serialize(Json::Value& writer) const
	    {
        writer[CPPTRANSPORT_NODE_TASK_TYPE] = std::string(CPPTRANSPORT_NODE_TASK_TYPE_FNL);

        switch(this->type)
	        {
            case derived_data::fNL_local_template:
	            writer[CPPTRANSPORT_NODE_FNL_TASK_TEMPLATE] = std::string(CPPTRANSPORT_NODE_FNL_TASK_TEMPLATE_LOCAL);
            break;

            case derived_data::fNL_equi_template:
	            writer[CPPTRANSPORT_NODE_FNL_TASK_TEMPLATE] = std::string(CPPTRANSPORT_NODE_FNL_TASK_TEMPLATE_EQUI);
            break;

            case derived_data::fNL_ortho_template:
	            writer[CPPTRANSPORT_NODE_FNL_TASK_TEMPLATE] = std::string(CPPTRANSPORT_NODE_FNL_TASK_TEMPLATE_ORTHO);
            break;

            case derived_data::fNL_DBI_template:
	            writer[CPPTRANSPORT_NODE_FNL_TASK_TEMPLATE] = std::string(CPPTRANSPORT_NODE_FNL_TASK_TEMPLATE_DBI);
            break;

            default:
	            assert(false);
            throw runtime_exception(runtime_exception::RUNTIME_ERROR, CPPTRANSPORT_FNL_TASK_UNKNOWN_TEMPLATE);
	        }

        this->postintegration_task<number>::serialize(writer);
	    }

	}   // namespace transport


#endif //__fNL_task_H_
