//
// Created by David Seery on 01/12/2015.
// Copyright (c) 2013-2016 University of Sussex. All rights reserved.
//

#include "lexstream_data.h"


lexstream_data::lexstream_data(const boost::filesystem::path& name, error_context::error_handler e, error_context::warning_handler w, finder& s, argument_cache& c)
  : filename(name),
    err(e),
    wrn(w),
    cache(c),
    search_paths(s)
  {
  }
