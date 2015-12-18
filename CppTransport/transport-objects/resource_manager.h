//
// Created by David Seery on 18/12/2015.
// Copyright (c) 2015 University of Sussex. All rights reserved.
//

#ifndef CPPTRANSPORT_RESOURCE_MANAGER_H
#define CPPTRANSPORT_RESOURCE_MANAGER_H


#include "contexted_value.h"

#include "boost/optional.hpp"


class resource_manager
  {

    //! CONSTRUCTOR, DESTRUCTOR

  public:

    //! constructor
    resource_manager() = default;

    //! destructor
    ~resource_manager() = default;


    // INTERFACE - QUERY FOR RESOURCES

  public:

    //! get parameters
    const boost::optional< contexted_value<std::string> >& parameters() const { return(this->parameters_cache); }

    //! get phase-space coordinates
    const boost::optional< contexted_value<std::string> >& coordinates() const { return(this->coordinates_cache); }

    //! get V,i
    const boost::optional< contexted_value<std::string> >& dV() const { return(this->dV_cache); }

    //! get V,ij
    const boost::optional< contexted_value<std::string> >& ddV() const { return(this->ddV_cache); }

    //! get V,ijk
    const boost::optional< contexted_value<std::string> >& dddV() const { return(this->dddV_cache); }

    //! get connexion
    const boost::optional< contexted_value<std::string> >& connexion() const { return(this->connexion_cache); }

    //! get Riemann
    const boost::optional< contexted_value<std::string> >& Riemann() const { return(this->Riemann_cache); }


    // INTERFACE - ASSIGN RESOURCES

  public:

    //! assign parameter resource
    void assign_parameters(const contexted_value<std::string>& p) { this->parameters_cache = p; }

    //! assign phase-space coordinate resource
    void assign_coordinates(const contexted_value<std::string>& c) { this->coordinates_cache = c; }

    //! assign V,i resource
    void assign_dV(const contexted_value<std::string>& d) { this->dV_cache = d; }

    //! assign V,ij resource
    void assign_ddV(const contexted_value<std::string>& d) { this->ddV_cache = d; }

    //! assign V,ijk resource
    void assign_dddV(const contexted_value<std::string>& d) { this->dddV_cache = d; }

    //! assign connexion resource
    void assign_connexion(const contexted_value<std::string>& c) { this->connexion_cache = c; }

    //! assign Riemann resource
    void assign_Riemann(const contexted_value<std::string>& R) { this->Riemann_cache = R; }


    // INTERFACE - RELEASE RESOURCES

  public:

    //! release parameter resource
    void release_parameter() { this->parameters_cache.reset(); }

    //! release phase-space coordinate resource
    void release_coordinates() { this->connexion_cache.reset(); }

    //! release V,i resource
    void release_dV() { this->dV_cache.reset(); }

    //! release V,ij resource
    void release_ddV() { this->ddV_cache.reset(); }

    //! release V,ijk resource
    void release_dddV() { this->dddV_cache.reset(); }

    //! release connexion resource
    void release_connexion() { this->connexion_cache.reset(); }

    //! release Riemann resource
    void release_Riemann() { this->Riemann_cache.reset(); }


    // INTERFACE - UTILITY FUNCTIONS

  public:

    //! release all resources
    void release();


    // INTERNAL DATA

  private:

    //! cache parameters resource
    boost::optional< contexted_value<std::string> > parameters_cache;

    //! cache parameters resource
    boost::optional< contexted_value<std::string> > coordinates_cache;

    //! cache V, resource
    boost::optional< contexted_value<std::string> > dV_cache;

    //! cache V,ij resource
    boost::optional< contexted_value<std::string> > ddV_cache;

    //! cache V,ijk resource
    boost::optional< contexted_value<std::string> > dddV_cache;

    //! cache connexion resource
    boost::optional< contexted_value<std::string> > connexion_cache;

    //! cache Riemann resource
    boost::optional< contexted_value<std::string> > Riemann_cache;

  };


#endif //CPPTRANSPORT_RESOURCE_MANAGER_H
