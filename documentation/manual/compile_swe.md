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
- The boolean `readNetCDF` and `parseCDL` compiler configuration ops, which enable reading NetCDF and CDL files respectively.
- A scenario which tests this functionality.
- A system to save and load checkpoints