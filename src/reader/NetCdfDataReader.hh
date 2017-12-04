/**
 * @file NetCdfDataReader.hh
 * @brief Functionality for reading data files
 */

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

/**
 * @brief Functionality to read from a 2D NetCDF data file
 */
class io::NetCdfDataReader  
{

private:

    //! NetCDF file handle
    int dataFile;

    int xVar, yVar, zVar, xDim, yDim;

  public:

    //! Length of the x values buffer
    size_t xLength;

    //! Length of the y values buffer
    size_t yLength;

    //! X minimum value
    float xMin;

    //! X maximum value
    float xMax;

    //! Y minimum value
    float yMin;

    //! Y maximum value
    float yMax;

    //! Pointer to the Z data buffer
    float* zData;

    /**
     * @brief Constructor
     * 
     * @param i_fileName The file path of the data file to read
     */
    NetCdfDataReader(const string &i_fileName);

    /**
     * @brief samples from the z data
     * 
     * @param x X position
     * @param y Y position
     * @param extend Extend over bounds
     * @param fallback Default value
     * 
     * @return The sampled value
     */
    float sample(float x, float y, bool extend = false, float fallback = 0);

    ~NetCdfDataReader();

};

#endif /* NETCDFDATAREADER_HH_ */
