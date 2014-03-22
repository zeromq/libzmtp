#ifndef __ZMTP_H_INCLUDED__
#define __ZMTP_H_INCLUDED__

//  ZMTP version macros for compile-time API detection

#define ZMTP_VERSION_MAJOR 0
#define ZMTP_VERSION_MINOR 1
#define ZMTP_VERSION_PATCH 0

#define ZMTP_MAKE_VERSION(major, minor, patch) \
    ((major) * 10000 + (minor) * 100 + (patch))
#define ZMTP_VERSION \
    ZMTP_MAKE_VERSION(ZMTP_VERSION_MAJOR, ZMTP_VERSION_MINOR, ZMTP_VERSION_PATCH)

#include <assert.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <netdb.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>

typedef unsigned char byte;

#include "zmtp_msg.h"
#include "zmtp_connection.h"
#include "zmtp_dealer.h"

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

#endif
