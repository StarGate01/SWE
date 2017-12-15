/**
 * @file CoarseComputation.hh
 * @brief Provides Methods for coarse cell computation
 */

#ifndef COARSE_COMPUTATION_HH
#define COARSE_COMPUTATION_HH

#include <cassert>
#include <cmath>                //Standard math
#include "tools/help.hh"        //Float1D, Float2D
#include "Writer.hh"            //BoundarySize

namespace io
{

    class CoarseComputation
    {

        public:
            
            int scale;

        private:

            BoundarySize bsize;

            /**
            * @brief Returns the number of rows of the downscaled Float2D object
            * @param ny Number of original vertical cells
            *
            * @return Number of cells in downscaled field
            */
            int numCellsVertical(int ny);

            /**
            * @brief Returns the number of columns of the downscaled Float2D object
            * @param nx Number of original horizontal cells
            *
            * @return Number of cells in downscaled field
            */
            int numCellsHorizontal(int nx);

            /**
            * @brief Calculates the average of a subfield within the \param data object from ( \param xmin, \param ymin ) to (\param xmin + scale, \param ymin + scale)
            * @param data Float2D object that is to be downscaled
            * @param xmin Start x index
            * @param ymin Start y index
            *
            * @return Average value of 
            */
            float average(const Float2D& data, int xmin, int ymin);

        public: 

            int oldWidth, oldHeight, newWidth, newHeight;
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