#!/bin/bash

echo You are on $(hostname)
echo Running with srun on $(srun hostname)

export TSNOW=$(date +%s)
echo Current timestamp: $TSNOW

export NTHREADS=28

echo Running light profile
source profile_light.sh
./launch_core.sh

echo Running heavy profile
source profile_heavy.sh
./launch_core.sh
