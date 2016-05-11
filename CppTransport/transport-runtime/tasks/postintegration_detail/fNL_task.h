//
// Created by David Seery on 15/04/15.
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


#ifndef CPPTRANSPORT_FNL_TASK_H
#define CPPTRANSPORT_FNL_TASK_H


#include "transport-runtime/tasks/postintegration_detail/common.h"
#include "transport-runtime/tasks/postintegration_detail/abstract.h"
#include "transport-runtime/tasks/postintegration_detail/zeta_twopf_db_task.h"
#include "transport-runtime/tasks/postintegration_detail/zeta_threepf_task.h"


#define CPPTRANSPORT_NODE_FNL_TASK_TEMPLATE               "template"
#define CPPTRANSPORT_NODE_FNL_TASK_TEMPLATE_LOCAL         "local"
#define CPPTRANSPORT_NODE_FNL_TASK_TEMPLATE_EQUI          "equilateral"
#define CPPTRANSPORT_NODE_FNL_TASK_TEMPLATE_ORTHO         "orthogonal"
#define CPPTRANSPORT_NODE_FNL_TASK_TEMPLATE_DBI           "DBI"


namespace transport
	{

    // FNL TASK

    //! An 'fNL_task' is a postintegration task which produces an fNL amplitude
    template <typename number=default_number_type>
    class fNL_task: public postintegration_task<number>
	    {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! construct an fNL task; this depends on output from a zeta_threepf_task
        fNL_task(const std::string& nm, const zeta_threepf_task<number>& t, derived_data::bispectrum_template ty=derived_data::bispectrum_template::local);

        //! deserialization constructor
        fNL_task(const std::string& nm, Json::Value& reader, task_finder<number>& finder);

        //! destructor is default
        virtual ~fNL_task() = default;


        // INTERFACE

      public:

        //! supply 'derivable_task' interface
        task_type get_type() const override final { return task_type::postintegration; }

        //! respond to task type query
        postintegration_task_type get_task_type() const override final { return postintegration_task_type::fNL; }


        // GET/SET TEMPLATE TYPE

      public:

        //! get current template setting
        derived_data::bispectrum_template get_template() const { return(this->type); }

        //! set template setting
        void set_template(derived_data::bispectrum_template t) { this->type = t; }


        // SERIALIZATION

      public:

        virtual void serialize(Json::Value& writer) const override;


        // CLONE

      public:

        virtual fNL_task<number>* clone() const override { return new fNL_task<number>(static_cast<const fNL_task<number>&>(*this)); }


        // INTERNAL DATA

      protected:

        //! template type
        derived_data::bispectrum_template type;

	    };


    template <typename number>
    struct postintegration_task_traits<number, postintegration_task_type::fNL>
      {
        typedef fNL_task<number> task_type;
      };


    template <typename number>
    fNL_task<number>::fNL_task(const std::string& nm, const zeta_threepf_task<number>& t, derived_data::bispectrum_template ty)
	    : postintegration_task<number>(nm, t),
	      type(ty)
	    {
        // ensure we are trying to construct this fNL task from an integrable threepf task
        if(!t.is_integrable())
	        {
            std::ostringstream msg;
            msg << CPPTRANSPORT_FNL_TASK_NOT_INTEGRABLE << " '" << t.get_name() << "'";
            throw runtime_exception(exception_type::DERIVED_PRODUCT_ERROR, msg.str());
	        }
	    }


    template <typename number>
    fNL_task<number>::fNL_task(const std::string& nm, Json::Value& reader, task_finder<number>& finder)
	    : postintegration_task<number>(nm, reader, finder)
	    {
        std::string type_str = reader[CPPTRANSPORT_NODE_FNL_TASK_TEMPLATE].asString();

        if     (type_str == CPPTRANSPORT_NODE_FNL_TASK_TEMPLATE_LOCAL) type = derived_data::bispectrum_template::local;
        else if(type_str == CPPTRANSPORT_NODE_FNL_TASK_TEMPLATE_EQUI)  type = derived_data::bispectrum_template::equilateral;
        else if(type_str == CPPTRANSPORT_NODE_FNL_TASK_TEMPLATE_ORTHO) type = derived_data::bispectrum_template::orthogonal;
        else if(type_str == CPPTRANSPORT_NODE_FNL_TASK_TEMPLATE_DBI)   type = derived_data::bispectrum_template::DBI;
        else
	        {
            std::ostringstream msg;
            msg << CPPTRANSPORT_FNL_TASK_UNKNOWN_TEMPLATE << " '" << type_str << "'";
            throw runtime_exception(exception_type::SERIALIZATION_ERROR, msg.str());
	        }
	    }


    template <typename number>
    void fNL_task<number>::serialize(Json::Value& writer) const
	    {
        writer[CPPTRANSPORT_NODE_TASK_TYPE] = std::string(CPPTRANSPORT_NODE_TASK_TYPE_FNL);

        switch(this->type)
	        {
            case derived_data::bispectrum_template::local:
	            writer[CPPTRANSPORT_NODE_FNL_TASK_TEMPLATE] = std::string(CPPTRANSPORT_NODE_FNL_TASK_TEMPLATE_LOCAL);
            break;

            case derived_data::bispectrum_template::equilateral:
	            writer[CPPTRANSPORT_NODE_FNL_TASK_TEMPLATE] = std::string(CPPTRANSPORT_NODE_FNL_TASK_TEMPLATE_EQUI);
            break;

            case derived_data::bispectrum_template::orthogonal:
	            writer[CPPTRANSPORT_NODE_FNL_TASK_TEMPLATE] = std::string(CPPTRANSPORT_NODE_FNL_TASK_TEMPLATE_ORTHO);
            break;

            case derived_data::bispectrum_template::DBI:
	            writer[CPPTRANSPORT_NODE_FNL_TASK_TEMPLATE] = std::string(CPPTRANSPORT_NODE_FNL_TASK_TEMPLATE_DBI);
            break;

            default:
	            assert(false);
            throw runtime_exception(exception_type::RUNTIME_ERROR, CPPTRANSPORT_FNL_TASK_UNKNOWN_TEMPLATE);
	        }

        this->postintegration_task<number>::serialize(writer);
	    }

	}   // namespace transport


#endif //CPPTRANSPORT_FNL_TASK_H
