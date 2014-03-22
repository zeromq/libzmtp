/*  =========================================================================
    zmtp_msg - message class

    Copyright contributors as noted in the AUTHORS file.
    This file is part of libzmtp, the C ZMTP stack.

    This Source Code Form is subject to the terms of the Mozilla Public
    License, v. 2.0. If a copy of the MPL was not distributed with this
    file, You can obtain one at http://mozilla.org/MPL/2.0/.
    =========================================================================
*/

#include "../include/zmtp.h"

struct zmtp_msg {
    byte flags;
    byte *msg_data;
    size_t msg_size;
};

zmtp_msg_t *
zmtp_msg_new (byte flags, byte *msg_data, size_t msg_size)
{
    zmtp_msg_t *self = (zmtp_msg_t *) malloc (sizeof *self);
    if (self)
        *self = (zmtp_msg_t) { flags, msg_data, msg_size };
    return self;
}

void
zmtp_msg_destroy (zmtp_msg_t **self_p)
{
    assert (self_p);
    if (*self_p) {
        zmtp_msg_t *self = *self_p;
        free (self->msg_data);
        free (self);
        *self_p = NULL;
    }
}

byte
zmtp_msg_flags (zmtp_msg_t *self)
{
    assert (self);
    return self->flags;
}

byte *
zmtp_msg_data (zmtp_msg_t *self)
{
    assert (self);
    return self->msg_data;
}

size_t
zmtp_msg_size (zmtp_msg_t *self)
{
    assert (self);
    return self->msg_size;
}
