all : neural

CFLAGS:=-O4

neural : process_neural.o node.o
	g++ -o $@ $^ -O4 -lm

clean :
	rm -rf *.o *~ neural

