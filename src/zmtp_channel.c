/*  =========================================================================
    zmtp_channel - channel class

    Copyright (c) contributors as noted in the AUTHORS file.
    This file is part of libzmtp, the C ZMTP stack.

    This Source Code Form is subject to the terms of the Mozilla Public
    License, v. 2.0. If a copy of the MPL was not distributed with this
    file, You can obtain one at http://mozilla.org/MPL/2.0/.
    =========================================================================
*/

#include "zmtp_classes.h"

//  ZMTP greeting (64 bytes)

struct zmtp_greeting {
    byte signature [10];
    byte version [2];
    byte mechanism [20];
    byte as_server [1];
    byte filler [31];
};

//  Structure of our class

struct _zmtp_channel_t {
    int fd;             //  BSD socket handle
};

static int
    s_negotiate (zmtp_channel_t *self);
static int
    s_tcp_send (int fd, const void *data, size_t len);
static int
    s_tcp_recv (int fd, void *buffer, size_t len);


//  --------------------------------------------------------------------------
//  Constructor

zmtp_channel_t *
zmtp_channel_new ()
{
    zmtp_channel_t *self = (zmtp_channel_t *) zmalloc (sizeof *self);
    assert (self);              //  For now, memory exhaustion is fatal
    self->fd = -1;
    return self;
}


//  --------------------------------------------------------------------------
//  Destructor; closes fd if connected

void
zmtp_channel_destroy (zmtp_channel_t **self_p)
{
    assert (self_p);
    if (*self_p) {
        zmtp_channel_t *self = *self_p;
        if (self->fd != -1)
            close (self->fd);
        free (self);
        *self_p = NULL;
    }
}


//  --------------------------------------------------------------------------
//  Connect channel to local endpoint

int
zmtp_channel_ipc_connect (zmtp_channel_t *self, const char *path)
{
    assert (self);

    if (self->fd != -1)
        return -1;
    struct sockaddr_un remote = { .sun_family = AF_UNIX };
    if (strlen (path) >= sizeof remote.sun_path)
        return -1;
    strcpy (remote.sun_path, path);
    //  Create socket
    const int s = socket (AF_UNIX, SOCK_STREAM, 0);
    if (s == -1)
        return -1;
    //  Connect the socket
    const int rc =
        connect (s, (const struct sockaddr *) &remote, sizeof remote);
    if (rc == -1) {
        close (s);
        return -1;
    }
    self->fd = s;
    if (s_negotiate (self) == -1) {
        close (self->fd);
        self->fd = -1;
        return -1;
    }
    return 0;
}


//  --------------------------------------------------------------------------
//  Connect channel to TCP endpoint

int
zmtp_channel_tcp_connect (zmtp_channel_t *self,
                          const char *addr, unsigned short port)
{
    assert (self);

    if (self->fd != -1)
        return -1;
    //  Create socket
    const int s = socket (AF_INET, SOCK_STREAM, 0);
    if (s == -1)
        return -1;
    //  Resolve address
    const struct addrinfo hints = {
        .ai_family   = AF_INET,
        .ai_socktype = SOCK_STREAM,
        .ai_flags    = AI_NUMERICHOST | AI_NUMERICSERV
    };
    char service [8 + 1];
    snprintf (service, sizeof service, "%u", port);
    struct addrinfo *result = NULL;
    if (getaddrinfo (addr, service, &hints, &result)) {
        close (s);
        return -1;
    }
    assert (result);
    //  Create socket
    const int rc = connect (s, result->ai_addr, result->ai_addrlen);
    freeaddrinfo (result);
    if (rc == -1) {
        close (s);
        return -1;
    }
    self->fd = s;
    if (s_negotiate (self) == -1) {
        close (self->fd);
        self->fd = -1;
        return -1;
    }
    return 0;
}


//  --------------------------------------------------------------------------
//  Negotiate a ZMTP channel
//  This currently does only ZMTP v3, and will reject older protocols.
//  TODO: test sending random/wrong data to this handler.

static int
s_negotiate (zmtp_channel_t *self)
{
    assert (self);
    assert (self->fd != -1);

    const int s = self->fd;

    //  This is our greeting (64 octets)
    const struct zmtp_greeting outgoing = {
        .signature = { 0xff, 0, 0, 0, 0, 0, 0, 0, 1, 0x7f },
        .version   = { 3, 0 },
        .mechanism = { 'N', 'U', 'L', 'L', '\0' }
    };
    //  Send protocol signature
    if (s_tcp_send (s, outgoing.signature, sizeof outgoing.signature) == -1)
        goto io_error;

    //  Read the first byte.
    struct zmtp_greeting incoming;
    if (s_tcp_recv (s, incoming.signature, 1) == -1)
        goto io_error;
    assert (incoming.signature [0] == 0xff);

    //  Read the rest of signature
    if (s_tcp_recv (s, incoming.signature + 1, 9) == -1)
        goto io_error;
    assert ((incoming.signature [9] & 1) == 1);

    //  Exchange major version numbers
    if (s_tcp_send (s, outgoing.version, 1) == -1)
        goto io_error;
    if (s_tcp_recv (s, incoming.version, 1) == -1)
        goto io_error;

    assert (incoming.version [0] == 3);

    //  Send the rest of greeting to the peer.
    if (s_tcp_send (s, outgoing.version + 1, 1) == -1)
        goto io_error;
    if (s_tcp_send (s, outgoing.mechanism, sizeof outgoing.mechanism) == -1)
        goto io_error;
    if (s_tcp_send (s, outgoing.as_server, sizeof outgoing.as_server) == -1)
        goto io_error;
    if (s_tcp_send (s, outgoing.filler, sizeof outgoing.filler) == -1)
        goto io_error;

    //  Receive the rest of greeting from the peer.
    if (s_tcp_recv (s, incoming.version + 1, 1) == -1)
        goto io_error;
    if (s_tcp_recv (s, incoming.mechanism, sizeof incoming.mechanism) == -1)
        goto io_error;
    if (s_tcp_recv (s, incoming.as_server, sizeof incoming.as_server) == -1)
        goto io_error;
    if (s_tcp_recv (s, incoming.filler, sizeof incoming.filler) == -1)
        goto io_error;

    //  Send READY command
    zmtp_msg_t *ready = zmtp_msg_from_const_data (0x04, "\5READY", 6);
    assert (ready);
    zmtp_channel_send (self, ready);
    zmtp_msg_destroy (&ready);

    //  Receive READY command
    ready = zmtp_channel_recv (self);
    if (!ready)
        goto io_error;
    assert ((zmtp_msg_flags (ready) & 0x04) == 0x04);
    zmtp_msg_destroy (&ready);

    return 0;

io_error:
    return -1;
}


//  --------------------------------------------------------------------------
//  Send a ZMTP message to the channel

int
zmtp_channel_send (zmtp_channel_t *self, zmtp_msg_t *msg)
{
    assert (self);
    assert (msg);

    byte frame_flags = zmtp_msg_flags (msg) & 0x04;
    if (zmtp_msg_size (msg) > 255)
        frame_flags |= 0x02;
    if (s_tcp_send (self->fd, &frame_flags, sizeof frame_flags) == -1)
        return -1;

    if (zmtp_msg_size (msg) <= 255) {
        const byte msg_size = zmtp_msg_size (msg);
        if (s_tcp_send (self->fd, &msg_size, sizeof msg_size) == -1)
            return -1;
    }
    else {
        byte buffer [8];
        const uint64_t msg_size = (uint64_t) zmtp_msg_size (msg);
        buffer [0] = msg_size >> 56;
        buffer [1] = msg_size >> 48;
        buffer [2] = msg_size >> 40;
        buffer [3] = msg_size >> 32;
        buffer [4] = msg_size >> 24;
        buffer [5] = msg_size >> 16;
        buffer [6] = msg_size >> 8;
        buffer [7] = msg_size;
        if (s_tcp_send (self->fd, buffer, sizeof buffer) == -1)
            return -1;
    }
    if (s_tcp_send (self->fd, zmtp_msg_data (msg), zmtp_msg_size (msg)) == -1)
        return -1;
    return 0;
}


//  --------------------------------------------------------------------------
//  Receive a ZMTP message off the channel

zmtp_msg_t *
zmtp_channel_recv (zmtp_channel_t *self)
{
    assert (self);

    byte frame_flags;
    size_t size;

    if (s_tcp_recv (self->fd, &frame_flags, 1) == -1)
        return NULL;
    //  Check large flag
    if ((frame_flags & 0x02) == 0) {
        byte buffer [1];
        if (s_tcp_recv (self->fd, buffer, 1) == -1)
            return NULL;
        size = (size_t) buffer [0];
    }
    else {
        byte buffer [8];
        if (s_tcp_recv (self->fd, buffer, sizeof buffer) == -1)
            return NULL;
        size = (uint64_t) buffer [0] << 56 |
               (uint64_t) buffer [1] << 48 |
               (uint64_t) buffer [2] << 40 |
               (uint64_t) buffer [3] << 32 |
               (uint64_t) buffer [4] << 24 |
               (uint64_t) buffer [5] << 16 |
               (uint64_t) buffer [6] << 8  |
               (uint64_t) buffer [7];
    }
    byte *data = zmalloc (size);
    assert (data);
    if (s_tcp_recv (self->fd, data, size) == -1) {
        free (data);
        return NULL;
    }
    return zmtp_msg_from_data (frame_flags & 0x04, &data, size);
}


//  --------------------------------------------------------------------------
//  Lower-level TCP and ZMTP message I/O functions

static int
s_tcp_send (int fd, const void *data, size_t len)
{
    size_t bytes_sent = 0;
    while (bytes_sent < len) {
        const ssize_t rc = send (
            fd, (char *) data + bytes_sent, len - bytes_sent, 0);
        if (rc == -1 && errno == EINTR)
            continue;
        if (rc == -1)
            return -1;
        bytes_sent += rc;
    }
    return 0;
}

static int
s_tcp_recv (int fd, void *buffer, size_t len)
{
    size_t bytes_read = 0;
    while (bytes_read < len) {
        const ssize_t n = recv (
            fd, (char *) buffer + bytes_read, len - bytes_read, 0);
        if (n == -1 && errno == EINTR)
            continue;
        if (n == -1 || n == 0)
            return -1;
        bytes_read += n;
    }
    return 0;
}

#include <poll.h>

//  Simple TCP echo server. It listens on a TCP port and after
//  accepting a new connection, echoes all received data.
//  This is to test the encodining/decoding compatibility.

struct echo_serv_t {
    unsigned short port;
};

static void *
s_echo_serv (void *arg)
{
    struct echo_serv_t *params = (struct echo_serv_t *) arg;

    //  Create socket
    const int s = socket (AF_INET, SOCK_STREAM, 0);
    assert (s != -1);
    //  Allow port reuse
    const int on = 1;
    int rc = setsockopt (s, SOL_SOCKET, SO_REUSEADDR, &on, sizeof on);
    assert (rc == 0);
    //  Fill address
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons (params->port);
    server_addr.sin_addr.s_addr = htonl (INADDR_ANY);
    //  Bind socket
    rc = bind (s, (struct sockaddr *) &server_addr, sizeof server_addr);
    assert (rc == 0);
    //  Listen for connections
    rc = listen (s, 1);
    assert (rc != -1);
    //  Accept connection
    int fd = accept (s, NULL, NULL);
    assert (fd != -1);
    //  Set non-blocking mode
    const int flags = fcntl (fd, F_GETFL, 0);
    assert (flags != -1);
    rc = fcntl (fd, F_SETFL, flags | O_NONBLOCK);
    assert (rc == 0);
    unsigned char buf [80];
    //  Echo all received data
    while (1) {
        struct pollfd pollfd;
        pollfd.fd = fd;
        pollfd.events = POLLIN;
        rc = poll (&pollfd, 1, -1);
        assert (rc == 1);
        rc = read (fd, buf, sizeof buf);
        if (rc == 0)
            break;
        assert (rc > 0 || errno == EINTR);
        if (rc > 0) {
            rc = s_tcp_send (fd, buf, rc);
            assert (rc == 0);
        }
    }
    close (fd);
    close (s);
    return NULL;
}


//  --------------------------------------------------------------------------
//  Selftest

void
zmtp_channel_test (bool verbose)
{
    printf (" * zmtp_channel: ");
    //  @selftest
    pthread_t thread;
    struct echo_serv_t echo_serv_params = { .port = 22001 };
    pthread_create (&thread, NULL, s_echo_serv, &echo_serv_params);
    sleep (1);
    zmtp_channel_t *channel = zmtp_channel_new ();
    assert (channel);
    int rc = zmtp_channel_tcp_connect (channel, "127.0.0.1", 22001);
    assert (rc == 0);
    char *test_strings [] = {
        "1",
        "22",
        "333",
        "4444",
        "55555"};
    for (int i = 0; i < 5; i++) {
        zmtp_msg_t *msg = zmtp_msg_from_const_data (
            0, test_strings [i], strlen (test_strings [i]));
        assert (msg);
        rc = zmtp_channel_send (channel, msg);
        assert (rc == 0);
        zmtp_msg_t *msg2 = zmtp_channel_recv (channel);
        assert (msg2 != NULL);
        assert (zmtp_msg_size (msg) == zmtp_msg_size (msg2));
        assert (memcmp (zmtp_msg_data (msg),
            zmtp_msg_data (msg2), zmtp_msg_size (msg)) == 0);
        zmtp_msg_destroy (&msg);
        zmtp_msg_destroy (&msg2);
    }
    zmtp_channel_destroy (&channel);
    pthread_join (thread, NULL);
    //  @end
    printf ("OK\n");
}
