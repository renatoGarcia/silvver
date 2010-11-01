FIND_PACKAGE(Boost REQUIRED serialization system thread)
INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIRS})

ADD_LIBRARY(silvver_client SHARED abstractCamera.cpp
                                  exceptions.cpp
                                  target.cpp
                                  targetSet.cpp
                                  ${CMAKE_SOURCE_DIR}/../common/connection/exceptions.cpp
                                  ${CMAKE_SOURCE_DIR}/../common/connection/tcpIp.cpp
                                  ${CMAKE_SOURCE_DIR}/../common/connection/unixSocket.cpp
)
TARGET_LINK_LIBRARIES(silvver_client ${Boost_LIBRARIES})

INSTALL(TARGETS silvver_client
        LIBRARY DESTINATION lib)
INSTALL(FILES abstractCamera.hpp
              callbackWrapper.hpp
              exceptions.hpp
              target.hpp
              targetSet.hpp
              ${CMAKE_SOURCE_DIR}/../common/version.hpp
              ${CMAKE_SOURCE_DIR}/../common/silvverTypes.hpp
              ${CMAKE_SOURCE_DIR}/../common/serializations.hpp
        DESTINATION include/silvver)

SET_TARGET_PROPERTIES(silvver_client PROPERTIES VERSION 0.7.0 SOVERSION 0)
