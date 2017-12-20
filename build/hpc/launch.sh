#!/bin/bash

echo You are on $(hostname)
echo Running with srun on $(srun hostname)

export TSNOW=$(date +%s)
echo Current timestamp: $TSNOW

export NTHREADS=28

source launch_core.sh

run_suite() {
    echo Running optimized gcc release build
    run_swe gnu
    echo Running not optimized gcc release build
    run_swe gnu_noopt
    echo Running optimized intel release build
    run_swe intel_O2
    echo Running not optimized intel release build
    run_swe intel_O2_noopt
    echo Profiling optimized intel debug build
    run_swe intel amp
    echo Profiling not optimized intel debug build
    run_swe intel_noopt amp
}

echo Running light profile
source profile_light.sh
run_suite

echo Running heavy profile
source profile_heavy.sh
run_suite

echo Done.
