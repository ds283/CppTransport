//
// Created by David Seery on 01/04/15.
// Copyright (c) 2015-2016 University of Sussex. All rights reserved.
//


#ifndef CPPTRANSPORT_TRANSACTION_MANAGER_H
#define CPPTRANSPORT_TRANSACTION_MANAGER_H

#include <list>
#include <functional>
#include <thread>
#include <chrono>


#include "transport-runtime-api/version.h"
#include "transport-runtime-api/messages.h"
#include "transport-runtime-api/exceptions.h"

#include "boost/filesystem/operations.hpp"


namespace transport
	{

    class transaction_handler
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! constructor is default
        transaction_handler() = default;

        //! destructor is default
        ~transaction_handler() = default;


        // INTERFACE

      public:

        //! open transaction
        virtual void open() = 0;

        //! commit transaction
        virtual void commit() = 0;

        //! rollback transaciton
        virtual void rollback() = 0;

        //! release transaction
        virtual void release() = 0;

      };


    namespace transaction_manager_impl
      {

        //! abstract class giving interface definition for transaction journal records
        class journal_record
          {

            // CONSTRUCTOR, DESTRUCTOR

          public:

            //! constructor is default
            journal_record() = default;

            //! destructor is default
            virtual ~journal_record() = default;


            // INTERFACE

          public:

            virtual void commit() = 0;

            virtual void rollback() = 0;

          };


        //! concrete transaction journal record, corresponding to a file emplacement
        class emplace_record : public journal_record
          {

            // CONSTRUCTOR, DESTRUCTOR

          public:

            emplace_record(const boost::filesystem::path& j, const boost::filesystem::path& t)
              : journal(j),
                target(t)
              {
              }

            // TODO: Intel compiler complains if destructor is explicitly defaulted; can probably be reverted with new version of compiler [Intel say this issue is 'fixed']
            virtual ~emplace_record()
              {
              }


            // INTERFACE

          public:

            virtual void commit() override { boost::filesystem::rename(journal, target); }

            virtual void rollback() override { boost::filesystem::remove(journal); }


            // INTERNAL DATA

          private:

            //! journalled file
            boost::filesystem::path journal;

            //! target file
            boost::filesystem::path target;

          };

      }


    // import transaction_manager_impl for this block
    using namespace transaction_manager_impl;


    //! transaction manager
    class transaction_manager
	    {

		    // CONSTRUCTOR, DESTRUCTOR

      public:

        //! constructor captures ownership of handler object
        transaction_manager(boost::filesystem::path l, std::unique_ptr<transaction_handler> h);

		    // allow moving
		    transaction_manager(transaction_manager&& obj) = default;

		    // disable copying
		    transaction_manager(const transaction_manager& obj) = delete;

		    // disable assignment
		    transaction_manager& operator=(const transaction_manager& obj) = delete;

        //! destructor should rollback if not committed
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

        //! owned pointer to handlers
        std::unique_ptr<transaction_handler> handler;


        // LOCKFILE

        //! lockfile location
        boost::filesystem::path lockfile;


        // STATUS

		    //! has this transaction been committed?
		    bool committed;

		    //! has this transaction been rolled back?
		    bool dead;

		    //! file journal
		    std::list< std::unique_ptr<journal_record> > journal;

	    };


		transaction_manager::transaction_manager(boost::filesystem::path l, std::unique_ptr<transaction_handler> h)
			: handler(std::move(h)),
        lockfile(std::move(l)),
			  committed(false),
				dead(false)
			{
        unsigned int attempts = CPPTRANSPORT_DEFAULT_LOCKFILE_ATTEMPTS;
        bool locked = false;

        // set up lockfile
        while(!locked && attempts > 0)
          {
            if(boost::filesystem::exists(lockfile))
              {
                // repository commits normally don't take a long time, so sleep for a second
                // before trying again
                std::this_thread::sleep_for(std::chrono::seconds(1));
                --attempts;
              }
            else
              {
                locked = true;
              }
          }

        if(!locked) throw runtime_exception(exception_type::REPOSITORY_TRANSACTION_ERROR, CPPTRANSPORT_REPO_TRANSACTION_NO_LOCK);

        // no lockfile is present, so make one -- then we have exclusive access to the database until
        // the lockfile is removed
        std::ofstream make_lock(lockfile.string(), std::ios::out | std::ios::trunc);
        make_lock.close();

		    handler->open();
			}


    transaction_manager::~transaction_manager()
			{
		    // rollback the transaction if it was not committed
		    if(!this->committed && !this->dead) this->handler->rollback();

        // remove lockfile if present, releasing our exclusive lock on the database
        if(boost::filesystem::exists(this->lockfile)) boost::filesystem::remove(this->lockfile);
			}


		// TRANSACTION MANAGEMENT


		void transaction_manager::commit()
			{
        // check lockfile is present
        if(!boost::filesystem::exists(this->lockfile))
          {
            this->rollback();
            throw runtime_exception(exception_type::REPOSITORY_TRANSACTION_ERROR, CPPTRANSPORT_REPO_TRANSACTION_LOST_LOCK);
          }

				// work through the journal, committing
        for(std::unique_ptr<journal_record>& rec : this->journal)
					{
				    rec->commit();
					}

				this->handler->commit();
				this->committed = true;
				this->handler->release();

        // remove lockfile, releasing our exclusive lock on the database
        if(boost::filesystem::exists(this->lockfile)) boost::filesystem::remove(this->lockfile);
			}


		void transaction_manager::rollback()
			{
				// work through the journal, rolling back
        for(std::unique_ptr<journal_record>& rec : this->journal)
					{
				    rec->rollback();
					}

        this->handler->rollback();
				this->dead = true;
        this->handler->release();
			}


		// JOURNALLING


		void transaction_manager::journal_deposit(const boost::filesystem::path& journal, const boost::filesystem::path& target)
			{
				if(this->committed) throw runtime_exception(exception_type::REPOSITORY_TRANSACTION_ERROR, CPPTRANSPORT_REPO_TRANSACTION_COMMITTED);
				if(this->dead)      throw runtime_exception(exception_type::REPOSITORY_TRANSACTION_ERROR, CPPTRANSPORT_REPO_TRANSACTION_DEAD);

				this->journal.push_back(std::make_unique<emplace_record>(journal, target));
			}


	}


#endif //CPPTRANSPORT_TRANSACTION_MANAGER_H
