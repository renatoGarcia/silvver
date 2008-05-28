#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.61)
AC_INIT([silver_servidor], [0.1])
AM_INIT_AUTOMAKE([-Wall -Werror foreign])
AC_CONFIG_SRCDIR([src/saidas.h])
AC_CONFIG_HEADER([config.h])
AC_LANG([C++])

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

AC_CHECK_HEADERS([limits.h sys/timeb.h])

AC_CHECK_HEADERS([arpa/inet.h netdb.h netinet/in.h sys/socket.h],
                 [AC_DEFINE([HAVE_LINUX_SOCKETS],[1],[def])] )

AC_CHECK_HEADERS([winsock2.h],
                 [AC_DEFINE([HAVE_WINDOWS_SOCKETS],[1],[def])] )

dnl ========================================================================
dnl === Checks for typedefs, structures, and compiler characteristics.     =
dnl ========================================================================

AC_C_CONST
AC_HEADER_STDBOOL

dnl ========================================================================
dnl === Checks for library functions.
dnl ========================================================================

AC_CHECK_FUNCS([ftime socket])

AC_CONFIG_FILES([Makefile
                 src/Makefile])
AC_OUTPUT
