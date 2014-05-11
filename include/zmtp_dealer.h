/*  =========================================================================
    zmtp_dealer - DEALER socket class

    Copyright (c) contributors as noted in the AUTHORS file.
    This file is part of libzmtp, the C ZMTP stack.

    This Source Code Form is subject to the terms of the Mozilla Public
    License, v. 2.0. If a copy of the MPL was not distributed with this
    file, You can obtain one at http://mozilla.org/MPL/2.0/.
    =========================================================================
*/

#ifndef __ZMTP_DEALER_H_INCLUDED__
#define __ZMTP_DEALER_H_INCLUDED__

#ifdef __cplusplus
extern "C" {
#endif

//  Opaque class structure
typedef struct _zmtp_dealer_t zmtp_dealer_t;

//  @interface
//  Constructor; takes ownership of data and frees it when destroying the
//  message. Nullifies the data reference.
zmtp_dealer_t *
    zmtp_dealer_new (void);

void
    zmtp_dealer_destroy (zmtp_dealer_t **self_p);

int
    zmtp_dealer_ipc_connect (zmtp_dealer_t *self, const char *addr);

int
    zmtp_dealer_tcp_connect (zmtp_dealer_t *self,
                             const char *addr, unsigned short port);

int
    zmtp_dealer_connect (zmtp_dealer_t *self, const char *endpoint_str);

int
    zmtp_dealer_listen (zmtp_dealer_t *self, const char *endpoint_str);

int
    zmtp_dealer_send (zmtp_dealer_t *self, zmtp_msg_t *msg);

zmtp_msg_t *
    zmtp_dealer_recv (zmtp_dealer_t *self);

//  Self test of this class
void
    zmtp_dealer_test (bool verbose);
//  @end

#ifdef __cplusplus
}
#endif

#endif
