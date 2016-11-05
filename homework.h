#ifndef _HOMEWORK_H_
#define _HOMEWORK_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <math.h>
#include <sys/types.h>
#include <sys/times.h>
#include <sys/time.h>
#include <time.h>
#include <sys/stat.h>
#include <mpi.h>
#include "homework.h"

/* Program Parameters */
char RANK_FILENAME[100]; // File to output ranks of processes
char TIME_FILENAME[100]; // File to output the resulting times
int MB_PER_RANK; // Nomber of MB per rank
int rank, size; // To be used to store process informations

/* output the resulting time to TIME_FILE and to the output stream */
void print_time();
/* Set the program parameters from the command-line arguments */
void parameters(int argc, char **argv);
void first_part();
void second_part();
static void handle_error(int errcode, char *str);

#define MPI_CHECK(fn) { int errcode; errcode = (fn); \
    if (errcode != MPI_SUCCESS) handle_error(errcode, #fn ); }
#endif
