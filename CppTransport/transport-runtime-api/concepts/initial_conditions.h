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


#define __CPP_TRANSPORT_NODE_INITIAL_CONDITIONS "initial-conditions"
#define __CPP_TRANSPORT_NODE_COORDINATE         "coordinate"
#define __CPP_TRANSPORT_ATTR_NAME               "name"
#define __CPP_TRANSPORT_NODE_NSTAR              "nstar"
#define __CPP_TRANSPORT_NODE_ICS_VALUES         "values"
#define __CPP_TRANSPORT_NODE_PARAM_BLOCK        "parameter-block"


namespace transport
  {

    template <typename number> class initial_conditions;

    template <typename number>
    std::ostream& operator<<(std::ostream& out, const initial_conditions<number>& obj);

    template <typename number>
    class initial_conditions: public serializable
      {
      public:
        typedef std::function<void(const parameters<number>&, const std::vector<number>&, std::vector<number>&)> ics_validator;

        typedef std::function<void(const parameters<number>&, const std::vector<number>&, std::vector<number>&, double, double, unsigned int)> ics_finder;

        //! Construct named initial conditions from directly-supplied data
        initial_conditions(const std::string& nm, const parameters<number>& p,
                           const std::vector<number>& i, const std::vector<std::string>& n,
                           double Npre, ics_validator v);

        //! Construct anonymized initial conditions from directly-supplied data
        initial_conditions(const parameters<number>& p,
                           const std::vector<number>& i, const std::vector<std::string>& n,
                           double Npre, ics_validator v)
          : initial_conditions(random_string(), p, i, n, Npre, v)
          {
          }

        //! Construct named initial conditions offset from directly-supplied data using a supplied model
        initial_conditions(const std::string& nm, const parameters<number>& p,
                           const std::vector<number>& i, const std::vector<std::string>& n,
                           double Ninit, double Ncross, double Npre,
                           ics_validator v, ics_finder f);

        //! Construct anonymized initial conditions offset from directly-supplied data using a supplied model
        initial_conditions(const parameters<number>& p,
                           const std::vector<number>& i, const std::vector<std::string>& n,
                           double Ninit, double Ncross, double Npre,
                           ics_validator v, ics_finder f)
          : initial_conditions(random_string(), p, i, n, Ninit, Ncross, Npre, v, f)
          {
          }

        //! Return parameters associated with these initial conditions
        const parameters<number>& get_params() const { return(this->params); }

        //! Return std::vector of initial conditions
        const std::vector<number>& get_vector() const { return(this->ics); }

        //! Return relative time of horizon-crossing
        const double get_Nstar() const { return(this->Nstar); }

        //! Return name
        const std::string& get_name() const { return(this->name); }

        // SERIALIZATION INTERFACE

      public:
        void serialize(serialization_writer& writer) const;

      public:
        friend std::ostream& operator<< <>(std::ostream& out, const initial_conditions<number>& obj);

      protected:
        //! name of this ics/params combination, used for tagging with deposited in a repository
        const std::string name;

        //! copy of parameters
        const parameters<number> params;

        //! values of fields and their derivatives
        std::vector<number> ics;
        //! names of fields and their derivatives
        std::vector<std::string> names;

        //! number of e-folds from initial conditions to horizon-crossing
        double Nstar;
      };


    template <typename number>
    initial_conditions<number>::initial_conditions(const std::string& nm, const parameters<number>& p,
                                                   const std::vector<number>& i, const std::vector<std::string>& n,
                                                   double Npre, ics_validator v)
      : name(nm), params(p), Nstar(Npre), names(n)
      {
        // validate supplied initial conditions - we rely on the validator to throw
        // an exception if the supplied number of ics is incorrect
        v(params, i, ics);
      }


    template <typename number>
    initial_conditions<number>::initial_conditions(const std::string& nm, const parameters<number>& p,
                                                   const std::vector<number>& i, const std::vector<std::string>& n,
                                                   double Ninit, double Ncross, double Npre,
                                                   ics_validator v, ics_finder f)
      : name(nm), params(p), Nstar(Npre), names(n)
      {
        std::vector<number> validated_ics;

        // validate supplied initial conditions
        v(p, i, validated_ics);

        // call supplied finder function to correctly offset these ics
        f(p, validated_ics, ics, Ninit, Ncross, Npre);
      }


    template <typename number>
    void initial_conditions<number>::serialize(serialization_writer& writer) const
      {
        this->write_value_node(writer, __CPP_TRANSPORT_NODE_NSTAR, this->Nstar);

        this->begin_array(writer, __CPP_TRANSPORT_NODE_ICS_VALUES, false);

        for(unsigned int i = 0; i < this->ics.size(); i++)
          {
            this->write_value_node(writer, __CPP_TRANSPORT_NODE_COORDINATE, this->ics[i], __CPP_TRANSPORT_ATTR_NAME, this->names[i]);
          }

        this->end_element(writer, __CPP_TRANSPORT_NODE_ICS_VALUES);

		    this->begin_node(writer, __CPP_TRANSPORT_NODE_PARAM_BLOCK, false);
        this->params.serialize(writer);
		    this->end_element(writer, __CPP_TRANSPORT_NODE_PARAM_BLOCK);
      }


		namespace
			{

		    namespace initial_conditions
			    {

				    //! Deserialize an initial_conditions object
		        template <typename number>
		        initial_conditions<number> deserialize(serialization_reader* reader, const std::string& package_name,
		                                               const std::vector<std::string>& parameter_ordering,
		                                               const std::vector<std::string>& field_ordering,
		                                               typename parameters<number>::params_validator p_validator,
		                                               typename initial_conditions<number>::ics_validator ics_validator)
			        {
						    unsigned int Nstar;
						    reader->read_value(__CPP_TRANSPORT_NODE_NSTAR, Nstar);

						    reader->start_node(__CPP_TRANSPORT_NODE_PARAM_BLOCK);
						    reader->push_bookmark();
						    parameters<number> params = parameters::deserialize(reader, parameter_ordering, p_validator);
						    reader->pop_bookmark();
						    reader->end_element(__CPP_TRANSPORT_NODE_PARAM_BLOCK);

						    unsigned int fields = reader->start_array(__CPP_TRANSPORT_NODE_INITIAL_CONDITIONS);

				        std::vector< named_list::element<number> > temp;
				        for(unsigned int i = 0; i < fields; i++)
							    {
								    reader->start_node(__CPP_TRANSPORT_NODE_COORDINATE);

						        std::string field_name;
								    reader->read_attribute(__CPP_TRANSPORT_ATTR_NAME, field_name);

										double field_value;
								    reader->read_value(__CPP_TRANSPORT_NODE_COORDINATE, field_value);

				            temp.push_back(named_list::element<number>(field_name, static_cast<number>(field_value)));

								    reader->end_element(__CPP_TRANSPORT_NODE_PARAMETER);
							    }

						    reader->end_element(__CPP_TRANSPORT_NODE_INITIAL_CONDITIONS);

				        if(temp.size() != field_ordering.size()) throw runtime_exception(runtime_exception::REPOSITORY_BACKEND_ERROR, __CPP_TRANSPORT_BADLY_FORMED_ICS);

				        named_list::ordering order_map = named_list::make_ordering(field_ordering);
				        named_list::comparator<number> cmp(order_map);
				        std::sort(temp.begin(), temp.end(), cmp);

				        std::vector<number> c;
				        std::vector<std::string> n;
				        for(unsigned int i = 0; i < temp.size(); i++)
					        {
				            c.push_back((temp[i]).get_value());
				            n.push_back((temp[i]).get_name());
					        }

						    return(initial_conditions<number>(package_name, params, c, n, Nstar, ics_validator));
			        }

			    }   // namespace initial_conditions

			}   // unnamed namespace



    template <typename number>
    std::ostream& operator<<(std::ostream& out, const initial_conditions<number>& obj)
      {
        out << obj.params << std::endl;

        assert(obj.ics.size() == obj.names.size());

        out << __CPP_TRANSPORT_ICS_TAG << std::endl;
        for(unsigned int i = 0; i < obj.ics.size(); i++)
          {
            out << "  " << obj.names[i] << " = " << obj.ics[i] << std::endl;
          }

        return(out);
      }

  }


#endif //__initial_conditions_H_
