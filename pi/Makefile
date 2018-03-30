CC = clang
CFLAGS = -Wall -std=c99 -ggdb -I../common
LIBS = -lm

pi_mc: main.o
	$(CC) $(LIBS) main.o ../common/libutils.a -o pi_mc

main.o: main.c 
	$(CC) -c $(CFLAGS) main.c

