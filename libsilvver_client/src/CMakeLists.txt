FIND_PACKAGE(Boost REQUIRED serialization system thread)

INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIRS})
ADD_LIBRARY(silvver_client SHARED connection.cpp target.cpp abstractCamera.cpp)
TARGET_LINK_LIBRARIES(silvver_client ${Boost_LIBRARIES})

INSTALL(TARGETS silvver_client
        LIBRARY DESTINATION lib)
INSTALL(FILES target.hpp
              abstractCamera.hpp
              exceptions.hpp
              ../../common/version.hpp
              ../../common/silvverTypes.hpp
              ../../common/serializations.hpp
        DESTINATION include/silvver)

SET_TARGET_PROPERTIES(silvver_client PROPERTIES VERSION 0.6.0 SOVERSION 0)
