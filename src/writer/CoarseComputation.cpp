/**
 * @file CoarseComputation.cpp
 * @brief Implements the functionality defined in CoarseComputation.hh
 */

#include "CoarseComputation.hh"

double CoarseComputation::buildAverage(Float1D line, int startIndex, int endIndex, int excludeIndex)
{
    assert(startIndex >= 0 && startIndex <= endIndex && excludeIndex <= endIndex);

    double average = 0;
    for(int i = startIndex; i <= endIndex; i++)
    {
        if(i == excludeIndex) continue;
        average += line[i];
    }
    average /= line.getSize();
    return average;
};

Float2D CoarseComputation::processField(Float2D field, int scale)
{
    if(scale <= 1) return field;

    int remainder_x = field.getCols() % scale;
    int remainder_y = field.getRows() % scale;
    int new_xSize = (int) floor((float) field.getCols() / (float) scale) + remainder_x;
    int new_ySize = (int) floor((float) field.getRows() / (float) scale) + remainder_y;
    Float2D result_x(new_xSize, field.getRows());

    //Scale down x
    for(int y = 0; y < field.getRows(); y++)
    {
        int new_x = 0;
        for(int x = 0; x < field.getCols(); x += scale)
        {
            int targetIndex = x + (int) ceil((float) scale / 2.0);
            result_x[y][new_x] = buildAverage(field.getRowProxy(y), x, fmin(x + scale, field.getCols()), targetIndex);
            new_x++;
        }
    }

    //Scale down y
    Float2D result(new_xSize, new_ySize);
    for(int x = 0; x < result_x.getRows(); x += scale)
    {
        int new_y = 0;
        for(int y = 0; y < result_x.getRows(); y += scale)
        {
            int targetIndex = y + (int) ceil((float) scale / 2.0);
            result[x][new_y] = buildAverage(result_x.getColProxy(x), y, fmin(y + scale, result_x.getRows()), targetIndex);
            new_y++;
        }
    }

    return result;
};