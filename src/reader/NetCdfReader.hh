#ifndef NETCDFREADER_HH_
#define NETCDFREADER_HH_

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

  class NetCdfReader;

}

class io::NetCdfReader  
{

private:

    int dataFile;
    int timeVar, xVar, yVar, bVar, hVar, huVar, hvVar, timeDim, xDim, yDim;

  public:

    size_t timeLength, xLength, yLength;
    float* bData, hData, huData, hvData;

    NetCdfReader(const std::string &i_fileName);
    ~NetCdfReader();

};

#endif /* NETCDFREADER_HH_ */
