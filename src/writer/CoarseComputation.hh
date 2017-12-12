/**
 * @file CoarseComputation.hh
 * @brief Provides Methods for coarse cell computation
 */

#ifndef COARSE_COMPUTATION_HH
#define COARSE_COMPUTATION_HH

#include <cassert>
#include "tools/help.hh"        //Float1D, Float2D
#include <cmath>

class CoarseComputation
{

    private:

        static double buildAverage(Float1D line, int startIndex, int endIndex, int excludeIndex);
    public:

        static Float2D processField(Float2D field, int scale);

};

#endif