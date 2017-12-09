#ifndef SWE_COARSE_COMPUTATION_HH
#define SWE_COARSE_COMPUTATION_HH

#include <cassert>
#include "tools/help.hh"        //Float1D, Float2D

class CoarseComputation
{
private:
    static double buildAverage(Float1D line, int startIndex, int endIndex, int excludeIndex);
    static Float1D processLine(Float2D field, int line);
};

#endif