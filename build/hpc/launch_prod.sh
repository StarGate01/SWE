#!/bin/bash

echo You are on $(hostname)
echo Running with srun on $(srun hostname)

export TSNOW=$(date +%s)
echo Current timestamp: $TSNOW

export NTHREADS=28

source launch_core.sh

run_suite() {
    echo Running O2 intel release build
    run_swe intel_O2
}

echo Running tohoku profile
source profile_prod_tohoku.sh
run_suite

echo Running chile profile
source profile_prod_chile.sh
run_suite

echo Done.
