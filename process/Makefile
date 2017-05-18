all : process clocking

process : process.c
	gcc -o $@ $^

clocking : clocking.c
	gcc -o $@ $^

clean :
	rm -rf *.o *~ clocking process

