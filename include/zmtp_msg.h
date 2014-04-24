/*  =========================================================================
    zmtp_msg - message class

    Copyright (c) contributors as noted in the AUTHORS file.
    This file is part of libzmtp, the C ZMTP stack.

    This Source Code Form is subject to the terms of the Mozilla Public
    License, v. 2.0. If a copy of the MPL was not distributed with this
    file, You can obtain one at http://mozilla.org/MPL/2.0/.
    =========================================================================
*/

#ifndef __ZMTP_MSG_H_INCLUDED__
#define __ZMTP_MSG_H_INCLUDED__

#ifdef __cplusplus
extern "C" {
#endif

//  Definitions of message flags.
enum {
    ZMTP_MSG_MORE = 1,
    ZMTP_MSG_COMMAND = 4,
};

//  Opaque class structure
typedef struct _zmtp_msg_t zmtp_msg_t;

//  @interface
//  Constructor; it allocates buffer for message data.
//  The initial content of the allocated buffer is undefined.
zmtp_msg_t *
    zmtp_msg_new (byte flags, size_t size);

//  Constructor; takes ownership of data and frees it when destroying the
//  message. Nullifies the data reference.
zmtp_msg_t *
    zmtp_msg_from_data (byte flags, byte **data_p, size_t size);

//  Constructor that takes a constant data and does not copy, modify, or
//  free it.
zmtp_msg_t *
    zmtp_msg_from_const_data (byte flags, void *data, size_t size);

//  Destructor; frees message data and destroys the message
void
    zmtp_msg_destroy (zmtp_msg_t **self_p);

//  Return message flags property
byte
    zmtp_msg_flags (zmtp_msg_t *self);

//  Return message data property
byte *
    zmtp_msg_data (zmtp_msg_t *self);

//  Return message size property
size_t
    zmtp_msg_size (zmtp_msg_t *self);

//  Self test of this class
void
    zmtp_msg_test (bool verbose);
//  @end

#ifdef __cplusplus
}
#endif

#endif
