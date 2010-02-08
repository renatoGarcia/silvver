# - Locate the dc1394 version 1 library
#
# This module defines
#  DC1394v1_INCLUDE_DIRS, where to find the library headers
#  DC1394v1_LIBRARIES, the libraries names
#  DC1394v1_FOUND, If true the library was found

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

FIND_PATH(DC1394V1_INCLUDE_DIR dc1394_control.h
          PATH_SUFFIXES libdc1394)

FIND_LIBRARY(DC1394V1_LIBRARY dc1394_control)

INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(DC1394V1 DEFAULT_MSG
                                  DC1394V1_LIBRARY
                                  DC1394V1_INCLUDE_DIR)

SET(DC1394V1_INCLUDE_DIRS ${DC1394V1_INCLUDE_DIRS})
SET(DC1394V1_LIBRARIES ${DC1394V1_LIBRARY})

MARK_AS_ADVANCED(DC1394V1_LIBRARY DC1394V1_INCLUDE_DIR)
