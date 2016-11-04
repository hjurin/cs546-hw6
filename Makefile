CC=gcc
MPICC=mpicc
CFLAGS=-Wall
MPICCFLAGS=$(CFLAGS)

.PHONY: default all clean mrproper strats

default:
	$(MPICC) $(MPICCFLAGS) first.c

mrproper:
	rm -rf a.out
	rm -rf *~
	rm -rf \#*
