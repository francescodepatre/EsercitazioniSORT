OBJECTS = main.o fast_trigo.o 
CC = gcc -Wall 
project1: $(OBJECTS)
	$(CC) -o project1 $(OBJECTS)

main.o: main.c
	$(CC) -o main main.c

fast_trigo.o: fast_trigo.c fast_trigo.h
	$(CC) -o fast_trigo fast_trigo.c