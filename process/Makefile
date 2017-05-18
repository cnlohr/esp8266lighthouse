all : process lighthouse_clocking

process : process.c
	gcc -o $@ $^

lighthouse_clocking : lighthouse_clocking.c
	gcc -o $@ $^

clean :
	rm -rf *.o *~ lighthouse_clocking process

