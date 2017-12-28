/**
 * @file CoarseComputation.cpp
 * @brief Implements the functionality defined in CoarseComputation.hh
 */

#include "CoarseComputation.hh"

using namespace io;

int CoarseComputation::numCells(int n)
{
    return (int)ceil(((float)n)/((float)scale));
}

float CoarseComputation::average(const Float2D& data, int xmin, int ymin)
{
    float sum = 0;
    for(int x=xmin; x<xmin+scale && x<oldWidth; x++) 
        for(int y=ymin; y<ymin+scale && y<oldHeight; y++)
             sum += data[x+bsize.boundarySize[0]][y+bsize.boundarySize[3]];
    return sum / (scale * scale);
}

CoarseComputation::CoarseComputation(int s, const BoundarySize& bs, int ow, int oh)
    : scale(s), bsize(bs),
      oldWidth(ow), oldHeight(oh),
      newWidth(numCells(oldWidth)),
      newHeight(numCells(oldHeight))
{
    averages = new Float2D(newWidth, newHeight);
}

void CoarseComputation::updateAverages(const Float2D& data)
{
    for(int x=0; x<newWidth; x++)
        for(int y=0; y<newHeight; y++)
            (*averages)[x][y] = average(data, scale * x, scale * y);
}

CoarseComputation::~CoarseComputation()
{
    delete averages;
}