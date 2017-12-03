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

    /** netCDF file id*/
    int dataFile;

    /** Variable ids */
    int zVar, xDim, yDim;

  public:

    size_t xLength, yLength;

    NetCdfDataReader(const std::string &i_fileName);

    ~NetCdfDataReader();

    Float2D getZValues();

};

#endif /* NETCDFDATAREADER_HH_ */
