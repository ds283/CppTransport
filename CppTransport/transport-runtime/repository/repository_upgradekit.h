//
// Created by David Seery on 08/08/2017.
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

#ifndef CPPTRANSPORT_REPOSITORY_UPGRADEKIT_H
#define CPPTRANSPORT_REPOSITORY_UPGRADEKIT_H


#include "transport-runtime/repository/repository.h"

#include "transport-runtime/manager/argument_cache.h"
#include "transport-runtime/manager/message_handlers.h"


namespace transport
  {
    
    namespace upgradekit_impl
      {
        
        class NotifyGadget
          {
            
            // CONSTRUCTOR, DESTRUCTOR
            
          public:
            
            //! constructor captures name of container
            NotifyGadget(boost::filesystem::path c, message_handler mh)
              : ctr(std::move(c)),
                msg(mh)
              {
              }
            
            //! destructor is default
            ~NotifyGadget() = default;
            
            
            // INTERFACE
            
          public:
            
            //! issue notification
            void operator()();
            
            
            // INTERNAL DATA
            
          private:
            
            //! container name
            boost::filesystem::path ctr;
            
            //! message handler
            message_handler msg;
            
            //! notification status
            bool notify{false};
            
          };
        
        
        void NotifyGadget::operator()()
          {
            if(!notify)
              {
                std::ostringstream msg_str;
                msg_str << CPPTRANSPORT_UPGRADEKIT_UPGRADING << " '" << this->ctr.string() << "'";
                this->msg(msg_str.str());
                
                notify = true;
              }
          }
        
      }   // namespace upgradekit_impl
    
    
    template <typename number>
    class repository_upgradekit
      {
        
        // CONSTRUCTOR, DESTRUCTOR
        
      public:
    
        //! constructor captures resources
        repository_upgradekit(repository <number>& r, argument_cache& ac,
                              error_handler eh, warning_handler wh, message_handler mh)
          : repo(r),
            args(ac),
            err(eh),
            warn(wh),
            msg(mh),
            transactions(0)
          {
          }
        
        //! destructor is default
        virtual ~repository_upgradekit() = default;
    
    
        // INTERFACE
  
      public:
    
        //! perform upgrade
        virtual void operator()() = 0;
    
    
        // INTERNAL DATA
  
      protected:
    
        //! reference to repository object
        repository<number>& repo;
    
        //! reference to local argument cache
        argument_cache& args;
    
    
        // TRANSACTIONS
    
        //! count number of active transactions
        unsigned int transactions;
    
    
        // MESSAGE HANDLERS
    
        //! error handler
        error_handler err;
    
        //! warning handler
        warning_handler warn;
    
        //! message handler
        message_handler msg;
    
      };
    
  }   // namespace transport


#endif //CPPTRANSPORT_REPOSITORY_UPGRADEKIT_H
