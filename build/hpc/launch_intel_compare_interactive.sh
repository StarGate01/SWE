#!/bin/bash

srun launch.sh > >(tee -a launch_stdout.log) 2> >(tee -a launch_stderr.log >&2)
