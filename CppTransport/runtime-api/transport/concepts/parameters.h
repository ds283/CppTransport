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

#include "transport/db-xml/xml_serializable.h"
#include "transport/messages_en.h"


#define __CPP_TRANSPORT_NODE_PARAMETERS "parameters"
#define __CPP_TRANSPORT_NODE_MPLANCK    "mplanck"
#define __CPP_TRANSPORT_NODE_PRM_VALUES "values"
#define __CPP_TRANSPORT_NODE_PARAMETER  "parameter"
#define __CPP_TRANSPORT_ATTR_NAME       "name"


namespace transport
  {

    template <typename number, unsigned int N>
    class parameters: public flattener<N>, public xml_serializable;

    template <typename number, unsigned int N>
    std::ostream& operator<<(std::ostream& out, parameters<number, N>& obj);

    template <typename number, unsigned int N>
    class parameters: public xml_serializable
      {
      public:
        typedef std::function<void(const std::vector<number>&, std::vector<number>&)> params_validator;

        // construct parameter set
        parameters(number Mp, const std::vector<number>& p, const std::vector<std::string>& n, params_validator v);

        // return parameter vector
        const std::vector<number>& get_params() const { return(this->params); }

        // return M_Planck
        number get_Mp() const { return(this->M_Planck); }

        // XML SERIALIZATION INTERFACE

      public:
        void serialize_xml(DbXml::XmlEventWriter& writer);

      public:
        friend std::ostream& operator<< <>(std::ostream& out, parameters<number, N>& obj);

        // INTERNAL DATA

      protected:
        std::vector<number> params;     // values of parameters
        std::vector<std::string> names; // names of parameters

        number M_Planck;                // value of M_Planck, which sets the scale for all our units
      };


    template <typename number, unsigned int N>
    parameters<number, N>::parameters(number Mp, const std::vector<number>& p, const std::vector<std::string>& n, params_validator v)
      : M_Planck(Mp), names(n)
      {
        assert(p.size() == n.size());
        assert(p.size() == N);
        assert(n.size() == N);

        if(M_Planck <= 0.0)
          {
            throw std::invalid_argument(__CPP_TRANSPORT_MPLANCK_NEGATIVE);
          }

        if(p.size() == n.size())
          {
            // validate supplied parameters
            v(p, params);
          }
        else
          {
            throw std::invalid_argument(__CPP_TRANSPORT_PARAMS_MISMATCH);
          }
      }


    template <typename number, unsigned int N>
    void parameters<number, N>::serialize_xml(DbXml::XmlEventWriter& writer)
      {
        assert(this->params.size() == N);
        assert(this->names.size() == N);

        this->begin_node(writer, __CPP_TRANSPORT_NODE_PARAMETERS);
        this->write_value_node(writer, __CPP_TRANSPORT_NODE_MPLANCK, this->M_Planck);
        this->begin_node(writer, __CPP_TRANSPORT_NODE_PRM_VALUES);

        for(unsigned int i = 0; i < N; i++)
          {
            this->write_value_node(writer, __CPP_TRANSPORT_NODE_PARAMETER, this->params[i], __CPP_TRANSPORT_ATTR_NAME, this->names[i]);
          }

        this->end_node(writer, __CPP_TRANSPORT_NODE_PRM_VALUES);
        this->end_node(writer, __CPP_TRANSPORT_NODE_PARAMETERS);
      }

  }



#endif //__parameters_H_
