#!/bin/bash

echo You are on $(hostname)
echo Running with srun on $(srun hostname)
srun ./launch.sh
