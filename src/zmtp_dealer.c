/*  =========================================================================
    zmtp_dealer - DEALER socket class

    Copyright contributors as noted in the AUTHORS file.
    This file is part of libzmtp, the C ZMTP stack.

    This Source Code Form is subject to the terms of the Mozilla Public
    License, v. 2.0. If a copy of the MPL was not distributed with this
    file, You can obtain one at http://mozilla.org/MPL/2.0/.
    =========================================================================
*/

#include "../include/zmtp.h"

struct zmtp_dealer {
    zmtp_connection_t *conn;
};

zmtp_dealer_t *
zmtp_dealer_new (int fd)
{
    zmtp_dealer_t *self = (zmtp_dealer_t *) malloc (sizeof *self);
    if (self) {
        *self = (zmtp_dealer_t) { .conn = zmtp_connection_new (fd, ZMTP_DEALER) };
        if (!self->conn) {
            free (self);
            return NULL;
        }
        if (zmtp_connection_negotiate (self->conn) == -1) {
            zmtp_connection_destroy (&self->conn);
            free (self);
            return NULL;
        }
    }
    return self;
}

zmtp_dealer_t *
zmtp_dealer_ipc_connect (const char *addr)
{
    //  Create socket
    const int s = socket (AF_UNIX, SOCK_STREAM, 0);
    if (s == -1)
        return NULL;
    struct sockaddr_un remote =
        (struct sockaddr_un) { .sun_family = AF_UNIX };
    strcpy (remote.sun_path, addr);
    const socklen_t len = strlen (remote.sun_path) + sizeof remote.sun_family;
    //  Connect the socket
    const int rc = connect (s, (const struct sockaddr *) &remote, len);
    if (rc == -1) {
        close (s);
        return NULL;
    }
    else {
        zmtp_dealer_t *self = zmtp_dealer_new (s);
        if (!self)
            close (s);
        return self;
    }
}

void
zmtp_dealer_destroy (zmtp_dealer_t **self_p)
{
    assert (self_p);

    if (*self_p) {
        zmtp_dealer_t *self = *self_p;
        zmtp_connection_destroy (&self->conn);
        free (self);
        *self_p = NULL;
    }
}

int
zmtp_dealer_send (zmtp_dealer_t *self, zmtp_msg_t *msg)
{
    assert (self);
    assert (self->conn);

    return zmtp_connection_send (self->conn, msg);
}

zmtp_msg_t *
zmtp_dealer_recv (zmtp_dealer_t *self)
{
    assert (self);
    assert (self->conn);

    return zmtp_connection_recv (self->conn);
}
