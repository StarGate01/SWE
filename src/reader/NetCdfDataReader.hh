#ifndef NETCDFDATAREADER_HH_
#define NETCDFDATAREADER_HH_

#include <cstring>
#include <string>
#ifdef USEMPI
#include <mpi.h>
#ifndef MPI_INCLUDED
#define MPI_INCLUDED
#define MPI_INCLUDED_NETCDF
#endif
#endif
#include <netcdf.h>
#ifdef MPI_INCLUDED_NETCDF
#undef MPI_INCLUDED
#undef MPI_INCLUDED_NETCDF
#endif

#include "writer/Writer.hh"

using namespace std;

namespace io 
{

  class NetCdfDataReader;

}

class io::NetCdfDataReader  
{

private:

    int dataFile;
    int xVar, yVar, zVar, xDim, yDim;

  public:

    size_t xLength, yLength;
    float xMin, xMax, yMin, yMax;
    float* zData;

    NetCdfDataReader(const string &i_fileName);
    float sample(float x, float y, bool extend = false, float fallback = 0);
    ~NetCdfDataReader();

};

#endif /* NETCDFDATAREADER_HH_ */
