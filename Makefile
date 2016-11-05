CC=gcc
MPICC=mpicc
CFLAGS=-Wall
MPICCFLAGS=$(CFLAGS)

SRC=homework.c
EXEC=benchmark

.PHONY: default print

default:
	$(MPICC) $(MPICCFLAGS) $(SRC) -o $(EXEC)

print:
	od -td -v rank_file.dat

mrproper:
	rm -rf $(EXEC)*
	rm -rf elapsed_times.csv
	rm -rf *.dat
	rm -rf a.out
	rm -rf *~
	rm -rf \#*
