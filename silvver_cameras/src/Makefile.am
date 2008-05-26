bin_PROGRAMS = silvver_cameras

silvver_cameras_SOURCES = blobCamera.cpp \
                          blobCamera.hpp \
                          blobExtractor.cpp \
                          blobExtractor.hpp \
                          cameraConfig.cpp \
                          cameraConfig.hpp \
                          camera.cpp \
                          camera.hpp \
                          conexao.cpp \
                          conexao.hpp \
                          controlador.cpp \
                          controlador.hpp \
                          extratorMarca.cpp \
                          extratorMarca.hpp \
                          filtro.cpp \
                          filtro.hpp \
                          gerenciador.cpp \
                          gerenciador.hpp \
                          hardCamera.cpp \
                          hardCamera.hpp \
                          main.cpp \
                          marcoCamera.cpp \
                          marcoCamera.hpp \
                          silvver_tipos.hpp \
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
silvver_cameras_SOURCES += hardCameras/pgr.cpp hardCameras/pgr.hpp
endif

if COMPILE_DC1394
silvver_cameras_SOURCES += hardCameras/dc1394.cpp \
                           hardCameras/dc1394.hpp \
                           hardCameras/conversions.cpp \
                           hardCameras/conversions.hpp
endif
