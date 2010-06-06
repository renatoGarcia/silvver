# - Locate the OpenCv Python bindings
#
# This module defines
#  OpenCV_PYTHON_VERSION, the version of library

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

EXECUTE_PROCESS(COMMAND ${CMAKE_SOURCE_DIR}/cmake/pyOpenCv.py
                RESULT_VARIABLE result)

IF(result EQUAL 0)
  SET(result "")
ENDIF(result EQUAL 0)

SET(OpenCV_PYTHON_VERSION ${result}
    CACHE STRING "Version of OpenCv Python bindings. 1 is that in OpenCv 1.0 and 2 that in OpenCv 2.0")

MARK_AS_ADVANCED(OpenCV_PYTHON_VERSION)