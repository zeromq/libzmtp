#ifndef __ZMTP_H_INCLUDED__
#define __ZMTP_H_INCLUDED__

#ifdef __cplusplus
extern "C" {
#endif

#include <assert.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>

typedef unsigned char byte;

enum zmtp_socket_type {
    ZMTP_PAIR = 0,
    ZMTP_PUB,
    ZMTP_SUB,
    ZMTP_REQ,
    ZMTP_REP,
    ZMTP_DEALER,
    ZMTP_ROUTER,
    ZMTP_PULL,
    ZMTP_PUSH,
    ZMTP_XPUB,
    ZMTP_XSUB,
    ZMTP_STREAM

};

#ifdef __cplusplus
}
#endif

#endif
