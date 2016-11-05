CC=gcc
MPICC=mpicc
CFLAGS=-Wall
MPICCFLAGS=$(CFLAGS)

SRC=homework.c
EXEC=exec

.PHONY: default build

default: build

build:
	$(MPICC) $(MPICCFLAGS) $(SRC) -o $(EXEC)

test: build
	mpiexec -np 10 ./$(EXEC) rank_file.dat 1 time_file.dat
	od -td -v rank_file.dat

mrproper:
	rm -rf $(EXEC)*
	rm -rf *.txt
	rm -rf a.out
	rm -rf *~
	rm -rf \#*
