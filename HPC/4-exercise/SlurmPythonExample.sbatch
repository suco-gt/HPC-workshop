#!/bin/bash
#SBATCH                 # Name this job "PythonExample"
#SBATCH                 # Request 1 node with 4 cores
#SBATCH                 # Request 1 GB of memory per core
#SBATCH                 # Set the job to last at most 15 minutes
#SBATCH  -oReport-%j.out   # Combined output and error messages file (don't change)
#SBATCH                 # Set email to be send only when beginning and ending
#SBATCH                 # Enter your email address

module load anaconda3                           # Load module dependencies
srun python test.py                             # Example Process
