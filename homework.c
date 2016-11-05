#include "homework.h"

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
        printf("-----------------------------------------------------------\n");
        printf("Program parameters:\n");
        printf("\tRanks file = %s\n", RANK_FILENAME);
        printf("\tMB per rank = %d\n", MB_PER_RANK);
        printf("\tResulting times file = %s\n", TIME_FILENAME);
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

/* Prints timing results into the output time file, the standard output
and into a csv file for testing */
void print_timing_results(double write_time, double read_time) {
    double overall_time;
    float max_bw;
    char stat[50];

    overall_time = read_time + write_time;
    max_bw = MB_PER_RANK * size / overall_time;

    // prints to stdout and time file
    FILE * f = fopen(TIME_FILENAME, "w+");
    sprintf(stat, "write time: %1.5f seconds\n", write_time);
    printf("%s", stat);
    fprintf(f, "%s", stat);
    sprintf(stat, "read time: %1.5f seconds\n", read_time);
    printf("%s", stat);
    fprintf(f, "%s", stat);
    sprintf(stat, "overall time: %1.5f seconds\n", overall_time);
    printf("%s", stat);
    fprintf(f, "%s", stat);
    sprintf(stat, "maximum bandwidth: %3.1f MB/s\n", max_bw);
    printf("%s", stat);
    fprintf(f, "%s", stat);
    fclose(f);

    // prints to the csv file for testing
    char time_file[20] = "elapsed_times.csv";
    f = fopen(time_file, "r");
    if (f == NULL) { // if the file doesn't exist we create it with headers
        f = fopen(time_file, "a");
        fprintf(f, "nb_processes;mb_per_process;read_time;write_time;overall_time;max_bandwidth\n");
    }
    fclose(f);
    f = fopen(time_file, "a");
    fprintf(f, "%d;%d;%1.5f;%1.5f;%1.5f;%3.1f\n", size, MB_PER_RANK, read_time, write_time, overall_time, max_bw);
}


int main(int argc, char **argv) {
    MPI_CHECK(MPI_Init(&argc, &argv));
    MPI_CHECK(MPI_Comm_rank(MPI_COMM_WORLD, &rank));
    MPI_CHECK(MPI_Comm_size(MPI_COMM_WORLD, &size));

    /* Time variables */
    double start, finish, write_time, read_time;

    /* Process program parameters */
    parameters(argc, argv);

    /* Write the ranks of the processes*/
    write_ranks ();

    /* Write the data */
    {
        if (rank == 0) {
            printf("-----------------------------------------------------------\n");
            printf("Starts writing data.\n");
            start = MPI_Wtime();
        }
        MPI_CHECK(MPI_Barrier(MPI_COMM_WORLD));
        write_data();

        MPI_CHECK(MPI_Barrier(MPI_COMM_WORLD));
    }

    /* Read the data */
    {
        if (rank == 0) {
            printf("Done.\nStarts reading data.\n");
            finish = MPI_Wtime();
            write_time = finish - start;
            start = finish;
        }

        MPI_CHECK(MPI_Barrier(MPI_COMM_WORLD));

        read_data();

        MPI_CHECK(MPI_Barrier(MPI_COMM_WORLD));
        if (rank == 0) {
            printf("Done.\n\n");
            finish = MPI_Wtime();
            read_time = finish - start;
            start = finish;
        }
    }

    /* Display timing results */
    {
        if (rank == 0) {
            MPI_CHECK(MPI_File_delete("who_cares", MPI_INFO_NULL));
            print_timing_results(write_time, read_time);
            printf("-----------------------------------------------------------\n");
        }
    }

    MPI_CHECK(MPI_Finalize());
    exit(0);
}

/* Writes the rank of the process into the file RANK_FILENAME */
void write_ranks() {
    MPI_File fh;
    MPI_Offset offset = rank * sizeof(int);

    /* Write into the file */
    MPI_CHECK(MPI_File_open(MPI_COMM_WORLD,
                            RANK_FILENAME,
                            MPI_MODE_CREATE | MPI_MODE_RDWR,
                            MPI_INFO_NULL,
                            &fh));
    MPI_CHECK(MPI_File_set_size(fh, 0)); // to empty the file if it already exists
    MPI_CHECK(MPI_File_write_at(fh, offset, &rank, 1, MPI_INT, MPI_STATUS_IGNORE));

    /* Check correctness of the write */
    int read_rank;
    MPI_CHECK(MPI_File_read_at(fh, offset, &read_rank, 1, MPI_INT, MPI_STATUS_IGNORE));
    if (read_rank != rank) {
        printf("error: process %d didn't write his rank correctly\n\tread %d\n", rank, read_rank);
        exit(0);
    } else {
        if (rank == 0) {
            printf("Ranks have been wrote correctly\n");
        }
    }
    MPI_CHECK(MPI_File_close(&fh));
}

/* Writes random data to a file to stress the FS */
void write_data() {
    MPI_File fh;
    int data_size = 1024 * 1024 * MB_PER_RANK;
    char * data = (char*) malloc(data_size);
    MPI_Offset offset = data_size * rank;

    /* Write into the file and then delete it */
    MPI_CHECK(MPI_File_open(MPI_COMM_WORLD,
                            "who_cares",
                            MPI_MODE_CREATE | MPI_MODE_WRONLY,
                            MPI_INFO_NULL,
                            &fh));
    MPI_CHECK(MPI_File_write_at(fh, offset, data, data_size, MPI_CHAR, MPI_STATUS_IGNORE));
    free(data);
}

/* Read file's data and deletes it */
void read_data() {
    MPI_File fh;
    int data_size = 1024 * 1024 * MB_PER_RANK;
    char * data = (char*) malloc(data_size);
    MPI_Offset offset = data_size * rank;

    /* Write into the file and then delete it */
    MPI_CHECK(MPI_File_open(MPI_COMM_WORLD,
                            "who_cares",
                            MPI_MODE_RDONLY,
                            MPI_INFO_NULL,
                            &fh));
    MPI_CHECK(MPI_File_read_at(fh, offset, data, data_size, MPI_CHAR, MPI_STATUS_IGNORE));
    free(data);
}
