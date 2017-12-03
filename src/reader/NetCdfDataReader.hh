#ifndef NETCDFDATAREADER_HH_
#define NETCDFDATAREADER_HH_

#include <cstring>
#include <string>
#include <vector>
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

namespace io 
{

  class NetCdfDataReader;

}

class io::NetCdfDataReader  
{

private:

    int dataFile;
    int xVar, yVar, zVar, xDim, yDim;
    float* zData;

  public:

    size_t xLength, yLength;
    float xMin, xMax, yMin, yMax;
    Float2D* zData2D;

    NetCdfDataReader(const std::string &i_fileName);
    ~NetCdfDataReader();

};

#endif /* NETCDFDATAREADER_HH_ */
