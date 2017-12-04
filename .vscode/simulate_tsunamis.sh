#!/bin/bash

XSIZE=1000
YSIZE=1000
TIME=30000
STEPS=1000

echo "Now simulating tsunamis, woosh woosh schwipp schwapp"
cd ../build/data

echo "Tohoku"
rm swe_tohoku_00.nc
../SWE_gnu_debug_none_fwave --grid-size-x=$XSIZE --grid-size-y=$YSIZE --input-bathymetry=../input/tohoku_2011/tohoku_gebco_ucsb3_2000m_hawaii_bath.nc --input-displacement=../input/tohoku_2011/tohoku_gebco_ucsb3_2000m_hawaii_displ.nc --time-duration=$TIME --checkpoint-amount=$STEPS --boundary-condition-left=0 --boundary-condition-right=0 --boundary-condition-top=0 --boundary-condition-bottom=0 --output-basepath=swe_tohoku

echo "Chile"
rm swe_chile_00.nc
../SWE_gnu_debug_none_fwave --grid-size-x=$XSIZE --grid-size-y=$YSIZE --input-bathymetry=../input/chile_2010/chile_gebco_usgs_2000m_bath.nc --input-displacement=../input/chile_2010/chile_gebco_usgs_2000m_displ.nc --time-duration=$TIME --checkpoint-amount=$STEPS --boundary-condition-left=0 --boundary-condition-right=0 --boundary-condition-top=0 --boundary-condition-bottom=0 --output-basepath=swe_chile