#!/bin/bash

INPUTDIR="$HOME/SWE/build"
DATADIR="$SCRATCH/$USER/swe_data"
echo INPUTDIR: $INPUTDIR
echo DATADIR: $DATADIR

AMP=/lrz/mnt/sys.x86_64/intel/studio2017_u5/vtune_amplifier_xe_2017.5.0.526192/bin64/amplxe-cl
AMPPREFIX="$AMP -collect hotspots -knob enable-user-tasks=true -knob analyze-openmp=true -app-working-dir $INPUTDIR --"

TSNOW=$(date +%s)
echo Current timestamp: $TSNOW

echo You are on $(hostname)
echo Running with srun on $(srun hostname)

run_swe () {
	MODE=release
	MYDATADIR="$DATADIR/$TSNOW/$1$2/$OUTPNAME"
	echo Using $MYDATADIR
	mkdir -p $MYDATADIR
	RCMD=""
	if [[ $2 == "amp" ]]; then
		MODE=debug
		RCMD+="$AMPPREFIX "	
	fi
	RCMD+="$INPUTDIR/SWE_$1_$MODE"
	RCMD+="_none_fwave"
	RCMD+=" --grid-size-x=$SIZE --grid-size-y=$SIZE --input-bathymetry=$BATHY --input-displacement=$DISPL --time-duration=$TIME --checkpoint-amount=$CHECKP"
	RCMD+=" --boundary-condition-left=0 --boundary-condition-right=0 --boundary-condition-top=0 --boundary-condition-bottom=0 --output-basepath=$MYDATADIR/swe --output-scale=1 --limit-threads=$NTHREADS"
	echo Executing: $RCMD
	$RCMD > $MYDATADIR/stdout.log 2>$MYDATADIR/stderr.log
	LOGDIR="$HOME/swe_logs/$TSNOW"
	MYLOGDIR="$LOGDIR/$1$2/$OUTPNAME"
	echo Saving logs to $MYLOGDIR
	mkdir -p $MYLOGDIR
	cp "$MYDATADIR/stderr.log" "$MYLOGDIR/stderr.log"
	echo -e "\n$1$2\n" >> "$LOGDIR/stderr.all.log"
	cat "$MYLOGDIR/stderr.log" >> "$LOGDIR/stderr.all.log"
	head -50 "$MYDATADIR/stdout.log" >> "$MYLOGDIR/stdout.short.log"
	echo "[ ... ]" >> "$MYLOGDIR/stdout.short.log"
	tail -50 "$MYDATADIR/stdout.log" >> "$MYLOGDIR/stdout.short.log"
	echo -e "\n$1$2\n" >> "$LOGDIR/stdout.all.short.log"
	cat "$MYLOGDIR/stdout.short.log" >> "$LOGDIR/stdout.all.short.log"
}

echo Running optimized gcc release build
run_swe gnu

echo Running not optimized gcc release build
run_swe gnu_noopt

echo Running optimized intel release build
run_swe intel

echo Running not optimized intel release build
run_swe intel_noopt

echo Profiling optimized intel debug build
run_swe intel amp

echo Profiling not optimized intel debug build
run_swe intel_noopt amp

echo Done.
