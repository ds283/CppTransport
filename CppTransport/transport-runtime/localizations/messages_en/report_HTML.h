//
// Created by David Seery on 23/03/2016.
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

#ifndef CPPTRANSPORT_MESSAGES_EN_HTML_REPORT_H
#define CPPTRANSPORT_MESSAGES_EN_HTML_REPORT_H

constexpr auto CPPTRANSPORT_HTML_ROOT_EXISTS_A                    = "Specified HTML root directory";
constexpr auto CPPTRANSPORT_HTML_ROOT_EXISTS_B                    = "already exists; renamed as";
constexpr auto CPPTRANSPORT_HTML_CREATE_FAILURE                   = "Could not create HTML root directory";
constexpr auto CPPTRANSPORT_HTML_ASSET_CREATE_FAILURE             = "Could not create asset directory";
constexpr auto CPPTRANSPORT_HTML_ASSET_EMPLACE_FAILURE_A          = "Could not emplace asset";
constexpr auto CPPTRANSPORT_HTML_ASSET_EMPLACE_FAILURE_B          = "from source location";
constexpr auto CPPTRANSPORT_HTML_ASSET_NOT_FOUND                  = "Could not locate asset";
constexpr auto CPPTRANSPORT_HTML_ASSERT_DIRECTORY_EXISTS          = "Asset location already exists but is not a directory for";
constexpr auto CPPTRANSPORT_HTML_ASSET_DIR_FAILURE                = "Could not create asset directory";

constexpr auto CPPTRANSPORT_HTML_CANT_OPEN_FILE                   = "Cannot open output file";


#endif //CPPTRANSPORT_MESSAGES_EN_HTML_REPORT_H
