#!/bin/bash
#SBATCH --job-name=omp_job     ### name your job 
#SBATCH --time=00:10:00         ### hh:mm:ss or dd-hh:mm:ss
#SBATCH --mem=16G                 ### memory setting is max @ 2 GB per core
#SBATCH --ntasks=1                 ### launch one process
#SBATCH --cpus-per-task=8         ### multi-threaded processes
#SBATCH --output=omp.%j.out
#SBATCH --partition=defq

if [ ! -f omp ]
then
 g++ bfparallel.cpp -fopenmp -o omp
else
 if [ omp.c -nt omp ]
 then
  g++ bfparallel.cpp -fopenmp -o omp
 fi
fi

export OMP_NUM_THREADS=$SLURM_CPUS_PER_TASK
./omp

exit 0