//  ZMTP connection class

#ifndef __ZMTP_CONNECTION_H_INCLUDED__
#define __ZMTP_CONNECTION_H_INCLUDED__

#ifdef __cplusplus
extern "C" {
#endif

#include "../include/zmtp_msg.h"

typedef struct zmtp_connection zmtp_connection_t;

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
