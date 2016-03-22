/*  =========================================================================
    zmtp_ipc_endpoint - IPC endpoint class

    Copyright (c) contributors as noted in the AUTHORS file.
    This file is part of libzmtp, the C ZMTP stack.

    This Source Code Form is subject to the terms of the Mozilla Public
    License, v. 2.0. If a copy of the MPL was not distributed with this
    file, You can obtain one at http://mozilla.org/MPL/2.0/.
    =========================================================================
*/

#include "zmtp_classes.h"

struct zmtp_ipc_endpoint {
    zmtp_endpoint_t base;
    struct sockaddr_un sockaddr;
};

zmtp_ipc_endpoint_t *
zmtp_ipc_endpoint_new (const char *path)
{
    zmtp_ipc_endpoint_t *self =
        (zmtp_ipc_endpoint_t *) zmalloc (sizeof *self);
    if (!self)
        return NULL;

    //  Initialize base class
    self->base = (zmtp_endpoint_t) {
        .connect = (int (*) (zmtp_endpoint_t *)) zmtp_ipc_endpoint_connect,
        .listen = (int (*) (zmtp_endpoint_t *)) zmtp_ipc_endpoint_listen,
        .destroy = (void (*) (zmtp_endpoint_t **)) zmtp_ipc_endpoint_destroy,
    };

    if (strlen (path) >= sizeof self->sockaddr.sun_path) {
        free (self);
        return NULL;
    }

    self->sockaddr = (struct sockaddr_un) { .sun_family = AF_UNIX };
    strcpy (self->sockaddr.sun_path, path);

    //  Initial '@' in the path designates abstract namespace.
    //  See unix(7) for details.
    if (path [0] == '@')
        self->sockaddr.sun_path [0] = '\0';

    return self;
}


void
zmtp_ipc_endpoint_destroy (zmtp_ipc_endpoint_t **self_p)
{
    assert (self_p);
    if (*self_p) {
        zmtp_ipc_endpoint_t *self = *self_p;
        free (self);
        *self_p = NULL;
    }
}


int
zmtp_ipc_endpoint_connect (zmtp_ipc_endpoint_t *self)
{
    assert (self);

    //  Create socket
    const int s = socket (AF_UNIX, SOCK_STREAM, 0);
    if (s == -1)
        return -1;

    //  Compute socket address length
    const socklen_t addrlen =
        self->sockaddr.sun_path [0] == '\0'
            ? sizeof (sa_family_t) + 1 + strlen (self->sockaddr.sun_path + 1)
            : sizeof self->sockaddr;

    //  Connect the socket
    const int rc = connect (s, (const struct sockaddr *) &self->sockaddr, addrlen);
    if (rc == -1) {
        close (s);
        return -1;
    }

    return s;
}

int
zmtp_ipc_endpoint_listen (zmtp_ipc_endpoint_t *self)
{
    assert (self);

    const int s = socket (AF_UNIX, SOCK_STREAM, 0);
    if (s == -1)
        return -1;

    //  Compute address length
    const socklen_t addrlen =
        self->sockaddr.sun_path [0] == '\0'
            ? sizeof (sa_family_t) + 1 + strlen (self->sockaddr.sun_path + 1)
            : sizeof self->sockaddr;

    int rc = bind (s, (const struct sockaddr *) &self->sockaddr, addrlen);
    if (rc == 0) {
        rc = listen (s, 1);
        if (rc == 0)
            rc = accept (s, NULL, NULL);
    }
    close (s);
    return rc;
}
