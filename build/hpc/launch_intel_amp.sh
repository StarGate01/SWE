#!/bin/bash

echo You are on $(hostname)
echo Running with srun on $(srun hostname)

export TSNOW=$(date +%s)
echo Current timestamp: $TSNOW

export NTHREADS=28

source launch_core.sh

run_suite() {
    echo Profiling optimized intel debug build
    run_swe intel_O2 amp
    echo Profiling not optimized intel debug build
    run_swe intel_O2_noopt amp
}

echo Running heavy profile
source profile_heavy.sh
run_suite

echo Done.
