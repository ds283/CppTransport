//
// Created by David Seery on 24/07/2013.
// Copyright (c) 2013 University of Sussex. All rights reserved.
//
// To change the template use AppCode | Preferences | File Templates.
//

#ifndef __CPP_TRANSPORT_INDEX_SELECTOR_H_
#define __CPP_TRANSPORT_INDEX_SELECTOR_H_


#include <array>
#include <vector>

#include <assert.h>

#include "transport-runtime-api/serialization/serializable.h"


#define __CPP_TRANSPORT_NODE_INDEX_RANGE       "index-range"
#define __CPP_TRANSPORT_NODE_INDEX_RANGE_ALL   "all"
#define __CPP_TRANSPORT_NODE_INDEX_RANGE_FIELD "field"
#define __CPP_TRANSPORT_NODE_INDEX_TOGGLES     "enabled-indices"
#define __CPP_TRANSPORT_NODE_INDEX_TOGGLE      "enabled"


namespace transport
  {

		//! Select active indices for a d-component object in an N_f-field model
    template <unsigned int indices>
    class index_selector: public serializable
      {

      public:

		    //! validator object.
		    //! used to confirm that a particular index_selector is compatible with a particular model.
		    typedef std::function<bool(const index_selector<indices>&)> validator;

		    typedef enum { field_range, all_range } range_type;

      public:

        index_selector(unsigned int N_f, range_type r=all_range);
		    ~index_selector() = default;

		    //! Disable all indices
        void none   ();
				//! Activate all indices
        void all    ();
		    //! Set a specific index combination on
        void set_on (std::array<unsigned int, indices>& which);
		    //! Set a specific index combination off
        void set_off(std::array<unsigned int, indices>& which);
		    //! Check whether a specific combination is active
        bool is_on  (std::array<unsigned int, indices>& which);

		    //! Get range -- do the indices cover fields, or field+momenta?
        range_type get_range() const { return(this->range); }
		    //! Get number of fields
        unsigned int get_number_fields() const { return(this->N_fields); }


		    // SERIALIZATION

      public:

		    virtual void serialize(serialization_writer& writer) const override;


		    //! Write self to standard output
		    void write(std::ostream& out, const std::vector<std::string>& state_names);

		    // INTERNAL DATA

      protected:

		    //! Number of fields
        const unsigned int        N_fields;
		    //! Range -- do the indices cover fields only, or fields+momenta?
        const range_type          range;

		    //! Total number of on/of toggles
        unsigned int              size;           // total number of components
		    //! Vector representing the state of each on/off toggle
        std::vector<bool>         enabled;
		    //! Look-up table of displacements into the vector of on/off toggles
        std::vector<unsigned int> displacements;
      };


    template <unsigned int indices>
    index_selector<indices>::index_selector(unsigned int N_f, range_type r)
      : N_fields(N_f), range(r)
      {
        // work out how many components this object has
        size = 1;
		    unsigned int scale_factor = (r == all_range ? 2 : 1);

        for(int i = 0; i < indices; i++)
          {
            size *= scale_factor * N_fields;
          }
        enabled.assign(size, true);   // by default, enable all components

        displacements.resize(indices);
        unsigned int count = 1;
        for(int i = 0; i < indices; i++)
          {
            displacements[indices-i-1] = count;
            count *= scale_factor * this->N_fields;
          }
      }


    template <unsigned int indices>
    void index_selector<indices>::none()
      {
        this->enabled.assign(size, false);
      }


    template <unsigned int indices>
    void index_selector<indices>::all()
      {
        this->enabled.assign(size, true);
      }


    template <unsigned int indices>
    void index_selector<indices>::set_on(std::array<unsigned int, indices>& which)
      {
        unsigned int index = 0;

        for(unsigned int i = 0; i < indices; i++)
	        {
            assert(which[i] < (this->range == all_range ? 2 : 1) * this->N_fields);  // basic sanity check: TODO: add error handling

            index += this->displacements[i] * which[i];
	        }

        assert(index < this->size);
        this->enabled[index] = true;
      }


    template <unsigned int indices>
    void index_selector<indices>::set_off(std::array<unsigned int, indices>& which)
      {
        unsigned int index = 0;

        for(unsigned int i = 0; i < indices; i++)
	        {
            assert(which[i] < (this->range == all_range ? 2 : 1) * this->N_fields);  // basic sanity check: TODO: add error handling

            index += this->displacements[i] * which[i];
	        }

        assert(index < this->size);
        this->enabled[index] = false;
      }


    template <unsigned int indices>
    bool index_selector<indices>::is_on(std::array<unsigned int, indices>& which)
	    {
        unsigned int index = 0;

        for(unsigned int i = 0; i < indices; i++)
	        {
            assert(which[i] < (this->range == all_range ? 2 : 1) * this->N_fields); // basic sanity check: TODO: add error handling

            index += this->displacements[i] * which[i];
	        }

        assert(index < this->size);
        return (this->enabled[index]);
	    }


		template <unsigned int indices>
		void index_selector<indices>::serialize(serialization_writer& writer) const
			{
				this->write_value_node(writer, __CPP_TRANSPORT_NODE_INDEX_RANGE,
				                       this->range == all_range ? __CPP_TRANSPORT_NODE_INDEX_RANGE_ALL : __CPP_TRANSPORT_NODE_INDEX_RANGE_FIELD);

				this->begin_array(writer, __CPP_TRANSPORT_NODE_INDEX_TOGGLES, this->size == 0);
				for(unsigned int i = 0; i < this->size; i++)
					{
						this->begin_node(writer, "arrayelt", false);   // node names are ignored for arrays
						this->write_value_node(writer, __CPP_TRANSPORT_NODE_INDEX_TOGGLE, this->enabled[i]);
						this->end_element(writer, "arrayelt");
					}
				this->end_element(writer, __CPP_TRANSPORT_NODE_INDEX_TOGGLES);
			}


		template <unsigned int indices>
		void index_selector<indices>::write(std::ostream& out, const std::vector<std::string>& state_names)
			{
				// demap available indices, by working through all available toggles
				// and printing the ones which are switched on

				unsigned int count = 0;

				for(unsigned int i = 0; i < this->size; i++)
					{
						if(this->enabled[i])
							{
								if(count > 0) out << ", ";
								if(indices > 1) out << "(";

								for(unsigned int j = 0; j < indices; j++)
									{
										unsigned int index = i / this->displacements[j];

										assert((this->range == all_range && index < state_names.size()) || (this->range == field_range && index < state_names.size()/2));

										if(j > 0) out << ",";
										out << state_names[index];
									}

								if(indices > 1) out << ")";
								count++;
							}
					}
			}


  }  // namespace transport

#endif // __CPP_TRANSPORT_INDEX_SELECTOR_H_
