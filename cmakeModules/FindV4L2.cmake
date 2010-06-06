# - Locate the Video for Linux 2 library
#
# This module defines
#  DC1394v2_INCLUDE_DIRS, where to find the library headers
#  DC1394v2_LIBRARIES, the libraries names
#  DC1394v2_FOUND, If true the library was found

#=============================================================================
# Copyright 2010 Renato Florentino Garcia
#
# Distributed under the OSI-approved BSD License (the "License");
# see accompanying file Copyright.txt for details.
#
# This software is distributed WITHOUT ANY WARRANTY; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
# See the License for more information.
#=============================================================================
# (To distributed this file outside of CMake, substitute the full
#  License text for the above reference.)

FIND_PATH(V4L2_INCLUDE_DIR libv4l2.h)

FIND_LIBRARY(V4L2_LIBRARY v4l2)

INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(V4L2 DEFAULT_MSG
                                  V4L2_LIBRARY
                                  V4L2_INCLUDE_DIR)

SET(V4L2_INCLUDE_DIRS ${V4L2_INCLUDE_DIRS})
SET(V4L2_LIBRARIES ${V4L2_LIBRARY})

MARK_AS_ADVANCED(V4L2_LIBRARY V4L2_INCLUDE_DIR)
