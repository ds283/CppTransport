//
// Created by David Seery on 30/11/2015.
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
// @contributor: Alessandro Maraio <am963@sussex.ac.uk>
// --@@
//


#include "translator_data.h"


translator_data::translator_data(const boost::filesystem::path& file, error_context::error_handler e,
                                 error_context::warning_handler w, message_handler m, finder& f, output_stack& os, symbol_factory& s,
                                 model_descriptor& desc, argument_cache& c, version_policy& vp)
  : filename(file),
    err(e),
    wrn(w),
    msg(m),
    search_paths(f),
    outstack(os),
    sym_factory(s),
    root(desc),
    model(desc.model),
    meta(desc.meta),
    templates(desc.templates),
    misc(desc.misc),
    cosmo(desc.cosmo),
    cache(c),
    policy(vp)
  {
  }


bool translator_data::do_cse() const
  {
    return(this->cache.do_cse());
  }


bool translator_data::annotate() const
  {
    return(this->cache.annotate());
  }


unsigned int translator_data::unroll_policy() const
  {
    return(this->cache.unroll_policy());
  }


bool translator_data::fast() const
  {
    return(this->cache.fast());
  }


void translator_data::set_core_implementation(const boost::filesystem::path& co, const std::string& cg,
                                              const boost::filesystem::path& io, const std::string& ig)
  {
    this->core_output = co;
    this->core_guard = cg;
    this->implementation_output = io;
    this->implementation_guard = ig;
  }

void translator_data::set_sampling_output(const boost::filesystem::path& so, const std::string& sg,
                                          const boost::filesystem::path& sv, const boost::filesystem::path& sp,
                                          const boost::filesystem::path& sm, const boost::filesystem::path& sc,
                                          const boost::filesystem::path& samplinggetdistPy,
                                          const boost::filesystem::path& samplinggetdistLatex)
{
  this->sampling_output         = so;
  this->sampling_guard          = sg;
  this->sampling_mcmc_output    = sm;
  this->sampling_values_output  = sv;
  this->sampling_priors_output  = sp;
  this->sampling_cmake_output   = sc;
  this->sampling_getdist_python_output = samplinggetdistPy;
  this->sampling_getdist_latex_output  = samplinggetdistLatex;
}
