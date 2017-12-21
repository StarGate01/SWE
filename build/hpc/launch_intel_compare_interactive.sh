#!/bin/bash

srun launch_intel_compare.sh > >(tee -a launch_stdout.log) 2> >(tee -a launch_stderr.log >&2)
