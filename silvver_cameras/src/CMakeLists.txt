SET(WITH_DC1394V2 ON
    CACHE BOOL "Enable or disable the seek for dc1394v2 library")
SET(WITH_V4L2 ON
    CACHE BOOL "Enable or disable the seek for Video for Linux 2 library")

SET(SUPPORTED_CAMERAS "Pseudocameras")

SET(silvver_cameras_SRCS cfParser.cpp
                         abstractCameras/abstractCamera.cpp
                         connection.cpp
                         sceneMounter.cpp
                         hardCameraFactory.cpp
                         main.cpp
                         abstractCameras/artkpCamera.cpp
                         abstractCameraFactory.cpp
                         hardCameras/hardCamera.cpp
                         hardCameras/pseudoCamera.cpp
                         observer.cpp)

IF(WITH_DC1394V2)
  FIND_PACKAGE(DC1394V2)
  IF(DC1394V2_FOUND)
    SET(SUPPORTED_CAMERAS ${SUPPORTED_CAMERAS}
        "Cameras conform to the IEEE 1394-based Digital Camera Specification")
    ADD_DEFINITIONS(-DHAS_DC1394V2)
    SET(silvver_cameras_SRCS ${silvver_cameras_SRCS} hardCameras/dc1394_2x.cpp)
  ENDIF(DC1394V2_FOUND)
ENDIF(WITH_DC1394V2)

IF(WITH_V4L2)
  FIND_PACKAGE(V4L2)
  IF(V4L2_FOUND)
    SET(SUPPORTED_CAMERAS ${SUPPORTED_CAMERAS}
        "WebCams supported by Video for Linux protocol")
    ADD_DEFINITIONS(-DHAS_V4L2)
    SET(silvver_cameras_SRCS ${silvver_cameras_SRCS} hardCameras/v4l2.cpp)
  ENDIF(V4L2_FOUND)
ENDIF(WITH_V4L2)

FIND_PACKAGE(Boost REQUIRED filesystem program_options thread serialization)
FIND_PACKAGE(Lua51 REQUIRED lua m)
FIND_PACKAGE(OpenCV REQUIRED)
FIND_PACKAGE(ARTKP REQUIRED)

INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIR}
                    ${LUA_INCLUDE_DIR}
                    ${OpenCV_INCLUDE_DIR}
                    ${ARTKP_INCLUDE_DIRS}
                    ${DC1394V2_INCLUDE_DIRS}
                    ${V4L2_INCLUDE_DIRS})

ADD_EXECUTABLE(silvver_cameras ${silvver_cameras_SRCS})

TARGET_LINK_LIBRARIES(silvver_cameras ${Boost_LIBRARIES}
                                      ${LUA_LIBRARIES}
                                      ${OpenCV_LIBRARIES}
                                      ${ARTKP_LIBRARIES}
                                      ${DC1394V2_LIBRARIES}
                                      ${V4L2_LIBRARIES})

INSTALL(TARGETS silvver_cameras
        RUNTIME DESTINATION bin)

MESSAGE("")
MESSAGE("Supported cameras:")
FOREACH(CAM ${SUPPORTED_CAMERAS})
  MESSAGE("  * " ${CAM})
ENDFOREACH(CAM)
MESSAGE("")
