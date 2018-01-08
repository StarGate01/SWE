# Compilation {#compilepageswe}

### Required packages:
 - `g++`
 - `scons`
 - Additional compilers depending on your SWE configuration

Compile with `scons buildVariablesFile=build/options/SWE_gnu.py` or `scons buildVariablesFile=build/options/SWE_gnu.py compileMode=debug` for debug symbol support to `build/SWE_gnu_debug_none_fwave`.

Run this command in the root directory of the framework repository. There are several build variable files provided in `build/options`.

The original documentation is available where our fork originates: https://github.com/TUM-I5/SWE/wiki.

However, we added several extensions:

- The boolean `dimsplit` compiler configuration option, which enables the dimensional splitting approach. This is enabled by default.
- The boolean `readNetCDF` and `parseCDL` compiler configuration options, which enable reading NetCDF and CDL files respectively.
- The boolean `compressNetCDF` which enables HDF5 compression of NetCDF files. This is disabled by default, because it takes a lot of computing power to compress and decompress.
- The boolean `customOpt` which enables some custom optimizations. This is enabled by default.
- The switch `intelOptParam` which specifies the level of optimization for the intel compiler.
- The boolean `compressNetCDF` which enabled HDF5 data compression on the output files.
- A scenario which tests this functionality.
- A system to save and load checkpoints.
- A system to compress data via rough scaling.