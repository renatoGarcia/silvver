LIB_FLAG = -lsilvver_client -lboost_thread-mt
CPP_FLAGS = -I../src -I../../common -L../build

OBJECTS = silvver_client 2dPlane
all: $(OBJECTS)

clean:
	rm -f $(OBJECTS)

%: %.cpp
	$(CXX) $(LIB_FLAG) $(CPP_FLAGS) $^ -o $@

