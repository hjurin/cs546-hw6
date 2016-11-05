CC=gcc
MPICC=mpicc
CFLAGS=-Wall
MPICCFLAGS=$(CFLAGS)

SRC=homework.c
EXEC=benchmark

.PHONY: default

default:
	$(MPICC) $(MPICCFLAGS) $(SRC) -o $(EXEC)

print:
	od -td -v rank_file.dat

mrproper:
	rm -rf $(EXEC)*
	rm -rf *.dat
	rm -rf a.out
	rm -rf *~
	rm -rf \#*
