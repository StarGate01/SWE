#!/bin/sh

INPUTDIR=$HOME/SWE/build
DATADIR=$SCRATCH/$USER/swe_data
echo INPUTDIR: $INPUTDIR
echo DATADIR: $DATADIR

rm -rf $DATADIR
mkdir -p $DATADIR

echo Loading netcdf module
module load netcdf

echo Running with srun...

srun $INPUTDIR/SWE_intel_release_none_fwave --grid-size-x=1000 --grid-size-y=1000 --input-bathymetry=$INPUTDIR/input/tohoku_2011/tohoku_gebco_ucsb3_2000m_hawaii_bath.nc --input-displacement=$INPUTDIR/input/tohoku_2011/tohoku_gebco_ucsb3_2000m_hawaii_displ.nc --time-duration=30000 --checkpoint-amount=1000 --boundary-condition-left=0 --boundary-condition-right=0 --boundary-condition-top=0 --boundary-condition-bottom=0 --output-basepath=$DATADIR/swe_tohoku > $DATADIR/stdout.log 2>$DATADIR/stderr.log

echo Done.