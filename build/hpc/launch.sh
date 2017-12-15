#!/bin/bash

echo You are on $(hostname)
echo Running with srun on $(srun hostname)

export NTHREADS=28

source profile_light.sh
srun ./launch_core.sh
source profile_heavy.sh
srun ./launch_core.sh
