.PHONY: all

all: a.out b.out

b.out: teste.c
	gcc teste.c -o b.out

a.out: trabalho.c
	gcc trabalho.c