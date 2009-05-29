LIB_FLAG = -lsilver_client -lboost_thread-mt
CPP_FLAGS = -I../src -I../../common -L../build

OBJECTS = silver_client
all: $(OBJECTS)

clean:
	rm -f $(OBJECTS)

%: %.cpp
	$(CXX) $(LIB_FLAG) $(CPP_FLAGS) $^ -o $@

