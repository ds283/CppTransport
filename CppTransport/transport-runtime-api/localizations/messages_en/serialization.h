//
// Created by David Seery on 05/05/2014.
// Copyright (c) 2014 University of Sussex. All rights reserved.
//


#ifndef __CPP_TRANSPORT_MESSAGES_EN_SERIALIZATION_H
#define __CPP_TRANSPORT_MESSAGES_EN_SERIALIZATION_H


#define __CPP_TRANSPORT_SERIAL_NONODE         "Serialization error: node stack is empty, but attempt to add new node '"
#define __CPP_TRANSPORT_SERIAL_NONODEATTR     "Serialization error: node stack is empty, but attempt to write attribute '"
#define __CPP_TRANSPORT_SERIAL_NONODEVALUE    "Serialization error: node stack is empty, but attempt to write value '"
#define __CPP_TRANSPORT_SERIAL_ENDNODE        "Serialization error: no current node, but attempt to end node '"
#define __CPP_TRANSPORT_SERIAL_ENDNAME_A      "Serialization error: current node is '"
#define __CPP_TRANSPORT_SERIAL_ENDNAME_B      "', but attempt to end node '"

#define __CPP_TRANSPORT_SERIAL_NOT_FINISHED   "Serialization error: node stack is non-empty, but attempt to read contents"

#define __CPP_TRANSPORT_SERIAL_VALIDATE_FAIL  "Serialization error: validation failed for element '"

#endif // __CPP_TRANSPORT_MESSAGES_EN_SERIALIZATION_H
