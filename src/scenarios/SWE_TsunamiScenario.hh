/**
 * @file SWE_TsunamiScenario.hh
 * @brief Implements a data file driven scenario
 */

#ifndef __SWE_TSUNAMI_SCENARIO_H
#define __SWE_TSUNAMI_SCENARIO_H

#include <string>

#include "reader/NetCdfReader.hh"
#include "reader/NetCdfDataReader.hh"
#include "SWE_Scenario.hh"

/**
 * @brief A Scenario which loads bathymatry and displacement from files, or restarts from a checkpoint file.
 */
class SWE_TsunamiScenario : public SWE_Scenario 
{

  private:

    //! The outflow conditions
    BoundaryType* outflowConditions;

    //! Wether this scenario is run from a checkpoint
    bool isCheckpoint = false;

    //! Data reader for the bathymetry
    io::NetCdfDataReader* bathyReader = nullptr;

    //! Data reader for the disposition
    io::NetCdfDataReader* dispReader = nullptr;

    //! Reader for the checkpoint data
    io::NetCdfReader* checkpReader = nullptr;

    //! Amount of cells in x dimension
    int nx;
    
    //! Amount of cells in y dimension
    int ny;

    //! Total simulation time
    int simulationTime;

  public:

    /**
     * @brief Constructor
     * 
     * @param dispFilePath File path to the disposition data
     * @param bathyFilePath File path to the bathymetry data
     * @param outConditions The outflow conditions
     * @param time The total simulation time
     */
    SWE_TsunamiScenario(std::string dispFilePath, std::string bathyFilePath, BoundaryType* outConditions, int time)
      : outflowConditions(outConditions), 
        simulationTime(time)
    {
      bathyReader = new io::NetCdfDataReader(bathyFilePath);
      dispReader = new io::NetCdfDataReader(dispFilePath);
      isCheckpoint = false;
    };

    /**
     * @brief Constructor
     * 
     * @param checkpReader A reader for a checkpoint file
     * @param nx Amount of cells in x dimension
     * @param ny Amount of cells in y dimension
     * @param time The total simulation time
     */
    SWE_TsunamiScenario(io::NetCdfReader* checkpReader, int nx, int ny, int time)
      : checkpReader(checkpReader),
        nx(nx), ny(ny),
        simulationTime(time)
    {
      isCheckpoint = true;
    };

    /**
     * @brief Wether this scenario is run from a checkpoint
     * 
     * @return Wether this scenario can privide values for the data buffers directly
     */
    bool providesRawData() 
    {
      return isCheckpoint; 
    };

    /**
     * @brief Get a bathymetry value a indices
     * 
     * @param x Index for x
     * @param y Index for y
     * 
     * @return The bathymetry value at this index
     */
    float getB(int x, int y) 
    {
      #ifndef NDEBUG
        assert(isCheckpoint);
      #endif

      if(x < 0) x = 0;
      if(x >= nx) x = nx-1;
      if(y < 0) y = 0;
      if(y >= ny) y = ny -1;
      return checkpReader->bData[(y * nx) + x];
    };

     /**
     * @brief Get a water height value a indices
     * 
     * @param x Index for x
     * @param y Index for y
     * 
     * @return The water height value at this index
     */
    float getH(int x, int y) 
    {
      #ifndef NDEBUG
        assert(isCheckpoint);
      #endif
      return checkpReader->hData[(y * nx) + x];
    };

    /**
     * @brief Get a horizontal flux value a indices
     * 
     * @param x Index for x
     * @param y Index for y
     * 
     * @return The horizontal flux value at this index
     */
    float getHu(int x, int y) 
    {
      #ifndef NDEBUG
        assert(isCheckpoint);
      #endif
      return checkpReader->huData[(y * nx) + x];
    };

    /**
     * @brief Get a vertical flux value a indices
     * 
     * @param x Index for x
     * @param y Index for y
     * 
     * @return The vertical flux value at this index
     */
    float getHv(int x, int y)
    { 
      #ifndef NDEBUG
        assert(isCheckpoint);
      #endif
      return checkpReader->hvData[(y * nx) + x];
    };

     /**
     * @brief Get the bathymetry at a position
     *      
     * @param x X position
     * @param y Y position
     * 
     * @return The bathymetry
     */
    float getBathymetry(float x, float y)
    {
      #ifndef NDEBUG
        assert(!isCheckpoint);
      #endif
      float bathy = bathyReader->sample(x, y, true) + dispReader->sample(x, y);
      if(bathy <= 0 && bathy >= -20) bathy = -20;
      if(bathy >= 0 && bathy <= 20) bathy = 20;
      return bathy;
    };

    /**
     * @brief Get water height at a position
     *      
     * @param x X position
     * @param y Y position
     * 
     * @return The water height
     */
    float getWaterHeight(float x, float y)
    { 
      #ifndef NDEBUG
        assert(!isCheckpoint);
      #endif
      float bathy = bathyReader->sample(x, y, true);
      if(bathy <= 0 && bathy >= -20) bathy = -20;
      if(bathy >= 0 && bathy <= 20) bathy = 20;
      return -min(bathy, 0.0F);
    };

    /**
     * @brief Get the maximum simulation time
     * 
     * @return the maximum scenario time
     */
	  float endSimulation()
    { 
        return simulationTime;
    };

    /** 
     * @brief Get the boundary type
     * 
     * @param edge Which edge
     * 
     * @return The boundary type for this edge
     */
    BoundaryType getBoundaryType(BoundaryEdge edge) 
    {
      if(isCheckpoint) 
      {
        int32_t* vptr = checkpReader->getGlobalIntPtrAttribute("outconditions", 4);
        return (BoundaryType)vptr[(int)edge];
      }
      else return outflowConditions[(int)edge];
    };

    /**
     * @brief Get the domain size
     * 
     * @param i_edge Which edge
     * 
     * @return The size in this direction
     */
    float getBoundaryPos(BoundaryEdge i_edge) 
    {
      if(isCheckpoint)
      {
        if (i_edge == BND_LEFT) return checkpReader->xMin;
        else if (i_edge == BND_RIGHT) return checkpReader->xMax;
        else if (i_edge == BND_BOTTOM) return checkpReader->yMin;
        else return checkpReader->yMax;
      }
      else
      {
        if (i_edge == BND_LEFT) return bathyReader->xMin;
        else if (i_edge == BND_RIGHT) return bathyReader->xMax;
        else if (i_edge == BND_BOTTOM) return bathyReader->yMin;
        else return bathyReader->yMax;
      }
    };

    /**
     * @brief Destructor
     * 
     * Cleans up the data readers
     */
    ~SWE_TsunamiScenario()
    {
      if(bathyReader != nullptr) delete bathyReader;
      if(dispReader != nullptr) delete dispReader;
    };

};

#endif
