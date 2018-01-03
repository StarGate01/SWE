#!/bin/sh

NCDFDIR=/lrz/sys/libraries/netcdf/4.2.1.1

cd ../..
scons buildVariablesFile=build/options/SWE_intel_prod.py compileMode=release netCDFDir=$NCDFDIR
scons buildVariablesFile=build/options/SWE_intel_fast.py compileMode=release netCDFDir=$NCDFDIR
