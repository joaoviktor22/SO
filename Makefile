.PHONY: all

all: Processo0.out b.out

b.out: teste.c
	gcc teste.c -o b.out

Processo0.out: trabalho.c
	gcc trabalho.c -o Processo0.out