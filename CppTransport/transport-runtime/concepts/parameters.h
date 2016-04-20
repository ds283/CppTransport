//
// Created by David Seery on 28/12/2013.
// Copyright (c) 2013-2016 University of Sussex. All rights reserved.
//


#ifndef __parameters_H_
#define __parameters_H_


#include <assert.h>
#include <vector>
#include <functional>
#include <stdexcept>

#include "transport-runtime/serialization/serializable.h"
#include "transport-runtime/exceptions.h"
#include "transport-runtime/messages.h"

#include "transport-runtime/utilities/named_list.h"

// forward-declare model class if needed
#include "transport-runtime/models/model_forward_declare.h"

#include "boost/lexical_cast.hpp"
#include "boost/log/utility/formatting_ostream.hpp"


#define CPPTRANSPORT_NODE_PARAMS_MPLANCK   "mplanck"
#define CPPTRANSPORT_NODE_PARAMS_VALUES    "values"
#define CPPTRANSPORT_NODE_PARAMS_VALUE     "value"
#define CPPTRANSPORT_NODE_PARAMS_NAME      "name"


namespace transport
  {

    template <typename number>
    class parameters: public serializable
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! Construct 'parameter' object from an explicit model and parameter combination
        parameters(number Mp, const std::vector<number>& p, model<number>* m);

        //! Convenience constructor which accepts a shared_ptr<> to a model instance, but doesn't actually use this
        //! to manage the lifetime; we work with raw pointers
        parameters(number Mp, const std::vector<number>& p, std::shared_ptr< model<number> > m)
          : parameters(Mp, p, m.get())
        {
        }

        //! Deserialization constructor
        parameters(Json::Value& reader, model<number>* m);

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


        // WRITE SELF TO STREAM

      public:

        //! write details
        template <typename Stream> void write(Stream& out) const;


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
      : M_Planck(Mp),
        mdl(m)
      {
		    assert(m != nullptr);

				if(m == nullptr) throw runtime_exception(exception_type::RUNTIME_ERROR, CPPTRANSPORT_PARAMS_NULL_MODEL);
        if(M_Planck <= 0.0) throw runtime_exception(exception_type::RUNTIME_ERROR, CPPTRANSPORT_MPLANCK_NEGATIVE);

        // validate supplied parameters
		    mdl->validate_params(p, params);
      }


    template <typename number>
    parameters<number>::parameters(Json::Value& reader, model<number>* m)
      {
		    mdl = m;

        // deserialize value of Planck mass
		    M_Planck = static_cast<number>(reader[CPPTRANSPORT_NODE_PARAMS_MPLANCK].asDouble());

        // deserialize array of parameter values
        Json::Value& param_array = reader[CPPTRANSPORT_NODE_PARAMS_VALUES];
		    assert(param_array.isArray());

        std::vector< named_list::element<number> > temp;
		    for(Json::Value::iterator t = param_array.begin(); t != param_array.end(); ++t)
          {
            std::string param_name = (*t)[CPPTRANSPORT_NODE_PARAMS_NAME].asString();
            double param_value = (*t)[CPPTRANSPORT_NODE_PARAMS_VALUE].asDouble();

            temp.push_back(named_list::element<number>(param_name, static_cast<number>(param_value)));
          }

		    const std::vector<std::string>& order = mdl->get_param_names();
        if(temp.size() != order.size()) throw runtime_exception(exception_type::REPOSITORY_BACKEND_ERROR, CPPTRANSPORT_BADLY_FORMED_PARAMS);

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
        // serialize value of Planck mass
        writer[CPPTRANSPORT_NODE_PARAMS_MPLANCK] = static_cast<double>(this->M_Planck);

        // serialize array of parameter values
		    const std::vector<std::string>& names = this->mdl->get_param_names();
		    assert(names.size() == this->params.size());

        if(this->params.size() == names.size())
          {
            Json::Value param_array(Json::arrayValue);

            for(unsigned int i = 0; i < this->params.size(); ++i)
              {
                Json::Value param_element(Json::objectValue);
		            param_element[CPPTRANSPORT_NODE_PARAMS_NAME] = names[i];
		            param_element[CPPTRANSPORT_NODE_PARAMS_VALUE] = static_cast<double>(this->params[i]);
		            param_array.append(param_element);
              }
		        writer[CPPTRANSPORT_NODE_PARAMS_VALUES] = param_array;
          }
        else throw std::out_of_range(CPPTRANSPORT_PARAM_DATA_MISMATCH);
      }


    template <typename number>
    template <typename Stream>
    void parameters<number>::write(Stream& out) const
      {
        out << CPPTRANSPORT_PARAMS_TAG << '\n';
        out << "  " << CPPTRANSPORT_MPLANCK_TAG << this->get_Mp() << '\n';

        const std::vector<std::string>& names = this->get_model()->get_param_names();
        const std::vector<number>& params = this->get_vector();
        assert(names.size() == params.size());

        for(unsigned int i = 0; i < params.size(); ++i)
          {
            out << "  " << names[i] << " = " << params[i] << '\n';
          }
      }


    template <typename number, typename Char, typename Traits>
    std::basic_ostream<Char, Traits>& operator<<(std::basic_ostream<Char, Traits>& out, const parameters<number>& obj)
      {
        obj.write(out);
        return(out);
      }


    template <typename number, typename Char, typename Traits, typename Allocator>
    boost::log::basic_formatting_ostream<Char, Traits, Allocator>& operator<<(boost::log::basic_formatting_ostream<Char, Traits, Allocator>& out, const parameters<number>& obj)
      {
        obj.write(out);
        return(out);
      }

  }



#endif //__parameters_H_
