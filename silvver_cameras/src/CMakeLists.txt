SET(WITH_DC1394V1 ON
    CACHE BOOL "Enable or disable the seek for dc1394v1 library")
SET(WITH_DC1394V2 ON
    CACHE BOOL "Enable or disable the seek for dc1394v2 library")
SET(WITH_V4L2 ON
    CACHE BOOL "Enable or disable the seek for Video for Linux 2 library")

SET(WITH_ARTKP ON
    CACHE BOOL "Enable or disable the seek for ARToolKitPlus library")
SET(WITH_BORBOLETA ON
    CACHE BOOL "Enable or disable the seek for Borboleta library")

SET(SUPPORTED_CAMERAS "Pseudocameras")
SET(SUPPORTED_TARGETS "")

SET(silvver_cameras_SRCS cfParser.cpp
                         abstractCameras/abstractCamera.cpp
                         connection.cpp
                         sceneMounter.cpp
                         hardCameraFactory.cpp
                         main.cpp
                         abstractCameraFactory.cpp
                         hardCameras/hardCamera.cpp
                         hardCameras/pseudoCamera.cpp
                         hardCameras/colorConverter.cpp
                         observer.cpp
                         log.cpp
                         exceptions.cpp)

IF(WITH_DC1394V2)
  FIND_PACKAGE(DC1394V2)
  IF(DC1394V2_FOUND)
    SET(SUPPORTED_CAMERAS ${SUPPORTED_CAMERAS}
        "Cameras conform to the IEEE 1394-based Digital Camera Specification (using libdc1394v2).")
    ADD_DEFINITIONS(-DHAS_DC1394V2)
    SET(silvver_cameras_SRCS ${silvver_cameras_SRCS} hardCameras/dc1394_2x.cpp)
  ENDIF(DC1394V2_FOUND)
ENDIF(WITH_DC1394V2)

IF(WITH_DC1394V1 AND NOT DC1394V2_FOUND)
  FIND_PACKAGE(DC1394V1)
  IF(DC1394V1_FOUND)
    SET(SUPPORTED_CAMERAS ${SUPPORTED_CAMERAS}
        "Cameras conform to the IEEE 1394-based Digital Camera Specification (using libdc1394v1).")
    ADD_DEFINITIONS(-DHAS_DC1394V1)
    SET(silvver_cameras_SRCS ${silvver_cameras_SRCS} hardCameras/dc1394_1x.cpp)
  ENDIF(DC1394V1_FOUND)
ENDIF(WITH_DC1394V1 AND NOT DC1394V2_FOUND)

IF(WITH_V4L2)
  FIND_PACKAGE(V4L2)
  IF(V4L2_FOUND)
    SET(SUPPORTED_CAMERAS ${SUPPORTED_CAMERAS}
        "WebCams supported by Video for Linux protocol")
    ADD_DEFINITIONS(-DHAS_V4L2)
    SET(silvver_cameras_SRCS ${silvver_cameras_SRCS} hardCameras/v4l2.cpp)
  ENDIF(V4L2_FOUND)
ENDIF(WITH_V4L2)

IF(WITH_ARTKP)
  FIND_PACKAGE(ARTKP)
  IF(ARTKP_FOUND)
    SET(SUPPORTED_TARGETS ${SUPPORTED_TARGETS}
        "ARToolKitPlus")
    ADD_DEFINITIONS(-DHAS_ARTKP)
    SET(silvver_cameras_SRCS ${silvver_cameras_SRCS}
                             abstractCameras/artkpCamera.cpp)
  ENDIF(ARTKP_FOUND)
ENDIF(WITH_ARTKP)

IF(WITH_BORBOLETA)
  FIND_PACKAGE(Borboleta)
  IF(BORBOLETA_FOUND)
    SET(SUPPORTED_TARGETS ${SUPPORTED_TARGETS}
        "Borboleta")
    ADD_DEFINITIONS(-DHAS_BORBOLETA)
    SET(silvver_cameras_SRCS ${silvver_cameras_SRCS}
                             abstractCameras/borboletaCamera.cpp)
  ENDIF(BORBOLETA_FOUND)
ENDIF(WITH_BORBOLETA)

FIND_PACKAGE(Boost REQUIRED filesystem program_options thread serialization)
FIND_PACKAGE(Lua51 REQUIRED lua m)
FIND_PACKAGE(OpenCV REQUIRED)

INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIR}
                    ${LUA_INCLUDE_DIR}
                    ${OpenCV_INCLUDE_DIR})

ADD_EXECUTABLE(silvver_cameras ${silvver_cameras_SRCS})

TARGET_LINK_LIBRARIES(silvver_cameras ${Boost_LIBRARIES}
                                      ${LUA_LIBRARIES}
                                      ${OpenCV_LIBRARIES})

IF(DC1394V2_FOUND)
  INCLUDE_DIRECTORIES(${DC1394V2_INCLUDE_DIRS})
  TARGET_LINK_LIBRARIES(silvver_cameras ${DC1394V2_LIBRARIES})
ENDIF(DC1394V2_FOUND)

IF(DC1394V1_FOUND)
  INCLUDE_DIRECTORIES(${DC1394V1_INCLUDE_DIRS})
  TARGET_LINK_LIBRARIES(silvver_cameras ${DC1394V1_LIBRARIES})
ENDIF(DC1394V1_FOUND)

IF(V4L2_FOUND)
  INCLUDE_DIRECTORIES(${V4L2_INCLUDE_DIRS})
  TARGET_LINK_LIBRARIES(silvver_cameras ${V4L2_LIBRARIES})
ENDIF(V4L2_FOUND)

IF(ARTKP_FOUND)
  INCLUDE_DIRECTORIES(${ARTKP_INCLUDE_DIRS})
  TARGET_LINK_LIBRARIES(silvver_cameras ${ARTKP_LIBRARIES})
ENDIF(ARTKP_FOUND)

IF(BORBOLETA_FOUND)
  INCLUDE_DIRECTORIES(${BORBOLETA_INCLUDE_DIRS})
  TARGET_LINK_LIBRARIES(silvver_cameras ${BORBOLETA_LIBRARIES})
ENDIF(BORBOLETA_FOUND)

INSTALL(TARGETS silvver_cameras
        RUNTIME DESTINATION bin)

MESSAGE("")
MESSAGE("Supported cameras:")
FOREACH(CAM ${SUPPORTED_CAMERAS})
  MESSAGE("  * " ${CAM})
ENDFOREACH(CAM)
MESSAGE("")

MESSAGE("")
MESSAGE("Supported targets:")
FOREACH(TARGET ${SUPPORTED_TARGETS})
  MESSAGE("  * " ${TARGET})
ENDFOREACH(TARGET)
MESSAGE("")
