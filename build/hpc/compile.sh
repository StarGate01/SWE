#!/bin/sh

cd ../..
scons buildVariablesFile=build/options/SWE_hpc.py compileMode=debug netCDFDir=/lrz/sys/libraries/netcdf/4.3.3/intel/serial_160
