AM_CPPFLAGS = -Wall
bin_PROGRAMS = silver_cameras

silver_cameras_SOURCES = xmlParser.cpp \
                         xmlParser.hpp \
                         abstractCamera.cpp \
                         abstractCamera.hpp \
                         connection.cpp \
                         connection.hpp \
                         extratorMarca.cpp \
                         extratorMarca.hpp \
                         sceneMounter.cpp \
                         sceneMounter.hpp \
                         hardCamera.cpp \
                         hardCamera.hpp \
                         hardCameraFactory.cpp \
                         hardCameraFactory.hpp \
                         main.cpp \
                         markerCamera.cpp \
                         markerCamera.hpp \
                         blobCamera.hpp \
                         blobCamera.cpp \
                         blobExtractor.hpp \
                         blobExtractor.cpp \
                         silverTypes.hpp \
                         timer.hpp \
                         cmdline.h \
                         cmdline.c \
                         hardCameras/modelos.hpp \
                         hardCameras/pseudoCamera.cpp \
                         hardCameras/pseudoCamera.hpp \
                         tinyXml/tinystr.cpp \
                         tinyXml/tinystr.h \
                         tinyXml/tinyxml.cpp \
                         tinyXml/tinyxmlerror.cpp \
                         tinyXml/tinyxml.h \
                         tinyXml/tinyxmlparser.cpp

if COMPILE_PGR
silver_cameras_SOURCES += hardCameras/pgr.cpp \
                          hardCameras/pgr.hpp
endif

if COMPILE_DC1394
silver_cameras_SOURCES += hardCameras/dc1394.cpp \
                          hardCameras/dc1394.hpp \
                          hardCameras/conversions.cpp \
                          hardCameras/conversions.hpp
endif
