/*  =========================================================================
    zmtp_msg - message class

    Copyright contributors as noted in the AUTHORS file.
    This file is part of libzmtp, the C ZMTP stack.

    This Source Code Form is subject to the terms of the Mozilla Public
    License, v. 2.0. If a copy of the MPL was not distributed with this
    file, You can obtain one at http://mozilla.org/MPL/2.0/.
    =========================================================================
*/

#ifndef __ZMTP_MSG_H_INCLUDED__
#define __ZMTP_MSG_H_INCLUDED__

typedef struct zmtp_msg zmtp_msg_t;

#ifdef __cplusplus
extern "C" {
#endif

zmtp_msg_t *
    zmtp_msg_new (byte flags, byte *msg_data, size_t msg_size);

void
    zmtp_msg_destroy (zmtp_msg_t **self_p);

byte
    zmtp_msg_flags (zmtp_msg_t *self);

byte *
    zmtp_msg_data (zmtp_msg_t *self);

size_t
    zmtp_msg_size (zmtp_msg_t *self);

#ifdef __cplusplus
}
#endif

#endif
