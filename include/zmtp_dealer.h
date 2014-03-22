/*  =========================================================================
    zmtp_dealer - DEALER socket class

    Copyright contributors as noted in the AUTHORS file.
    This file is part of libzmtp, the C ZMTP stack.

    This Source Code Form is subject to the terms of the Mozilla Public
    License, v. 2.0. If a copy of the MPL was not distributed with this
    file, You can obtain one at http://mozilla.org/MPL/2.0/.
    =========================================================================
*/

#ifndef __ZMTP_DEALER_H_INCLUDED__
#define __ZMTP_DEALER_H_INCLUDED__

typedef struct zmtp_dealer zmtp_dealer_t;

#ifdef __cplusplus
extern "C" {
#endif

zmtp_dealer_t *
    zmtp_dealer_new (int fd);

zmtp_dealer_t *
    zmtp_dealer_ipc_connect (const char *addr);

zmtp_dealer_t *
    zmtp_dealer_tcp_connect (const char *addr, unsigned short port);

void
    zmtp_dealer_destroy (zmtp_dealer_t **self_p);

int
    zmtp_dealer_send (zmtp_dealer_t *self, zmtp_msg_t *msg);

zmtp_msg_t *
    zmtp_dealer_recv (zmtp_dealer_t *self);

#ifdef __cplusplus
}
#endif

#endif
