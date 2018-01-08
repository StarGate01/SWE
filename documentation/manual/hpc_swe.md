# Running On HPC {#hpcpageswe}

All scripts for usage of SWE on a high performance cluster are located in `build/hpc`.

## Loading modules
`module load $(./modules.sh)`

## Script families
 - Compiling: `compile_*.sh` and `clean_*.sh`
 - Launching: `launch_core.sh`, and its wrappers `launch_*.sh`
 - Profile definitions: `profile_*.sh`
 - Batch jobs: `batch_*.cmd`