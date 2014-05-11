/*  =========================================================================
    zmtp_endpoint - endpoint base class

    Copyright (c) contributors as noted in the AUTHORS file.
    This file is part of libzmtp, the C ZMTP stack.

    This Source Code Form is subject to the terms of the Mozilla Public
    License, v. 2.0. If a copy of the MPL was not distributed with this
    file, You can obtain one at http://mozilla.org/MPL/2.0/.
    =========================================================================
*/

#include "zmtp_classes.h"


//  --------------------------------------------------------------------------
//  Destructor

void
zmtp_endpoint_destroy (zmtp_endpoint_t **self_p)
{
    assert (self_p);
    if (*self_p) {
        zmtp_endpoint_t *self = *self_p;
        assert (self->destroy);
        self->destroy (self_p);
    }
}


//  --------------------------------------------------------------------------
//  Connect to the endpoint

int
zmtp_endpoint_connect (zmtp_endpoint_t *self)
{
    assert (self);
    assert (self->connect);

    return self->connect (self);
}


//  --------------------------------------------------------------------------
//  Listen for new connection on endpoint

int
zmtp_endpoint_listen (zmtp_endpoint_t *self)
{
    assert (self);
    assert (self->listen);

    return self->listen (self);
}
