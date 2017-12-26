#!/bin/bash
#SBATCH -o /home/hpc/t1221/di57yec/SWE/build/hpc/logs/job.%j.%N.log 
#SBATCH -D /home/hpc/t1221/di57yec/SWE/build/hpc 
#SBATCH -J swe_prod
#SBATCH --get-user-env 
#SBATCH --clusters=mpp2
#SBATCH --nodes=1-1 
#SBATCH --cpus-per-task=28 
#SBATCH --mail-type=ALL 
#SBATCH --mail-user=christoph.honal@tum.de 
#SBATCH --export=NONE 
#SBATCH --time=06:00:00 
source /etc/profile.d/modules.sh
./launch_prod.sh
