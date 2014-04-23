/*  =========================================================================
    zmtp_selftest - run self tests

    Copyright (c) contributors as noted in the AUTHORS file.
    This file is part of libzmtp, the C ZMTP stack.

    This Source Code Form is subject to the terms of the Mozilla Public
    License, v. 2.0. If a copy of the MPL was not distributed with this
    file, You can obtain one at http://mozilla.org/MPL/2.0/.
    =========================================================================
*/

#include "zmtp_classes.h"

int main (int argc, char *argv [])
{
//     bool verbose;
//     if (argc == 2 && strcmp (argv [1], "-v") == 0)
//         verbose = true;
//     else
//         verbose = false;
//
//     printf ("Running self tests...\n");
//     zmtp_msg_test (verbose);
//     printf ("Tests passed OK\n");
    zmtp_msg_test (false);
    zmtp_channel_test (false);
    return 0;
}
