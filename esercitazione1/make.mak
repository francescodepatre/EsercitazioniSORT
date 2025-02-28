OBJECTS = fast_trigo.o main.o 
CFLAGS = -Wall -Wextra -std=c11
LDFLAGS = -lm
TARGET = project1
CC = gcc 
$(TARGET): $(OBJECTS)
	$(CC) -o $(TARGET) $(OBJECTS) $(LDFLAGS)

fast_trigo.o: fast_trigo.c fast_trigo.h 
	$(CC) $(CFLAGS) -c fast_trigo.c 

main.o: main.c 
	$(CC) $(CFLAGS) -c main.c 

clean: 
	rm *.o $(TARGET)