//
// Created by David Seery on 30/11/2015.
// Copyright (c) 2013-15 University of Sussex. All rights reserved.
//

#ifndef CPPTRANSPORT_ARGUMENT_CACHE_H
#define CPPTRANSPORT_ARGUMENT_CACHE_H


#include <list>

#include "boost/program_options.hpp"
#include "boost/filesystem/operations.hpp"


class argument_cache
  {

    // CONSTRUCTOR, DESTRUCTOR

  public:

    //! constructor
    argument_cache(int argc, const char* argv[]);

    //! destructor is default
    ~argument_cache() = default;


    // INTERFACE

  public:

    //! get verbose output setting
    bool verbose() const { return(this->verbose_flag); }

    //! get colourized setting
    bool colourize() const { return(this->colour_flag); }

    //! get common-subexpression elimination setting
    bool do_cse() const { return(this->cse_flag); }

    //! search environment paths for eg. template files?
    bool search_environment() const { return(!this->no_search_environment); }

    //! get search paths
    const std::list<boost::filesystem::path>& search_paths() const { return(this->search_path_list); }

    //! get input files
    const std::list<boost::filesystem::path>& input_files() const { return(this->input_file_list); }

    //! get core output file
    const std::string& core_out() const { return(this->core_output); };

    //! get implementation output file
    const std::string& implementation_out() const { return(this->implementation_output); }


    // INTERNAL DATA

  private:

    // OPTION FLAGS

    //! verbose flag
    bool verbose_flag;

    //! colourized output flag
    bool colour_flag;

    //! CSE flag
    bool cse_flag;

    //! don't search environment directories
    bool no_search_environment;

    //! core output file
    std::string core_output;

    //! implementation output files
    std::string implementation_output;

    //! list of search paths
    std::list< boost::filesystem::path > search_path_list;

    //! list of files to process
    std::list< boost::filesystem::path > input_file_list;

  };


#endif //CPPTRANSPORT_ARGUMENT_CACHE_H
