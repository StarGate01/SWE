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

    /** netCDF file id*/
    int dataFile;

    /** Variable ids */
    int timeVar, hVar, huVar, hvVar;

  public:

    NetCdfReader(const std::string &i_fileName,
                 const BoundarySize &i_boundarySize,
                 int i_nX, int i_nY);

    ~NetCdfReader();

    void getLatestTimeStep(const Float2D &i_h,
                        const Float2D &i_hu,
                        const Float2D &i_hv);

};

#endif /* NETCDFREADER_HH_ */
