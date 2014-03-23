lib_LTLIBRARIES = libzmtp.la

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = libzmtp.pc

include_HEADERS = \
    ../include/zmtp.h \
    ../include/zmtp_msg.h \
    ../include/zmtp_dealer.h

libzmtp_la_SOURCES = \
    platform.h \
    zmtp_msg.c \
    zmtp_channel.h \
    zmtp_channel.c \
    zmtp_dealer.c

AM_CFLAGS = -g
AM_CPPFLAGS = -I$(top_srcdir)/include
bin_PROGRAMS = libzmtp_selftest
libzmtp_selftest_LDADD = libzmtp.la
libzmtp_selftest_SOURCES = zmtp_selftest.c
libzmtp_la_LDFLAGS = -version-info @LTVER@
TESTS = zmtp_selftest
