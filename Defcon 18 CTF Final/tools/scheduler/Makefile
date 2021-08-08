.PHONY: all clean

all:sploitd 

sploitd: sploitd.o service.o state.o bin_worker.o sploit_worker.o killer.o
	gcc -pedantic -Wall -lsqlite3 -o $@ $+

clean:
	rm -f a.out *.o
