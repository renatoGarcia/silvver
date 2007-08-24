CXX = g++
LIB_FLAG= -lsilver_cliente

all: cliente
	$(CXX) cliente.o $(LIB_FLAG) -o cliente

cliente: cliente.cpp
	$(CXX) $(LIBCLIENTE_INCLUDE) -c cliente.cpp

clean:
	rm -f *.o cliente
