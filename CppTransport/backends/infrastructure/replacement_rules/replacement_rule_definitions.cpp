//
// Created by David Seery on 15/12/2015.
// Copyright (c) 2015 University of Sussex. All rights reserved.
//


#include "replacement_rule_definitions.h"


namespace macro_packages
  {

    std::string simple_rule::operator()(const macro_argument_list& args)
      {
        // check that correct number of arguments have been supplied
        if(args.size() != this->args)
          {
            std::ostringstream msg;

            msg << ERROR_WRONG_ARGUMENT_COUNT << " '" << this->name << "'; " << ERROR_EXPECTED_ARGUMENT_COUNT << " " << this->args << ", " << ERROR_RECEIVED_ARGUMENT_COUNT << " " << args.size();
            throw macro_packages::rule_apply_fail(msg.str());
          }


        if(this->rule == nullptr)
          {
            std::ostringstream msg;

            msg << ERROR_NO_RULE_HANDLER << " '" << this->name << "'";
            throw macro_packages::rule_apply_fail(msg.str());
          }

        return (this->rule)(args);
      }


    std::string index_rule::operator()(const macro_argument_list& args, const assignment_list& indices, cse_map* state)
      {
        // check that correct number of arguments have been supplied
        if(args.size() != this->args)
          {
            std::ostringstream msg;

            msg << ERROR_WRONG_ARGUMENT_COUNT << " '" << this->name << "'; " << ERROR_EXPECTED_ARGUMENT_COUNT << " " << this->args << ", " << ERROR_RECEIVED_ARGUMENT_COUNT << " " << args.size();
            throw rule_apply_fail(msg.str());
          }

        // check that correct number of indices have been supplied
        if(indices.size() != this->indices)
          {
            std::ostringstream msg;

            msg << ERROR_WRONG_INDEX_COUNT << " '" << this->name << "'; " << ERROR_EXPECTED_INDEX_COUNT << " " << this->indices << ", " << ERROR_RECEIVED_INDEX_COUNT << " " << indices.size();
            throw macro_packages::rule_apply_fail(msg.str());
          }

        // check that index types are compatible
        for(const assignment_record& rec : indices)
          {
            switch(this->range)
              {
                case index_class::full:
                  {
                    // full is compatible with either full or field_only
                    if(rec.get_class() != index_class::full && rec.get_class() != index_class::field_only)
                      {
                        std::ostringstream msg;

                        msg << ERROR_WRONG_INDEX_CLASS << " '" << this->name << "' " << ERROR_WRONG_INDEX_LABEL << " '" << rec.get_label() << "'";
                        throw macro_packages::rule_apply_fail(msg.str());
                      }

                    break;
                  }

                case index_class::field_only:
                  {
                    // field_only is compatible only with field_only

                    if(rec.get_class() != index_class::field_only)
                      {
                        std::ostringstream msg;

                        msg << ERROR_WRONG_INDEX_CLASS << " '" << this->name << "' " << ERROR_WRONG_INDEX_LABEL << " '" << rec.get_label() << "'";
                        throw macro_packages::rule_apply_fail(msg.str());
                      }

                    break;
                  }

                case index_class::parameter:
                  {
                    // parameter is compatible only with parameter

                    if(rec.get_class() != index_class::parameter)
                      {
                        std::ostringstream msg;

                        msg << ERROR_WRONG_INDEX_CLASS << " '" << this->name << "' " << ERROR_WRONG_INDEX_LABEL << " '" << rec.get_label() << "'";
                        throw macro_packages::rule_apply_fail(msg.str());
                      }

                    break;
                  }
              }
          }

        if(this->rule == nullptr)
          {
            std::ostringstream msg;

            msg << ERROR_NO_RULE_HANDLER << " '" << this->name << "'";
            throw macro_packages::rule_apply_fail(msg.str());
          }

        return (this->rule)(args, indices, state);
      }


    std::unique_ptr<cse_map> index_rule::pre_evaluate(const macro_argument_list& args)
      {
        // check that correct number of arguments have been supplied
        if(args.size() != this->args)
          {
            std::ostringstream msg;

            msg << ERROR_WRONG_ARGUMENT_COUNT << " '" << this->name << "'; " << ERROR_EXPECTED_ARGUMENT_COUNT << " " << this->args << ", " << ERROR_RECEIVED_ARGUMENT_COUNT << " " << args.size();
            throw rule_apply_fail(msg.str());
          }

        if(this->pre != nullptr)
          {
            return (this->pre)(args);
          }
        else
          {
            return std::unique_ptr<cse_map>();
          }
      }


    void index_rule::post_evaluate(cse_map* state)
      {
        if(this->post != nullptr)
          {
            (this->post)(state);
          }
      }

  }   // namespace macro_packages
