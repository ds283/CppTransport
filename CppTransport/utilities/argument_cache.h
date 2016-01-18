//
// Created by David Seery on 30/11/2015.
// Copyright (c) 2013-2016 University of Sussex. All rights reserved.
//

#ifndef CPPTRANSPORT_ARGUMENT_CACHE_H
#define CPPTRANSPORT_ARGUMENT_CACHE_H


#include <list>

#include "local_environment.h"

#include "boost/program_options.hpp"
#include "boost/filesystem/operations.hpp"


class argument_cache
  {

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


    // INPUT FILES

  public:

    //! get input files
    const std::list<boost::filesystem::path>& input_files() const { return(this->input_file_list); }


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


    // INPUT FILES

    //! list of files to process
    std::list< boost::filesystem::path > input_file_list;

  };


#endif //CPPTRANSPORT_ARGUMENT_CACHE_H
