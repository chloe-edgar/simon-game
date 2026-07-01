CC = gcc
CFLAGS = -O2 -Wall 
LFLAGS = -lm 


all:	simon

###

simon:	simon.o display.o gpio.o
	$(CC) -o simon simon.o display.o gpio.o $(LFLAGS)

simon.o: simon.c display.h gpio.h
	$(CC) $(CFLAGS) -c simon.c -lm

gpio.o: gpio.c gpio.h
	$(CC) $(CFLAGS) -c gpio.c -lm

display.o: display.c display.h
	$(CC) $(CFLAGS) -c display.c -lm
