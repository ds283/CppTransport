//
// Created by David Seery on 05/05/2014.
// Copyright (c) 2014 University of Sussex. All rights reserved.
//


#ifndef __CPP_TRANSPORT_MESSAGES_EN_UNQLITE_H
#define __CPP_TRANSPORT_MESSAGES_EN_UNQLITE_H


#define __CPP_TRANSPORT_UNQLITE_FAIL_COMPILE           "Internal error: failed to compile Jx9 script (backend code="
#define __CPP_TRANSPORT_UNQLITE_FAIL_COMPILE_LOG       "Internal error: failed to compile Jx9 script; compiler error log = "

#define __CPP_TRANSPORT_UNQLITE_FAIL_REGISTER_CONSUMER "Internal error: failed to register Jx9 output consumer"

#define __CPP_TRANSPORT_UNQLITE_FAIL_EXECUTE_SCRIPT    "Internal error: failed to execute Jx9 script"
#define __CPP_TRANSPORT_UNQLITE_VM_OUPTUT              "Internal error: Jx9 output"

#define __CPP_TRANSPORT_UNQLITE_MULTIPLE_JSON          "Internal error: attempt to extract JSON representation for multiple records (query="

#define __CPP_TRANSPORT_UNQLITE_READ_NODE_FAIL         "Internal error: failed to read serialized node"
#define __CPP_TRANSPORT_UNQLITE_READ_ARRAY_FAIL        "Internal error: failed to read serialized array"
#define __CPP_TRANSPORT_UNQLITE_READ_ATTR_FAIL         "Internal error: failed to read node attribute"
#define __CPP_TRANSPORT_UNQLITE_READ_VALUE_FAIL        "Internal error: failed to read value"

#define __CPP_TRANSPORT_UNQLITE_READER_NOT_JSON_OBJECT "Internal error: attempt to construct unqlite_serialization_reader from something which isn't a JSON object"
#define __CPP_TRANSPORT_UNQLITE_READER_KEY_FAIL        "Internal error: while building stack, key is null or not simple type"
#define __CPP_TRANSPORT_UNQLITE_READER_VALUE_FAIL      "Internal error: while building stack, value is null"


#endif // __CPP_TRANSPORT_MESSAGES_EN_UNQLITE_H
