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
        virtual ~transaction_handler() = default;


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

            virtual void commit() override { boost::filesystem::rename(this->journal, this->target); }    // boost::filesystem::rename() will remove any existing file at target

            virtual void rollback() override { boost::filesystem::remove(this->journal); }


            // INTERNAL DATA

          private:

            //! journalled file
            boost::filesystem::path journal;

            //! target file
            boost::filesystem::path target;

          };


        //! concrete transaction journal record, corresponding to moving a file or directory
        class move_record : public journal_record
          {

            // CONSTRUCTOR, DESTRUCTOR

          public:

            move_record(const boost::filesystem::path& s, const boost::filesystem::path& t)
              : source(s),
                target(t)
              {
              }

            // TODO: Intel compiler complains if destructor is explicitly defaulted; can probably be reverted with new version of compiler [Intel say this issue is 'fixed']
            virtual ~move_record()
              {
              }


            // INTERFACE

          public:

            virtual void commit() override { boost::filesystem::rename(this->source, this->target); }    // boost::filesystem::rename() will remove any existing file at target

            virtual void rollback() override { ; }    // do nothing


            // INTERNAL DATA

          private:

            //! journalled file
            boost::filesystem::path source;

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

		    //! Add an emplacement record to the journal
        //! On commit, the temporary file located at 'journal' is moved to its permanent home at 'target'
        //! 'target' is removed if it already exists.
        //! On rollback, 'journal' is removed.
		    void journal_deposit(const boost::filesystem::path& journal, const boost::filesystem::path& target);

        //! Add a move record to the journal
        //! On commit, the object located at 'source' is moved to 'target'
        //! On rollback, nothing happens
        void journal_move(const boost::filesystem::path& source, const boost::filesystem::path& target);

		    // INTERNAL DATA

      private:

		    // DATABASE OPEN, COMMIT AND ROLLBACK HANDLERS

        //! owned pointer to handlers
        std::unique_ptr<transaction_handler> handler;


        // LOCKFILE

        //! lockfile location
        const boost::filesystem::path lockfile;

        //! magic string written to lockfile to identify ourselves
        std::string magic_string;


        // STATUS

		    //! has this transaction been committed?
		    bool committed;

		    //! has this transaction been rolled back?
		    bool dead;

		    //! file journal
		    std::list< std::unique_ptr<journal_record> > journal;

	    };


		transaction_manager::transaction_manager(const boost::filesystem::path l, std::unique_ptr<transaction_handler> h)
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

        if(!locked) throw runtime_exception(exception_type::TRANSACTION_ERROR, CPPTRANSPORT_TRANSACTION_NO_LOCK);

        // no lockfile is present, so make one -- then we have exclusive access to the database until
        // the lockfile is removed
        std::ofstream make_lock(lockfile.string(), std::ios::out | std::ios::trunc);

        // write magic string consisting of current POSIX time, used to identify if lockfile has been changed
        // by another process
        boost::posix_time::ptime now = boost::posix_time::second_clock::local_time();
        magic_string = boost::posix_time::to_iso_string(now);
        make_lock << magic_string;

        make_lock.close();
		    handler->open();
			}


    transaction_manager::~transaction_manager()
			{
		    // rollback the transaction if it was not committed
		    if(!this->committed && !this->dead)
          {
            try
              {
                this->rollback();
              }
            catch(runtime_exception& xe)
              {
                // indicates that exclusive lock was lost, and lockfile has not been removed.
                // Here we opt to just swallow the exception rather than risking throw from
                // inside a destructor.
                // Probably --recover will need to be run on the repository later.
              }
          }
			}


		// TRANSACTION MANAGEMENT


		void transaction_manager::commit()
			{
        // check lockfile is present; if not, we have somehow lost the exclusive lock
        // so rollback and throw an exception
        if(!boost::filesystem::exists(this->lockfile))
          {
            this->handler->rollback();
            this->handler->release();

            std::ostringstream msg;
            msg << CPPTRANSPORT_TRANSACTION_LOST_LOCK << " [" << this->lockfile.string() << "]" << '\n';

            throw runtime_exception(exception_type::TRANSACTION_ERROR, msg.str());
          }

        // read magic string from lockfile, and check it matches what we expect
        // if not, rollback and throw an exception
        std::ifstream lock_stream(this->lockfile.string(), std::ios::in);
        std::string read_magic;
        lock_stream >> read_magic;
        lock_stream.close();

        if(read_magic != this->magic_string)
          {
            this->handler->rollback();
            this->handler->release();
            throw runtime_exception(exception_type::TRANSACTION_ERROR, CPPTRANSPORT_TRANSACTION_LOST_LOCK);
          }

        // all is well with the locking, so proceed to commit

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
        // First, unwind all journalled actions

				// work through the journal, rolling back each record
        for(std::unique_ptr<journal_record>& rec : this->journal)
					{
				    rec->rollback();
					}

        this->handler->rollback();
				this->dead = true;
        this->handler->release();

        // Second, check status of locking; we should still have an exclusive lock on the database,
        // and if not something has gone wrong

        if(!boost::filesystem::exists(this->lockfile))
          {
            throw runtime_exception(exception_type::TRANSACTION_ERROR, CPPTRANSPORT_TRANSACTION_LOST_LOCK);
          }

        // read magic string from lockfile, and check it matches what we expect
        std::ifstream lock_stream(this->lockfile.string(), std::ios::in);
        std::string read_magic;
        lock_stream >> read_magic;
        lock_stream.close();

        if(read_magic != this->magic_string)
          {
            throw runtime_exception(exception_type::TRANSACTION_ERROR, CPPTRANSPORT_TRANSACTION_LOST_LOCK);
          }

        // remove lockfile if exists, releasing our exclusive lock on the database
        if(boost::filesystem::exists(this->lockfile)) boost::filesystem::remove(this->lockfile);
			}


		// JOURNALLING


		void transaction_manager::journal_deposit(const boost::filesystem::path& journal, const boost::filesystem::path& target)
			{
				if(this->committed) throw runtime_exception(exception_type::TRANSACTION_ERROR, CPPTRANSPORT_TRANSACTION_COMMITTED);
				if(this->dead)      throw runtime_exception(exception_type::TRANSACTION_ERROR, CPPTRANSPORT_TRANSACTION_DEAD);

				this->journal.push_back(std::make_unique<emplace_record>(journal, target));
			}


    void transaction_manager::journal_move(const boost::filesystem::path& source, const boost::filesystem::path& target)
      {
        if(this->committed) throw runtime_exception(exception_type::TRANSACTION_ERROR, CPPTRANSPORT_TRANSACTION_COMMITTED);
        if(this->dead)      throw runtime_exception(exception_type::TRANSACTION_ERROR, CPPTRANSPORT_TRANSACTION_DEAD);

        this->journal.push_back(std::make_unique<move_record>(source, target));
      }


	}


#endif //CPPTRANSPORT_TRANSACTION_MANAGER_H
