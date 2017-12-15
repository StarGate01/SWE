/**
 * @file CoarseComputation.cpp
 * @brief Implements the functionality defined in CoarseComputation.hh
 */

#include "CoarseComputation.hh"

using namespace io;

//TODO: Fusion numCellsVertical and numCellsHorzontal

/**
* @brief Returns the number of rows of the downscaled Float2D object
* @param ny Number of original vertical cells
*
* @return Number of cells in downscaled field
*/
int CoarseComputation::numCellsVertical(int ny)
{
    return (int)ceil(((float)ny)/((float)scale));
}

/**
* @brief Returns the number of columns of the downscaled Float2D object
* @param nx Number of original horizontal cells
*
* @return Number of cells in downscaled field
*/
int CoarseComputation::numCellsHorizontal(int nx)
{
    return (int)ceil(((float)nx)/((float)scale));
}

/**
* @brief Calculates the average of a subfield within the \param data object from ( \param xmin, \param ymin ) to (\param xmin + scale, \param ymin + scale)
* @param data Float2D object that is to be downscaled
* @param xmin Start x index
* @param ymin Start y index
*
* @return Average value of 
*/
float CoarseComputation::average(const Float2D& data, int xmin, int ymin)
{
    float sum = 0;
    for(int x=xmin; x<xmin+scale && x<oldWidth; x++) 
        for(int y=ymin; y<ymin+scale && y<oldHeight; y++)
             sum += data[x+bsize.boundarySize[0]][y+bsize.boundarySize[3]];
    return sum / (scale * scale);
}

/**
* @brief Constructor for io::CoarseComputation
* @param s Scale
* @param bs BoundarySize object
* @param ow Original width of the field
* @param oh Original height of the field
*/
CoarseComputation::CoarseComputation(int s, const BoundarySize& bs, int ow, int oh)
    : scale(s), bsize(bs),
      oldWidth(ow), oldHeight(oh),
      newWidth(numCellsHorizontal(oldWidth)),
      newHeight(numCellsVertical(oldHeight)),
      averages(new Float2D(newWidth, newHeight))
{}

/**
* @brief Calculates the average values for the entire field
*
* @param data Field to be downscaled
*/
void CoarseComputation::updateAverages(const Float2D& data)
{
    for(int x=0; x<newWidth; x++)
        for(int y=0; y<newHeight; y++)
            (*averages)[x][y] = average(data, scale * x, scale * y);
}

/**
* @brief Deconstructor of io::CoarseComputation
*/
CoarseComputation::~CoarseComputation()
{
    delete averages;
}