void gauss() {
    int norm, row, col;  /* Normalization row, and zeroing
    * element row and col */
    float multiplier;

    sprintf(print_tmp, "Computing Serially.\n");
    print_all();

    /* Gaussian elimination */
    for (row = 0; row < N ; row++) {
        MPI_Bcast((float *)A[row], N, MPI_FLOAT, 0, MPI_COMM_WORLD);
    }
    MPI_Bcast((float *)B, N, MPI_FLOAT, 0, MPI_COMM_WORLD);

    // loop that iterates on sub-matrices
    for (norm = 0; norm < N - 1; norm++) {
        // loop that iterates on each row of the sub_matrix
        for (row = norm + 1 ; row < N ; row += size) {
            if (row + rank >= N) {
                break;
            }
            multiplier = A[row + rank][norm] / A[norm][norm];
            for (col = norm; col < N; col++) {
                A[row + rank][col] -= A[norm][col] * multiplier;
            }
            B[row + rank] -= B[norm] * multiplier;

        }
        // we gather the new computed rows
        for (row = norm + 1 ; row < N ; row += size) {
            int r;
            for (r = 0; r < size; r++) {
                MPI_Bcast((float *)A[row + r], N, MPI_FLOAT, r, MPI_COMM_WORLD);
                MPI_Bcast((float *)&(B[row + r]), 1, MPI_FLOAT, r, MPI_COMM_WORLD);
            }
        }
        // before getting to the next iteration, we want every process to have the same A and B
        MPI_Barrier(MPI_COMM_WORLD);

    }
}
