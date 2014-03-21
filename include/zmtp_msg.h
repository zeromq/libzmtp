//  ZMTP message class

#ifndef __ZMTP_MSG_H_INCLUDED__
#define __ZMTP_MSG_H_INCLUDED__

#ifdef __cplusplus
extern "C" {
#endif

#include "../include/zmtp.h"

typedef struct zmtp_msg zmtp_msg_t;

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
