//
// Created by David Seery on 22/12/2013.
// Copyright (c) 2013-15 University of Sussex. All rights reserved.
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

// forward-declare model class if needed
#include "transport-runtime-api/models/model_forward_declare.h"

// forward-declare initial conditions class
#include "transport-runtime-api/concepts/initial_conditions_forward_declare.h"


#define CPPTRANSPORT_NODE_ICS_VALUE         "value"
#define CPPTRANSPORT_NODE_ICS_MODEL_UID     "model-uid"
#define CPPTRANSPORT_NODE_ICS_NAME          "name"
#define CPPTRANSPORT_NODE_ICS_N_SUB_HORIZON "sub-horizon-efolds"
#define CPPTRANSPORT_NODE_ICS_N_INIT        "initial-time"
#define CPPTRANSPORT_NODE_ICS_VALUES        "values"
#define CPPTRANSPORT_NODE_ICS_PARAMETERS    "parameters"


namespace transport
  {

    template <typename number>
    class initial_conditions: public serializable
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! Construct named initial conditions from directly-supplied data.
        //! There are Npre e-folds of evolution prior to horizon exit for the conventionally-normalized
        //! mode with k=1, so N* (the horizon-crossing time for the k=1 mode) is equal to Ninit + Npre
        initial_conditions(const std::string& nm, model<number>* m, const parameters<number>& p, const std::vector<number>& i,
                           double Nini, double Npre);

        //! Construct anonymized initial conditions from directly-supplied data
        initial_conditions(model<number>* m, const parameters<number>& p, const std::vector<number>& i,
                           double Nini, double Npre)
          : initial_conditions(random_string(), m, p, i, Nini, Npre)
          {
          }

        //! Construct named initial conditions *offset* from directly-supplied data using a model.
        //! Ninit is the time at which intial conditions are set up
        //! Ncross is the time of horizon-crossing for the k=1 mode
        //! Npre is the desired number of e-folds of subhorizon evolution
        //! N* should equal Ncross
        initial_conditions(const std::string& nm, model<number>* m,
                           const parameters<number>& p, const std::vector<number>& i,
                           double Nini, double Ncross, double Npre);

        //! Construct anonymized initial conditions offset from directly-supplied data using a supplied model
        initial_conditions(model<number>* m,
                           const parameters<number>& p, const std::vector<number>& i,
                           double Nini, double Ncross, double Npre)
          : initial_conditions(random_string(), m, p, i, Nini, Ncross, Npre)
          {
          }

        //! Deserialization constructor
        initial_conditions(const std::string& nm, Json::Value& reader, typename instance_manager<number>::model_finder f);

        virtual ~initial_conditions() = default;


        // INTERFACE

      public:

        //! Return name of this 'package'
        const std::string& get_name() const { return(this->name); }

        //! Return parameters associated with these initial conditions
        const parameters<number>& get_params() const { return(this->params); }

        //! Return model associated with this package
        model<number>* get_model() const { return(this->mdl); }


		    // INITIAL CONDITIONS HANDLING

      public:

        //! Return std::vector of initial conditions
        const std::vector<number>& get_vector() const { return(this->ics); }

		    //! Return std::vector of initial conditions, offset by N e-folds from the raw ones
		    std::vector<number> get_offset_vector(double N) const;

        //! Return relative time of horizon-crossing
        double get_N_subhorion_efolds() const { return(this->N_sub_horizon); }


		    // SPECIAL TIMES

      public:

        //! Return initial time
        double get_N_initial() const { return(this->N_init); }

        //! Return horizon-crossing time for the k=1 mode
        double get_N_horizon_crossing() const { return(this->N_init + this->N_sub_horizon); }


        // SERIALIZATION -- implements a 'serializable' interface

      public:

        //! Serialize this object
        virtual void serialize(Json::Value& writer) const override;


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

        //! initial time
        double N_init;

        //! number of e-folds from initial conditions to horizon-crossing
        double N_sub_horizon;

      };


    template <typename number>
    initial_conditions<number>::initial_conditions(const std::string& nm, model<number>* m,
                                                   const parameters<number>& p, const std::vector<number>& i,
                                                   double Nini, double Npre)
      : name(nm), mdl(m), params(p), N_init(Nini), N_sub_horizon(Npre)
      {
		    assert(m != nullptr);
		    if(m == nullptr) throw runtime_exception(runtime_exception::RUNTIME_ERROR, CPPTRANSPORT_ICS_NULL_MODEL);

        // check model matches the one supplied with parameters
        if(m->get_identity_string() != p.get_model()->get_identity_string())
          {
            std::ostringstream msg;
            msg << CPPTRANSPORT_ICS_MODEL_MISMATCH << " '" << nm << "'";
            throw runtime_exception(runtime_exception::TASK_STRUCTURE_ERROR, msg.str());
          }

        // validate supplied initial conditions - we rely on the validator to throw
        // an exception if the supplied number of ics is incorrect

        mdl->validate_ics(params, i, ics);
      }


    template <typename number>
    initial_conditions<number>::initial_conditions(const std::string& nm, model<number>* m,
                                                   const parameters<number>& p, const std::vector<number>& i,
                                                   double Nini, double Ncross, double Npre)
      : name(nm), mdl(m), params(p), N_init(Ncross-Npre), N_sub_horizon(Npre)
      {
        assert(m != nullptr);
        if(m == nullptr) throw runtime_exception(runtime_exception::RUNTIME_ERROR, CPPTRANSPORT_ICS_NULL_MODEL);

        std::vector<number> validated_ics;

        // validate supplied initial conditions
        mdl->validate_ics(p, i, validated_ics);

        // call supplied finder function to correctly offset these ics
        mdl->offset_ics(p, validated_ics, ics, Nini, Ncross, Npre);
      }


    template <typename number>
    initial_conditions<number>::initial_conditions(const std::string& nm, Json::Value& reader,
                                                   typename instance_manager<number>::model_finder f)
      : name(nm), params(reader[CPPTRANSPORT_NODE_ICS_PARAMETERS], f)
      {
		    // construct model object
        std::string uid = reader[CPPTRANSPORT_NODE_ICS_MODEL_UID].asString();
		    mdl = f(uid);

        // deserialize time parameters
        N_init        = reader[CPPTRANSPORT_NODE_ICS_N_INIT].asDouble();
        N_sub_horizon = reader[CPPTRANSPORT_NODE_ICS_N_SUB_HORIZON].asDouble();

        // deserialize array of initial values
        Json::Value& ics_array = reader[CPPTRANSPORT_NODE_ICS_VALUES];
		    assert(ics_array.isArray());

        std::vector< named_list::element<number> > temp;
		    for(Json::Value::iterator t = ics_array.begin(); t != ics_array.end(); ++t)
          {
            std::string field_name = (*t)[CPPTRANSPORT_NODE_ICS_NAME].asString();
            double field_value = (*t)[CPPTRANSPORT_NODE_ICS_VALUE].asDouble();

            temp.push_back(named_list::element<number>(field_name, static_cast<number>(field_value)));
          }

		    // sort into order required by model object
        const std::vector<std::string>& field_ordering = mdl->get_state_names();

        if(temp.size() != field_ordering.size()) throw runtime_exception(runtime_exception::REPOSITORY_BACKEND_ERROR, CPPTRANSPORT_BADLY_FORMED_ICS);

        named_list::ordering order_map = named_list::make_ordering(field_ordering);
        named_list::comparator<number> cmp(order_map);
        std::sort(temp.begin(), temp.end(), cmp);

        std::vector<number> i;
        i.reserve(field_ordering.size());
        for(unsigned int j = 0; j < temp.size(); ++j)
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

				// N is the time at which we want to know the values of the fields
				// It is an absolute time

        // offset_ics() takes a triple Ninit, Ncross, Npre

        // Ninit specifies the initial time for our set of initial conditions. It is an absolute time.
        // Ncross specifies where horizon-crossing happens for the k=1 mode. It is an absolute time.
        // Npre specifies how many e-folds of subhorizon evolution we want, which can be negative if N falls later
        // than horizon crossing. This can happen for configurations with large enough k_t. It is measured relative to Ncross.

        // if we want initial conditions which start at time N, then we need Ncross-N efolds of subhorizon evolution

				double Ncross = this->N_init + this->N_sub_horizon;
				this->mdl->offset_ics(this->params, this->ics, offset_ics, this->N_init, Ncross, Ncross - N);

        return(offset_ics);
			}


    template <typename number>
    void initial_conditions<number>::serialize(Json::Value& writer) const
      {
		    // serialize model UID
		    writer[CPPTRANSPORT_NODE_ICS_MODEL_UID] = this->mdl->get_identity_string();

        // serialize time parameters
        writer[CPPTRANSPORT_NODE_ICS_N_SUB_HORIZON] = this->N_sub_horizon;
        writer[CPPTRANSPORT_NODE_ICS_N_INIT]        = this->N_init;

        // serialize array of initial values
        Json::Value ics(Json::arrayValue);

		    const std::vector<std::string>& names = this->mdl->get_state_names();
		    assert(names.size() == this->ics.size());

        for(unsigned int i = 0; i < this->ics.size(); ++i)
          {
            Json::Value ics_element(Json::objectValue);
		        ics_element[CPPTRANSPORT_NODE_ICS_NAME] = names[i];
		        ics_element[CPPTRANSPORT_NODE_ICS_VALUE] = static_cast<double>(this->ics[i]);
		        ics.append(ics_element);
          }
		    writer[CPPTRANSPORT_NODE_ICS_VALUES] = ics;

        Json::Value params_serialize(Json::objectValue);
        this->params.serialize(params_serialize);
        writer[CPPTRANSPORT_NODE_ICS_PARAMETERS] = params_serialize;
      }


    template <typename number>
    std::ostream& operator<<(std::ostream& out, const initial_conditions<number>& obj)
      {
        out << obj.params << std::endl;

		    const std::vector<std::string>& names = obj.mdl->get_state_names();
        assert(obj.ics.size() == names.size());

        out << CPPTRANSPORT_ICS_TAG << std::endl;
        for(unsigned int i = 0; i < obj.ics.size(); ++i)
          {
            out << "  " << names[i] << " = " << obj.ics[i] << std::endl;
          }

        return(out);
      }

  }


#endif //__initial_conditions_H_
