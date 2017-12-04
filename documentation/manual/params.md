# Command Line Parameters {#paramspage}

The following command line parameters are available for SWE (when used with `swe_dimensionalsplitting.cpp`)

- `-x, --grid-size-x [GRID_SIZE_X]` Number of cells in x direction
- `-y, --grid-size-y [GRID_SIZE_Y]` Number of cells in y direction
- `-a, --input-bathymetry [INPUT_BATHYMETRY]` Input bathymetry file name
- `-c, --input-displacement [INPUT_DISPLACEMENT]` Input displacement file name
- `-d, --time-duration [TIME_DURATION]` Time duration
- `-p, --checkpoint-amount [CHECKPOINT_AMOUNT]` Amount of checkpoints
- `-l, --boundary-condition-left [BOUNDARY_CONDITION_LEFT]` Boundary condition left
- `-r, --boundary-condition-right [BOUNDARY_CONDITION_RIGHT]` Boundary condition right##
- `-t, --boundary-condition-top [BOUNDARY_CONDITION_TOP]` Boundary condition top
- `-b, --boundary-condition-bottom [BOUNDARY_CONDITION_BOTTOM]` Boundary condition bottom
- `-o, --output-basepath [OUTPUT_BASEPATH]` Output base file name
- `-m, --input-checkpoint [INPUT_CHECKPOINT]` Input checkpoint file name
- `-f, --simulate-failure [SIMULATE_FAILURE]` Simulate failure after n timesteps
- `-h, --help` Show help

### Note: 
- `--input-checkpoint` and all other parameters are mutually exclusive
- Except for `--simulate-failure` which can be used to test the checkpointing system
