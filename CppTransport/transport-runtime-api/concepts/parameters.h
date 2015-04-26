//
// Created by David Seery on 28/12/2013.
// Copyright (c) 2013-15 University of Sussex. All rights reserved.
//


#ifndef __parameters_H_
#define __parameters_H_


#include <assert.h>
#include <vector>
#include <functional>
#include <stdexcept>

#include "transport-runtime-api/serialization/serializable.h"
#include "transport-runtime-api/exceptions.h"
#include "transport-runtime-api/messages.h"

#include "transport-runtime-api/utilities/named_list.h"

// forward-declare model class if needed
#include "transport-runtime-api/models/model_forward_declare.h"

#include "boost/lexical_cast.hpp"


#define __CPP_TRANSPORT_NODE_PARAMS_MPLANCK   "mplanck"
#define __CPP_TRANSPORT_NODE_PARAMS_VALUES    "values"
#define __CPP_TRANSPORT_NODE_PARAMS_VALUE     "value"
#define __CPP_TRANSPORT_NODE_PARAMS_NAME      "name"
#define __CPP_TRANSPORT_NODE_PARAMS_MODEL_UID "model-uid"


namespace transport
  {

    template <typename number> class parameters;

    template <typename number>
    std::ostream& operator<<(std::ostream& out, const parameters<number>& obj);

    template <typename number>
    class parameters: public serializable
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! Construct 'parameter' object from explicit
        parameters(number Mp, const std::vector<number>& p, model<number>* m);

        //! Deserialization constructor
        parameters(Json::Value& reader, typename instance_manager<number>::model_finder f);

        virtual ~parameters() = default;


        // INTERFACE

      public:

        //! Return std::vector of parameters
        const std::vector<number>& get_vector() const { return(this->params); }

        //! Return value of M_Planck
        number get_Mp() const { return(this->M_Planck); }

        //! Get model
        model<number>* get_model() const { return(this->mdl); }


        // SERIALIZATION -- implements a 'serializable' interface

      public:

        //! Serialize this object
        virtual void serialize(Json::Value& writer) const override;


        // WRITE TO A STREAM

      public:

        //! Write self to output stream
        friend std::ostream& operator<< <>(std::ostream& out, const parameters<number>& obj);


        // INTERNAL DATA

      protected:

        //! std::vector representing values of parameters
        std::vector<number> params;

		    //! model object associated with these parameters
		    model<number>* mdl;

        //! Value of M_Planck, which sets the scale for all units
        number M_Planck;

      };


    template <typename number>
    parameters<number>::parameters(number Mp, const std::vector<number>& p, model<number>* m)
      : M_Planck(Mp), mdl(m)
      {
		    assert(m != nullptr);

				if(m == nullptr) throw runtime_exception(runtime_exception::RUNTIME_ERROR, __CPP_TRANSPORT_PARAMS_NULL_MODEL);
        if(M_Planck <= 0.0) throw runtime_exception(runtime_exception::RUNTIME_ERROR, __CPP_TRANSPORT_MPLANCK_NEGATIVE);

        // validate supplied parameters
		    mdl->validate_params(p, params);
      }


    template <typename number>
    parameters<number>::parameters(Json::Value& reader, typename instance_manager<number>::model_finder f)
      {
		    // construct model object
        std::string uid = reader[__CPP_TRANSPORT_NODE_PARAMS_MODEL_UID].asString();
		    mdl = f(uid);

        // deserialize value of Planck mass
		    M_Planck = static_cast<number>(reader[__CPP_TRANSPORT_NODE_PARAMS_MPLANCK].asDouble());

        // deserialize array of parameter values
        Json::Value& param_array = reader[__CPP_TRANSPORT_NODE_PARAMS_VALUES];
		    assert(param_array.isArray());

        std::vector< named_list::element<number> > temp;
		    for(Json::Value::iterator t = param_array.begin(); t != param_array.end(); ++t)
          {
            std::string param_name = (*t)[__CPP_TRANSPORT_NODE_PARAMS_NAME].asString();
            double param_value = (*t)[__CPP_TRANSPORT_NODE_PARAMS_VALUE].asDouble();

            temp.push_back(named_list::element<number>(param_name, static_cast<number>(param_value)));
          }

		    const std::vector<std::string>& order = mdl->get_param_names();
        if(temp.size() != order.size()) throw runtime_exception(runtime_exception::REPOSITORY_BACKEND_ERROR, __CPP_TRANSPORT_BADLY_FORMED_PARAMS);

        named_list::ordering order_map = named_list::make_ordering(order);
        named_list::comparator<number> cmp(order_map);
        std::sort(temp.begin(), temp.end(), cmp);

        std::vector<number> p;
        for(unsigned int i = 0; i < temp.size(); ++i)
          {
            p.push_back((temp[i]).get_value());
          }

        // validate supplied parameters
		    mdl->validate_params(p, params);
      }


    template <typename number>
    void parameters<number>::serialize(Json::Value& writer) const
      {
		    // serialize model UID
		    writer[__CPP_TRANSPORT_NODE_PARAMS_MODEL_UID] = this->mdl->get_identity_string();

        // serialize value of Planck mass
        writer[__CPP_TRANSPORT_NODE_PARAMS_MPLANCK] = this->M_Planck;

        // serialize array of parameter values
		    const std::vector<std::string>& names = this->mdl->get_param_names();
		    assert(names.size() == this->params.size());

        if(this->params.size() == names.size())
          {
            Json::Value param_array(Json::arrayValue);

            for(unsigned int i = 0; i < this->params.size(); ++i)
              {
                Json::Value param_element(Json::objectValue);
		            param_element[__CPP_TRANSPORT_NODE_PARAMS_NAME] = names[i];
		            param_element[__CPP_TRANSPORT_NODE_PARAMS_VALUE] = this->params[i];
		            param_array.append(param_element);
              }
		        writer[__CPP_TRANSPORT_NODE_PARAMS_VALUES] = param_array;
          }
        else throw std::out_of_range(__CPP_TRANSPORT_PARAM_DATA_MISMATCH);
      }


    template <typename number>
    std::ostream& operator<<(std::ostream& out, const parameters<number>& obj)
      {
        out << __CPP_TRANSPORT_PARAMS_TAG << std::endl;
        out << "  " << __CPP_TRANSPORT_MPLANCK_TAG << obj.M_Planck << std::endl;

		    const std::vector<std::string>& names = obj.mdl->get_param_names();
        assert(obj.params.size() == names.size());

        for(unsigned int i = 0; i < obj.params.size(); ++i)
          {
            out << "  " << names[i] << " = " << obj.params[i] << std::endl;
          }

        return(out);
      }

  }



#endif //__parameters_H_
