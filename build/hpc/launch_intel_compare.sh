#!/bin/bash

echo You are on $(hostname)
echo Running with srun on $(srun hostname)

export TSNOW=$(date +%s)
echo Current timestamp: $TSNOW

export NTHREADS=28

source launch_core.sh

run_suite() {
    echo Running O0 intel release build
    run_swe intel_O0
    echo Running O1 intel release build
    run_swe intel_O1
    echo Running O0 intel release build
    run_swe intel_O2
    echo Running O0 intel release build
    run_swe intel_O3
    echo Running Fast intel release build
    run_swe intel_fast
}

echo Running heavy profile
source profile_heavy.sh
run_suite

echo Done.
