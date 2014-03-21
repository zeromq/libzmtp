//  ZMTP connection class

#include <stdlib.h>
#include <stddef.h>
#include <stdint.h>
#include "../include/zmtp.h"
#include "../include/zmtp_msg.h"
#include "../include/zmtp_connection.h"

//  ZMTP greeting (64 bytes)
struct zmtp_greeting {
    byte signature [10];
    byte version [2];
    byte mechanism [20];
    byte as_server [1];
    byte filler [31];
};

struct zmtp_connection {
    int fd;             //  BSD socket handle
    int socktype;       //  0MQ socket type
};

zmtp_connection_t *
zmtp_connection_new (int fd, int socktype)
{
    zmtp_connection_t *self = (zmtp_connection_t *) malloc (sizeof *self);
    if (self)
        *self = (zmtp_connection_t) { fd, socktype };
    return self;
}

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

static void
tcp_send (int fd, const void *data, size_t len)
{
    const ssize_t rc = send (fd, data, len, 0);
    assert (rc == len);
    assert (rc != -1);
}

static void
s_send_msg (zmtp_connection_t *self, zmtp_msg_t *msg)
{
    assert (self);
    assert (msg);

    const byte flags = zmtp_msg_flags (msg);
    tcp_send (self->fd, &flags, sizeof flags);
    if (zmtp_msg_size (msg) < 255) {
        const byte msg_size = zmtp_msg_size (msg);
        tcp_send (self->fd, &msg_size, sizeof msg_size);
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
        tcp_send (self->fd, buffer, sizeof buffer);
    }
    tcp_send (self->fd, zmtp_msg_data (msg), zmtp_msg_size (msg));
}

static void
tcp_recv (int fd, void *buffer, size_t len)
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

static zmtp_msg_t *
s_recv_msg (zmtp_connection_t *self)
{
    assert (self);

    byte flags, b;
    size_t msg_size;

    tcp_recv (self->fd, &flags, sizeof flags);
    tcp_recv (self->fd, &b, sizeof b);
    if (b < 255)
        msg_size = (size_t) b;
    else {
        byte buffer [8];
        tcp_recv (self->fd, buffer, sizeof buffer);
        const uint64_t msg_size =
            (uint64_t) buffer [7] << 56 |
            (uint64_t) buffer [6] << 48 |
            (uint64_t) buffer [5] << 40 |
            (uint64_t) buffer [4] << 32 |
            (uint64_t) buffer [3] << 24 |
            (uint64_t) buffer [2] << 16 |
            (uint64_t) buffer [1] << 8  |
            (uint64_t) buffer [0];
    }
    byte *data = malloc (msg_size);
    assert (data);
    tcp_recv (self->fd, data, msg_size);

    zmtp_msg_t *msg = zmtp_msg_new (flags, data, msg_size);
    if (!msg)
        free (data);
    return msg;
}

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
    tcp_send (self->fd, outgoing.signature, sizeof outgoing.signature);
    //  Read the first byte.
    struct zmtp_greeting incoming;
    tcp_recv (self->fd, incoming.signature, 1);
    assert (incoming.signature [0] == 0xff);
    //  Read the rest of signature
    tcp_recv (self->fd, incoming.signature + 1, 9);
    assert ((incoming.signature [9] & 1) == 1);
    //  Exchange major version numbers
    tcp_send (self->fd, outgoing.version, 1);
    tcp_recv (self->fd, incoming.version, 1);

    assert (incoming.version [0] == 3);
    //  Send the rest of greeting to the peer.
    tcp_send (self->fd, outgoing.version + 1, 1);
    tcp_send (self->fd, outgoing.mechanism, sizeof outgoing.mechanism);
    tcp_send (self->fd, outgoing.as_server, sizeof outgoing.as_server);
    tcp_send (self->fd, outgoing.filler, sizeof outgoing.filler);
    //  Receive the rest of greeting from the peer.
    tcp_recv (self->fd, incoming.version + 1, 1);
    tcp_recv (self->fd, incoming.mechanism, sizeof incoming.mechanism);
    tcp_recv (self->fd, incoming.as_server, sizeof incoming.as_server);
    tcp_recv (self->fd, incoming.filler, sizeof incoming.filler);
    //  Send READY command
    zmtp_msg_t *ready = zmtp_msg_new (0x04, strdup ("READY   "), 8);
    assert (ready);
    s_send_msg (self, ready);
    zmtp_msg_destroy (&ready);
    //  Receive READY command
    ready = s_recv_msg (self);
    assert ((zmtp_msg_flags (ready) & 0x04) == 0x04);
    zmtp_msg_destroy (&ready);

    return 0;
}

void
zmtp_connection_send (zmtp_connection_t *self, zmtp_msg_t *msg)
{
    assert (self);
    assert (msg);

    s_send_msg (self, msg);
}

zmtp_msg_t *
zmtp_connection_recv (zmtp_connection_t *self)
{
    assert (self);
    return s_recv_msg (self);
}
