//
// Created by David Seery on 22/12/2013.
// Copyright (c) 2013 University of Sussex. All rights reserved.
//


#ifndef __initial_conditions_H_
#define __initial_conditions_H_


#include <assert.h>
#include <vector>
#include <functional>
#include <stdexcept>

#include "transport-runtime-api/concepts/parameters.h"
#include "transport-runtime-api/serialization/serializable.h"
#include "transport-runtime-api/messages.h"

#include "transport-runtime-api/utilities/named_list.h"
#include "transport-runtime-api/utilities/random_string.h"


#define __CPP_TRANSPORT_NODE_ICS_VALUE     "value"
#define __CPP_TRANSPORT_NODE_ICS_MODEL_UID "ics-model-uid"
#define __CPP_TRANSPORT_NODE_ICS_NAME      "name"
#define __CPP_TRANSPORT_NODE_ICS_NSTAR     "ics-nstar"
#define __CPP_TRANSPORT_NODE_ICS_VALUES    "ics-values"


namespace transport
  {

		// forward-declare model class
		template <typename number> class model;

    template <typename number> class initial_conditions;

    template <typename number>
    std::ostream& operator<<(std::ostream& out, const initial_conditions<number>& obj);

    template <typename number>
    class initial_conditions: public serializable
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! Construct named initial conditions from directly-supplied data
        initial_conditions(const std::string& nm, model<number>* m, const parameters<number>& p, const std::vector<number>& i, double Npre);

        //! Construct anonymized initial conditions from directly-supplied data
        initial_conditions(model<number>* m, const parameters<number>& p, const std::vector<number>& i, double Npre)
          : initial_conditions(random_string(), m, p, i, Npre)
          {
          }

        //! Construct named initial conditions offset from directly-supplied data using a supplied model
        initial_conditions(const std::string& nm, model<number>* m,
                           const parameters<number>& p, const std::vector<number>& i,
                           double Ninit, double Ncross, double Npre);

        //! Construct anonymized initial conditions offset from directly-supplied data using a supplied model
        initial_conditions(model<number>* m,
                           const parameters<number>& p, const std::vector<number>& i,
                           double Ninit, double Ncross, double Npre)
          : initial_conditions(random_string(), m, p, i, Ninit, Ncross, Npre)
          {
          }

        //! Deserialization constructor
        initial_conditions(const std::string& nm, serialization_reader* reader, typename instance_manager<number>::model_finder f);

        virtual ~initial_conditions() = default;


        // INTERFACE

      public:

        //! Return parameters associated with these initial conditions
        const parameters<number>& get_params() const { return(this->params); }

        //! Return model associated with this package
        model<number>* get_model() const { return(this->mdl); }

        //! Return std::vector of initial conditions
        const std::vector<number>& get_vector() const { return(this->ics); }

		    //! Return std::vector of initial conditions, offset by N e-folds from the raw ones
		    std::vector<number> get_offset_vector(double N) const;

        //! Return relative time of horizon-crossing
        double get_Nstar() const { return(this->Nstar); }

        //! Return name of this 'package'
        const std::string& get_name() const { return(this->name); }


        // SERIALIZATION -- implements a 'serializable' interface

      public:

        //! Serialize this object
        virtual void serialize(serialization_writer& writer) const override;


        // WRITE TO A STREAM

      public:

        //! Write ourselves to a stream
        friend std::ostream& operator<< <>(std::ostream& out, const initial_conditions<number>& obj);


        // INTERNAL DATA

      protected:

        //! name of this ics/params package, used for tagging when deposited in a repository
        std::string name;

		    //! model object associated with this package
		    model<number>* mdl;

        //! copy of parameters
        parameters<number> params;

        //! values of fields and their derivatives, constituting the initial conditions
        std::vector<number> ics;

        //! number of e-folds from initial conditions to horizon-crossing
        double Nstar;

      };


    template <typename number>
    initial_conditions<number>::initial_conditions(const std::string& nm, model<number>* m,
                                                   const parameters<number>& p, const std::vector<number>& i, double Npre)
      : name(nm), mdl(m), params(p), Nstar(Npre)
      {
		    assert(m != nullptr);
		    if(m == nullptr) throw runtime_exception(runtime_exception::RUNTIME_ERROR, __CPP_TRANSPORT_ICS_NULL_MODEL);

        // check model matches the one supplied with parameters
        if(m->get_identity_string() != p.get_model()->get_identity_string())
          {
            std::ostringstream msg;
            msg << __CPP_TRANSPORT_ICS_MODEL_MISMATCH << " '" << nm << "'";
            throw runtime_exception(runtime_exception::TASK_STRUCTURE_ERROR, msg.str());
          }

        // validate supplied initial conditions - we rely on the validator to throw
        // an exception if the supplied number of ics is incorrect

        mdl->validate_ics(params, i, ics);
      }


    template <typename number>
    initial_conditions<number>::initial_conditions(const std::string& nm, model<number>* m,
                                                   const parameters<number>& p, const std::vector<number>& i,
                                                   double Ninit, double Ncross, double Npre)
      : name(nm), mdl(m), params(p), Nstar(Npre)
      {
        assert(m != nullptr);
        if(m == nullptr) throw runtime_exception(runtime_exception::RUNTIME_ERROR, __CPP_TRANSPORT_ICS_NULL_MODEL);

        std::vector<number> validated_ics;

        // validate supplied initial conditions
        mdl->validate_ics(p, i, validated_ics);

        // call supplied finder function to correctly offset these ics
        mdl->offset_ics(p, validated_ics, ics, Ninit, Ncross, Npre);
      }


    template <typename number>
    initial_conditions<number>::initial_conditions(const std::string& nm, serialization_reader* reader,
                                                   typename instance_manager<number>::model_finder f)
      : name(nm), params(reader, f)
      {
        assert(reader != nullptr);
        if(reader == nullptr) throw runtime_exception(runtime_exception::SERIALIZATION_ERROR, __CPP_TRANSPORT_ICS_NULL_SERIALIZATION_READER);

		    // construct model object
        std::string uid;
		    reader->read_value(__CPP_TRANSPORT_NODE_ICS_MODEL_UID, uid);
		    mdl = f(uid);

        // deserialize N*
        reader->read_value(__CPP_TRANSPORT_NODE_ICS_NSTAR, Nstar);

        // deserialize array of initial values
        unsigned int fields = reader->start_array(__CPP_TRANSPORT_NODE_ICS_VALUES);
        std::vector< named_list::element<number> > temp;
        for(unsigned int i = 0; i < fields; i++)
          {
            reader->start_array_element();

            std::string field_name;
            reader->read_value(__CPP_TRANSPORT_NODE_ICS_NAME, field_name);

            double field_value;
            reader->read_value(__CPP_TRANSPORT_NODE_ICS_VALUE, field_value);

            temp.push_back(named_list::element<number>(field_name, static_cast<number>(field_value)));

            reader->end_array_element();
          }
        reader->end_element(__CPP_TRANSPORT_NODE_ICS_VALUES);

		    // sort into order required by model object
        const std::vector<std::string>& field_ordering = mdl->get_state_names();

        if(temp.size() != field_ordering.size()) throw runtime_exception(runtime_exception::REPOSITORY_BACKEND_ERROR, __CPP_TRANSPORT_BADLY_FORMED_ICS);

        named_list::ordering order_map = named_list::make_ordering(field_ordering);
        named_list::comparator<number> cmp(order_map);
        std::sort(temp.begin(), temp.end(), cmp);

        std::vector<number> i;
        i.reserve(field_ordering.size());
        for(unsigned int j = 0; j < temp.size(); j++)
          {
            i.push_back((temp[j]).get_value());
          }

        // validate supplied initial conditions
		    mdl->validate_ics(params, i, ics);
      }


		template <typename number>
		std::vector<number> initial_conditions<number>::get_offset_vector(double N) const
			{
		    std::vector<number> offset_ics;

        // offset_ics() changes the initial conditions to give horizon-crossing at Nstar, with Npre e-folds before horizon crossing.
        // we want there to be Nstar-N efolds to horizon crossing. That can be negative if N > Nstar
				this->mdl->offset_ics(this->params, this->ics, offset_ics, 0.0, this->Nstar, this->Nstar - N);

        return(offset_ics);
			}


    template <typename number>
    void initial_conditions<number>::serialize(serialization_writer& writer) const
      {
		    // serialize model UID
		    writer.write_value(__CPP_TRANSPORT_NODE_ICS_MODEL_UID, this->mdl->get_identity_string());

        // serialize N*
        writer.write_value(__CPP_TRANSPORT_NODE_ICS_NSTAR, this->Nstar);

        // serialize array of initial values
		    const std::vector<std::string>& names = this->mdl->get_state_names();
		    assert(names.size() == this->ics.size());

        writer.start_array(__CPP_TRANSPORT_NODE_ICS_VALUES, this->ics.size()==0);
        for(unsigned int i = 0; i < this->ics.size(); i++)
          {
            writer.start_node("arrayelt", false);   // node name is ignored in arrays
		        writer.write_value(__CPP_TRANSPORT_NODE_ICS_NAME, names[i]);
		        writer.write_value(__CPP_TRANSPORT_NODE_ICS_VALUE, this->ics[i]);
            writer.end_element("arrayelt");
          }
        writer.end_element(__CPP_TRANSPORT_NODE_ICS_VALUES);

        // serialize parameter values
        this->params.serialize(writer);
      }


    template <typename number>
    std::ostream& operator<<(std::ostream& out, const initial_conditions<number>& obj)
      {
        out << obj.params << std::endl;

		    const std::vector<std::string>& names = obj.mdl->get_state_names();
        assert(obj.ics.size() == names.size());

        out << __CPP_TRANSPORT_ICS_TAG << std::endl;
        for(unsigned int i = 0; i < obj.ics.size(); i++)
          {
            out << "  " << names[i] << " = " << obj.ics[i] << std::endl;
          }

        return(out);
      }

  }


#endif //__initial_conditions_H_
