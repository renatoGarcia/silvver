LIB_FLAG = -lsilvver_client

OBJECTS = targetPose targetTimedWait abstractCamera saveImage
all: $(OBJECTS)

clean:
	rm -f $(OBJECTS)

%: %.cpp
	$(CXX) $(LIB_FLAG) $^ -o $@
