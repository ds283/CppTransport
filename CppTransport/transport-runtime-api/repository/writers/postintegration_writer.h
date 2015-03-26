//
// Created by David Seery on 25/03/15.
// Copyright (c) 2015 University of Sussex. All rights reserved.
//


#ifndef __postintegration_writer_H_
#define __postintegration_writer_H_


#include <iostream>
#include <string>
#include <memory>
#include <functional>

#include "transport-runtime-api/serialization/serializable.h"

#include "transport-runtime-api/exceptions.h"
#include "transport-runtime-api/localizations/messages_en.h"

#include "transport-runtime-api/derived-products/template_types.h"

#include "transport-runtime-api/repository/records/repository_records.h"
#include "transport-runtime-api/repository/writers/generic_writer.h"

#include "boost/log/core.hpp"
#include "boost/log/trivial.hpp"
#include "boost/log/sources/severity_feature.hpp"
#include "boost/log/sources/severity_logger.hpp"
#include "boost/log/sinks/sync_frontend.hpp"
#include "boost/log/sinks/text_file_backend.hpp"
#include "boost/log/utility/setup/common_attributes.hpp"


namespace transport
	{

    // WRITER FOR POSTINTEGRATION OUTPUT

    //! Postintegration writer: used to commit postprocessing of integration output to the database
		template <typename number>
    class postintegration_writer: public generic_writer< postintegration_writer<number> >
	    {

      public:

        //! Callback for merging postintegration correlation-function output between data containers
        typedef std::function<void(const boost::filesystem::path&, const boost::filesystem::path&)> merge_callback;

        //! Callback for merging postintegration fNL output between data containers
        typedef std::function<void(const boost::filesystem::path&, const boost::filesystem::path&, derived_data::template_type)> fNL_merge_callback;

        class merge_group
	        {
          public:
            merge_callback     zeta_twopf;
            merge_callback     zeta_threepf;
            merge_callback     zeta_redbsp;
            fNL_merge_callback fNL;
	        };

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! Construct a postintegration writer object.
        //! After creation it must be initialized by a suitable data_manager
        postintegration_writer(postintegration_task_record<number>* rec,
                               const typename generic_writer< postintegration_writer<number> >::callback_group& c,
                               const typename generic_writer< postintegration_writer<number> >::metadata_group& m,
                               const typename generic_writer< postintegration_writer<number> >::paths_group& p,
                               unsigned int w);

        //! disallow copying to ensure consistency of RAII idiom
        postintegration_writer(const postintegration_writer<number>& obj) = delete;

        //! Destroy a postintegration writer object
        virtual ~postintegration_writer();


        // MERGE CONTAINER OUTPUT

        //! assign merge handlers
        void set_merge_handlers(const merge_group& d) { this->mergers = d; }

        //! merge zeta twopf
        void merge_zeta_twopf(const boost::filesystem::path& source, const boost::filesystem::path& dest);

        //! merge zeta threepf
        void merge_zeta_threepf(const boost::filesystem::path& source, const boost::filesystem::path& dest);

        //! merge zeta reduced bispectrum
        void merge_zeta_redbsp(const boost::filesystem::path& source, const boost::filesystem::path& dest);

        //! merge fNL_local
        void merge_fNL(const boost::filesystem::path& source, const boost::filesystem::path& dest, derived_data::template_type type);


        // METADATA

      public:

        //! Return task
        postintegration_task_record<number>* get_record() const { return(this->parent_record); }

        //! Set metadata
        void set_metadata(const output_metadata& data) { this->metadata = data; }

        //! Get metadata
        const output_metadata& get_metadata() const { return(this->metadata); }


        // INTERNAL DATA

      private:

        // METADATA

        //! task associated with this integration writer
        postintegration_task_record<number>* parent_record;

        //! output metadata for this task
        output_metadata metadata;


        // MERGE CALLBACKS

        //! merge callbacks
        merge_group mergers;

	    };


    // POSTINTEGRATION WRITER METHODS


    template <typename number>
    postintegration_writer<number>::postintegration_writer(postintegration_task_record<number>* rec,
                                                           const typename generic_writer< postintegration_writer<number> >::callback_group& c,
                                                           const typename generic_writer< postintegration_writer<number> >::metadata_group& m,
                                                           const typename generic_writer< postintegration_writer<number> >::paths_group& p,
                                                           unsigned int w)
	    : generic_writer< postintegration_writer<number> >(c, m, p, w),
	      parent_record(dynamic_cast< postintegration_task_record<number>* >(rec->clone())),
	      metadata()
	    {
        assert(this->parent_record != nullptr);
	    }


		template <typename number>
    postintegration_writer<number>::~postintegration_writer()
	    {
        // if not committed, abort. WARNING! Although this behaviour is common to all writers,
        // this has to happen in the derived destructor, not the base generic_writer<> destructor
        // because by the time we arrive in the generic_writer<> destructor we have lost
        // the identity of the derived class
        if(!this->committed) this->callbacks.abort(*this);

        delete this->parent_record;
	    }


		template <typename number>
    void postintegration_writer<number>::merge_zeta_twopf(const boost::filesystem::path& source, const boost::filesystem::path& dest)
	    {
        if(!this->mergers.zeta_twopf)
	        {
            assert(false);
            throw runtime_exception(runtime_exception::RUNTIME_ERROR, __CPP_TRANSPORT_REPO_WRITER_TWOPF_MERGER_UNSET);
	        }

        this->mergers.zeta_twopf(source, dest);
	    }


		template <typename number>
    void postintegration_writer<number>::merge_zeta_threepf(const boost::filesystem::path& source, const boost::filesystem::path& dest)
	    {
        if(!this->mergers.zeta_threepf)
	        {
            assert(false);
            throw runtime_exception(runtime_exception::RUNTIME_ERROR, __CPP_TRANSPORT_REPO_WRITER_THREEPF_MERGER_UNSET);
	        }

        this->mergers.zeta_threepf(source, dest);
	    }


		template <typename number>
    void postintegration_writer<number>::merge_zeta_redbsp(const boost::filesystem::path& source, const boost::filesystem::path& dest)
	    {
        if(!this->mergers.zeta_redbsp)
	        {
            assert(false);
            throw runtime_exception(runtime_exception::RUNTIME_ERROR, __CPP_TRANSPORT_REPO_WRITER_REDBSP_MERGER_UNSET);
	        }

        this->mergers.zeta_redbsp(source, dest);
	    }


		template <typename number>
    void postintegration_writer<number>::merge_fNL(const boost::filesystem::path& source, const boost::filesystem::path& dest, derived_data::template_type type)
	    {
        if(!this->mergers.fNL)
	        {
            assert(false);
            throw runtime_exception(runtime_exception::RUNTIME_ERROR, __CPP_TRANSPORT_REPO_WRITER_FNL_MERGER_UNSET);
	        }

        this->mergers.fNL(source, dest, type);
	    }

	}


#endif //__postintegration_writer_H_
