LIB_FLAG = -lsilvver_client
OBJECTS = abstractCameraCallback simpleAbstractCamera simpleTarget simpleTargetSet targetTimedWait

all: $(OBJECTS)

clean:
	rm -f $(OBJECTS) *.o

%: %.cpp
	$(CXX) $(LIB_FLAG) $^ -o $@


