#include "SWE_CoarseComputation.hh"

double CoarseComputation::buildAverage(Float1D line, int startIndex, int endIndex, int excludeIndex)
{
    assert(startIndex > 0 && startIndex <= endIndex && excludeIndex <= endIndex);

    double average = 0;
    for(int i = startIndex; i <= endIndex; i++)
    {
        if(i == excludeIndex)
            continue;
        average += line[i];
    }
    average /= line.getSize();
};

Float1D processLine(Float2D field, int line)
{
    
}