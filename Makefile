CC=gcc
MPICC=mpicc
CFLAGS=-Wall
MPICCFLAGS=$(CFLAGS)

SRC=homework.c

.PHONY: default all clean mrproper strats

default:
	$(MPICC) $(MPICCFLAGS) $(SRC)

mrproper:
	rm -rf a.out
	rm -rf *~
	rm -rf \#*
