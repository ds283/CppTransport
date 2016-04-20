//
// Created by David Seery on 22/01/2016.
// Copyright (c) 2016 University of Sussex. All rights reserved.
//

#ifndef CPPTRANSPORT_MANAGER_DETAIL_AGGREGATION_H
#define CPPTRANSPORT_MANAGER_DETAIL_AGGREGATION_H


#include "boost/optional.hpp"


namespace transport
  {

    namespace master_controller_impl
      {

        // AGGREGATION HANDLERS

        template <typename number>
        class integration_aggregator
          {

            // CONSTRUCTOR, DESTRUCTOR

          public:

            //! empty constructor
            integration_aggregator() = default;

            //! non-empty constructor
            integration_aggregator(master_controller<number>& c, integration_writer<number>& w)
              : controller(c),
                writer(w)
              {
              }

            //! destructor is default
            ~integration_aggregator() = default;


            // CONVERSION TO BOOL

          public:

            //! allow conversion to bool
            operator bool() const { return(static_cast<bool>(controller)); }


            // PERFORM AGGREGATION

          public:

            //! perform aggregation
            void operator()(unsigned int worker, unsigned int id,
                            MPI::data_ready_payload& payload, integration_metadata& metadata)
              {
                if(static_cast<bool>(controller))
                  {
                    (*controller).aggregate_integration(*writer, worker, id, payload, metadata);
                  }
              }


            // INTERNAL DATA

          private:

            //! reference to controller, if one supplied
            boost::optional< master_controller<number>& > controller;

            //! reference to integration writer, if one supplied
            boost::optional< integration_writer<number>& > writer;

          };


        template <typename number>
        class postintegration_aggregator
          {

            // CONSTRUCTOR, DESTRUCTOR

          public:

            //! empty constructor
            postintegration_aggregator() = default;

            //! non-empty constructor
            postintegration_aggregator(master_controller<number>& c, postintegration_writer<number>& w)
              : controller(c),
                writer(w)
              {
              }

            //! destructor is default
            ~postintegration_aggregator() = default;


            // CONVERSION TO BOOL

          public:

            //! allow conversion to bool
            operator bool() const { return(static_cast<bool>(controller)); }


            // PERFORM AGGREGATION

          public:

            //! perform aggregation
            void operator()(unsigned int worker, unsigned int id,
                            MPI::data_ready_payload& payload, output_metadata& metadata)
              {
                if(static_cast<bool>(controller))
                  {
                    (*controller).aggregate_postprocess(*writer, worker, id, payload, metadata);
                  }
              }


            // INTERNAL DATA

          private:

            //! reference to controller, if one supplied
            boost::optional< master_controller<number>& > controller;

            //! reference to integration writer, if one supplied
            boost::optional< postintegration_writer<number>& > writer;

          };


        template <typename number>
        class derived_content_aggregator
          {

            // CONSTRUCTOR, DESTRUCTOR

          public:

            //! empty constructor
            derived_content_aggregator() = default;

            //! non-empty constructor
            derived_content_aggregator(master_controller<number>& c, derived_content_writer<number>& w)
              : controller(c),
                writer(w)
              {
              }

            //! destructor is default
            ~derived_content_aggregator() = default;


            // CONVERSION TO BOOL

          public:

            //! allow conversion to bool
            operator bool() const { return(static_cast<bool>(controller)); }


            // PERFORM AGGREGATION

          public:

            //! perform aggregation
            void operator()(unsigned int worker, unsigned int id,
                            MPI::content_ready_payload& payload, output_metadata& metadata)
              {
                if(static_cast<bool>(controller))
                  {
                    (*controller).aggregate_content(*writer, worker, id, payload, metadata);
                  }
              }


            // INTERNAL DATA

          private:

            //! reference to controller, if one supplied
            boost::optional< master_controller<number>& > controller;

            //! reference to integration writer, if one supplied
            boost::optional< derived_content_writer<number>& > writer;

          };


        // AGGREGATION RECORDS

        class aggregation_record
          {

            // CONSTRUCTOR, DESTRUCTOR

          public:

            //! constructor
            aggregation_record(unsigned int w, unsigned int i)
              : worker(w),
                id(i)
              {
              }

            //! destructor is default
            virtual ~aggregation_record() = default;


            // INTERFACE

          public:

            //! perform aggregation. Should be implemented by concrete classes
            virtual void aggregate() = 0;

            //! get aggregation id
            unsigned int get_id() const { return(this->id); }

            //! get worker for whom we are aggregating
            unsigned int get_worker() const { return(this->worker); }


            // INTERNAL DATA

          private:

            //! worker number which generated this aggregation
            unsigned int worker;

            //! unique identifier for this aggregation
            unsigned int id;

          };


        template <typename number>
        class integration_aggregation_record: public aggregation_record
          {

            // CONSTRUCTOR, DESTRUCTOR

          public:

            //! constructor
            integration_aggregation_record(unsigned int w, unsigned int id, integration_aggregator<number>& agg, integration_metadata& m, MPI::data_ready_payload& p)
              : aggregation_record(w, id),
                handler(agg),
                payload(p),
                metadata(m)
              {
              }

            //! destructor is default
            virtual ~integration_aggregation_record() = default;


            // INTERFACE

          public:

            //! perform aggregation
            void aggregate() override { this->handler(this->get_worker(), this->get_id(), payload, metadata); }


            // INTERNAL DATA

          private:

            //! aggregation handler
            integration_aggregator<number>& handler;

            //! metadata that came along with this integration
            integration_metadata& metadata;

            //! MPI payload that came with this aggregation request
            MPI::data_ready_payload payload;

          };


        template <typename number>
        class postintegration_aggregation_record: public aggregation_record
          {

            // CONSTRUCTOR, DESTRUCTOR

          public:

            //! constructor
            postintegration_aggregation_record(unsigned int w, unsigned int id, postintegration_aggregator<number>& agg, output_metadata& m, MPI::data_ready_payload& p)
              : aggregation_record(w, id),
                handler(agg),
                payload(p),
                metadata(m)
              {
              }

            //! destructor is default
            virtual ~postintegration_aggregation_record() = default;


            // INTERFACE

          public:

            //! perform aggregation
            void aggregate() override { this->handler(this->get_worker(), this->get_id(), payload, metadata); }


            // INTERNAL DATA

          private:

            //! aggregation handler
            postintegration_aggregator<number>& handler;

            //! metadata that came along with this postintegration
            output_metadata& metadata;

            //! MPI payload that came along with this aggregation request
            MPI::data_ready_payload payload;

          };


        template <typename number>
        class derived_content_aggregation_record: public aggregation_record
          {

            // CONSTRUCTOR, DESTRUCTOR

          public:

            //! constructor
            derived_content_aggregation_record(unsigned int w, unsigned int id, derived_content_aggregator<number>& agg, output_metadata& m, MPI::content_ready_payload& p)
              : aggregation_record(w, id),
                handler(agg),
                payload(p),
                metadata(m)
              {
              }

            //! destructor is default
            virtual ~derived_content_aggregation_record() = default;


            // INTERFACE

          public:

            void aggregate() override { this->handler(this->get_worker(), this->get_id(), payload, metadata); }


            // INTERNAL DATA

          private:

            //! aggregation handler
            derived_content_aggregator<number>& handler;

            //! metadata that came along with this output job
            output_metadata& metadata;

            //! MPI payload that came along with this aggregation request
            MPI::content_ready_payload payload;

          };

      }   // namespace master_controller_impl

  }   // namespace transport

#endif //CPPTRANSPORT_MANAGER_DETAIL_AGGREGATION_H
