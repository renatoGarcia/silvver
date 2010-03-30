# - Locate the Borboleta library
#
# This module defines
#  BORBOLETA_INCLUDE_DIRS, where to find the library headers
#  BORBOLETA_LIBRARIES, the libraries names
#  BORBOLETA_FOUND, If true the library was found

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

FIND_PATH(BORBOLETA_INCLUDE_DIR borboleta.h)

FIND_LIBRARY(BORBOLETA_LIBRARY borboleta)

INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(BORBOLETA DEFAULT_MSG
                                  BORBOLETA_LIBRARY
                                  BORBOLETA_INCLUDE_DIR)

SET(BORBOLETA_INCLUDE_DIRS ${BORBOLETA_INCLUDE_DIR})
SET(BORBOLETA_LIBRARIES ${BORBOLETA_LIBRARY})

MARK_AS_ADVANCED(BORBOLETA_LIBRARY BORBOLETA_INCLUDE_DIR)
