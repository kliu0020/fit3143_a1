#!/bin/bash
#SBATCH --job-name=serial_job     ### name your job 
#SBATCH --time=00:10:00         ### hh:mm:ss or dd-hh:mm:ss
#SBATCH --mem=16G                 ### memory setting is max @ 2 GB per core
#SBATCH --ntasks=1                 ### launch one process
#SBATCH --cpus-per-task=1         ### multi-threaded processes
#SBATCH --output=serial.%j.out
#SBATCH --partition=defq

 g++ bloomfilters.cpp -o serial -lm;

./serial

exit 0