SET(silvver_server_SRCS clientMultiMap.cpp
                        exceptions.cpp
                        inputFactory.cpp
                        log.cpp
                        main.cpp
                        markerProcessor.cpp
                        processorFactory.cpp
                        receptionist.cpp
                        ${CMAKE_SOURCE_DIR}/../common/connection/exceptions.cpp
                        ${CMAKE_SOURCE_DIR}/../common/connection/tcpIp.cpp
                        ${CMAKE_SOURCE_DIR}/../common/connection/unixSocket.cpp)

FIND_PACKAGE(Boost REQUIRED program_options serialization system thread)
INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIR})

ADD_EXECUTABLE(silvver_server ${silvver_server_SRCS})
TARGET_LINK_LIBRARIES(silvver_server ${Boost_LIBRARIES})

INSTALL(TARGETS silvver_server
        RUNTIME DESTINATION bin)