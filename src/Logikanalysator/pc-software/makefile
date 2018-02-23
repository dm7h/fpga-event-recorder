CC=gcc
FLAGS=-lncurses -lmenu

all: main

main: handling.o main.c
	$(CC) handling.o main.c -o hsa-logic

handling.o: handling.c
	$(CC) -c handling.c