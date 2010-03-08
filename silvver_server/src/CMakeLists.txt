SET(silvver_server_SRCS clientsMap.cpp
                        log.cpp
                        inputFactory.cpp
                        ioConnection.cpp
                        main.cpp
                        markerProcessor.cpp
                        receptionist.cpp)

FIND_PACKAGE(Boost REQUIRED program_options serialization system thread)

ADD_EXECUTABLE(silvver_server ${silvver_server_SRCS})
TARGET_LINK_LIBRARIES(silvver_server ${Boost_LIBRARIES})

INSTALL(TARGETS silvver_server
        RUNTIME DESTINATION bin)