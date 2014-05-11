/*  =========================================================================
    zmtp_endpoint - endpoint base class

    Copyright (c) contributors as noted in the AUTHORS file.
    This file is part of libzmtp, the C ZMTP stack.

    This Source Code Form is subject to the terms of the Mozilla Public
    License, v. 2.0. If a copy of the MPL was not distributed with this
    file, You can obtain one at http://mozilla.org/MPL/2.0/.
    =========================================================================
*/

#ifndef __ZMTP_ENDPOINT_H_INCLUDED__
#define __ZMTP_ENDPOINT_H_INCLUDED__

#ifdef __cplusplus
extern "C" {
#endif

struct zmtp_endpoint {
    void (*destroy) (struct zmtp_endpoint **self_p);
    int (*connect) (struct zmtp_endpoint *self);
    int (*listen) (struct zmtp_endpoint *self);
};

typedef struct zmtp_endpoint zmtp_endpoint_t;

void
    zmtp_endpoint_destroy (zmtp_endpoint_t **self_p);

int
    zmtp_endpoint_connect (zmtp_endpoint_t *self);

int
    zmtp_endpoint_listen (zmtp_endpoint_t *self);

#endif
