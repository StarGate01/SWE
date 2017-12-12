#!/bin/bash

INPUTDIR=$HOME/SWE/build
DATADIR=$SCRATCH/$USER/swe_data
echo INPUTDIR: $INPUTDIR
echo DATADIR: $DATADIR

MODE=release
SIZE=1000
BATHY=$INPUTDIR/input/tohoku_2011/tohoku_gebco_ucsb3_2000m_hawaii_bath.nc
DISPL=$INPUTDIR/input/tohoku_2011/tohoku_gebco_ucsb3_2000m_hawaii_displ.nc
TIME=30000
CHECKP=1000
OUTPNAME=tohoku_2011

TSNOW=$(date +%s)
echo Current timestamp: $TSNOW

echo You are on $(hostname)
echo Running with srun on $(srun hostname)...

run_swe () {
	MYDATADIR=$DATADIR/$TSNOW/$1/$OUTPNAME
	echo Using $MYDATADIR
	mkdir -p $MYDATADIR
	srun $INPUTDIR/SWE_$1_$MODE_none_fwave --grid-size-x=$SIZE --grid-size-y=$SIZE --input-bathymetry=$BATHY --input-displacement=$DISPL --time-duration=$TIME --checkpoint-amount=$CHECKP --boundary-condition-left=0 --boundary-condition-right=0 --boundary-condition-top=0 --boundary-condition-bottom=0 --output-basepath=$MYDATADIR/swe --output-scale=1 > $MYDATADIR/stdout.log 2>$MYDATADIR/stderr.log
}

echo Running gcc build
run_swe gnu
echo Running intel build
run_swe intel

echo Done.
