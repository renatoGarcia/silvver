# - Find the OpenCV library
#
# This module will look by OpenCV libraries listed in components.
#
#   FIND_PACKAGE(OPENCV [REQUIRED|COMPONENTS] components...)
#
# It will work with both OpenCV 2.2.0 new libraries names and the previous
# libraries names.
#
# If OPENCV_ROOT_DIR is set, the search under it will have precedence above
# the system path.
#
# This module defines:
#  OPENCV_INCLUDE_DIRS          Where to find the library headers.
#  OPENCV_LIBRARIES             All found libraries path.
#  OPENCV_FOUND                 True if the library was found.
#
#  OPENCV_VERSION_STRING        The found OpenCV version.
#  OPENCV_VERSION_MAJOR         The major version of the OpenCV found.
#  OPENCV_VERSION_MINOR         The minor version of the OpenCV found.
#  OPENCV_VERSION_PATCH         The patch version of the OpenCV found.
#
# Cached Variables:
#  OPENCV_INCLUDE_DIR           Path to library headers.
#  OPENCV_${COMPONENT}_LIBRARY  Path to the found library.
#
#=============================================================================
# Copyright 2011 Renato Florentino Garcia
#
# This program is free software: you can redistribute it and/or modify it
# under the terms of the GNU Lesser General Public License as published by the
# Free Software Foundation, on version 3 of the License.
#
# This program is distributed in the hope that it will be useful, but WITHOUT
# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
# FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License
# for more details.
#
# You should have received a copy of the GNU Lesser General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
# =========================================================================


FOREACH(_opencv_component ${OPENCV_FIND_COMPONENTS})
  STRING(TOUPPER ${_opencv_component} _opencv_upper_component)
  FIND_LIBRARY(OPENCV_${_opencv_upper_component}_LIBRARY ${_opencv_component}
    HINTS ${OPENCV_ROOT_DIR}/lib)

  LIST(APPEND _opencv_components_list OPENCV_${_opencv_upper_component}_LIBRARY)
  LIST(APPEND OPENCV_LIBRARIES ${OPENCV_${_opencv_upper_component}_LIBRARY})
ENDFOREACH()

FIND_PATH(OPENCV_INCLUDE_DIR opencv/cv.h
  HINTS ${OPENCV_ROOT_DIR}/include)

SET(OPENCV_INCLUDE_DIRS ${OPENCV_INCLUDE_DIR})

IF(EXISTS ${OPENCV_INCLUDE_DIR}/opencv/cvver.h)
  SET(_opencv_version_file ${OPENCV_INCLUDE_DIR}/opencv/cvver.h)
ELSEIF(EXISTS ${OPENCV_INCLUDE_DIR}/opencv2/core/version.hpp)
  SET(_opencv_version_file ${OPENCV_INCLUDE_DIR}/opencv2/core/version.hpp)
ENDIF()

# Version find based in another FindOpenCV module by Benoit Rat
FILE(STRINGS ${_opencv_version_file} _opencv_versions
  REGEX "^#define CV_[A-Z]+_VERSION[ \t]+[0-9]+$")
STRING(REGEX REPLACE ".*#define CV_MAJOR_VERSION[ \t]+([0-9]+).*" "\\1"
  OPENCV_VERSION_MAJOR ${_opencv_versions})
STRING(REGEX REPLACE ".*#define CV_MINOR_VERSION[ \t]+([0-9]+).*" "\\1"
  OPENCV_VERSION_MINOR ${_opencv_versions})
STRING(REGEX REPLACE ".*#define CV_SUBMINOR_VERSION[ \t]+([0-9]+).*" "\\1"
  OPENCV_VERSION_PATCH ${_opencv_versions})
SET(OPENCV_VERSION_STRING
  ${OPENCV_VERSION_MAJOR}.${OPENCV_VERSION_MINOR}.${OPENCV_VERSION_PATCH})

INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(OPENCV DEFAULT_MSG
  ${_opencv_components_list}
  OPENCV_INCLUDE_DIR)

MARK_AS_ADVANCED(OPENCV_INCLUDE_DIR ${_opencv_components_list})
