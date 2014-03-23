/*  =========================================================================
    zmtp_connection - connection class

    Copyright (c) contributors as noted in the AUTHORS file.
    This file is part of libzmtp, the C ZMTP stack.

    This Source Code Form is subject to the terms of the Mozilla Public
    License, v. 2.0. If a copy of the MPL was not distributed with this
    file, You can obtain one at http://mozilla.org/MPL/2.0/.
    =========================================================================
*/

#include "../include/zmtp.h"

//  ZMTP greeting (64 bytes)

struct zmtp_greeting {
    byte signature [10];
    byte version [2];
    byte mechanism [20];
    byte as_server [1];
    byte filler [31];
};

//  Structure of our class

struct _zmtp_connection_t {
    int fd;             //  BSD socket handle
    int socktype;       //  0MQ socket type
};

static void
    s_tcp_send (int fd, const void *data, size_t len);
static void
    s_tcp_recv (int fd, void *buffer, size_t len);


//  --------------------------------------------------------------------------
//  Constructor; takes ownership of fd and closes it automatically when
//  destroying this object.

zmtp_connection_t *
zmtp_connection_new (int fd, int socktype)
{
    zmtp_connection_t *self = (zmtp_connection_t *) zmalloc (sizeof *self);
    assert (self);              //  For now, memory exhaustion is fatal
    self->fd = fd;
    self->socktype = socktype;
    return self;
}


//  --------------------------------------------------------------------------
//  Destructor; closes fd passed to constructor

void
zmtp_connection_destroy (zmtp_connection_t **self_p)
{
    assert (self_p);
    if (*self_p) {
        zmtp_connection_t *self = *self_p;
        close (self->fd);
        free (self);
        *self_p = NULL;
    }
}


//  --------------------------------------------------------------------------
//  Negotiate a ZMTP connection
//  This currently does only ZMTP v3, and will reject older protocols.
//  TODO: test sending random/wrong data to this handler.

int
zmtp_connection_negotiate (zmtp_connection_t *self)
{
    //  This is our greeting (64 octets)
    const struct zmtp_greeting outgoing = {
        .signature = { 0xff, 0, 0, 0, 0, 0, 0, 0, 1, 0x7f },
        .version   = { 3, 0 },
        .mechanism = { 'N', 'U', 'L', 'L', '\0' }
    };
    //  Send protocol signature
    s_tcp_send (self->fd, outgoing.signature, sizeof outgoing.signature);

    //  Read the first byte.
    struct zmtp_greeting incoming;
    s_tcp_recv (self->fd, incoming.signature, 1);
    assert (incoming.signature [0] == 0xff);

    //  Read the rest of signature
    s_tcp_recv (self->fd, incoming.signature + 1, 9);
    assert ((incoming.signature [9] & 1) == 1);

    //  Exchange major version numbers
    s_tcp_send (self->fd, outgoing.version, 1);
    s_tcp_recv (self->fd, incoming.version, 1);

    assert (incoming.version [0] == 3);

    //  Send the rest of greeting to the peer.
    s_tcp_send (self->fd, outgoing.version + 1, 1);
    s_tcp_send (self->fd, outgoing.mechanism, sizeof outgoing.mechanism);
    s_tcp_send (self->fd, outgoing.as_server, sizeof outgoing.as_server);
    s_tcp_send (self->fd, outgoing.filler, sizeof outgoing.filler);

    //  Receive the rest of greeting from the peer.
    s_tcp_recv (self->fd, incoming.version + 1, 1);
    s_tcp_recv (self->fd, incoming.mechanism, sizeof incoming.mechanism);
    s_tcp_recv (self->fd, incoming.as_server, sizeof incoming.as_server);
    s_tcp_recv (self->fd, incoming.filler, sizeof incoming.filler);

    //  Send READY command
    zmtp_msg_t *ready = zmtp_msg_new_const (0x04, "READY   ", 8);
    zmtp_connection_send (self, ready);
    zmtp_msg_destroy (&ready);

    //  Receive READY command
    ready = zmtp_connection_recv (self);
    assert ((zmtp_msg_flags (ready) & 0x04) == 0x04);
    zmtp_msg_destroy (&ready);

    return 0;
}


//  --------------------------------------------------------------------------
//  Send a ZMTP message to the connection

int
zmtp_connection_send (zmtp_connection_t *self, zmtp_msg_t *msg)
{
    assert (self);
    assert (msg);

    const byte flags = zmtp_msg_flags (msg);
    s_tcp_send (self->fd, &flags, sizeof flags);
    if (zmtp_msg_size (msg) < 255) {
        const byte msg_size = zmtp_msg_size (msg);
        s_tcp_send (self->fd, &msg_size, sizeof msg_size);
    }
    else {
        byte buffer [9];
        const uint64_t msg_size = (uint64_t) zmtp_msg_size (msg);
        buffer [0] = 0xff;
        buffer [1] = msg_size >> 56;
        buffer [2] = msg_size >> 48;
        buffer [3] = msg_size >> 40;
        buffer [4] = msg_size >> 32;
        buffer [5] = msg_size >> 24;
        buffer [6] = msg_size >> 16;
        buffer [7] = msg_size >> 8;
        buffer [8] = msg_size;
        s_tcp_send (self->fd, buffer, sizeof buffer);
    }
    s_tcp_send (self->fd, zmtp_msg_data (msg), zmtp_msg_size (msg));
    return 0;
}


//  --------------------------------------------------------------------------
//  Receive a ZMTP message off the connection

zmtp_msg_t *
zmtp_connection_recv (zmtp_connection_t *self)
{
    assert (self);
    assert (self);

    byte flags, first_byte;
    size_t size;

    s_tcp_recv (self->fd, &flags, 1);
    s_tcp_recv (self->fd, &first_byte, 1);
    if (first_byte != 255)
        size = (size_t) first_byte;
    else {
        byte buffer [8];
        s_tcp_recv (self->fd, buffer, sizeof buffer);
        size = (uint64_t) buffer [7] << 56 |
               (uint64_t) buffer [6] << 48 |
               (uint64_t) buffer [5] << 40 |
               (uint64_t) buffer [4] << 32 |
               (uint64_t) buffer [3] << 24 |
               (uint64_t) buffer [2] << 16 |
               (uint64_t) buffer [1] << 8  |
               (uint64_t) buffer [0];
    }
    byte *data = zmalloc (size);
    assert (data);
    s_tcp_recv (self->fd, data, size);
    return zmtp_msg_new (flags, &data, size);
}


//  --------------------------------------------------------------------------
//  Lower-level TCP and ZMTP message I/O functions

static void
s_tcp_send (int fd, const void *data, size_t len)
{
    const ssize_t rc = send (fd, data, len, 0);
    assert (rc == len);
    assert (rc != -1);
}

static void
s_tcp_recv (int fd, void *buffer, size_t len)
{
    size_t bytes_read = 0;
    while (bytes_read < len) {
        const ssize_t n = read (
            fd, (char *) buffer + bytes_read, len - bytes_read);
        assert (n != 0);
        assert (n != -1);
        bytes_read += n;
    }
}


//  --------------------------------------------------------------------------
//  Selftest

void
zmtp_connection_test (bool verbose)
{
    printf (" * zmtp_connection: ");
    //  @selftest
    //  @end
    printf ("OK\n");
}
