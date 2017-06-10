//
// Created by David Seery on 07/06/2017.
// --@@
// Copyright (c) 2017 University of Sussex. All rights reserved.
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

#include "directive_definitions.h"
#include "macro.h"


namespace macro_packages
  {
    
    std::string directive_simple::operator()(const macro_argument_list& args)
      {
        this->validate(args);
        
        macro_agent& ma = this->payload.get_stack().top_macro_package();
        if(!ma.is_enabled() && !this->always_evaluate())
          {
            std::ostringstream msg;
            msg << NOTIFY_DIRECTIVE_NOT_EVALUATED << " '" << this->name << "'";
            return msg.str();
          }

        return this->evaluate(args);
      }
    
    
    void directive_simple::validate(const macro_argument_list& args)
      {
        // check that correct number of arguments have been supplied
        if(args.size() == this->num_args) return;
    
        std::ostringstream msg;
    
        msg << ERROR_DIRECTIVE_WRONG_ARGUMENT_COUNT << " '" << this->name << "; " << ERROR_EXPECTED_ARGUMENT_COUNT
            << " " << this->num_args << ", " << ERROR_RECEIVED_ARGUMENT_COUNT << " " << args.size();
        throw argument_mismatch(msg.str());
      }
    
    
    std::string directive_index::operator()(const macro_argument_list& args, const index_literal_list& indices)
      {
        this->validate(args);
        this->validate(indices);
    
        macro_agent& ma = this->payload.get_stack().top_macro_package();
        if(!ma.is_enabled() && !this->always_evaluate())
          {
            std::ostringstream msg;
            msg << NOTIFY_DIRECTIVE_NOT_EVALUATED << " '" << this->name << "'";
            return msg.str();
          }

        return this->evaluate(args, indices);
      }
    
    
    void directive_index::validate(const macro_argument_list& args)
      {
        if(args.size() == this->num_args) return;
        
        std::ostringstream msg;
        
        msg << ERROR_DIRECTIVE_WRONG_ARGUMENT_COUNT << " '" << this->name << "; " << ERROR_EXPECTED_ARGUMENT_COUNT
            << " " << this->num_args << ", " << ERROR_RECEIVED_ARGUMENT_COUNT << " " << args.size();
        throw argument_mismatch(msg.str());
      }
    
    
    template <typename IndexDatabase>
    void directive_index::validate(const IndexDatabase& indices)
      {
        // check that correct number of indices have been supplied
        if((this->num_indices && indices.size() != this->num_indices.get())
           || (this->idx_classes && indices.size() != this->idx_classes.get().size()))
          {
            std::ostringstream msg;
    
            msg << ERROR_DIRECTIVE_WRONG_INDEX_COUNT << " '" << this->name << "'; " << ERROR_EXPECTED_INDEX_COUNT << " "
                << *this->num_indices << ", " << ERROR_RECEIVED_INDEX_COUNT << " " << indices.size();
            throw rule_apply_fail(msg.str());
          }
        
        // check that index types are compatible
        if(!this->idx_classes) return;
    
        const auto& expected_classes = *this->idx_classes;

        for(unsigned int i = 0; i < indices.size(); ++i)
          {
            const auto& T = indices[i];
            index_class expected = expected_classes[i];
            
            index_class cl = index_traits_impl::get_index_class<decltype(T)>(T);
            char lb = index_traits_impl::get_index_label<decltype(T)>(T);
            const error_context& ctx = index_traits_impl::get_index_declaration<decltype(T)>(T);

            switch(expected)
              {
                case index_class::full:
                  {
                    // full is compatible with either full or field_only
                    if(cl != index_class::full && cl != index_class::field_only)
                      {
                        std::ostringstream msg;

                        msg << ERROR_WRONG_INDEX_CLASS << " '" << this->name << "' " << ERROR_WRONG_INDEX_LABEL
                            << " '" << lb << "'; " << ERROR_WRONG_INDEX_EXPECTED << " '" << to_string(expected) << "'";
                        throw index_mismatch(msg.str(), ctx);
                      }

                    break;
                  }

                case index_class::field_only:
                  {
                    // field_only is compatible only with field_only

                    if(cl != index_class::field_only)
                      {
                        std::ostringstream msg;

                        msg << ERROR_WRONG_INDEX_CLASS << " '" << this->name << "' " << ERROR_WRONG_INDEX_LABEL
                            << " '" << lb << "'; " << ERROR_WRONG_INDEX_EXPECTED << " '" << to_string(expected) << "'";
                        throw index_mismatch(msg.str(), ctx);
                      }

                    break;
                  }

                case index_class::parameter:
                  {
                    // parameter is compatible only with parameter

                    if(cl != index_class::parameter)
                      {
                        std::ostringstream msg;

                        msg << ERROR_WRONG_INDEX_CLASS << " '" << this->name << "' " << ERROR_WRONG_INDEX_LABEL
                            << " '" << lb << "'; " << ERROR_WRONG_INDEX_EXPECTED << " '" << to_string(expected) << "'";
                        throw index_mismatch(msg.str(), ctx);
                      }

                    break;
                  }
              }
          }
      }
    
  }   // namespace macro_packages