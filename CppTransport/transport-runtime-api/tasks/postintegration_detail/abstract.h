//
// Created by David Seery on 15/04/15.
// Copyright (c) 2015 University of Sussex. All rights reserved.
//


#ifndef __postintegration_abstract_task_H_
#define __postintegration_abstract_task_H_


#include "transport-runtime-api/tasks/postintegration_detail/common.h"


#define __CPP_TRANSPORT_NODE_POSTINTEGRATION_TASK_PARENT     "parent-task"
#define __CPP_TRANSPORT_NODE_POSTINTEGRATION_TASK_PAIRED     "paired"


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

        ptk = dynamic_cast< derivable_task<number>* >(record->get_abstract_task()->clone());
        if(ptk == nullptr)
	        {
            std::stringstream msg;
            msg << __CPP_TRANSPORT_REPO_ZETA_TASK_NOT_DERIVABLE << " '" << tk_name << "'";
            throw runtime_exception(runtime_exception::REPOSITORY_ERROR, msg.str());
	        }
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

	}   // namespace transport


#endif //__postintegration_abstract_task_H_
