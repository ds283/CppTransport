//
// Created by David Seery on 13/06/2014.
// Copyright (c) 2014 University of Sussex. All rights reserved.
//


#ifndef __json_repository_interface_H_
#define __json_repository_interface_H_


#include <string>

#include "transport-runtime-api/repository/repository.h"

#include "transport-runtime-api/messages.h"
#include "transport-runtime-api/exceptions.h"

#include "json/json.h"


namespace transport
  {

    // 'json_repository_interface' is one interface for getting data in and out of
    // a repository. It can be implemented by a concrete repository class.
    template <typename number>
    class json_interface_repository: public repository<number>
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        json_interface_repository(const std::string& path, typename repository<number>::access_type type,
                                  typename repository<number>::error_callback e,
                                  typename repository<number>::warning_callback w,
                                  typename repository<number>::message_callback m,
                                  typename repository_finder<number>::package_finder pf,
                                  typename repository_finder<number>::task_finder tf,
                                  typename repository_finder<number>::derived_product_finder dpf);

        virtual ~json_interface_repository() = default;


        // PULL RECORDS FROM THE REPOSITORY IN JSON FORMAT

      public:

        //! export a package record in JSON format
        virtual std::string export_JSON_package_record(const std::string& name) = 0;

        //! export a task record in JSON format
        virtual std::string export_JSON_task_record(const std::string& name) = 0;

        //! export a derived product record in JSON format
        virtual std::string export_JSON_product_record(const std::string& name) = 0;

        //! export an output group record in JSON format
        virtual std::string export_JSON_content_record(const std::string& name) = 0;


        // JSON SERVICES

      protected:

        // format a JSON document for human-readable output
        std::string format_JSON(const std::string& document);

      };


    // IMPLEMENTATION


    template <typename number>
    json_interface_repository<number>::json_interface_repository(const std::string& path, typename repository<number>::access_type type,
                                                                 typename repository<number>::error_callback e,
                                                                 typename repository<number>::warning_callback w,
                                                                 typename repository<number>::message_callback m,
                                                                 typename repository_finder<number>::package_finder pf,
                                                                 typename repository_finder<number>::task_finder tf,
                                                                 typename repository_finder<number>::derived_product_finder dpf)
      : repository<number>(path, type, e, w, m, pf, tf, dpf)
      {
      }


    template <typename number>
    std::string json_interface_repository<number>::format_JSON(const std::string& document)
      {
        Json::Value root;
        Json::Reader reader;

        bool success = reader.parse(document, root);
        if (!success)
          {
            std::ostringstream msg;
            msg << __CPP_TRANSPORT_JSON_REPO_PARSE_FAIL << ": " << reader.getFormattedErrorMessages();
            throw runtime_exception(runtime_exception::JSON_FORMAT_ERROR, msg.str());
          }

        // remove internal __id field if it exists
        root.removeMember(std::string("__id"));

        Json::StyledWriter writer;
        std::string formatted_document = writer.write(root);

        return(formatted_document);
      }

  }


#endif //__json_repository_interface_H_
