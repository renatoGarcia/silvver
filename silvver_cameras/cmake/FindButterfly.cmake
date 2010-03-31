# - Locate the Butterfly library
#
# This module defines
#  BUTTERFLY_INCLUDE_DIRS, where to find the library headers
#  BUTTERFLY_LIBRARIES, the libraries names
#  BUTTERFLY_FOUND, If true the library was found

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

FIND_PATH(BUTTERFLY_INCLUDE_DIR butterfly.h)

FIND_LIBRARY(BUTTERFLY_LIBRARY butterfly)

INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(BUTTERFLY DEFAULT_MSG
                                  BUTTERFLY_LIBRARY
                                  BUTTERFLY_INCLUDE_DIR)

SET(BUTTERFLY_INCLUDE_DIRS ${BUTTERFLY_INCLUDE_DIR})
SET(BUTTERFLY_LIBRARIES ${BUTTERFLY_LIBRARY})

MARK_AS_ADVANCED(BUTTERFLY_LIBRARY BUTTERFLY_INCLUDE_DIR)
