/**
 * @file CoarseComputation.hh
 * @brief Provides methods for coarse cell computation
 */

#ifndef COARSE_COMPUTATION_HH
#define COARSE_COMPUTATION_HH

#include <cassert>
#include <cmath>                //Standard math
#include "tools/help.hh"        //Float1D, Float2D
#include "Writer.hh"            //BoundarySize

namespace io
{

    /**
     * @brief This singleton class provides functions for coarse scaling
     */
    class CoarseComputation
    {

        public:
            
            //! The scaling factor, how many cells are merged in each dimension
            int scale;

        private:

            //! The size of the boundary
            BoundarySize bsize;

            /**
            * @brief Returns the number of cells in one dimension of the downscaled Float2D object
            * @param n Number of cells of original field
            *
            * @return Number of cells of downscaled field
            */
            int numCells(int n);

            /**
            * @brief Calculates the average of a subfield within the data object from (xmin, ymin) to (xmin + scale, ymin + scale)
            * @param data Float2D object that is to be downscaled
            * @param xmin Start x index
            * @param ymin Start y index
            *
            * @return Average value of the subfield
            */
            float average(const Float2D& data, int xmin, int ymin);

        public: 

            //! The original width
            int oldWidth;

            //! The original height
            int oldHeight;

            //! The new width
            int newWidth;

            //!The new height
            int newHeight;

            //! The computed (scaled) data
            Float2D* averages;

            /**
            * @brief Constructor for io::CoarseComputation
            * @param s Scale
            * @param bs BoundarySize object
            * @param ow Original width of the field
            * @param oh Original height of the field
            */
            CoarseComputation(int s, const BoundarySize& bs, int ow, int oh);

            /**
            * @brief Calculates the average values for the entire field
            *
            * @param data Field to be downscaled
            */
            void updateAverages(const Float2D& data);

            /**
            * @brief Deconstructor of io::CoarseComputation
            */
            ~CoarseComputation();
    };
}

#endif