#!/bin/sh

cd ../..
netCDFDir=$NETCDF_INC
scons buildVariablesFile=build/options/SWE_gnu.py compileMode=release