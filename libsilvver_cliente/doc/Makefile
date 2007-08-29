CXX = g++
LIB_FLAG= -lsilvver_cliente

all: cliente
	$(CXX) cliente.o $(LIB_FLAG) -o cliente

cliente: cliente.cpp
	$(CXX) -c cliente.cpp

clean:
	rm -f *.o cliente
