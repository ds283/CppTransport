//
// Created by David Seery on 03/04/2016.
// Copyright (c) 2016 University of Sussex. All rights reserved.
//

#ifndef CPPTRANSPORT_REPOSITORY_MODE_H
#define CPPTRANSPORT_REPOSITORY_MODE_H


namespace transport
  {

    // specify read/write mode for an entire repository
    enum class repository_mode
      {
        readonly,
        readwrite
      };

  }


#endif //CPPTRANSPORT_REPOSITORY_MODE_H
