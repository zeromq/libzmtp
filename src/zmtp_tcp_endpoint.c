/*  =========================================================================
    zmtp_tcp_endpoint - TCP endpoint class

    Copyright (c) contributors as noted in the AUTHORS file.
    This file is part of libzmtp, the C ZMTP stack.

    This Source Code Form is subject to the terms of the Mozilla Public
    License, v. 2.0. If a copy of the MPL was not distributed with this
    file, You can obtain one at http://mozilla.org/MPL/2.0/.
    =========================================================================
*/

#include "zmtp_classes.h"

struct zmtp_tcp_endpoint {
    zmtp_endpoint_t base;
    struct addrinfo *addrinfo;
};


zmtp_tcp_endpoint_t *
zmtp_tcp_endpoint_new (const char *ip_addr, unsigned short port)
{
    zmtp_tcp_endpoint_t *self =
        (zmtp_tcp_endpoint_t *) zmalloc (sizeof *self);
    if (!self)
        return NULL;

    //  Initialize base class
    self->base = (zmtp_endpoint_t) {
        .connect = (int (*) (zmtp_endpoint_t *)) zmtp_tcp_endpoint_connect,
        .destroy = (void (*) (zmtp_endpoint_t **)) zmtp_tcp_endpoint_destroy,
    };

    //  Resolve address
    const struct addrinfo hints = {
        .ai_family   = AF_INET,
        .ai_socktype = SOCK_STREAM,
        .ai_flags    = AI_NUMERICHOST | AI_NUMERICSERV
    };
    char service [8 + 1];
    snprintf (service, sizeof service, "%u", port);
    if (getaddrinfo (ip_addr, service, &hints, &self->addrinfo)) {
        free (self);
        return NULL;
    }

    return self;
}


void
zmtp_tcp_endpoint_destroy (zmtp_tcp_endpoint_t **self_p)
{
    assert (self_p);
    if (*self_p) {
        zmtp_tcp_endpoint_t *self = *self_p;
        freeaddrinfo (self->addrinfo);
        free (self);
        *self_p = NULL;
    }
}


int
zmtp_tcp_endpoint_connect (zmtp_tcp_endpoint_t *self)
{
    assert (self);

    const int s = socket (AF_INET, SOCK_STREAM, 0);
    if (s == -1)
        return -1;

    const int rc = connect (
        s, self->addrinfo->ai_addr, self->addrinfo->ai_addrlen);
    if (rc == -1) {
        close (s);
        return -1;
    }

    return s;
}
