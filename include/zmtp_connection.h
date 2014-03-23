/*  =========================================================================
    zmtp_connection - connection class

    Copyright (c) contributors as noted in the AUTHORS file.
    This file is part of libzmtp, the C ZMTP stack.

    This Source Code Form is subject to the terms of the Mozilla Public
    License, v. 2.0. If a copy of the MPL was not distributed with this
    file, You can obtain one at http://mozilla.org/MPL/2.0/.
    =========================================================================
*/

#ifndef __ZMTP_CONNECTION_H_INCLUDED__
#define __ZMTP_CONNECTION_H_INCLUDED__

#ifdef __cplusplus
extern "C" {
#endif

//  Opaque class structure
typedef struct _zmtp_connection_t zmtp_connection_t;

//  @interface
//  Constructor; takes ownership of fd and closes it automatically when
//  destroying this object.
zmtp_connection_t *
    zmtp_connection_new (int fd, int socktype);

//  Destructor; closes fd passed to constructor
void
    zmtp_connection_destroy (zmtp_connection_t **self_p);

//  Negotiate a ZMTP connection
//  This currently does only ZMTP v3, and will reject older protocols.
//  TODO: test sending random/wrong data to this handler.
int
    zmtp_connection_negotiate (zmtp_connection_t *self);

//  Send a ZMTP message to the connection
int
    zmtp_connection_send (zmtp_connection_t *self, zmtp_msg_t *msg);

//  Receive a ZMTP message off the connection
zmtp_msg_t *
    zmtp_connection_recv (zmtp_connection_t *self);

//  Self test of this class
void
    zmtp_connection_test (bool verbose);
//  @end

#ifdef __cplusplus
}
#endif

#endif
