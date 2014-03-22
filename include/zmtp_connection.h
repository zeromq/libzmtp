/*  =========================================================================
    zmtp_connection - connection class

    Copyright contributors as noted in the AUTHORS file.
    This file is part of libzmtp, the C ZMTP stack.

    This Source Code Form is subject to the terms of the Mozilla Public
    License, v. 2.0. If a copy of the MPL was not distributed with this
    file, You can obtain one at http://mozilla.org/MPL/2.0/.
    =========================================================================
*/

#ifndef __ZMTP_CONNECTION_H_INCLUDED__
#define __ZMTP_CONNECTION_H_INCLUDED__

typedef struct zmtp_connection zmtp_connection_t;

#ifdef __cplusplus
extern "C" {
#endif

zmtp_connection_t *
    zmtp_connection_new (int fd, int socktype);

void
    zmtp_connection_destroy (zmtp_connection_t **self_p);

int
    zmtp_connection_negotiate (zmtp_connection_t *self);

int
    zmtp_connection_send (zmtp_connection_t *self, zmtp_msg_t *msg);

zmtp_msg_t *
    zmtp_connection_recv (zmtp_connection_t *self);

#ifdef __cplusplus
}
#endif

#endif