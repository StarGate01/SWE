/**
 * @file NetCdfWriter.hh
 * @brief Functions to write a NetCdf checkpoint file
 */

#ifndef NETCDFWRITER_HH_
#define NETCDFWRITER_HH_

#include <cstring>
#include <string>
#include <vector>
#ifdef USEMPI
#include <mpi.h>
#ifndef MPI_INCLUDED
#define MPI_INCLUDEDh
#define MPI_INCLUDED_NETCDF
#endif
#endif
#include <netcdf.h>
#ifdef MPI_INCLUDED_NETCDF
#undef MPI_INCLUDED
#undef MPI_INCLUDED_NETCDF
#endif

#include "writer/Writer.hh"
#include "SWE_CoarseComputation.hh"

/**
 * @brief Provides data writers and readers
 */
namespace io 
{

  class NetCdfWriter;

}

/**
 * @brief Provides functions to write a NetCdf checkpoint
 */
class io::NetCdfWriter : public io::Writer
{

private:

    //! netCDF file id
    int dataFile;

    int timeVar, hVar, huVar, hvVar, bVar;

    //! Flush after every x write operation? 
    unsigned int flush;

    const bool is_checkpoint = false;
    const int scale = 1;

    /**
     * @brief Writes time dependent data to a netCDF-file (-> constructor) with respect to the boundary sizes.
     *
     * boundarySize[0] == left
     * boundarySize[1] == right
     * boundarySize[2] == bottom
     * boundarySize[3] == top
     *
     * @param i_matrix Array which contains time dependent data.
     * @param i_boundarySize Size of the boundaries.
     * @param i_ncVariable Time dependent netCDF-variable to which the output is written to.
     */
    void writeVarTimeDependent(const Float2D i_matrix, int i_ncVariable);

    /**
     * @brief Write time independent data to a netCDF-file (-> constructor) with respect to the boundary sizes.
     * 
     * Variable is time-independent
     * boundarySize[0] == left
     * boundarySize[1] == right
     * boundarySize[2] == bottom
     * boundarySize[3] == top
     *
     * @param i_matrix Array which contains time independent data.
     * @param i_boundarySize Size of the boundaries.
     * @param i_ncVariable time Independent netCDF-variable to which the output is written to.
     */
    void writeVarTimeIndependent(const Float2D &i_matrix, int i_ncVariable);

  public:

    /**
     * Create or append to a NetCdf-file
     *
     * @param i_baseName Base name of the netCDF-file to which the data will be written to
     * @param i_filebasename Common name of the file
     * qparam i_boundarySize The boundary size
     * @param i_nX Number of cells in the horizontal direction
     * @param i_nY Number of cells in the vertical direction
     * @param i_dX Cell size in x-direction
     * @param i_dY Cell size in y-direction
     * @param i_outConditions The outflow conditions
     * @param i_timeDuration The time duration
     * @param i_checkpoints Amount of checkpoints
     * @param i_originX Origin X
     * @param i_originY Origin Y
     * @param timestep Current timestep
     * @param append Wether to append to an existing file
     * @param i_flush If > 0, flush data to disk every i_flush write operation
     * @param output_scale the output is averaged to comply with this cell amount multiplyer
     * @param is_checkpoint if true output is not scaled, if false scale is aplied
     */
    NetCdfWriter(const std::string &i_fileName,
      const std::string &i_filebaseName,
      const Float2D &i_b,
      const BoundarySize &i_boundarySize,
      int i_nX, int i_nY,
      float i_dX, float i_dY,
      int* i_outConditions,
      float i_timeDuration,
      int i_checkpoints,
      float i_originX = 0, float i_originY = 0,
      size_t timestep = 0,
      bool append = false,
      unsigned int i_flush = 0,
      const bool ischeckpoint = false,
      const int outscale = 1);

    virtual ~NetCdfWriter();

    /**
     * @brief Writes the unknwons to a netCDF-file (-> constructor) with respect to the boundary sizes.
     *
     * boundarySize[0] == left
     * boundarySize[1] == right
     * boundarySize[2] == bottom
     * boundarySize[3] == top
     *
     * @param i_h Water heights at a given time step.
     * @param i_hu Momentums in x-direction at a given time step.
     * @param i_hv Momentums in y-direction at a given time step.
     * @param i_boundarySize Size of the boundaries.
     * @param i_time simulation Time of the time step.
     */
    void writeTimeStep(const Float2D &i_h,const Float2D &i_hu,
      const Float2D &i_hv, float i_time);

  private:

    /**
     * @brief This is a small wrapper for `nc_put_att_text` which automatically sets the length.
     * 
     * @param varid Variable ID
     * @param name The name
     * @param value The value 
     */
    void ncPutAttText(int varid, const char* name, const char *value)
    {
    	nc_put_att_text(dataFile, varid, name, strlen(value), value);
    }

};

#endif /* NETCDFWRITER_HH_ */
