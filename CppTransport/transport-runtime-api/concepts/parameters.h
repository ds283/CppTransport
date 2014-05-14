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


#define __CPP_TRANSPORT_NODE_MPLANCK    "mplanck"
#define __CPP_TRANSPORT_NODE_PRM_VALUES "values"
#define __CPP_TRANSPORT_NODE_PARAMETER  "parameter"
#define __CPP_TRANSPORT_ATTR_NAME       "name"


namespace transport
  {


    template <typename number> class parameters;

    template <typename number>
    std::ostream& operator<<(std::ostream& out, const parameters<number>& obj);

    template <typename number>
    class parameters: public serializable
      {
      public:
        typedef std::function<void(const std::vector<number>&, std::vector<number>&)> params_validator;

        // CONSTRUCTOR, DESTRUCTOR

        //! Construct 'parameter' object from explicit
        parameters(number Mp, const std::vector<number>& p, const std::vector<std::string>& n, params_validator v);

        //! Return std::vector of parameters
        const std::vector<number>& get_vector() const { return(this->params); }

        //! Return value of M_Planck
        number get_Mp() const { return(this->M_Planck); }

        // SERIALIZATION INTERFACE

      public:
        virtual void serialize(serialization_writer& writer) const override;

      public:
        friend std::ostream& operator<< <>(std::ostream& out, const parameters<number>& obj);

        // INTERNAL DATA

      protected:
        //! std::vector representing values of parameters
        std::vector<number> params;
        //! std::vector representing names of parameters
        std::vector<std::string> names;

        //! Value of M_Planck, which sets the scale for all units
        number M_Planck;
      };


    template <typename number>
    parameters<number>::parameters(number Mp, const std::vector<number>& p, const std::vector<std::string>& n, params_validator v)
      : M_Planck(Mp), names(n)
      {
        assert(p.size() == n.size());

        if(M_Planck <= 0.0) throw std::invalid_argument(__CPP_TRANSPORT_MPLANCK_NEGATIVE);

        // validate supplied parameters
        if(p.size() == n.size()) v(p, params);
        else throw std::invalid_argument(__CPP_TRANSPORT_PARAMS_MISMATCH);
      }


    template <typename number>
    void parameters<number>::serialize(serialization_writer& writer) const
      {
        assert(this->params.size() == this->names.size());

        this->write_value_node(writer, __CPP_TRANSPORT_NODE_MPLANCK, this->M_Planck);

        this->begin_array(writer, __CPP_TRANSPORT_NODE_PRM_VALUES, false);

        if(this->params.size() == this->names.size())
          {
            for(unsigned int i = 0; i < this->params.size(); i++)
              {
                this->write_value_node(writer, __CPP_TRANSPORT_NODE_PARAMETER, this->params[i], __CPP_TRANSPORT_ATTR_NAME, this->names[i]);
              }
          }
        else
          {
            throw std::out_of_range(__CPP_TRANSPORT_PARAM_DATA_MISMATCH);
          }

        this->end_element(writer, __CPP_TRANSPORT_NODE_PRM_VALUES);
      }


		namespace
			{

				namespace parameters_helper
					{

						template <typename number>
						transport::parameters<number> deserialize(serialization_reader* reader,
						                                          const std::vector<std::string>& ordering,
						                                          typename transport::parameters<number>::params_validator p_validator)
							{
								double MPlanck;
								reader->read_value(__CPP_TRANSPORT_NODE_MPLANCK, MPlanck);

								unsigned int parameters = reader->start_array(__CPP_TRANSPORT_NODE_PRM_VALUES);

						    std::vector< named_list::element<number> > temp;
								for(unsigned int i = 0; i < parameters; i++)
									{
										reader->start_array_element();

								    std::string param_name;
										reader->read_attribute(__CPP_TRANSPORT_ATTR_NAME, param_name);

										double param_value;
										reader->read_value(__CPP_TRANSPORT_NODE_PARAMETER, param_value);

										temp.push_back(named_list::element<number>(param_name, static_cast<number>(param_value)));

										reader->end_array_element();
									}

								reader->end_element(__CPP_TRANSPORT_NODE_PRM_VALUES);

						    if(temp.size() != ordering.size()) throw runtime_exception(runtime_exception::REPOSITORY_BACKEND_ERROR, __CPP_TRANSPORT_BADLY_FORMED_PARAMS);

						    named_list::ordering order_map = named_list::make_ordering(ordering);
						    named_list::comparator<number> cmp(order_map);
						    std::sort(temp.begin(), temp.end(), cmp);

						    std::vector<number> p;
						    std::vector<std::string> n;
						    for(unsigned int i = 0; i < temp.size(); i++)
							    {
						        p.push_back((temp[i]).get_value());
						        n.push_back((temp[i]).get_name());
							    }

								return(transport::parameters<number>(static_cast<number>(MPlanck), p, n, p_validator));
							}

					}   // namespace parameters

			}   // unnamed namespacae


    template <typename number>
    std::ostream& operator<<(std::ostream& out, const parameters<number>& obj)
      {
        out << __CPP_TRANSPORT_PARAMS_TAG << std::endl;
        out << "  " << __CPP_TRANSPORT_MPLANCK_TAG << obj.M_Planck << std::endl;

        assert(obj.params.size() == obj.names.size());

        for(unsigned int i = 0; i < obj.params.size(); i++)
          {
            out << "  " << obj.names[i] << " = " << obj.params[i] << std::endl;
          }

        return(out);
      }

  }



#endif //__parameters_H_
