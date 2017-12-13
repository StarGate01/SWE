/**
 * @file CoarseComputation.hh
 * @brief Provides Methods for coarse cell computation
 */

#ifndef COARSE_COMPUTATION_HH
#define COARSE_COMPUTATION_HH

#include <cassert>
#include <cmath>
#include "tools/help.hh"        //Float1D, Float2D
#include "Writer.hh"            //BoundarySize

namespace io
{

    class CoarseComputation
    {

        // private:

        //     static double buildAverage(Float1D line, int startIndex, int endIndex, int excludeIndex);
        // public:

        //     static Float2D processField(Float2D field, int scale);

        public:
            
            int scale;

        private:

            BoundarySize bsize;

            int numCellsVertical(int ny);

            int numCellsHorizontal(int nx);

            float average(const Float2D& data, int xmin, int ymin);

        public: 

            int oldWidth, oldHeight, newWidth, newHeight;
            Float2D* averages;

            CoarseComputation(int s, const BoundarySize& bs, int ow, int oh);

            void updateAverages(const Float2D& data);

            ~CoarseComputation();

    };

}

#endif