CC = gcc
CFLAGS = -g -std=c99 -Wall -Wtype-limits -pedantic -Wconversion -Wno-sign-conversion -Werror -o
VALGRIND = valgrind --leak-check=full --track-origins=yes --show-reachable=yes
OBJET = procesar_tweets
ARG =4 2
TEXT =twee.txt
ARCH_C = cola.o hash.o heap.o pila.o strutil.o procesar_tweets.c count_min_sketch.c
RAM :compilar
	./$(OBJET) $(ARG)

all:valgrind

	echo
	notify-send "Compilado!" -t 1500

valgrind:compilar

	echo
	$(VALGRIND) cat twee.txt |./$(OBJET) $(ARG)
compilar:

	echo
	$(CC) $(ARCH_C) $(CFLAGS) $(OBJET)

clean:

	rm $(OBJET)
