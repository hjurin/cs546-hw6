#include "homework.h"

// /* output the resulting time to TIME_FILE and to the output stream */
// void print_time() {
//     if (rank != 0) {
//         return;
//     }
//
//     printf("%s", print_tmp);
//
//     /* builds the repertory for output files */
//     struct stat sb;
//     if (stat("output_files", &sb) != 0 || !S_ISDIR(sb.st_mode)) {
//         mode_t mode = 0774;
//         mkdir("output_files", mode);
//     }
//
//     char filepath[40];
//     sprintf(filepath, "output_files/%s", F);
//     FILE *f = fopen(filepath, "a");
//     fprintf(f, "%s", print_tmp);
//     fclose(f);
// }

/* Set the program parameters from the command-line arguments */
void parameters(int argc, char **argv) {
    /* Read command-line arguments */
    if (argc != 4) {
        printf("Usage: %s <rank_filename> <MB_per_rank> <time_filename>\n", argv[0]);
        exit(0);
    }
    strcpy(RANK_FILENAME, argv[1]);
    MB_PER_RANK = atoi(argv[2]);
    strcpy(TIME_FILENAME, argv[3]);

    /* Print parameters */
    if (rank == 0) {
        printf("\nRanks file = %s\n", RANK_FILENAME);
        printf("\nMB per rank = %d\n", MB_PER_RANK);
        printf("\nResulting times file = %s\n", TIME_FILENAME);
    }
}

/* Error handler */
static void handle_error(int errcode, char *str)
{
    char msg[MPI_MAX_ERROR_STRING];
    int resultlen;
    MPI_Error_string(errcode, msg, &resultlen);
    fprintf(stderr, "%s: %s\n", str, msg);
    /* Aborting on error might be too aggressive.  If
    * you're sure you can
    * continue after an error, comment or remove
    * the following line */
    MPI_Abort(MPI_COMM_WORLD, 1);
}


int main(int argc, char **argv) {
    MPI_CHECK(MPI_Init(&argc, &argv));
    MPI_CHECK(MPI_Comm_rank(MPI_COMM_WORLD, &rank));
    MPI_CHECK(MPI_Comm_size(MPI_COMM_WORLD, &size));

    /* Timing variables */
    struct timeval etstart, etstop;  /* Elapsed times using gettimeofday() */
    struct timezone tzdummy;
    clock_t etstart2, etstop2;  /* Elapsed times using times() */
    unsigned long long usecstart, usecstop;
    struct tms cputstart, cputstop;  /* CPU times for my processes */

    /* Process program parameters */
    parameters(argc, argv);

    /* Start Clock */
    if (rank == 0) {
        printf("\nStarting clock.\n");
        gettimeofday(&etstart, &tzdummy);
        etstart2 = times(&cputstart);
    }

    first_part();

    // second_part();

    /* Stop Clock */
    if (rank == 0) {
        gettimeofday(&etstop, &tzdummy);
        etstop2 = times(&cputstop);
        usecstart = (unsigned long long)etstart.tv_sec * 1000000 + etstart.tv_usec;
        usecstop = (unsigned long long)etstop.tv_sec * 1000000 + etstop.tv_usec;
        printf("Stopped clock.\n");
    }

    /* Display timing results */
    if (rank == 0) {
        printf("\nElapsed time = %g ms.\n",
        (float)(usecstop - usecstart)/(float)1000);
        printf("(CPU times are accurate to the nearest %g ms)\n",
        1.0/(float)CLOCKS_PER_SEC * 1000.0);
        printf("My system CPU time for parent = %g ms.\n",
        (float)(cputstop.tms_stime - cputstart.tms_stime) /
        (float)CLOCKS_PER_SEC * 1000);
        printf("My total CPU time for child processes = %g ms.\n",
        (float)((cputstop.tms_cutime + cputstop.tms_cstime) -
        (cputstart.tms_cutime + cputstart.tms_cstime) ) /
        (float)CLOCKS_PER_SEC * 1000);
        /* Contrary to the man pages, this appears not to include the parent */
        printf("--------------------------------------------\n");
    }

    MPI_CHECK(MPI_Finalize());
    exit(0);
}

void first_part() {
    MPI_File fh;
    MPI_Offset offset = rank * sizeof(int);

    MPI_CHECK(MPI_File_open(MPI_COMM_WORLD,
                            RANK_FILENAME,
                            MPI_MODE_CREATE | MPI_MODE_RDWR,
                            MPI_INFO_NULL,
                            &fh));

    MPI_CHECK(MPI_File_set_size(fh, 0));

    MPI_CHECK(MPI_File_write_at(fh, offset, &rank, sizeof(int), MPI_INT, MPI_STATUS_IGNORE));

    MPI_CHECK(MPI_File_close(&fh));
}

// void first_part() {
//     MPI_File fh;
//     char buf[20];
//     sprintf(buf, "%d", rank);
//     // sprintf(buf, "I am process %d\n", rank);
//     MPI_Offset offset = rank * strlen(buf);
//
//     MPI_CHECK(MPI_File_open(MPI_COMM_WORLD,
//                             RANK_FILENAME,
//                             MPI_MODE_CREATE | MPI_MODE_RDWR,
//                             MPI_INFO_NULL,
//                             &fh));
//
//     MPI_CHECK(MPI_File_set_size(fh, 0));
//
//     MPI_CHECK(MPI_File_write_at(fh, offset, buf, strlen(buf), MPI_CHAR, MPI_STATUS_IGNORE));
//
//     MPI_CHECK(MPI_File_close(&fh));
// }

void second_part() {
    int n = 10;
    char buf[n];
    int count;
    long long nb_bytes;

    MPI_Status status;
    MPI_File fh;
    MPI_Offset offset;
    MPI_CHECK(MPI_File_open(MPI_COMM_WORLD, "test",
                  MPI_MODE_RDONLY, MPI_INFO_NULL, &fh));
    offset = rank;
    MPI_CHECK(MPI_File_get_size(fh, &nb_bytes));
    MPI_CHECK(MPI_File_read_at(fh, offset, buf, 1, MPI_CHAR, &status));
    // MPI_CHECK(MPI_File_read(fh, buf, n, MPI_CHAR, &status));
    MPI_CHECK(MPI_Get_count(&status, MPI_CHAR, &count));
    printf("process %d read %d char : %c\n", rank, count, buf[0]);
    MPI_CHECK(MPI_File_close(&fh));
}
