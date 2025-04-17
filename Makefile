

all: main

clean: main monitor.o print.o index.o util.o
	rm main monitor.o print.o index.o util.o

main: main.o monitor.o print.o index.o util.o
	gcc -g -o main main.o monitor.o print.o index.o util.o

main.o: main.c
	gcc -g -c -o main.o main.c

monitor.o: monitor.c monitor.h
	gcc -g -c -o monitor.o monitor.c

print.o: print.c print.h
	gcc -g -c -o print.o print.c

index.o: index.c index.h
	gcc -g -c -o index.o index.c

util.o: util.c util.h
	gcc -g -c -o util.o util.c


