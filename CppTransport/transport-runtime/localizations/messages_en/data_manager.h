//
// Created by David Seery on 05/05/2014.
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


#ifndef CPPTRANSPORT_MESSAGES_EN_DATA_MANAGER_H
#define CPPTRANSPORT_MESSAGES_EN_DATA_MANAGER_H


constexpr auto CPPTRANSPORT_NFIELDS_BACKG = "Storage error: Unexpected number of elements when batching output of background";
constexpr auto CPPTRANSPORT_NFIELDS_TWOPF = "Storage error: Unexpected number of elements when batching output of twopf";
constexpr auto CPPTRANSPORT_NFIELDS_THREEPF = "Storage error: Unexpected number of elements when batching output of threepf";
constexpr auto CPPTRANSPORT_NFIELDS_TENSOR_TWOPF = "Storage error: Unexpected number of elements when batching output of tensor twopf";

constexpr auto CPPTRANSPORT_DATACTR_NOT_CLOSED = "Data container error: Some containers not closed when deleting 'data_manager' object";
constexpr auto CPPTRANSPORT_DATACTR_CLOSE = "Data container error: Unexpected error when closing containers on destruction of 'data_manager' object (backend code=";
constexpr auto CPPTRANSPORT_DATACTR_CREATE_A = "Data container error: Could not create data container";
constexpr auto CPPTRANSPORT_DATACTR_CREATE_B = "(backend code=";
constexpr auto CPPTRANSPORT_DATACTR_OPEN_A = "Data container error: Could not open data container";
constexpr auto CPPTRANSPORT_DATACTR_OPEN_B = "(backend code=";

constexpr auto CPPTRANSPORT_DATACTR_TEMPCTR_FAIL_A = "Data container error: Failed to create temporary container";
constexpr auto CPPTRANSPORT_DATACTR_TEMPCTR_FAIL_B = "(backend code=";

constexpr auto CPPTRANSPORT_DATACTR_TIMETAB_FAIL = "Data container error: Failed to create time-sample table in data container (backend code=";
constexpr auto CPPTRANSPORT_DATACTR_TWOPFTAB_FAIL = "Data container error: Failed to create twopf-sample table in data container (backend code=";
constexpr auto CPPTRANSPORT_DATACTR_THREEPFTAB_FAIL = "Data container error: Failed to create threepf-sample table in data container (backend code=";

constexpr auto CPPTRANSPORT_DATACTR_STATS_INSERT_FAIL = "Data container error: Failed to create per-configuration statistics table in data container (backend code=";
constexpr auto CPPTRANSPORT_DATACTR_ICS_INSERT_FAIL = "Data container error: Failed to create initial conditions table in data container (backend code=";
constexpr auto CPPTRANSPORT_DATACTR_WORKER_INSERT_FAIL = "Data container error: Failed to create worker information table in data container (backend code=";
constexpr auto CPPTRANSPORT_DATACTR_BACKG_DATATAB_FAIL = "Data container error: Failed to create background value table in data container (backend code=";
constexpr auto CPPTRANSPORT_DATACTR_TWOPF_DATATAB_FAIL = "Data container error: Failed to create twopf value table in data container (backend code=";
constexpr auto CPPTRANSPORT_DATACTR_TWOPF_SI_DATATAB_FAIL = "Data container error: Failed to create twopf spectral index value table in data container (backend code=";
constexpr auto CPPTRANSPORT_DATACTR_TENSOR_TWOPF_DATATAB_FAIL = "Data container error: Failed to create tensor twopf value table in data container (backend code=";
constexpr auto CPPTRANSPORT_DATACTR_TENSOR_TWOPF_SI_DATATAB_FAIL = "Data container error: Failed to create tensor twopf spectral index value table in data container (backend code=";
constexpr auto CPPTRANSPORT_DATACTR_THREEPF_MOMENTUM_DATATAB_FAIL = "Data container error: Failed to create threepf-momentum value table in data container (backend code=";
constexpr auto CPPTRANSPORT_DATACTR_THREEPF_DERIV_DATATAB_FAIL = "Data container error: Failed to create threepf-derivative value table in data container (backend code=";

constexpr auto CPPTRANSPORT_DATACTR_ZETA_TWOPF_DATATAB_FAIL = "Data container error: Failed to create zeta twopf value table in data container (backend code=";
constexpr auto CPPTRANSPORT_DATACTR_ZETA_THREEPF_DATATAB_FAIL = "Data container error: Failed to create zeta threepf value table in data container (backend code=";
constexpr auto CPPTRANSPORT_DATACTR_FNL_DATATAB_FAIL = "Data container error: Failed to create fNL value table in data container (backend code=";
constexpr auto CPPTRANSPORT_DATACTR_GAUGE1_DATATAB_FAIL = "Data container error: Failed to create linear gauge-xfm value table in data container (backend code=";
constexpr auto CPPTRANSPORT_DATACTR_GAUGE2_DATATAB_FAIL = "Data container error: Failed to create quadratic gauge-xfm value table in data container (backend code=";

constexpr auto CPPTRANSPORT_DATACTR_BACKGROUND_COPY = "Data container error: Failed to copy background values from temporary container (backend code=";
constexpr auto CPPTRANSPORT_DATACTR_TWOPF_COPY = "Data container error: Failed to copy twopf values from temporary container (backend code=";
constexpr auto CPPTRANSPORT_DATACTR_TWOPF_SI_COPY = "Data container error: Failed to copy twopf spectral index values from temporary container (backend code=";
constexpr auto CPPTRANSPORT_DATACTR_TENSOR_TWOPF_COPY = "Data container error: Failed to copy tensor twopf values from temporary container (backend code=";
constexpr auto CPPTRANSPORT_DATACTR_TENSOR_TWOPF_SI_COPY = "Data container error: Failed to copy tensor twopf spectral index values from temporary container (backend code=";
constexpr auto CPPTRANSPORT_DATACTR_STATISTICS_COPY = "Data container error: Failed to copy per-configuration statistics from temporary container (backend code=";
constexpr auto CPPTRANSPORT_DATACTR_ICS_COPY = "Data container error: Failed to copy initial conditions from temporary container (backend code=";
constexpr auto CPPTRANSPORT_DATACTR_WORKERS_COPY = "Data container error: Failed to copy worker information from temporary container (backend code=";
constexpr auto CPPTRANSPORT_DATACTR_THREEPF_MOMENTUM_COPY = "Data container error: Failed to copy threepf-momentum values from temporary container (backend code=";
constexpr auto CPPTRANSPORT_DATACTR_THREEPF_DERIV_COPY = "Data container error: Failed to copy threepf-derivative values from temporary container (backend code=";
constexpr auto CPPTRANSPORT_DATACTR_ZETA_TWOPF_COPY = "Data container error: Failed to copy zeta twopf values from temporary container (backend code=";
constexpr auto CPPTRANSPORT_DATACTR_ZETA_THREEPF_COPY = "Data container error: Failed to copy zeta threepf values from temporary container (backend code=";
constexpr auto CPPTRANSPORT_DATACTR_GAUGE_XFM1_COPY = "Data container error: Failed to copy linear gauge-xfm values from temporary container (backend code=";
constexpr auto CPPTRANSPORT_DATACTR_GAUGE_XFM2_COPY = "Data container error: Failed to copy quadratic gauge-xfm values from temporary container (backend code=";
constexpr auto CPPTRANSPORT_DATACTR_FNL_COPY = "Data container error: Failed to copy fNL values from temporary bispectrum container (backend code=";
constexpr auto CPPTRANSPORT_DATACTR_REMOVE_TEMP = "Data container error: Could not remove temporary container";
constexpr auto CPPTRANSPORT_DATACTR_ATTACH_FAIL = "Data container error: Could not attach temporary database (backend code=";
constexpr auto CPPTRANSPORT_DATACTR_DETACH_FAIL = "Data container error: Could not detach temporary database (backend code=";

constexpr auto CPPTRANSPORT_DATAMGR_NULL_DATAPIPE = "Data manager error: Null datapipe specifier";
constexpr auto CPPTRANSPORT_DATAMGR_DETACH_PIPE_NOT_ATTACHED = "Data manager error: Attempt to detach datapipe, but no content group is attached";
constexpr auto CPPTRANSPORT_DATAMGR_PIPE_WRONG_CONTENT = "Data manager error: Datapipe attached to wrong type of content group";
constexpr auto CPPTRANSPORT_DATAMGR_ATTACH_PIPE_ALREADY_ATTACHED = "Data manager error: Attempt to attach datapipe, but content group already attached";
constexpr auto CPPTRANSPORT_DATAMGR_UNKNOWN_DERIVABLE_TASK = "Data manager error: Attempt to attach datapipe to derivable task of unknown type: task name";
constexpr auto CPPTRANSPORT_DATAMGR_PIPE_NOT_ATTACHED = "Data manager error: Attempt to use datapipe when no content group is attached";
constexpr auto CPPTRANSPORT_DATAMGR_PIPE_NULL_TASK = "Data manager error: Null task supplied when attaching datapipe";

constexpr auto CPPTRANSPORT_DATAMGR_CONTAINER_NOT_EXIST = "Data manager error: Could not find requested data container";

constexpr auto CPPTRANSPORT_DATAMGR_NULL_DERIVED_PRODUCT = "Data manager error: Null derived product";
constexpr auto CPPTRANSPORT_DATAMGR_NULL_BATCHER = "Data manager error: Null batcher";

constexpr auto CPPTRANSPORT_DATAMGR_DERIVED_PRODUCT_MISSING = "Data manager error: Can not find expected derived product in temporary location";

constexpr auto CPPTRANSPORT_DATAMGR_TIME_SERIAL_READ_FAIL = "Data manager error: Failed to select time sample (backend code=";
constexpr auto CPPTRANSPORT_DATAMGR_KCONFIG_SERIAL_READ_FAIL = "Data manager error: Failed to select k-configuration sample (backend code=";
constexpr auto CPPTRANSPORT_DATAMGR_WORKER_TABLE_READ_FAIL = "Data manager error: Failed to read worker information table (backend code=";
constexpr auto CPPTRANSPORT_DATAMGR_STATISTICS_TABLE_READ_FAIL = "Data manager error: Failed to read statistics information table (backend code=";

constexpr auto CPPTRANSPORT_DATAMGR_INTEGRITY_READ_FAIL = "Data manager error: Failure while performing integrity check (backend code=";


#endif // CPPTRANSPORT_MESSAGES_EN_DATA_MANAGER_H
