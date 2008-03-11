#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.61)
AC_INIT([silvver_cameras], [0.1], [fgarcia.renato@gmail.com])
AM_INIT_AUTOMAKE([-Wall -Werror foreign])
AC_CONFIG_SRCDIR([src/hardCamera.hpp])
AC_CONFIG_HEADER([config.h])
AC_LANG([C++])

dnl ========================================================================
dnl === Checks for programs.
dnl ========================================================================

AC_PROG_CXX

dnl ========================================================================
dnl === Checks for libraries.
dnl ========================================================================

AC_CHECK_LIB([boost_thread], [sleep])
AC_CHECK_LIB([cv], [cvReleaseImage])
AC_CHECK_LIB([highgui], [cvLoadImage])
AC_CHECK_LIB([ARToolKitPlus], [main])

dnl ========================================================================
dnl === Checks for header files.                                           =
dnl ========================================================================

AC_CHECK_HEADERS([ARToolKitPlus/TrackerSingleMarkerImpl.h],
                 [],[AC_MSG_ERROR([ARToolKitPlus headers was not found])])

AC_CHECK_HEADERS([opencv/cv.h opencv/highgui.h],
                 [],[AC_MSG_ERROR([OpenCV headers was not found])])

AC_CHECK_HEADERS([pgrflycapture.h pgrflycaptureplus.h],
                 [AC_DEFINE([HAVE_PGRFLYCAPTURE_HEADERS],[1],[Cameras dragonFly])] [compile_pgr=true],
		 [compile_pgr=false])
AM_CONDITIONAL([COMPILE_PGR], [test $compile_pgr = true])

dnl ====
AC_CHECK_HEADERS([limits.h sys/timeb.h])

AC_CHECK_HEADERS([arpa/inet.h netdb.h netinet/in.h sys/socket.h],
                 [AC_DEFINE([HAVE_LINUX_SOCKETS],[1],[def])])

AC_CHECK_HEADERS([winsock2.h],
                 [AC_DEFINE([HAVE_WINDOWS_SOCKETS],[1],[def])] )

dnl ========================================================================
dnl === Checks for library functions.
dnl ========================================================================

AC_CHECK_FUNCS([ftime socket])

AC_CONFIG_FILES([Makefile
                 src/Makefile])
AC_OUTPUT
