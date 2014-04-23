/*  =========================================================================
    zmtp_msg - message class

    Copyright (c) contributors as noted in the AUTHORS file.
    This file is part of libzmtp, the C ZMTP stack.

    This Source Code Form is subject to the terms of the Mozilla Public
    License, v. 2.0. If a copy of the MPL was not distributed with this
    file, You can obtain one at http://mozilla.org/MPL/2.0/.
    =========================================================================
*/

#include "zmtp_classes.h"

//  Structure of our class

struct _zmtp_msg_t {
    byte flags;                 //  Flags byte for message
    byte *data;                 //  Data part of message
    size_t size;                //  Size of data in bytes
    bool greedy;                //  Did we take ownership of data?
};


//  --------------------------------------------------------------------------
//  Constructor; it allocates buffer for message data.
//  The initial content of the allocated buffer is undefined.

zmtp_msg_t *
zmtp_msg_new (byte flags, size_t size)
{
    zmtp_msg_t *self = (zmtp_msg_t *) zmalloc (sizeof *self);
    assert (self);              //  For now, memory exhaustion is fatal
    self->flags = flags;
    self->data = (byte *) malloc (size);
    assert (size == 0 || self->data);
    self->size = size;
    self->greedy = true;
    return self;
}


//  --------------------------------------------------------------------------
//  Constructor; takes ownership of data and frees it when destroying the
//  message. Nullifies the data reference.

zmtp_msg_t *
zmtp_msg_from_data (byte flags, byte **data_p, size_t size)
{
    assert (data_p);
    zmtp_msg_t *self = (zmtp_msg_t *) zmalloc (sizeof *self);
    assert (self);              //  For now, memory exhaustion is fatal
    self->flags = flags;
    self->data = *data_p;
    self->size = size;
    self->greedy = true;
    *data_p = NULL;
    return self;
}


//  --------------------------------------------------------------------------
//  Constructor that takes a constant data and does not copy, modify, or
//  free it.

zmtp_msg_t *
zmtp_msg_from_const_data (byte flags, void *data, size_t size)
{
    zmtp_msg_t *self = (zmtp_msg_t *) zmalloc (sizeof *self);
    assert (self);              //  For now, memory exhaustion is fatal
    self->flags = flags;
    self->data = data;
    self->size = size;
    self->greedy = false;
    return self;
}


//  --------------------------------------------------------------------------
//  Destructor; frees message data and destroys the message

void
zmtp_msg_destroy (zmtp_msg_t **self_p)
{
    assert (self_p);
    if (*self_p) {
        zmtp_msg_t *self = *self_p;
        if (self->greedy)
            free (self->data);
        free (self);
        *self_p = NULL;
    }
}

//  --------------------------------------------------------------------------
//  Return message flags property

byte
zmtp_msg_flags (zmtp_msg_t *self)
{
    assert (self);
    return self->flags;
}

//  --------------------------------------------------------------------------
//  Return message data property

byte *
zmtp_msg_data (zmtp_msg_t *self)
{
    assert (self);
    return self->data;
}


//  --------------------------------------------------------------------------
//  Return message size property

size_t
zmtp_msg_size (zmtp_msg_t *self)
{
    assert (self);
    return self->size;
}


//  --------------------------------------------------------------------------
//  Selftest

void
zmtp_msg_test (bool verbose)
{
    printf (" * zmtp_msg: ");
    //  @selftest
    zmtp_msg_t *msg = zmtp_msg_from_const_data (0, "hello", 6);
    assert (msg);
    assert (zmtp_msg_flags (msg) == 0);
    assert (zmtp_msg_size (msg) == 6);
    assert (memcmp (zmtp_msg_data (msg), "hello", 6) == 0);
    zmtp_msg_destroy (&msg);
    assert (msg == NULL);
    //  @end
    printf ("OK\n");
}
