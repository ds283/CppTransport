//
// Created by David Seery on 28/12/2013.
// Copyright (c) 2013 University of Sussex. All rights reserved.
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

#include "boost/lexical_cast.hpp"
#include "model.h"


#define __CPP_TRANSPORT_NODE_PARAMS_MPLANCK   "param-mplanck"
#define __CPP_TRANSPORT_NODE_PARAMS_VALUES    "param-values"
#define __CPP_TRANSPORT_NODE_PARAMS_VALUE     "value"
#define __CPP_TRANSPORT_NODE_PARAMS_NAME      "name"
#define __CPP_TRANSPORT_NODE_PARAMS_MODEL_UID "param-model-uid"


namespace transport
  {

		// forward-declare model class
		template <typename number> class model;

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
        parameters(serialization_reader* reader, typename instance_manager<number>::model_finder f);

        virtual ~parameters() = default;


        // INTERFACE

      public:

        //! Return std::vector of parameters
        const std::vector<number>& get_vector() const { return(this->params); }

        //! Return value of M_Planck
        number get_Mp() const { return(this->M_Planck); }

        //! Get model
        model<number>* get_model() const { return(this->model); }


        // SERIALIZATION -- implements a 'serializable' interface

      public:

        //! Serialize this object
        virtual void serialize(serialization_writer& writer) const override;


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
    parameters<number>::parameters(serialization_reader* reader, typename instance_manager<number>::model_finder f)
      {
        assert(reader != nullptr);
        if(reader == nullptr) throw runtime_exception(runtime_exception::SERIALIZATION_ERROR, __CPP_TRANSPORT_PARAMS_NULL_SERIALIZATION_READER);

		    // construct model object
        std::string uid;
		    reader->read_value(__CPP_TRANSPORT_NODE_PARAMS_MODEL_UID, uid);
		    mdl = f(uid);

        // deserialize value of Planck mass
        reader->read_value(__CPP_TRANSPORT_NODE_PARAMS_MPLANCK, M_Planck);

        // deserialize array of parameter values
        unsigned int parameters = reader->start_array(__CPP_TRANSPORT_NODE_PARAMS_VALUES);
        std::vector< named_list::element<number> > temp;
        for(unsigned int i = 0; i < parameters; i++)
          {
            reader->start_array_element();

            std::string param_name;
            reader->read_value(__CPP_TRANSPORT_NODE_PARAMS_NAME, param_name);

            double param_value;
            reader->read_value(__CPP_TRANSPORT_NODE_PARAMS_VALUE, param_value);

            temp.push_back(named_list::element<number>(param_name, static_cast<number>(param_value)));

            reader->end_array_element();
          }
        reader->end_element(__CPP_TRANSPORT_NODE_PARAMS_VALUES);

		    const std::vector<std::string>& order = mdl->get_param_names();
        if(temp.size() != order.size()) throw runtime_exception(runtime_exception::REPOSITORY_BACKEND_ERROR, __CPP_TRANSPORT_BADLY_FORMED_PARAMS);

        named_list::ordering order_map = named_list::make_ordering(order);
        named_list::comparator<number> cmp(order_map);
        std::sort(temp.begin(), temp.end(), cmp);

        std::vector<number> p;
        for(unsigned int i = 0; i < temp.size(); i++)
          {
            p.push_back((temp[i]).get_value());
          }

        // validate supplied parameters
		    mdl->validate_params(p, params);
      }


    template <typename number>
    void parameters<number>::serialize(serialization_writer& writer) const
      {
		    // serialize model UID
		    writer.write_value(__CPP_TRANSPORT_NODE_PARAMS_MODEL_UID, this->mdl->get_identity_string());

        // serialize value of Planck mass
        writer.write_value(__CPP_TRANSPORT_NODE_PARAMS_MPLANCK, this->M_Planck);

        // serialize array of parameter values
		    const std::vector<std::string>& names = this->mdl->get_param_names();
		    assert(names.size() == this->params.size());

        writer.start_array(__CPP_TRANSPORT_NODE_PARAMS_VALUES, this->params.size()==0);
        if(this->params.size() == names.size())
          {
            for(unsigned int i = 0; i < this->params.size(); i++)
              {
		            writer.write_value(__CPP_TRANSPORT_NODE_PARAMS_NAME, names[i]);
		            writer.write_value(__CPP_TRANSPORT_NODE_PARAMS_VALUE, this->params[i]);
              }
          }
        else throw std::out_of_range(__CPP_TRANSPORT_PARAM_DATA_MISMATCH);
        writer.end_element(__CPP_TRANSPORT_NODE_PARAMS_VALUES);
      }


    template <typename number>
    std::ostream& operator<<(std::ostream& out, const parameters<number>& obj)
      {
        out << __CPP_TRANSPORT_PARAMS_TAG << std::endl;
        out << "  " << __CPP_TRANSPORT_MPLANCK_TAG << obj.M_Planck << std::endl;

		    const std::vector<std::string>& names = obj.mdl->get_param_names();
        assert(obj.params.size() == names.size());

        for(unsigned int i = 0; i < obj.params.size(); i++)
          {
            out << "  " << names[i] << " = " << obj.params[i] << std::endl;
          }

        return(out);
      }

  }



#endif //__parameters_H_
