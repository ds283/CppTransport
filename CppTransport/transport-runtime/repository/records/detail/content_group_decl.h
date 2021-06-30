//
// Created by David Seery on 25/01/2016.
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

#ifndef CPPTRANSPORT_CONTENT_GROUP_DECL_H
#define CPPTRANSPORT_CONTENT_GROUP_DECL_H


#include <utility>


namespace transport
  {

    // OUTPUT DESCRIPTORS

    //! Derived product descriptor. Used to enumerate the content associated with
    //! a output task content group.
    class derived_content: public serializable
      {

      public:

        //! Create a derived_product descriptor
        derived_content(std::string prod, std::string fnam, const boost::posix_time::ptime& now,
                        content_group_name_set gp, std::list<note> nt, tag_list tg)
          : parent_product(std::move(prod)),
            filename(fnam),
            created(now),
            content_groups(std::move(gp)),
            notes(std::move(nt)),
            tags(std::move(tg))
          {
          }

        //! Deserialization constructor
        explicit derived_content(Json::Value& reader);

        //! Destroy a derived_product descriptor
        ~derived_content() override = default;


        // INTERFACE

      public:

        //! Get product name
        const std::string& get_parent_product() const { return (this->parent_product); }

        //! Get product pathname
        const boost::filesystem::path& get_filename() const { return(this->filename); }

        //! Get notes
        const std::list<note>& get_notes() const { return(this->notes); }

        //! Add note
        void add_note(const std::string& uid, const std::string& note) { this->notes.emplace_back(uid, note); }

        //! Get tags
        const std::list<std::string>& get_tags() const { return(this->tags); }

        //! Add tag
        void add_tag(const std::string& tag) { this->tags.push_back(tag); }

        //! Get creation time for this piece of content
        const boost::posix_time::ptime& get_creation_time() const { return(this->created); }

        //! Get names of content groups on which this piece of content depends
        const content_group_name_set& get_content_groups() const { return(this->content_groups); }


        // SERIALIZATION -- implements a 'serializable' interface

      public:

        //! Serialize this object
        void serialize(Json::Value& writer) const override;


        // INTERNAL DATA

      protected:

        //! Name of parent derived product
        std::string parent_product;

        //! Path to output
        boost::filesystem::path filename;

        //! Creation time
        boost::posix_time::ptime created;

        //! Notes
        std::list<note> notes;

        //! Tags
        tag_list tags;

        //! content groups used to create
        content_group_name_set content_groups;

      };


    // PAYLOADS FOR OUTPUT GROUPS
    class precomputed_products: public serializable
      {

      public:

        //! Create a precomputed products record
        precomputed_products()
          : zeta_twopf{false},
            zeta_twopf_spectral{false},
            zeta_threepf{false},
            zeta_redbsp{false},
            fNL_local{false},
            fNL_equi{false},
            fNL_ortho{false},
            fNL_DBI{false}
          {
          }

        //! Create a precomputed products record: value constructor
        precomputed_products(bool z_2pf, bool z_2pf_si, bool z_3pf, bool z_rb,
                             bool fNL_l, bool fNL_e, bool fNL_o, bool fNL_D)
          : zeta_twopf{z_2pf},
            zeta_twopf_spectral{z_2pf_si},
            zeta_threepf{z_3pf},
            zeta_redbsp{z_rb},
            fNL_local{fNL_l},
            fNL_equi{fNL_e},
            fNL_ortho{fNL_o},
            fNL_DBI{fNL_D}
          {
          }

        //! Deserialization constructor
        explicit precomputed_products(const Json::Value& reader);

        //! Destroy a precomputed products record
        ~precomputed_products() override = default;


        // OPERATIONS

      public:

        //! set properties via assignment
        precomputed_products& operator=(const precomputed_products& obj);

        //! merge with another precomputed_products instance
        //! the resulting object should satisfy the conditions of both
        precomputed_products& operator|=(const precomputed_products& obj);


        // GET AND SET PROPERTIES

      public:

        //! Get precomputed zeta_twopf availability
        bool has_zeta_twopf() const { return(this->zeta_twopf); }

        //! Add zeta_twopf availability flag
        void add_zeta_twopf() { this->zeta_twopf = true; }


        //! Get precomputed zeta_twopf spectral index availability
        bool has_zeta_twopf_spectral() const { return(this->zeta_twopf_spectral); }

        //! Set precomputed zeta twopf spectral index availability
        bool set_zeta_twopf_spectral(bool f) { this->zeta_twopf_spectral = f; return f; }

        //! Add zeta twopf spectral availability flag
        void add_zeta_twopf_spectral() { this->zeta_twopf_spectral = true; }


        //! Get precomputed zeta_threepf availability
        bool has_zeta_threepf() const { return(this->zeta_threepf); }

        //! Add zeta_threepf availability flag
        void add_zeta_threepf() { this->zeta_threepf = true; }


        //! Get precomputed zeta reduced bispectrum availability
        bool has_zeta_redbsp() const { return(this->zeta_redbsp); }

        //! Add zeta_threepf availability flag
        void add_zeta_redbsp() { this->zeta_redbsp = true; }


        //! Get precomputed fNL_local availability
        bool has_fNL_local() const { return(this->fNL_local); }

        //! Add fNL_local availability flag
        void add_fNL_local() { this->fNL_local = true; }


        //! Get precomputed fNL_equi availability
        bool has_fNL_equi() const { return(this->fNL_equi); }

        //! Add fNL_equi availability flag
        void add_fNL_equi() { this->fNL_equi = true; }


        //! Get precomputed fNL_ortho availability
        bool has_fNL_ortho() const { return(this->fNL_ortho); }

        //! Add fNL_ortho availability flag
        void add_fNL_ortho() { this->fNL_ortho = true; }


        //! Get precomputed fNL_DBI availability
        bool has_fNL_DBI() const { return(this->fNL_DBI); }

        //! Add fNL_DBI availability flag
        void add_fNL_DBI() { this->fNL_DBI = true; }


        // WRITE TO A STREAM

      public:

        template <typename Stream> void write(Stream& out) const;


        // SERIALIZATION -- implements a 'serializable' interface

      public:

        //! Serialize this object
        void serialize(Json::Value& writer) const override;


        // INTERNAL DATA

      protected:

        //! group has pre-computed zeta twopf data?
        bool zeta_twopf;

        //! group has pre-cmoputed zeta twopf spectral index data?
        bool zeta_twopf_spectral;

        //! group has pre-computed zeta threepf data?
        bool zeta_threepf;

        //! group had pre-computed zeta reduced bispectrum data?
        bool zeta_redbsp;

        //! group has pre-computed fNL_local data?
        bool fNL_local;

        //! group has pre-computed fNL_equi data?
        bool fNL_equi;

        //! group has pre-computed fNL_ortho data?
        bool fNL_ortho;

        //! group has pre-computed fNL_DBI data?
        bool fNL_DBI;

      };


    bool operator==(const precomputed_products& a, const precomputed_products& b)
      {
        return a.has_zeta_twopf() == b.has_zeta_twopf() &&
               a.has_zeta_twopf_spectral() == b.has_zeta_twopf_spectral() &&
               a.has_zeta_threepf() == b.has_zeta_threepf() &&
               a.has_zeta_redbsp() == b.has_zeta_redbsp() &&
               a.has_fNL_local() == b.has_fNL_local() &&
               a.has_fNL_equi() == b.has_fNL_equi() &&
               a.has_fNL_ortho() == b.has_fNL_ortho() &&
               a.has_fNL_DBI() == b.has_fNL_DBI();
      }


    //! Integration payload
    class integration_payload: public serializable
      {

      public:

        //! Create an empty payload
        integration_payload()
          : metadata{},
            fail{false},
            workgroup_number{0},
            seeded{false},
            statistics{false},
            initial_conditions{false},
            spectral_data{false},
            size{0}
          {
          }

        //! Deserialization constructor
        explicit integration_payload(Json::Value& reader);

        //! Destroy a payload
        ~integration_payload() override = default;


        // GET AND SET RECORD (META)DATA

      public:

        //! Get path of data container
        const boost::filesystem::path& get_container_path() const { return(this->container); }

        //! Set path of data container
        void set_container_path(const boost::filesystem::path& pt) { this->container = pt; }

        //! Get metadata
        const integration_metadata& get_metadata() const { return(this->metadata); }

        //! Set metadata
        void set_metadata(const integration_metadata& data) { this->metadata = data; }

        //! Get fail status
        bool is_failed() const { return(this->fail); }

        //! Set fail status
        void set_fail(bool g) { this->fail = g; }

        //! Get list of failed serial numbers
        const serial_number_list& get_failed_serials() const { return(this->failed_serials); }

        //! Set list of failed serial numbers
        void set_failed_serials(serial_number_list f) { this->failed_serials = std::move(f); }

        //! Get workgroup number
        unsigned int get_workgroup_number() const { return(this->workgroup_number); }

        //! Set workgroup number
        void set_workgroup_number(unsigned int w) { this->workgroup_number = w; }

        //! Set seed
        void set_seed(std::string s) { this->seeded = true; this->seed_group = std::move(s); }

        //! Query seeded status
        bool is_seeded() const { return(this->seeded); }

        //! Query seed group
        const std::string& get_seed_group() const { return(this->seed_group); }

        //! Set data type
        void set_data_type(std::string s) { this->data_type = std::move(s); }

        //! Get data type
        const std::string& get_data_type() const { return(this->data_type); }

        //! Set statistics flag
        void set_statistics(bool g) { this->statistics = g; }

        //! Get statistics flag
        bool has_statistics() const { return(this->statistics); }

        //! Set initial conditions flag
        void set_initial_conditions(bool g) { this->initial_conditions = g;}

        //! Get initial conditions flag
        bool has_initial_conditions() const { return(this->initial_conditions); }

        //! Does this integration payload have spectral data?
        bool has_spectral_data() const { return(this->spectral_data); }

        //! Set spectral data flag
        void set_spectral_data(bool g) { this->spectral_data = g; }

        //! Set container size
        void set_size(unsigned int s) { this->size = s; }

        //! Get container size
        unsigned int get_size() const { return(this->size); }


        // WRITE TO A STREAM

      public:

        template <typename Stream> void write(Stream& out) const;


        // SERIALIZATION -- implements a 'serializable' interface

      public:

        //! Serialize this object
        void serialize(Json::Value& writer) const override;


        // INTERNAL DATA

      protected:

        //! Path to data container
        boost::filesystem::path container;

        //! Metadata
        integration_metadata metadata;

        //! mark this group as failed?
        bool fail;

        //! serial numbers reported failed
        std::set< unsigned int > failed_serials;

        //! workgroup number associated with this integration
        unsigned int workgroup_number;

        //! was this integration seeded?
        bool seeded;

        //! if this integration was seeded, parent content group
        std::string seed_group;

        //! data type used during integration
        std::string data_type;

        //! does this group have per-configuration statistics?
        bool statistics;

        //! does this group have initial conditions data?
        bool initial_conditions;

        //! does this group have spectral index information?
        //! this is always true for integration payloads produced from 2021.1 onwards, but
        //! will be missing for payloads produced using earlier versions
        bool spectral_data;

        //! record container size
        unsigned int size;

      };


    //! Postintegration payload
    class postintegration_payload: public serializable
      {

      public:

        //! Create a payload
        postintegration_payload()
          : metadata(),
            precomputed(),
            paired(false),
            seeded(false),
            size(0)
          {
          }

        //! Deserialization constructor
        explicit postintegration_payload(Json::Value& reader);

        //! Destroy a payload
        ~postintegration_payload() override = default;


        // GET AND SET RECORD (META)DATA

      public:

        //! Get path of data container
        const boost::filesystem::path& get_container_path() const { return(this->container); }

        //! Set path of data container
        void set_container_path(const boost::filesystem::path& pt) { this->container = pt; }

        //! Get metadata
        const output_metadata& get_metadata() const { return(this->metadata); }

        //! Set metadata
        void set_metadata(const output_metadata& data) { this->metadata = data; }


        // PRECOMPUTED PRODUCTS

      public:

        //! Set precomputed products by assignment
        postintegration_payload& operator=(const precomputed_products& in);

        //! Get precomputed products record
        precomputed_products& get_precomputed_products() { return(this->precomputed); }

        //! Get precomputed products record, const version
        const precomputed_products& get_precomputed_products() const { return(this->precomputed); }


        // GET AND SET PROPERTIES

      public:

        //! Get fail status
        bool is_failed() const { return(this->fail); }

        //! Set fail status
        void set_fail(bool g) { this->fail = g; }

        //! Get list of failed serial numbers
        const serial_number_list& get_failed_serials() const { return(this->failed_serials); }

        //! Set list of failed serial numbers
        void set_failed_serials(serial_number_list f) { this->failed_serials = std::move(f); }

        //! Set pair
        void set_pair(bool g) { this->paired = g; }

        //! Query paired status
        bool is_paired() const { return(this->paired); }

        //! Set parent content group
        void set_parent_group(std::string p) { this->parent_group = std::move(p); }

        //! Query paired gorup
        const std::string& get_parent_group() const { return(this->parent_group); }

        //! Set seed
        void set_seed(std::string s) { this->seeded = true; this->seed_group = std::move(s); }

        //! Query seeded status
        bool is_seeded() const { return(this->seeded); }

        //! Query seed group
        const std::string& get_seed_group() const { return(this->seed_group); }

        //! Set container size
        void set_size(unsigned int s) { this->size = s; }

        //! Get container size
        unsigned int get_size() const { return(this->size); }


        // WRITE TO A STREAM

      public:

        //! write self to stream
        template <typename Stream> void write(Stream& out) const;


        // SERIALIZATION -- implements a 'serializable' interface

      public:

        //! Serialize this object
        virtual void serialize(Json::Value& writer) const override;


        // INTERNAL DATA

      protected:

        //! Path to data container
        boost::filesystem::path container;

        //! Metadata
        output_metadata metadata;

        //! Precomputed products
        precomputed_products precomputed;

        //! mark this group as failed?
        bool fail{};

        //! serial numbers reported failed
        std::set< unsigned int > failed_serials;

        //! Paired to an integration content group?
        bool paired;

        //! Paired content group name, if used
        std::string parent_group;

        //! was this postintegration seeded?
        bool seeded;

        //! if this postintegration was seeded, parent content group
        std::string seed_group;

        //! record container size
        unsigned int size;

      };


    //! Derived product payload
    class output_payload: public serializable
      {

      public:

        //! Create a payload
        output_payload()
          : metadata(),
            fail(false)
          {
          }

        //! Deserialization constructor
        explicit output_payload(Json::Value& reader);

        //! Destroy a payload
        ~output_payload() override = default;


        // ADMIN

      public:

        //! Add an output
        void add_derived_content(derived_content prod) { this->content.emplace_back(std::move(prod)); }

        //! Get derived content records
        const std::list<derived_content>& get_derived_content() const { return(this->content); }

        //! Get metadata
        const output_metadata& get_metadata() const { return(this->metadata); }

        //! Set metadata
        void set_metadata(const output_metadata& data) { this->metadata = data; }


        // GET/SET PROPERTIES

      public:

        //! Get fail status
        bool is_failed() const { return(this->fail); }

        //! Set fail status
        void set_fail(bool g) { this->fail = g; }

        //! Get list of used content groups;
        //! this information is replicated at a more granular level in the content list,
        //! but a summary set is provided here for convenience
        const content_group_name_set& get_content_groups_summary() const { return this->used_groups; }

        //! Set list of used content groups
        void set_content_groups_summary(const content_group_name_set& list) { this->used_groups = list; }


        // WRITE TO A STREAM

      public:

        //! write details
        template <typename Stream> void write(Stream& out) const;


        // SERIALIZATION -- implements a 'serializable' interface

      public:

        //! Serialize this object
        void serialize(Json::Value& writer) const override;


        // INTERNAL DATA

      protected:

        //! List of derived outputs
        std::list<derived_content> content;

        //! Metadata
        output_metadata metadata;

        //! failed flag
        bool fail;

        //! list of content groups used to produce this output
        content_group_name_set used_groups;

      };


    //! Content group descriptor. Used to enumerate the content groups available for a particular task
    template <typename Payload>
    class content_group_record: public repository_record
      {

      public:

        //! make payload type available for template programming
        using payload_type = Payload;

        // encapsulate paths associated with this record
        class paths_group
          {
          public:
            boost::filesystem::path root;
            boost::filesystem::path output;
          };

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! Create a content_group_record descriptor
        content_group_record(std::string tn, const paths_group& p,
                            bool lock, std::list<note> nt, std::list<std::string> tg,
                            repository_record::handler_package& pkg);

        //! Deserialization constructor
        content_group_record(Json::Value& reader, const boost::filesystem::path& root, repository_record::handler_package& pkg);

        //! Destroy a content_group_record descriptor
        ~content_group_record() override = default;


        // GET AND SET METADATA

      public:

        //! Get name of task that produced this content group
        const std::string& get_task_name() const { return(this->task); }

        //! Get locked status
        bool get_lock_status() const { return (this->locked); }

        //! Set locked status
        void set_lock_status(bool g);

        //! Get notes
        const std::list<note>& get_notes() const { return (this->notes); }

        //! Add note
        void add_note(std::string note);

        //! Remove numbered note
        void remove_note(unsigned int number);

        //! Get tags
        const std::list<std::string>& get_tags() const { return (this->tags); }

        //! Add tag
        void add_tag(const std::string& tag);

        //! Remove tag
        void remove_tag(const std::string& tag);

        //! Check whether we match a set of tags; returns true if so, false otherwise
        bool check_tags(const tag_list& match_tags) const;


        // ABSOLUTE PATHS

      public:

        //! Get path to repository root
        const boost::filesystem::path& get_abs_repo_path() const { return(this->paths.root); }

        //! Get path to output root (typically a subdir of the repository root)
        boost::filesystem::path get_abs_output_path() const { return(this->paths.root/this->paths.output); }


        // PAYLOAD

      public:

        //! Get payload
        Payload& get_payload()             { return(this->payload); }

        //! Get payload (const version)
        const Payload& get_payload() const { return(this->payload); }


        // SERIALIZATION -- implements a 'serializable' interface

      public:

        //! Serialize this object
        void serialize(Json::Value& writer) const override;


        // CLONE

      public:

        //! clone this object
        content_group_record<Payload>* clone() const override { return new content_group_record<Payload>(static_cast<const content_group_record<Payload>&>(*this)); };


        // WRITE TO A STREAM

      public:

        //! Write self to output stream
        template <typename Stream> void write(Stream& out) const;


        // INTERNAL DATA

      private:

        // PAYLOAD

        //! Payload
        Payload payload;


        // METADATA

        //! Parent task associated with this output.
        std::string task;

        //! Flag indicating whether or not this content group is locked
        bool locked;

        //! Array of strings representing notes attached to this group
        std::list<note> notes;

        //! Array of strings representing metadata tags
        tag_list tags;


        // PATHS

        //! Paths associated with this content group
        paths_group paths;

      };


    namespace content_group_helper
      {

        // used for sorting a list of content_groups into decreasing chronological order
        template <typename Payload>
        bool comparator(const std::unique_ptr< content_group_record<Payload> >& A,
                        const std::unique_ptr< content_group_record<Payload> >& B)
          {
            return(A->get_creation_time() > B->get_creation_time());
          }

      }   // namespace content_group_helper

    constexpr auto CPPTRANSPORT_NODE_CONTENT_GROUP_SPECIFIER_TYPE        = "type";
    constexpr auto CPPTRANSPORT_NODE_CONTENT_GROUP_SPECIFIER_ICS         = "require-ics";
    constexpr auto CPPTRANSPORT_NODE_CONTENT_GROUP_SPECIFIER_STATISTICS  = "require-statistics";
    constexpr auto CPPTRANSPORT_NODE_CONTENT_GROUP_SPECIFIER_SPECTRAL    = "require-spectral-data";
    constexpr auto CPPTRANSPORT_NODE_CONTENT_GROUP_SPECIFIER_PRECOMPUTED = "required-precomputed";


    //! used to specify which properties are needed in a content group for each task
    //! that supplies data to a derived_product<> via a derivable_task<>
    class content_group_specifier: public serializable
      {

        // We need to capture information about different types of content groups, and while we could do that
        // polymorphically, that woud lead to lots of dynamic_cast<> usage which is messy and slow.
        // Instead we capture everything in a single object, with flags to indicate which kind of object is
        // under discussion. This is also messier than we would like, but we're stuck with it for now

      public:

        // each constructor captures all necessary data

        //! constructor: integration group
        content_group_specifier(bool i, bool s, bool sd)
          : type{task_type::integration},
            ics{i},
            statistics{s},
            spectral_data{sd}
          {
          }

        //! constructor: postintegration group
        explicit content_group_specifier(precomputed_products p)
          : type{task_type::postintegration},
            products(p),
            ics{false},
            statistics{false},
            spectral_data{false}
          {
          }

        //! deserialization constructor
        explicit content_group_specifier(const Json::Value& reader);

        //! destructor
        ~content_group_specifier() = default;


        // OPERATIONS

      public:

        //! merge with a second object
        //! the resulting flags should be suitable to satisfy the requirements of both original specifiers
        content_group_specifier& operator|=(const content_group_specifier& obj);


        // QUERY: TASK TYPE

      public:

        //! get type
        task_type get_type() const { return this->type; }


        // QUERY: INTEGRATION GROUPS

      public:

        //! integration content group flag: requires initial conditions
        bool requires_ics() const;

        //! integration content group flag: requires per-configuration statistics
        bool requires_statistics() const;

        //! integration content group flag: requires spectral data
        bool requires_spectral_data() const;


        // QUERY: POSTINTEGRATION GROUPS

      public:

        //! postintegration content group: which precomputed products are required?
        const precomputed_products& requires_products() const;


        // SERIALIZATION -- implements a serializable interface

      public:

        void serialize(Json::Value& writer) const override;


        // INTERNAL DATA

      protected:

        //! type of required content group
        task_type type;


        // FLAGS FOR INTEGRATION GROUPS

        //! true if we require a content group with initial conditions data (not currently used)
        bool ics;

        //! true if we require a content group with per-configuraiton statistics
        bool statistics;

        //! true if we require a content group with spectral data
        bool spectral_data;


        // PRECOMPUTED SPECIFIERS FOR POSTINTEGRATION GROUPS

        //! specify which precomputed products are needed
        precomputed_products products;

      };


    // deserialization constructor
    content_group_specifier::content_group_specifier(const Json::Value& reader)
      : type{task_type_from_string(reader[CPPTRANSPORT_NODE_CONTENT_GROUP_SPECIFIER_TYPE].asString())},
        ics{false},
        statistics{false},
        spectral_data{false}
      {
        switch(type)
          {
            case task_type::integration:
              {
                // deserialize data for integration content groups
                ics = reader[CPPTRANSPORT_NODE_CONTENT_GROUP_SPECIFIER_ICS].asBool();
                statistics = reader[CPPTRANSPORT_NODE_CONTENT_GROUP_SPECIFIER_STATISTICS].asBool();
                spectral_data = reader[CPPTRANSPORT_NODE_CONTENT_GROUP_SPECIFIER_SPECTRAL].asBool();
                break;
              }

            case task_type::postintegration:
              {
                // deserialize precomputed products information and assign it to our local copy
                const Json::Value& pdata = reader[CPPTRANSPORT_NODE_CONTENT_GROUP_SPECIFIER_PRECOMPUTED];
                precomputed_products p(pdata);
                products = p;
                break;
              }

            case task_type::output:
              {
                throw runtime_exception(exception_type::SERIALIZATION_ERROR,
                                        CPPTRANSPORT_TASK_CONTENT_GROUP_BAD_TASK_TYPE);
              }
          }
      }


    // serialize to a JSON document
    void content_group_specifier::serialize(Json::Value& writer) const
      {
        writer[CPPTRANSPORT_NODE_CONTENT_GROUP_SPECIFIER_TYPE] = task_type_to_string(this->type);

        switch(this->type)
          {
            case task_type::integration:
              {
                writer[CPPTRANSPORT_NODE_CONTENT_GROUP_SPECIFIER_ICS] = this->ics;
                writer[CPPTRANSPORT_NODE_CONTENT_GROUP_SPECIFIER_STATISTICS] = this->statistics;
                writer[CPPTRANSPORT_NODE_CONTENT_GROUP_SPECIFIER_SPECTRAL] = this->spectral_data;
                break;
              }

            case task_type::postintegration:
              {
                Json::Value p(Json::objectValue);
                this->products.serialize(p);
                writer[CPPTRANSPORT_NODE_CONTENT_GROUP_SPECIFIER_PRECOMPUTED] = p;
                break;
              }

            case task_type::output:
              {
                throw runtime_exception(exception_type::SERIALIZATION_ERROR, CPPTRANSPORT_TASK_CONTENT_GROUP_BAD_TASK_TYPE);
              }
          }
      }


    bool content_group_specifier::requires_ics() const
      {
        if(this->type != task_type::integration)
          throw runtime_exception(exception_type::RUNTIME_ERROR, CPPTRANSPORT_TASK_CONTENT_GROUP_OUT_OF_RANGE);

        return ics;
      }


    bool content_group_specifier::requires_statistics() const
      {
        if(this->type != task_type::integration)
          throw runtime_exception(exception_type::RUNTIME_ERROR, CPPTRANSPORT_TASK_CONTENT_GROUP_OUT_OF_RANGE);

        return statistics;
      }


    bool content_group_specifier::requires_spectral_data() const
      {
        if(this->type != task_type::integration)
          throw runtime_exception(exception_type::RUNTIME_ERROR, CPPTRANSPORT_TASK_CONTENT_GROUP_OUT_OF_RANGE);

        return spectral_data;
      }


    const precomputed_products& content_group_specifier::requires_products() const
      {
        if(this->type != task_type::postintegration)
          throw runtime_exception(exception_type::RUNTIME_ERROR, CPPTRANSPORT_TASK_CONTENT_GROUP_OUT_OF_RANGE);

        return products;
      }


    content_group_specifier& content_group_specifier::operator|=(const content_group_specifier& obj)
      {
        if(this->type != obj.type)
          throw runtime_exception(exception_type::RUNTIME_ERROR, CPPTRANSPORT_TASK_CONTENT_GROUP_BAD_MERGE);

        switch(this->type)
          {
            case task_type::integration:
              {
                this->ics |= obj.ics;
                this->statistics |= obj.statistics;
                this->spectral_data |= obj.spectral_data;
                break;
              }

            case task_type::postintegration:
              {
                this->products |= obj.products;
                break;
              }

            case task_type::output:
              throw runtime_exception(exception_type::RUNTIME_ERROR, CPPTRANSPORT_TASK_CONTENT_GROUP_BAD_TASK_TYPE);
          }

        return *this;
      }


    bool operator==(const content_group_specifier& a, const content_group_specifier& b)
      {
        auto a_type = a.get_type();
        auto b_type = b.get_type();

        if(a_type != b_type)
          return false;

        switch(a_type)
          {
            case task_type::integration:
              {
                return a.requires_ics() == b.requires_ics() &&
                       a.requires_statistics() == b.requires_statistics() &&
                       a.requires_spectral_data() == b.requires_spectral_data();
              }

            case task_type::postintegration:
              {
                return a.requires_products() == b.requires_products();
              }

            case task_type::output:
              throw runtime_exception(exception_type::RUNTIME_ERROR, CPPTRANSPORT_TASK_CONTENT_GROUP_BAD_TASK_TYPE);
          }
      }


    bool operator==(const integration_payload& payload, const content_group_specifier& b)
      {
        // no match if content group specifier is not of integration type
        if(b.get_type() != task_type::integration)
          return false;

        if(b.requires_ics() && !payload.has_initial_conditions())
          return false;

        if(b.requires_statistics() && !payload.has_initial_conditions())
          return false;

        if(b.requires_spectral_data() && !payload.has_spectral_data())
          return false;

        return true;
      }


    bool operator==(const postintegration_payload& payload, const content_group_specifier& b)
      {
        // no match if content group specifier is not of postintegration type
        if(b.get_type() != task_type::postintegration)
          return false;

        return payload.get_precomputed_products() == b.requires_products();
      }

  }   // namespace transport


#endif //CPPTRANSPORT_CONTENT_GROUP_DECL_H
