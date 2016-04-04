//
// Created by David Seery on 04/04/2016.
// Copyright (c) 2016 University of Sussex. All rights reserved.
//

#ifndef CPPTRANSPORT_MESSAGES_EN_REPOSITORY_TOOLKIT_H
#define CPPTRANSPORT_MESSAGES_EN_REPOSITORY_TOOLKIT_H

constexpr auto CPPTRANSPORT_REPO_TOOLKIT_VERTEX_NOT_FOUND                   = "Internal error: could not find vertex";
constexpr auto CPPTRANSPORT_REPO_TOOLKIT_D_NOT_INITIALIZED                  = "Internal error: attempt to use distance matrix before initialization";
constexpr auto CPPTRANSPORT_REPO_TOOLKIT_D_OUT_OF_RANGE                     = "Internal error: attempt to access out-of-range distance matrix element";
constexpr auto CPPTRANSPORT_REPO_TOOLKIT_CANNOT_DELETE_DEPENDENT_A          = "Cannot delete content group";
constexpr auto CPPTRANSPORT_REPO_TOOLKIT_CANNOT_DELETE_DEPENDENT_B          = "because other groups depend on it:";
constexpr auto CPPTRANSPORT_REPO_TOOLKIT_MISSING_OBJECT                     = "Could not find repository object";

#endif //CPPTRANSPORT_MESSAGES_EN_REPOSITORY_TOOLKIT_H
