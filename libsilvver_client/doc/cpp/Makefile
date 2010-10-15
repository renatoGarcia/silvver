LIB_FLAG = -lsilvver_client -ggdb

OBJECTS = targetPose targetTimedWait abstractCamera
all: $(OBJECTS)

clean:
	rm -f $(OBJECTS)

%: %.cpp
	$(CXX) $(LIB_FLAG) $^ -o $@
