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
// --@@
//

#ifndef CPPTRANSPORT_ARGUMENT_CACHE_H
#define CPPTRANSPORT_ARGUMENT_CACHE_H


#include <list>

#include "local_environment.h"

#include "boost/program_options.hpp"
#include "boost/filesystem/operations.hpp"


class argument_cache
  {

    // TYPES

  public:

    //! payload for a message
    using message_payload = std::pair< bool, std::string >;


    // CONSTRUCTOR, DESTRUCTOR

  public:

    //! constructor
    argument_cache(int argc, const char* argv[], local_environment& env);

    //! destructor is default
    ~argument_cache() = default;


    // MISCELLANEOUS OPTIONS

  public:

    //! get verbose output setting
    bool verbose() const { return(this->verbose_flag); }


    // CONFIGURATION OPTIONS

  public:

    //! get colourized setting
    bool colourize() const { return(this->colour_flag); }

    //! search environment paths for eg. template files?
    bool search_environment() const { return(!this->no_search_environment); }

    //! get core output file
    const std::string& core_out() const { return(this->core_output); };

    //! get implementation output file
    const std::string& implementation_out() const { return(this->implementation_output); }

    //! get search paths
    const std::list<boost::filesystem::path>& search_paths() const { return(this->search_path_list); }


    // CODE GENERATION OPTIONS

  public:

    //! get common-subexpression elimination setting
    bool do_cse() const { return(this->cse_flag); }

    //! get code annotation setting
    bool annotate() const { return(this->annotate_flag); }

    //! get unrolling policy
    unsigned int unroll_policy() const { return(this->unroll_policy_size); }

    bool fast() const { return(this->fast_flag); }


    // WARNINGS

  public:

    //! display profiling data?
    bool show_profiling() const { return this->profile_flag; }

    //! display developer warnings?
    bool report_developer_warnings() const { return this->develop_warnings; }
    
    //! report unroll policy violations?
    bool report_unroll_warnings() const { return this->unroll_warnings; }

    //! report on-the-fly index repositioning?
    bool report_reposition_warnings() const { return this->reposition_warnings; }


    // INPUT FILES

  public:

    //! get input files
    const std::list<boost::filesystem::path>& input_files() const { return(this->input_file_list); }


    // GET ERROR MESSAGES GENERATED DURING CONSTRUCTION

  public:

    //! get messages
    const std::list< message_payload >& get_messages() const { return this->err_msgs; }


    // INTERNAL DATA

  private:

    // OPTION FLAGS

    // MISCELLANEOUS OPTIONS

    //! verbose flag
    bool verbose_flag;


    // CONFIGURATION OPTIONS

    //! don't search environment directories
    bool no_search_environment;

    //! colourized output flag
    bool colour_flag;

    //! core output file
    std::string core_output;

    //! implementation output files
    std::string implementation_output;

    //! list of search paths
    std::list< boost::filesystem::path > search_path_list;


    // CODE GENERATION OPTIONS

    //! annotate output?
    bool annotate_flag;

    //! CSE flag
    bool cse_flag;

    //! unroll policy - maximum size of index set above which unrolling is disabled
    unsigned int unroll_policy_size;

    //! fast setting
    bool fast_flag;


    // WARNINGS

    //! display profiling information
    bool profile_flag;

    //! display general developer warnings?
    bool develop_warnings;
    
    //! report violations of the unrolling policy
    bool unroll_warnings;

    //! report on-the-fly index repositioning
    bool reposition_warnings;


    // INPUT FILES

    //! list of files to process
    std::list< boost::filesystem::path > input_file_list;


    // LIST OF EXCEPTIONS GENERATED DURING CONSTRUCTION

    //! exception messages
    std::list< message_payload > err_msgs;

  };


#endif //CPPTRANSPORT_ARGUMENT_CACHE_H
