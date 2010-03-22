CMAKE_MINIMUM_REQUIRED(VERSION 2.8)
SET(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/cmake)

PROJECT(silvver_cameras CXX)

INCLUDE_DIRECTORIES(../common)

ADD_SUBDIRECTORY(src)

INSTALL(DIRECTORY lua/
        DESTINATION share/lua/5.1
        PATTERN "*~" EXCLUDE
        PATTERN ".*" EXCLUDE)

FIND_PACKAGE(PyOpenCV)

IF(OpenCV_PYTHON_VERSION EQUAL 1)
  INSTALL(PROGRAMS calibration/silvverIntCalib_v1
          DESTINATION bin
          RENAME silvverIntCalib)
  INSTALL(PROGRAMS calibration/silvverExtCalib
          DESTINATION bin)
ELSEIF(OpenCV_PYTHON_VERSION EQUAL 2)
  INSTALL(PROGRAMS calibration/silvverIntCalib_v2
          DESTINATION bin
          RENAME silvverIntCalib)
  INSTALL(PROGRAMS calibration/silvverExtCalib
          DESTINATION bin)
ENDIF(OpenCV_PYTHON_VERSION EQUAL 1)
