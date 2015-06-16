//
// Created by David Seery on 01/04/15.
// Copyright (c) 2015 University of Sussex. All rights reserved.
//


#ifndef __transaction_manager_H_
#define __transaction_manager_H_

#include <list>
#include <functional>


#include "transport-runtime-api/version.h"
#include "transport-runtime-api/messages.h"
#include "transport-runtime-api/exceptions.h"

#include "boost/filesystem/operations.hpp"


namespace transport
	{

    //! transaction manager
    class transaction_manager
	    {

      public:

        typedef std::function<void()> open_handler;
        typedef std::function<void()> commit_handler;
		    typedef std::function<void()> rollback_handler;
		    typedef std::function<void()> release_handler;

      protected:

		    class journal_record
			    {

		      public:

				    journal_record() = default;

				    virtual ~journal_record() = default;

				    virtual void commit() = 0;

				    virtual void rollback() = 0;

			    };

		    class rename_record: public journal_record
			    {

		      public:

				    rename_record(const boost::filesystem::path& j, const boost::filesystem::path& t)
					    : journal(j),
		            target(t)
					    {
					    }

				    // TODO: Intel compiler complains if destructor is explicitly defaulted; can probably be reverted with new version of compiler [Intel say this issue is 'fixed']
				    virtual ~rename_record()
					    {
					    }

				    virtual void commit() override { boost::filesystem::rename(journal, target); }

				    virtual void rollback() override { boost::filesystem::remove(journal); }

		      private:

				    //! journaled file
				    boost::filesystem::path journal;

				    //! target file
				    boost::filesystem::path target;

			    };


		    // CONSTRUCTOR, DESTRUCTOR

      public:

        transaction_manager(open_handler& o, commit_handler& c, rollback_handler& r, release_handler& rel);

		    // allow moving
		    transaction_manager(transaction_manager&& obj) = default;

		    // disable copying
		    transaction_manager(const transaction_manager& obj) = delete;

		    // disable assignment
		    transaction_manager& operator=(const transaction_manager& obj) = delete;

        ~transaction_manager();


		    // TRANSACTION MANAGEMENT

      public:

		    //! commit this transaction
		    void commit();

		    //! rollback this transaction
		    void rollback();


		    // JOURNALLING

      public:

		    //! Add a record to the journal
		    void journal_deposit(const boost::filesystem::path& journal, const boost::filesystem::path& target);


		    // INTERNAL DATA

      private:

		    // DATABASE OPEN, COMMIT AND ROLLBACK HANDLERS

		    //! begin a new transaction on the database
        open_handler opener;

		    //! commit a transaction to the database
        commit_handler committer;

		    //! rollback a transaction
		    rollback_handler rollbacker;

		    //! notify owner to release resources following a commit
		    release_handler releaser;

		    //! has this transaction been committed?
		    bool committed;

		    //! has this transaction been rolled back?
		    bool dead;

		    //! file journal
		    std::list< journal_record* > journal;

	    };


		transaction_manager::transaction_manager(open_handler& o, commit_handler& c, rollback_handler& r, release_handler& rel)
			: opener(o),
			  committer(c),
			  rollbacker(r),
			  releaser(rel),
			  committed(false),
				dead(false)
			{
		    opener();
			}


    transaction_manager::~transaction_manager()
			{
		    // rollback the transaction if it was not committed
		    if(!this->committed) this->rollback();

		    for(std::list< journal_record* >::iterator t = this->journal.begin(); t != this->journal.end(); ++t)
			    {
				    delete *t;
			    }
			}


		// TRANSACTION MANAGEMENT


		void transaction_manager::commit()
			{
				// work through the journal, committing
				for(std::list< journal_record* >::iterator t = this->journal.begin(); t != this->journal.end(); ++t)
					{
				    (*t)->commit();
					}

				this->committer();
				this->committed = true;
				this->releaser();
			}


		void transaction_manager::rollback()
			{
				// work through the journal, rolling back
				for(std::list< journal_record* >::iterator t = this->journal.begin(); t != this->journal.end(); ++t)
					{
				    (*t)->rollback();
					}

				this->rollbacker();
				this->dead = true;
				this->releaser();
			}


		// JOURNALLING


		void transaction_manager::journal_deposit(const boost::filesystem::path& journal, const boost::filesystem::path& target)
			{
				if(this->committed) throw runtime_exception(runtime_exception::REPOSITORY_TRANSACTION_ERROR, __CPP_TRANSPORT_REPO_TRANSACTION_COMMITTED);
				if(this->dead)      throw runtime_exception(runtime_exception::REPOSITORY_TRANSACTION_ERROR, __CPP_TRANSPORT_REPO_TRANSACTION_DEAD);

				this->journal.push_back(new rename_record(journal, target));
			}


	}


#endif //__transaction_manager_H_
