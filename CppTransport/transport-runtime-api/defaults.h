//
// Created by David Seery on 14/05/2014.
// Copyright (c) 2014 University of Sussex. All rights reserved.
//


#ifndef __defaults_H_
#define __defaults_H_


#define __CPP_TRANSPORT_DEFAULT_WRAP_WIDTH               (135)
#define __CPP_TRANSPORT_DEFAULT_PLOT_PRECISION           (3)
#define __CPP_TRANSPORT_DEFAULT_TABLE_PRECISION          (17)

#define __CPP_TRANSPORT_DEFAULT_MESH_REFINEMENTS         (6)
#define __CPP_TRANSPORT_DEFAULT_FAST_FORWARD_EFOLDS      (5.0)

#define __CPP_TRANSPORT_DEFAULT_KCONFIG_SEARCH_PRECISION (1E-5)

#define __CPP_TRANSPORT_DEFAULT_PYTHON_PATH              "/usr/bin/python"

// default storage limit on nodes - 500 Mb
// on a machine with 8 workers, that would give 4000 Mb or 4 Gb
// this can be increased (either here, or when creating a
// task_manager object) on machines with more memory
#define __CPP_TRANSPORT_DEFAULT_STORAGE (500*1024*1024)


#endif //__defaults_H_
