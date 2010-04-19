LIB_FLAG = -lsilvver_client -lboost_thread-mt

OBJECTS = simple_client raw_client silvver_client
all: $(OBJECTS)

clean:
	rm -f $(OBJECTS)

%: %.cpp
	$(CXX) $(LIB_FLAG) $^ -o $@
