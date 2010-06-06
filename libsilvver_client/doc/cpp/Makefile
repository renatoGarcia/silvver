LIB_FLAG = -lsilvver_client
CPP_FLAG = -DHAS_OPENCV

OBJECTS = abstractCamera targetPose saveImage
all: $(OBJECTS)

clean:
	rm -f $(OBJECTS)

%: %.cpp
	$(CXX) $(LIB_FLAG) $(CPP_FLAG) $^ -o $@
