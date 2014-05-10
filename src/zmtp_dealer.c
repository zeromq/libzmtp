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
    zmtp_channel_t *channel;    //  At most one channel per socket now
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
    assert (self);
    if (self->channel)  //  At most one channel per socket now
        return -1;

    //  Create new channel if possible
    self->channel = zmtp_channel_new ();
    if (!self->channel)
        return -1;   

    //  Try to connect channel to specified endpoint
    if (zmtp_channel_ipc_connect (self->channel, path) == -1) {
        zmtp_channel_destroy (&self->channel);
        return -1;
    }
    return 0;
}


//  --------------------------------------------------------------------------
//

int
zmtp_dealer_tcp_connect (zmtp_dealer_t *self,
                         const char *addr, unsigned short port)
{
    assert (self);
    if (self->channel)  //  At most one channel per socket now
        return -1;
    
    //  Create new channel if possible
    self->channel = zmtp_channel_new ();
    if (!self->channel)
        return -1;
    
    //  Try to connect channel to specified endpoint
    if (zmtp_channel_tcp_connect (self->channel, addr, port) == -1) {
        zmtp_channel_destroy (&self->channel);
        return -1;
    }
    return 0;
}


//  --------------------------------------------------------------------------
//

int
zmtp_dealer_connect (zmtp_dealer_t *self, const char *endpoint_str)
{
    assert (self);
    if (self->channel)
        return -1;

    //  Create new channel if possible
    self->channel = zmtp_channel_new ();
    if (!self->channel)
        return -1;

    //  Try to connect channel to specified endpoint
    if (zmtp_channel_connect (self->channel, endpoint_str) == -1) {
        zmtp_channel_destroy (&self->channel);
        return -1;
    }
    return 0;
}

//  --------------------------------------------------------------------------
//  Send a message on a socket

int
zmtp_dealer_send (zmtp_dealer_t *self, zmtp_msg_t *msg)
{
    assert (self);
    if (!self->channel)
        return -1;
    
    return zmtp_channel_send (self->channel, msg);
}


//  --------------------------------------------------------------------------
//  Receive a message from a socket

zmtp_msg_t *
zmtp_dealer_recv (zmtp_dealer_t *self)
{
    assert (self);
    if (!self->channel)
        return NULL;
    
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
