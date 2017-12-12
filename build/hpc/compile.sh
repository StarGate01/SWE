#!/bin/sh

MODE=release
NCDFDIR=/lrz/sys/libraries/netcdf/4.2.1.1

cd ../..
scons buildVariablesFile=build/options/SWE_gnu.py compileMode=$MODE netCDFDir=$NCDFDIR
scons buildVariablesFile=build/options/SWE_intel.py compileMode=$MODE netCDFDir=$NCDFDIR

