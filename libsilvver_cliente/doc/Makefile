CXX = g++
LIB_FLAG = -lsilver_client
CPP_FLAGS = -I../src -I../../common -L../build

all: cliente
	$(CXX) cliente.o $(LIB_FLAG) $(CPP_FLAGS) -o cliente

cliente: cliente.cpp
	$(CXX) $(CPP_FLAGS) -c cliente.cpp

clean:
	rm -f *.o cliente
