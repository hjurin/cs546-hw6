#!/bin/bash

for mb_per_rank in `seq 1 1 16`;
do
    for nb_processes in `seq 1 1 8`
    do
        echo -e "Computing for $nb_processes processes and $mb_per_rank MB per process"
        mpiexec -n $nb_processes ./benchmark rank_file.dat $mb_per_rank time_file.dat > /dev/null
    done
done
