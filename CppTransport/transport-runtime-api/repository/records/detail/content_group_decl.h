//
// Created by David Seery on 25/01/2016.
// Copyright (c) 2016 University of Sussex. All rights reserved.
//

#ifndef CPPTRANSPORT_CONTENT_GROUP_DECL_H
#define CPPTRANSPORT_CONTENT_GROUP_DECL_H


namespace transport
  {

    // OUTPUT DESCRIPTORS

    //! Derived product descriptor. Used to enumerate the content associated with
    //! a output task content group.
    class derived_content: public serializable
      {

      public:

        //! Create a derived_product descriptor
        derived_content(const std::string& prod, const std::string& fnam, const boost::posix_time::ptime& now,
                        const std::list<std::string>& gp, const std::list<note>& nt, const std::list<std::string>& tg)
          : parent_product(prod),
            filename(fnam),
            created(now),
            content_groups(gp),
            notes(nt),
            tags(tg)
          {
          }

        //! Deserialization constructor
        derived_content(Json::Value& reader);

        //! Destroy a derived_product descriptor
        ~derived_content() = default;


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

        //! Get creation time
        const boost::posix_time::ptime& get_creation_time() const { return(this->created); }

        //! Get content groups
        const std::list<std::string>& get_content_groups() const { return(this->content_groups); }


        // SERIALIZATION -- implements a 'serializable' interface

      public:

        //! Serialize this object
        virtual void serialize(Json::Value& writer) const override;


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
        std::list<std::string> tags;

        //! content groups used to create
        std::list<std::string> content_groups;

      };


    // PAYLOADS FOR OUTPUT GROUPS
    class precomputed_products: public serializable
      {

      public:

        //! Create a precomputed products record
        precomputed_products()
          : zeta_twopf(false),
            zeta_threepf(false),
            zeta_redbsp(false),
            fNL_local(false),
            fNL_equi(false),
            fNL_ortho(false),
            fNL_DBI(false)
          {
          }

        //! Deserialization constructor
        precomputed_products(Json::Value& reader);

        //! Destroy a precomputed products record
        ~precomputed_products() = default;


        // GET AND SET PROPERTIES

      public:

        //! Get precomputed zeta_twopf availability
        bool get_zeta_twopf() const { return(this->zeta_twopf); }
        //! Add zeta_twopf availability flag
        void add_zeta_twopf() { this->zeta_twopf = true; }

        //! Get precomputed zeta_threepf availability
        bool get_zeta_threepf() const { return(this->zeta_threepf); }
        //! Add zeta_threepf availability flag
        void add_zeta_threepf() { this->zeta_threepf = true; }

        //! Get precomputed zeta reduced bispectrum availability
        bool get_zeta_redbsp() const { return(this->zeta_redbsp); }
        //! Add zeta_threepf availability flag
        void add_zeta_redbsp() { this->zeta_redbsp = true; }

        //! Get precomputed fNL_local availability
        bool get_fNL_local() const { return(this->fNL_local); }
        //! Add fNL_local availability flag
        void add_fNL_local() { this->fNL_local = true; }

        //! Get precomputed fNL_equi availability
        bool get_fNL_equi() const { return(this->fNL_equi); }
        //! Add fNL_equi availability flag
        void add_fNL_equi() { this->fNL_equi = true; }

        //! Get precomputed fNL_ortho availability
        bool get_fNL_ortho() const { return(this->fNL_ortho); }
        //! Add fNL_ortho availability flag
        void add_fNL_ortho() { this->fNL_ortho = true; }

        //! Get precomputed fNL_DBI availability
        bool get_fNL_DBI() const { return(this->fNL_DBI); }
        //! Add fNL_DBI availability flag
        void add_fNL_DBI() { this->fNL_DBI = true; }


        // WRITE TO A STREAM

      public:

        template <typename Stream> void write(Stream& out) const;


        // SERIALIZATION -- implements a 'serializable' interface

      public:

        //! Serialize this object
        virtual void serialize(Json::Value& writer) const override;


        // INTERNAL DATA

      protected:

        //! group has pre-computed zeta twopf data?
        bool zeta_twopf;

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


    //! Integration payload
    class integration_payload: public serializable
      {

      public:

        //! Create a payload
        integration_payload()
          : metadata(),
            fail(false),
            workgroup_number(0),
            seeded(false),
            statistics(false),
            initial_conditions(false),
            size(0)
          {
          }

        //! Deserialization constructor
        integration_payload(Json::Value& reader);

        //! Destroy a payload
        ~integration_payload() = default;


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
        const std::list<unsigned int>& get_failed_serials() const { return(this->failed_serials); }

        //! Set list of failed serial numbers
        void set_failed_serials(const std::list<unsigned int>& f) { this->failed_serials = f; }

        //! Get workgroup number
        unsigned int get_workgroup_number() const { return(this->workgroup_number); }

        //! Set workgroup number
        void set_workgroup_number(unsigned int w) { this->workgroup_number = w; }

        //! Set seed
        void set_seed(const std::string& s) { this->seeded = true; this->seed_group = s; }

        //! Query seeded status
        bool is_seeded() const { return(this->seeded); }

        //! Query seed group
        const std::string& get_seed_group() const { return(this->seed_group); }

        //! Set statistics flag
        void set_statistics(bool g) { this->statistics = g; }

        //! Get statistics flag
        bool has_statistics() const { return(this->statistics); }

        //! Set initial conditions flag
        void set_initial_conditions(bool g) { this->initial_conditions = g;}

        //! Get initial conditions flag
        bool has_initial_conditions() const { return(this->initial_conditions); }

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
        virtual void serialize(Json::Value& writer) const override;


        // INTERNAL DATA

      protected:

        //! Path to data container
        boost::filesystem::path container;

        //! Metadata
        integration_metadata metadata;

        //! mark this group as failed?
        bool fail;

        //! serial numbers reported failed
        std::list< unsigned int > failed_serials;

        //! workgroup number associated with this integration
        unsigned int workgroup_number;

        //! was this integration seeded?
        bool seeded;

        //! if this integration was seeded, parent content group
        std::string seed_group;

        //! does this group have per-configuration statistics?
        bool statistics;

        //! does this group has initial conditions data?
        bool initial_conditions;

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
        postintegration_payload(Json::Value& reader);

        //! Destroy a payload
        ~postintegration_payload() = default;


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


        // GET AND SET PROPERTIES

      public:

        //! Get precomputed products record
        precomputed_products& get_precomputed_products() { return(this->precomputed); }

        const precomputed_products& get_precomputed_products() const { return(this->precomputed); }

        //! Get fail status
        bool is_failed() const { return(this->fail); }

        //! Set fail status
        void set_fail(bool g) { this->fail = g; }

        //! Get list of failed serial numbers
        const std::list<unsigned int>& get_failed_serials() const { return(this->failed_serials); }

        //! Set list of failed serial numbers
        void set_failed_serials(const std::list<unsigned int>& f) { this->failed_serials = f; }

        //! Set pair
        void set_pair(bool g) { this->paired = g; }

        //! Query paired status
        bool is_paired() const { return(this->paired); }

        //! Set parent content group
        void set_parent_group(const std::string& p) { this->parent_group = p; }

        //! Query paired gorup
        const std::string& get_parent_group() const { return(this->parent_group); }

        //! Set seed
        void set_seed(const std::string& s) { this->seeded = true; this->seed_group = s; }

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
        bool fail;

        //! serial numbers reported failed
        std::list< unsigned int > failed_serials;

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
        output_payload(Json::Value& reader);

        //! Destroy a payload
        ~output_payload() = default;


        // ADMIN

      public:

        //! Add an output
        void add_derived_content(const derived_content& prod) { this->content.push_back(prod); }

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
        const std::list<std::string>& get_content_groups_summary() const { return(this->used_groups); }

        //! Set list of used content groups
        void set_content_groups_summary(const std::list<std::string>& list) { this->used_groups = list; }


        // WRITE TO A STREAM

      public:

        //! write details
        template <typename Stream> void write(Stream& out) const;


        // SERIALIZATION -- implements a 'serializable' interface

      public:

        //! Serialize this object
        virtual void serialize(Json::Value& writer) const override;


        // INTERNAL DATA

      protected:

        //! List of derived outputs
        std::list<derived_content> content;

        //! Metadata
        output_metadata metadata;

        //! failed flag
        bool fail;

        //! list of content groups used to produce this output
        std::list<std::string> used_groups;

      };


    //! Content group descriptor. Used to enumerate the content groups available for a particular task
    template <typename Payload>
    class content_group_record: public repository_record
      {

      public:

        //! make payload type available for template programming
        typedef Payload payload_type;

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
        content_group_record(const std::string& tn, const paths_group& p,
                            bool lock, const std::list<note>& nt, const std::list<std::string>& tg,
                            repository_record::handler_package& pkg);

        //! Deserialization constructor
        content_group_record(Json::Value& reader, const boost::filesystem::path& root, repository_record::handler_package& pkg);

        //! Destroy a content_group_record descriptor
        ~content_group_record() = default;


        // GET AND SET METADATA

      public:

        //! Get task name
        const std::string& get_task_name() const { return(this->task); }

        //! Get locked status
        bool get_lock_status() const { return (this->locked); }

        //! Set locked status
        void set_lock_status(bool g);

        //! Get notes
        const std::list<note>& get_notes() const { return (this->notes); }

        //! Add note
        void add_note(const std::string& note);

        //! Remove numbered note
        void remove_note(unsigned int number);

        //! Get tags
        const std::list<std::string>& get_tags() const { return (this->tags); }

        //! Add tag
        void add_tag(const std::string& tag);

        //! Remove tag
        void remove_tag(const std::string& tag);

        //! Check whether we match a set of tags
        bool check_tags(std::list<std::string> match_tags) const;


        // ABSOLUTE PATHS

      public:

        //! Get path to repository root
        const boost::filesystem::path& get_abs_repo_path() const { return(this->paths.root); }

        //! Get path to output root (typically a subdir of the repository root)
        const boost::filesystem::path get_abs_output_path() const { return(this->paths.root/this->paths.output); }


        // PAYLOAD

      public:

        //! Get payload
        Payload& get_payload()             { return(this->payload); }

        //! Get payload (const version)
        const Payload& get_payload() const { return(this->payload); }


        // SERIALIZATION -- implements a 'serializable' interface

      public:

        //! Serialize this object
        virtual void serialize(Json::Value& writer) const override;


        // CLONE

      public:

        //! clone this object
        virtual content_group_record<Payload>* clone() const override { return new content_group_record<Payload>(static_cast<const content_group_record<Payload>&>(*this)); };


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
        std::list<std::string> tags;


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


  }   // namespace transport


#endif //CPPTRANSPORT_CONTENT_GROUP_DECL_H
