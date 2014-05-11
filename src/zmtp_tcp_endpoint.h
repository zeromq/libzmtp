/*  =========================================================================
    zmtp_tcp_endpoint - TCP endpoint class

    Copyright (c) contributors as noted in the AUTHORS file.
    This file is part of libzmtp, the C ZMTP stack.

    This Source Code Form is subject to the terms of the Mozilla Public
    License, v. 2.0. If a copy of the MPL was not distributed with this
    file, You can obtain one at http://mozilla.org/MPL/2.0/.
    =========================================================================
*/

#ifndef __ZMTP_TCP_ENDPOINT_H_INCLUDED__
#define __ZMTP_TCP_ENDPOINT_H_INCLUDED__

#ifdef __cplusplus
extern "C" {
#endif

#include "zmtp_endpoint.h"

typedef struct zmtp_tcp_endpoint zmtp_tcp_endpoint_t;

zmtp_tcp_endpoint_t *
    zmtp_tcp_endpoint_new (const char *ip_addr, unsigned short port);

void
    zmtp_tcp_endpoint_destroy (zmtp_tcp_endpoint_t **self_p);

int
    zmtp_tcp_endpoint_connect (zmtp_tcp_endpoint_t *self);

int
    zmtp_tcp_endpoint_listen (zmtp_tcp_endpoint_t *self);

#endif
