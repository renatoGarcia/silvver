CXX = g++
LIB_FLAG = -lsilvver_cliente
CPP_FLAGS = -I../src -L../src/.libs

all: cliente
	$(CXX) cliente.o $(LIB_FLAG) $(CPP_FLAGS) -o cliente

cliente: cliente.cpp
	$(CXX) $(CPP_FLAGS) -c cliente.cpp

clean:
	rm -f *.o cliente
