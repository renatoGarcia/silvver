CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

PROJECT(silvver_client CXX)

INCLUDE_DIRECTORIES(../common)

ADD_SUBDIRECTORY(src)
ADD_SUBDIRECTORY(bindings)

INSTALL(DIRECTORY doc/
        DESTINATION share/doc/silvver/client_examples/
        PATTERN "*~" EXCLUDE
        PATTERN ".*" EXCLUDE)
