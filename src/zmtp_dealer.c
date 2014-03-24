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
    zmtp_channel_t *channel;
};


//  --------------------------------------------------------------------------
//  Constructor

zmtp_dealer_t *
zmtp_dealer_new ()
{
    zmtp_dealer_t *self = (zmtp_dealer_t *) zmalloc (sizeof *self);
    assert (self);              //  For now, memory exhaustion is fatal

    self->channel = NULL;
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
        zmtp_channel_destroy (&self->channel);
        free (self);
        *self_p = NULL;
    }
}


//  --------------------------------------------------------------------------
//

int
zmtp_dealer_ipc_connect (zmtp_dealer_t *self, const char *path)
{
    if (!self)
        return -1;
    if (self->channel)
        return -1;
    zmtp_channel_t *channel = zmtp_channel_new ();
    if (!channel)
        return -1;
    if (zmtp_channel_ipc_connect (channel, path) == -1) {
        zmtp_channel_destroy (&channel);
        return -1;
    }
    self->channel = channel;
    return 0;
}


//  --------------------------------------------------------------------------
//

int
zmtp_dealer_tcp_connect (zmtp_dealer_t *self,
                         const char *addr, unsigned short port)
{
    if (!self)
        return -1;
    if (self->channel)
        return -1;
    zmtp_channel_t *channel = zmtp_channel_new ();
    if (!channel)
        return -1;
    if (zmtp_channel_tcp_connect (channel, addr, port) == -1) {
        zmtp_channel_destroy (&channel);
        return -1;
    }
    self->channel = channel;
    return 0;
}


//  --------------------------------------------------------------------------
//

int
zmtp_dealer_send (zmtp_dealer_t *self, zmtp_msg_t *msg)
{
    assert (self);
    assert (self->channel);

    return zmtp_channel_send (self->channel, msg);
}


//  --------------------------------------------------------------------------
//

zmtp_msg_t *
zmtp_dealer_recv (zmtp_dealer_t *self)
{
    assert (self);
    assert (self->channel);

    return zmtp_channel_recv (self->channel);
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
