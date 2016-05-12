//
// Created by David Seery on 24/07/2013.
// --@@
// Copyright (c) 2016 University of Sussex. All rights reserved.
//
// This file is part of the CppTransport platform.
//
// CppTransport is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 2 of the License, or
// (at your option) any later version.
//
// CppTransport is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with CppTransport.  If not, see <http://www.gnu.org/licenses/>.
//
// @license: GPL-2
// @contributor: David Seery <D.Seery@sussex.ac.uk>
// --@@
//

#ifndef CPPTRANSPORT_INDEX_SELECTOR_H
#define CPPTRANSPORT_INDEX_SELECTOR_H


#include <array>
#include <vector>

#include <assert.h>

#include "transport-runtime/serialization/serializable.h"

#include "transport-runtime/messages.h"
#include "transport-runtime/exceptions.h"


#define CPPTRANSPORT_NODE_INDEX_ROOT        "index-selection"

#define CPPTRANSPORT_NODE_INDEX_RANGE       "index-range"
#define CPPTRANSPORT_NODE_INDEX_FIELDS      "num-fields"
#define CPPTRANSPORT_NODE_INDEX_RANGE_ALL   "all"
#define CPPTRANSPORT_NODE_INDEX_RANGE_FIELD "field"
#define CPPTRANSPORT_NODE_INDEX_TOGGLES     "enabled-indices"


namespace transport
  {

    namespace derived_data
      {

        template <unsigned int indices> class index_selector;

        template <unsigned int indices>
        index_selector<indices> operator+(const index_selector<indices>& lhs, const std::array<unsigned int, indices>& rhs);

        template <unsigned int indices>
        index_selector<indices> operator-(const index_selector<indices>& lhs, const std::array<unsigned int, indices>& rhs);


        enum class index_range { fields, all };


        //! Select active indices for a d-component object in an N_f-field model
        template <unsigned int indices>
        class index_selector: public serializable
          {

          public:

            //! Build a default index_selector, with all indices enabled.
            index_selector(unsigned int N_f, index_range r=index_range::all);

            //! Deserialization constructor
            index_selector(Json::Value& reader);

            ~index_selector() = default;


            // OVERLOAD ARITHMETIC OEPRATORS FOR CONVENIENCE

          public:

            //! compound addition is synonymous with set_on()
            index_selector<indices>& operator+=(const std::array<unsigned int, indices>& rhs) { this->set_on(rhs); return(*this); }

            //! compound subtraction is synonymous with set_off()
            index_selector<indices>& operator-=(const std::array<unsigned int, indices>& rhs) { this->set_off(rhs); return(*this); }

            //! addition, subtraction
            friend index_selector<indices> operator+ <>(const index_selector<indices>& lhs, const std::array<unsigned int, indices>& rhs);
            friend index_selector<indices> operator- <>(const index_selector<indices>& lhs, const std::array<unsigned int, indices>& rhs);

            //! Disable all indices
            index_selector<indices>& none();

            //! Activate all indices
            index_selector<indices>& all();

            //! Set a specific index combination on
            index_selector<indices>& set_on (const std::array<unsigned int, indices>& which);

            //! Set a specific index combination off
            index_selector<indices>& set_off(const std::array<unsigned int, indices>& which);

            //! Check whether a specific combination is active
            bool is_on  (const std::array<unsigned int, indices>& which) const;

            //! Get range -- do the indices cover fields, or field+momenta?
            index_range get_range() const { return(this->range); }

            //! Get number of fields
            unsigned int get_number_fields() const { return(this->N_fields); }


            // SERIALIZATION

          public:

            //! serialize
            virtual void serialize(Json::Value& writer) const override;

            //! Write self to standard output
            template <typename Stream, typename Container>
            void write(Stream& out, const Container& state_names) const;

            // INTERNAL DATA

          protected:

            //! Number of fields
            unsigned int              N_fields;

            //! Range -- do the indices cover fields only, or fields+momenta?
            index_range               range;

            //! Total number of on/of toggles
            unsigned int              size;           // total number of components

            //! Vector representing the state of each on/off toggle
            std::vector<bool>         enabled;

            //! Look-up table of displacements into the vector of on/off toggles
            std::vector<unsigned int> displacements;

          };


        template <unsigned int indices>
        index_selector<indices>::index_selector(unsigned int N_f, index_range r)
          : N_fields(N_f),
            range(r)
          {
            // work out how many components this object has
            size = 1;
            unsigned int scale_factor = (r == index_range::all ? 2 : 1);

            for(int i = 0; i < indices; ++i)
              {
                size *= scale_factor * N_fields;
              }
            enabled.assign(size, true);   // by default, enable all components

            displacements.resize(indices);
            unsigned int count = 1;
            for(int i = 0; i < indices; ++i)
              {
                displacements[indices-i-1] = count;
                count *= scale_factor * this->N_fields;
              }
          }


        template <unsigned int indices>
        index_selector<indices>& index_selector<indices>::none()
          {
            this->enabled.assign(size, false);
            return *this;
          }


        template <unsigned int indices>
        index_selector<indices>& index_selector<indices>::all()
          {
            this->enabled.assign(size, true);
            return *this;
          }


        template <unsigned int indices>
        index_selector<indices>& index_selector<indices>::set_on(const std::array<unsigned int, indices>& which)
          {
            unsigned int index = 0;

            for(unsigned int i = 0; i < indices; ++i)
              {
                assert(which[i] < (this->range == index_range::all ? 2 : 1) * this->N_fields);  // basic sanity check: TODO: add error handling

                index += this->displacements[i] * which[i];
              }

            assert(index < this->size);
            this->enabled[index] = true;

            return *this;
          }


        template <unsigned int indices>
        index_selector<indices>& index_selector<indices>::set_off(const std::array<unsigned int, indices>& which)
          {
            unsigned int index = 0;

            for(unsigned int i = 0; i < indices; ++i)
              {
                assert(which[i] < (this->range == index_range::all ? 2 : 1) * this->N_fields);  // basic sanity check: TODO: add error handling

                index += this->displacements[i] * which[i];
              }

            assert(index < this->size);
            this->enabled[index] = false;

            return *this;
          }


        template <unsigned int indices>
        bool index_selector<indices>::is_on(const std::array<unsigned int, indices>& which) const
          {
            unsigned int index = 0;

            for(unsigned int i = 0; i < indices; ++i)
              {
                assert(which[i] < (this->range == index_range::all ? 2 : 1) * this->N_fields); // basic sanity check: TODO: add error handling

                index += this->displacements[i] * which[i];
              }

            assert(index < this->size);
            return (this->enabled[index]);
          }


        template <unsigned int indices>
        void index_selector<indices>::serialize(Json::Value& writer) const
          {
            writer[CPPTRANSPORT_NODE_INDEX_ROOT][CPPTRANSPORT_NODE_INDEX_RANGE]  = this->range == index_range::all ? std::string(CPPTRANSPORT_NODE_INDEX_RANGE_ALL) : std::string(CPPTRANSPORT_NODE_INDEX_RANGE_FIELD);
            writer[CPPTRANSPORT_NODE_INDEX_ROOT][CPPTRANSPORT_NODE_INDEX_FIELDS] = this->N_fields;

            Json::Value toggle_array(Json::arrayValue);
            for(unsigned int i = 0; i < this->size; ++i)
              {
                Json::Value elt = static_cast<bool>(this->enabled[i]);
                toggle_array.append(elt);
              }
            writer[CPPTRANSPORT_NODE_INDEX_ROOT][CPPTRANSPORT_NODE_INDEX_TOGGLES] = toggle_array;
          }


        template <unsigned int indices>
        index_selector<indices>::index_selector(Json::Value& reader)
          {
            std::string range_string = reader[CPPTRANSPORT_NODE_INDEX_ROOT][CPPTRANSPORT_NODE_INDEX_RANGE].asString();
            N_fields                 = reader[CPPTRANSPORT_NODE_INDEX_ROOT][CPPTRANSPORT_NODE_INDEX_FIELDS].asUInt();

            if(range_string == CPPTRANSPORT_NODE_INDEX_RANGE_ALL)        range = index_range::all;
            else if(range_string == CPPTRANSPORT_NODE_INDEX_RANGE_FIELD) range = index_range::fields;
            else
              {
                std::ostringstream msg;
                msg << CPPTRANSPORT_INDEX_UNKNOWN_RANGE_TYPE << " '" << range_string << "'";
                throw runtime_exception(exception_type::SERIALIZATION_ERROR, msg.str());
              }

            // read array of toggles

            enabled.clear();
            Json::Value toggle_array = reader[CPPTRANSPORT_NODE_INDEX_ROOT][CPPTRANSPORT_NODE_INDEX_TOGGLES];
            assert(toggle_array.isArray());

            for(Json::Value::iterator t = toggle_array.begin(); t != toggle_array.end(); ++t)
              {
                enabled.push_back(t->asBool());
              }

            // work out how many components this object has (or should have)
            size = 1;
            unsigned int scale_factor = (range == index_range::all ? 2 : 1);

            for(int i = 0; i < indices; ++i)
              {
                size *= scale_factor * N_fields;
              }

            if(size != enabled.size())
              {
                std::ostringstream msg;
                msg << CPPTRANSPORT_INDEX_PRESET_MISMATCH << size << ", "
                << CPPTRANSPORT_INDEX_PRESET_MISMATCH_A << enabled.size();
                throw runtime_exception(exception_type::SERIALIZATION_ERROR, msg.str());
              }

            displacements.resize(indices);
            unsigned int count = 1;
            for(int i = 0; i < indices; ++i)
              {
                displacements[indices-i-1] = count;
                count *= scale_factor * this->N_fields;
              }
          }


        template <unsigned int indices>
        template <typename Stream, typename Container>
        void index_selector<indices>::write(Stream& out, const Container& state_names) const
          {
            // demap available indices, by working through all available toggles
            // and printing the ones which are switched on

            assert(state_names.size() == 2*this->N_fields);

            unsigned int count = 0;

            for(unsigned int i = 0; i < this->size; ++i)
              {
                if(this->enabled[i])
                  {
                    if(count > 0) out << ", ";
                    if(indices > 1) out << "(";

                    for(unsigned int j = 0; j < indices; ++j)
                      {
                        unsigned int remainder = i;
                        for(unsigned int k = 0; k < j; ++k) remainder = remainder % this->displacements[k];

                        unsigned int index = remainder / this->displacements[j];

                        assert((this->range == index_range::all && index < state_names.size()) || (this->range == index_range::fields && index < state_names.size()/2));

                        if(j > 0) out << ",";
                        out << state_names[index];
                      }

                    if(indices > 1) out << ")";
                    count++;
                  }
              }
          }


        template <unsigned int indices>
        index_selector<indices> operator+(const index_selector<indices>& lhs, const std::array<unsigned int, indices>& rhs)
          {
            return(index_selector<indices>(lhs) += rhs);
          }


        template <unsigned int indices>
        index_selector<indices> operator-(const index_selector<indices>& lhs, const std::array<unsigned int, indices>& rhs)
          {
            return(index_selector<indices>(lhs) -= rhs);
          }

      }

	}  // namespace transport

#endif // CPPTRANSPORT_INDEX_SELECTOR_H
