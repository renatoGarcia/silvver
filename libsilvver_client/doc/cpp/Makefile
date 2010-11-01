LIB_FLAG = -lsilvver_client -ggdb

OBJECTS = targetSetSimple targetPose targetTimedWait abstractCamera
all: $(OBJECTS)

clean:
	rm -f $(OBJECTS)

%: %.cpp
	$(CXX) $(LIB_FLAG) $^ -o $@
