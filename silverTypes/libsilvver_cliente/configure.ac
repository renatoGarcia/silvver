#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.61)
AC_INIT([libsilvver-cliente], [0.1], [renato@a.a])
AM_INIT_AUTOMAKE([-Wall -Werror foreign])
AM_PROG_LIBTOOL
AC_PROG_INSTALL

AC_CONFIG_SRCDIR([src/marco.cpp])
AC_CONFIG_HEADER([config.h])

dnl ========================================================================
dnl === Checks for programs.
dnl ========================================================================

AC_PROG_CXX
AC_PROG_CC

dnl ========================================================================
dnl === Checks for libraries.
dnl ========================================================================

AC_CHECK_LIB([boost_thread], [sleep])

dnl ========================================================================
dnl === Checks for header files.                                           =
dnl ========================================================================

AC_CHECK_HEADERS([arpa/inet.h netdb.h netinet/in.h sys/socket.h],
                 [AC_DEFINE([HAVE_LINUX_SOCKETS],[1],[Headers for use sockets in linux])] )


dnl ========================================================================
dnl === Checks for typedefs, structures, and compiler characteristics.     =
dnl ========================================================================

AC_HEADER_STDBOOL
AC_C_CONST

dnl ========================================================================
dnl === Checks for library functions.
dnl ========================================================================

AC_CHECK_FUNCS([pow socket sqrt])
AC_CONFIG_FILES([Makefile
                 src/Makefile])
AC_OUTPUT
