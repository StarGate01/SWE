/**
 * @file NetCdfReader.hh
 * @brief Functionality to read a checkpoint file
 */

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

// #include "writer/Writer.hh"

using namespace std;

namespace io 
{

  class NetCdfReader;

}

/**
 * @brief Functionality to read from a checkpoint file
 */
class io::NetCdfReader  
{

private:

    //! NetCDF file handle
    int dataFile;

  public:

    int timeVar, xVar, yVar, bVar, hVar, huVar, hvVar, timeDim, xDim, yDim;

    //! Sucess opening file
    bool success = false;

    //! Length of the time values buffer
    size_t timeLength;

    //! Length of the x values buffer
    size_t xLength;

    //! Length of the y values buffer
    size_t yLength;

    //! Pointer to the time data buffer
    float* timeData; 

    //! Pointer to the bathymetry data buffer
    float* bData; 

    //! Pointer to the water height data buffer
    float* hData; 

    //! Pointer to the horizontal flux data buffer
    float* huData; 

    //! Pointer to the vertical flux data buffer
    float* hvData;

    //! X minimum value
    float xMin;

    //! X maximum value
    float xMax;

    //! Y minimum value
    float yMin;

    //! Y maximum value
    float yMax;

    //! Time maximum value
    float timeMax;

    /**
     * @brief Constructor
     * 
     * @param i_fileName The file path of the data file to read
     * @param noassert Do not thow assert on file error
     * @param preloadLast Preload the last checkpoint
     */
    NetCdfReader(const string &i_fileName, bool noassert = false, bool preloadLast = true);

    /**
     * @brief Loads a specific timestep from the file
     * 
     * @param index the number of the timestep
     * 
     * @return Wether the call was successful
     */
    bool selectTimestep(uint32_t index);

    /**
     * @brief Get a text attribute
     * 
     * @param name The name
     * 
     * @return The attribute value
     */
    string getGlobalTextAttribute(const string& name);

    /**
     * @brief Get a integer attribute
     * 
     * @param name The name
     * 
     * @return The attribute value
     */
    int getGlobalIntAttribute(const string& name);

    /**
     * @brief Get a integer array attribute
     * 
     * @param name The name
     * 
     * @return The attribute value
     */
    int* getGlobalIntPtrAttribute(const string& name, int length);

    /**
     * @brief Get a float attribute
     * 
     * @param name The name
     * 
     * @return The attribute value
     */
    float getGlobalFloatAttribute(const string& name);

    /**
     * @brief Gets the value of a singe cell
     * 
     * @param pdata The layer variable id
     * @param x The x coordinate of the cell
     * @param y The y coordinate of the cell
     * @param timestamp The selected timestamp
     *
     * @return The cell value
     */
    float getCellValue(int pdata, int x, int y, int timestamp);

    ~NetCdfReader();

};

#endif /* NETCDFREADER_HH_ */
