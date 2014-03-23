/*  =========================================================================
    zmtp - public API

    Copyright (c) contributors as noted in the AUTHORS file.
    This file is part of libzmtp, the C ZMTP stack.

    This Source Code Form is subject to the terms of the Mozilla Public
    License, v. 2.0. If a copy of the MPL was not distributed with this
    file, You can obtain one at http://mozilla.org/MPL/2.0/.
    =========================================================================
*/

#ifndef __ZMTP_H_INCLUDED__
#define __ZMTP_H_INCLUDED__

//  Set up environment for the application

#include "zmtp_prelude.h"

//  ZMTP version macros for compile-time API detection

#define ZMTP_VERSION_MAJOR 0
#define ZMTP_VERSION_MINOR 1
#define ZMTP_VERSION_PATCH 0

#define ZMTP_MAKE_VERSION(major, minor, patch) \
    ((major) * 10000 + (minor) * 100 + (patch))
#define ZMTP_VERSION \
    ZMTP_MAKE_VERSION(ZMTP_VERSION_MAJOR, ZMTP_VERSION_MINOR, ZMTP_VERSION_PATCH)

//  Public API classes

#include "zmtp_msg.h"
#include "zmtp_dealer.h"

enum zmtp_socket_type {
    ZMTP_PAIR = 0,
    ZMTP_PUB,
    ZMTP_SUB,
    ZMTP_REQ,
    ZMTP_REP,
    ZMTP_DEALER,
    ZMTP_ROUTER,
    ZMTP_PULL,
    ZMTP_PUSH,
    ZMTP_XPUB,
    ZMTP_XSUB,
    ZMTP_STREAM
};

#endif
