/*  =========================================================================
    zmtp_dealer - DEALER socket class

    Copyright (c) contributors as noted in the AUTHORS file.
    This file is part of libzmtp, the C ZMTP stack.

    This Source Code Form is subject to the terms of the Mozilla Public
    License, v. 2.0. If a copy of the MPL was not distributed with this
    file, You can obtain one at http://mozilla.org/MPL/2.0/.
    =========================================================================
*/

#include "zmtp_classes.h"

//  Structure of our class

struct _zmtp_dealer_t {
    zmtp_connection_t *connection;
};


//  --------------------------------------------------------------------------
//  Constructor

zmtp_dealer_t *
zmtp_dealer_new ()
{
    zmtp_dealer_t *self = (zmtp_dealer_t *) zmalloc (sizeof *self);
    assert (self);              //  For now, memory exhaustion is fatal

    self->connection = NULL;
    return self;
}


//  --------------------------------------------------------------------------
//  Destructor

void
zmtp_dealer_destroy (zmtp_dealer_t **self_p)
{
    assert (self_p);

    if (*self_p) {
        zmtp_dealer_t *self = *self_p;
        zmtp_connection_destroy (&self->connection);
        free (self);
        *self_p = NULL;
    }
}


//  --------------------------------------------------------------------------
//

int
zmtp_dealer_ipc_connect (zmtp_dealer_t *self, const char *path)
{
    assert (self);

    if (self->connection)
        return -1;
    zmtp_connection_t *conn = zmtp_connection_new ();
    if (!conn)
        return -1;
    if (zmtp_connection_ipc_connect (conn, path) == -1) {
        zmtp_connection_destroy (&conn);
        return -1;
    }
    if (zmtp_connection_negotiate (conn, ZMTP_DEALER) == -1) {
        zmtp_connection_destroy (&conn);
        return -1;
    }
    self->connection = conn;
    return 0;
}


//  --------------------------------------------------------------------------
//

int
zmtp_dealer_tcp_connect (zmtp_dealer_t *self,
                         const char *addr, unsigned short port)
{
    assert (self);

    if (self->connection)
        return -1;
    zmtp_connection_t *conn = zmtp_connection_new ();
    if (!conn)
        return -1;
    if (zmtp_connection_tcp_connect (conn, addr, port) == -1) {
        zmtp_connection_destroy (&conn);
        return -1;
    }
    if (zmtp_connection_negotiate (conn, ZMTP_DEALER) == -1) {
        zmtp_connection_destroy (&conn);
        return -1;
    }
    self->connection = conn;
    return 0;
}


//  --------------------------------------------------------------------------
//

int
zmtp_dealer_send (zmtp_dealer_t *self, zmtp_msg_t *msg)
{
    assert (self);
    assert (self->connection);

    return zmtp_connection_send (self->connection, msg);
}


//  --------------------------------------------------------------------------
//

zmtp_msg_t *
zmtp_dealer_recv (zmtp_dealer_t *self)
{
    assert (self);
    assert (self->connection);

    return zmtp_connection_recv (self->connection);
}


//  --------------------------------------------------------------------------
//  Selftest

void
zmtp_dealer_test (bool verbose)
{
    printf (" * zmtp_dealer: ");
    //  @selftest
    //  @end
    printf ("OK\n");
}
