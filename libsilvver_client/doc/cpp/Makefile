LIB_FLAG = -lsilvver_client

OBJECTS = abstractCamera targetPose
all: $(OBJECTS)

clean:
	rm -f $(OBJECTS)

%: %.cpp
	$(CXX) $(LIB_FLAG) $^ -o $@
