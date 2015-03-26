//
// Created by David Seery on 30/12/2013.
// Copyright (c) 2013-15 University of Sussex. All rights reserved.
//


#ifndef __repository_creation_key_H_
#define __repository_creation_key_H_


namespace transport
  {

    class repository_creation_key
      {
      public:
        //! Default constructor
        repository_creation_key()
          {
          }

      private:
        //! Private copy constructor makes it more difficult to replicate this object
        repository_creation_key(const repository_creation_key& obj)
          {
          }
      };

  }   // namespace transport


#endif //__repository_creation_key_H_
