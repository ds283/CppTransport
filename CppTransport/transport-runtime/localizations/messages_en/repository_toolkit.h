//
// Created by David Seery on 04/04/2016.
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

#ifndef CPPTRANSPORT_MESSAGES_EN_REPOSITORY_TOOLKIT_H
#define CPPTRANSPORT_MESSAGES_EN_REPOSITORY_TOOLKIT_H

constexpr auto CPPTRANSPORT_REPO_TOOLKIT_VERTEX_NOT_FOUND           = "Internal error: could not find vertex";
constexpr auto CPPTRANSPORT_REPO_TOOLKIT_D_NOT_INITIALIZED          = "Internal error: attempt to use distance matrix before initialization";
constexpr auto CPPTRANSPORT_REPO_TOOLKIT_D_OUT_OF_RANGE             = "Internal error: attempt to access out-of-range distance matrix element";

constexpr auto CPPTRANSPORT_REPO_TOOLKIT_CANNOT_DELETE_DEPENDENT_A  = "Cannot delete content group";
constexpr auto CPPTRANSPORT_REPO_TOOLKIT_CANNOT_DELETE_DEPENDENT_B  = "because other groups depend on it:";

constexpr auto CPPTRANSPORT_REPO_TOOLKIT_MISSING_OBJECT             = "Found no matches for repository object";
constexpr auto CPPTRANSPORT_REPO_TOOLKIT_IGNORING_LOCKED            = "Ignoring locked repository object";
constexpr auto CPPTRANSPORT_REPO_TOOLKIT_DELETING_OBJECT            = "Deleting repository object";

#endif //CPPTRANSPORT_MESSAGES_EN_REPOSITORY_TOOLKIT_H
