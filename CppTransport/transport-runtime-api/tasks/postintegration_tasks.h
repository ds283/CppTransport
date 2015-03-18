//
// Created by David Seery on 04/07/2014.
// Copyright (c) 2014-15 University of Sussex. All rights reserved.
//


#ifndef __zeta_tasks_H_
#define __zeta_tasks_H_


#include "transport-runtime-api/tasks/task.h"
#include "transport-runtime-api/tasks/integration_tasks.h"
#include "transport-runtime-api/tasks/task_configurations.h"

#include "transport-runtime-api/derived-products/template_types.h"

#include "transport-runtime-api/messages.h"
#include "transport-runtime-api/defaults.h"


#define __CPP_TRANSPORT_NODE_POSTINTEGRATION_TASK_PARENT     "parent-task"
#define __CPP_TRANSPORT_NODE_POSTINTEGRATION_TASK_WRITE_BACK "write-back"

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
		class postintegration_task: public task<number>
			{

				// CONSTRUCTOR, DESTRUCTOR

		  public:

				//! Construct a named postintegration_task with supplied parent integration_task
				postintegration_task(const std::string& nm, const integration_task<number>& t, bool write=false);

				//! deserialization constructor
				postintegration_task(const std::string& nm, serialization_reader* reader, typename repository<number>::task_finder& finder);

				//! override copy constructor to perform a deep copy
				postintegration_task(const postintegration_task<number>& obj);

				//! destroy a postintegration_task
				virtual ~postintegration_task();


        // INTERFACE

      public:

        //! Get parent integration task
        integration_task<number>* get_parent_task() const { return(this->tk); }

				//! Set whether to write data back to parent
				bool get_write_back() const { return(this->write_back); }

				//! Set whether to write data back to parent
				void set_write_back(bool write) { this->write_back = write; }


				// SERIALIZATION (implements a 'serializable' interface)

		  public:

				//! serialize this object
				virtual void serialize(serialization_writer& writer) const override;


        // WRITE TO STREAM

      public:

        //! write to stream
        void write(std::ostream& out) const;


				// INTERNAL DATA

		  protected:

				//! Parent integration task
				integration_task<number>* tk;

				//! Write data back to parent?
				bool write_back;

			};


    template <typename number>
    std::ostream& operator<<(std::ostream& out, const postintegration_task<number>& obj)
      {
        obj.write(out);
        return(out);
      }


		template <typename number>
		postintegration_task<number>::postintegration_task(const std::string& nm, const integration_task<number>& t, bool write)
			: task<number>(nm),
        tk(dynamic_cast<integration_task<number>*>(t.clone())),
				write_back(write)
			{
				assert(tk != nullptr);
			}


		template <typename number>
		postintegration_task<number>::postintegration_task(const postintegration_task<number>& obj)
			: task<number>(obj),
			  tk(dynamic_cast<integration_task<number>*>(obj.tk->clone())),
				write_back(obj.write_back)
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

				// deserialize write-back status
				reader->read_value(__CPP_TRANSPORT_NODE_POSTINTEGRATION_TASK_WRITE_BACK, write_back);

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

				// serialize write-back status
				writer.write_value(__CPP_TRANSPORT_NODE_POSTINTEGRATION_TASK_WRITE_BACK, this->write_back);
			}


    template <typename number>
    void postintegration_task<number>::write(std::ostream& out) const
      {
        out << __CPP_TRANSPORT_PARENT_TASK << ": '" << this->tk->get_name() << "'" << std::endl;
      }


		// ZETA TWOPF TASK

		//! A 'zeta_twopf_task' task is a postintegration task which produces the zeta two-point function
		template <typename number>
		class zeta_twopf_task: public postintegration_task<number>
			{

				// CONSTRUCTOR, DESTRUCTOR

		  public:

				//! construct a zeta_twopf task
				zeta_twopf_task(const std::string& nm, const twopf_task<number>& t, bool w=false);

				//! deserialization constructor
				zeta_twopf_task(const std::string& nm, serialization_reader* reader, typename repository<number>::task_finder& finder);

				//! destructor is default
				virtual ~zeta_twopf_task() = default;


				// SERIALIZATION

		  public:

				virtual void serialize(serialization_writer& writer) const override;


				// CLONE

		  public:

				virtual task<number>* clone() const override { return new zeta_twopf_task<number>(static_cast<const zeta_twopf_task<number>&>(*this)); }

			};


    template <typename number>
    zeta_twopf_task<number>::zeta_twopf_task(const std::string& nm, const twopf_task<number>& t, bool w)
      : postintegration_task<number>(nm, t, w)
      {
      }


    template <typename number>
    zeta_twopf_task<number>::zeta_twopf_task(const std::string& nm, serialization_reader* reader, typename repository<number>::task_finder& finder)
      : postintegration_task<number>(nm, reader, finder)
      {
      }


    template <typename number>
    void zeta_twopf_task<number>::serialize(serialization_writer& writer) const
      {
        writer.write_value(__CPP_TRANSPORT_NODE_TASK_TYPE, std::string(__CPP_TRANSPORT_NODE_TASK_TYPE_ZETA_TWOPF));

        this->postintegration_task<number>::serialize(writer);
      }


		// ZETA THREEPF TASK

		//! A 'zeta_threepf_task' task is a postintegration task which produces the zeta three-point
		//! function and associated derived quantities (the reduced bispectrum at the moment)
		template <typename number>
		class zeta_threepf_task: public postintegration_task<number>
			{

				// CONSTRUCTOR, DESTRUCTOR

		  public:

				//! construct a zeta_threepf task
				zeta_threepf_task(const std::string& nm, const threepf_task<number>& t, bool w=false);

				//! deserialization constructor
				zeta_threepf_task(const std::string& nm, serialization_reader* reader, typename repository<number>::task_finder& finder);

				//! destructor is default
				virtual ~zeta_threepf_task() = default;


				// SERIALIZATION

		  public:

				virtual void serialize(serialization_writer& writer) const override;


				// CLONE

		  public:

				virtual task<number>* clone() const override { return new zeta_threepf_task<number>(static_cast<const zeta_threepf_task<number>&>(*this)); }

			};


    template <typename number>
    zeta_threepf_task<number>::zeta_threepf_task(const std::string& nm, const threepf_task<number>& t, bool w)
      : postintegration_task<number>(nm, t, w)
      {
      }


    template <typename number>
    zeta_threepf_task<number>::zeta_threepf_task(const std::string& nm, serialization_reader* reader, typename repository<number>::task_finder& finder)
      : postintegration_task<number>(nm, reader, finder)
      {
      }


    template <typename number>
    void zeta_threepf_task<number>::serialize(serialization_writer& writer) const
      {
        writer.write_value(__CPP_TRANSPORT_NODE_TASK_TYPE, std::string(__CPP_TRANSPORT_NODE_TASK_TYPE_ZETA_THREEPF));

        this->postintegration_task<number>::serialize(writer);
      }


		// FNL TASK

		//! An 'fNL_task' is a postintegration task which produces an fNL amplitude
		template <typename number>
		class fNL_task: public postintegration_task<number>
			{

				// CONSTRUCTOR, DESTRUCTOR

		  public:

				//! construct an fNL task
				fNL_task(const std::string& nm, const threepf_task<number>& t, derived_data::template_type ty=derived_data::fNLlocal, bool w=false);

				//! deserialization constructor
				fNL_task(const std::string& nm, serialization_reader* reader, typename repository<number>::task_finder& finder);

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

				virtual void serialize(serialization_writer& writer) const override;


				// CLONE

		  public:

				virtual task<number>* clone() const override { return new fNL_task<number>(static_cast<const fNL_task<number>&>(*this)); }


				// INTERNAL DATA

		  protected:

				derived_data::template_type type;

			};


    template <typename number>
    fNL_task<number>::fNL_task(const std::string& nm, const threepf_task<number>& t, derived_data::template_type ty, bool w)
      : postintegration_task<number>(nm, t, w),
        type(ty)
      {
      }


    template <typename number>
    fNL_task<number>::fNL_task(const std::string& nm, serialization_reader* reader, typename repository<number>::task_finder& finder)
      : postintegration_task<number>(nm, reader, finder)
      {
        assert(reader != nullptr);
        if(reader == nullptr) throw runtime_exception(runtime_exception::RUNTIME_ERROR, __CPP_TRANSPORT_TASK_NULL_SERIALIZATION_READER);

        std::string type_str;
        reader->read_value(__CPP_TRANSPORT_NODE_FNL_TASK_TEMPLATE, type_str);

        if     (type_str == __CPP_TRANSPORT_NODE_FNL_TASK_TEMPLATE_LOCAL) type = derived_data::fNLlocal;
        else if(type_str == __CPP_TRANSPORT_NODE_FNL_TASK_TEMPLATE_EQUI)  type = derived_data::fNLequi;
        else if(type_str == __CPP_TRANSPORT_NODE_FNL_TASK_TEMPLATE_ORTHO) type = derived_data::fNLortho;
        else if(type_str == __CPP_TRANSPORT_NODE_FNL_TASK_TEMPLATE_DBI)   type = derived_data::fNLDBI;
        else
          {
            std::ostringstream msg;
            msg << __CPP_TRANSPORT_FNL_TASK_UNKNOWN_TEMPLATE << " '" << type_str << "'";
            throw runtime_exception(runtime_exception::SERIALIZATION_ERROR, msg.str());
          }
      }


    template <typename number>
    void fNL_task<number>::serialize(serialization_writer& writer) const
      {
        writer.write_value(__CPP_TRANSPORT_NODE_TASK_TYPE, std::string(__CPP_TRANSPORT_NODE_TASK_TYPE_FNL));

        switch(this->type)
          {
            case derived_data::fNLlocal:
              writer.write_value(__CPP_TRANSPORT_NODE_FNL_TASK_TEMPLATE, std::string(__CPP_TRANSPORT_NODE_FNL_TASK_TEMPLATE_LOCAL));
              break;

            case derived_data::fNLequi:
              writer.write_value(__CPP_TRANSPORT_NODE_FNL_TASK_TEMPLATE, std::string(__CPP_TRANSPORT_NODE_FNL_TASK_TEMPLATE_EQUI));
              break;

            case derived_data::fNLortho:
              writer.write_value(__CPP_TRANSPORT_NODE_FNL_TASK_TEMPLATE, std::string(__CPP_TRANSPORT_NODE_FNL_TASK_TEMPLATE_ORTHO));
              break;

            case derived_data::fNLDBI:
              writer.write_value(__CPP_TRANSPORT_NODE_FNL_TASK_TEMPLATE, std::string(__CPP_TRANSPORT_NODE_FNL_TASK_TEMPLATE_DBI));
              break;

            default:
              assert(false);
              throw runtime_exception(runtime_exception::RUNTIME_ERROR, __CPP_TRANSPORT_FNL_TASK_UNKNOWN_TEMPLATE);
          }

        this->postintegration_task<number>::serialize(writer);
      }


	}   // namespace transport


#endif //__zeta_tasks_H_
