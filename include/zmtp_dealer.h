//  Dealer socket class
//
#ifndef __ZMTP_DEALER_H_INCLUDED__
#define __ZMTP_DEALER_H_INCLUDED__

#ifdef __cplusplus
extern "C" {
#endif

#include "../include/zmtp_msg.h"

typedef struct zmtp_dealer zmtp_dealer_t;

zmtp_dealer_t *
    zmtp_dealer_new (int fd);

zmtp_dealer_t *
    zmtp_dealer_ipc_connect (const char *addr);

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
