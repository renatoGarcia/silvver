# - Locate the ARToolKitPlus library
#
# This module defines
#  ARTKP_INCLUDE_DIRS, where to find the library headers
#  ARTKP_LIBRARIES, the libraries names
#  ARTKP_FOUND, If true the library was found

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

FIND_PATH(ARTKP_INCLUDE_DIR ARToolKitPlus.h
          PATH_SUFFIXES ARToolKitPlus)

FIND_LIBRARY(ARTKP_LIBRARY ARToolKitPlus)

INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(ARTKP DEFAULT_MSG
                                  ARTKP_LIBRARY
                                  ARTKP_INCLUDE_DIR)

SET(ARTKP_INCLUDE_DIRS ${ARTKP_INCLUDE_DIRS})
SET(ARTKP_LIBRARIES ${ARTKP_LIBRARY})

MARK_AS_ADVANCED(ARTKP_LIBRARY ARTKP_INCLUDE_DIR)
