LIB_FLAG = -lsilver_client
CPP_FLAGS = -I../src -I../../common -L../build

OBJECTS = cliente
all: $(OBJECTS)

clean:
	rm -f $(OBJECTS)

%: %.cpp
	$(CXX) $(LIB_FLAG) $(CPP_FLAGS) $^ -o $@

