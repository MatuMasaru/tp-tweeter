CC = gcc
CFLAGS = -g -std=c99 -Wall -Wtype-limits -pedantic -Wconversion -Wno-sign-conversion -Werror -o
VALGRIND = valgrind --leak-check=full --track-origins=yes --show-reachable=yes
OBJET = procesar_usuarios
ARG = twee.txt
ARCH_C = cola.o abb.o hash.o strutil.o procesar_usuarios.c
RAM :compilar
	./$(OBJET) $(ARG)

all:valgrind

	echo
	notify-send "Compilado!" -t 1500

valgrind:compilar

	echo
	$(VALGRIND) ./$(OBJET) $(ARG)
compilar:

	echo
	$(CC) $(ARCH_C) $(CFLAGS) $(OBJET)

clean:

	rm $(OBJET)
