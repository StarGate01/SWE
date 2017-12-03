#ifndef __SWE_TSUNAMI_SCENARIO_H
#define __SWE_TSUNAMI_SCENARIO_H

#include <string>

#include "reader/NetCdfReader.hh"
#include "reader/NetCdfDataReader.hh"
#include "SWE_Scenario.hh"

class SWE_TsunamiScenario : public SWE_Scenario 
{

  private:

    BoundaryType* outflowConditions;
    int simulationTime;
    bool isCheckpoint = false;
    io::NetCdfDataReader* bathyReader = nullptr;
    io::NetCdfDataReader* dispReader = nullptr;
    io::NetCdfReader* checkpReader = nullptr;

  public:

    SWE_TsunamiScenario(std::string dispFilePath, std::string bathyFilePath, BoundaryType* outConditions, int time)
      : outflowConditions(outConditions), 
        simulationTime(time)
    {
      bathyReader = new io::NetCdfDataReader(bathyFilePath);
      dispReader = new io::NetCdfDataReader(dispFilePath);
      isCheckpoint = false;
    };

    SWE_TsunamiScenario(std::string checkpFilePath)
    {
      checkpReader = new io::NetCdfReader(checkpFilePath);
      //TODO sanity check nx == dalatenth
      isCheckpoint = true;
    };

    bool providesRawData() 
    {
#if !defined(NDEBUG) || defined(DEBUG)
      assert(isCheckpoint);
#endif
      return isCheckpoint; 
    };

    float getB(int x, int y) 
    {
#if !defined(NDEBUG) || defined(DEBUG)
      assert(isCheckpoint);
#endif
      return checkpReader->bData[(y * nx) + x];
    };

    float getH(int x, int y) 
    {
#if !defined(NDEBUG) || defined(DEBUG)
      assert(isCheckpoint);
#endif
      return checkpReader->hData[(y * nx) + x];
    };

    float getHu(int x, int y) 
    {
#if !defined(NDEBUG) || defined(DEBUG)
      assert(isCheckpoint);
#endif
      return checkpReader->huData[(y * nx) + x];
    };

    float getHv(int x, int y)
    { 
#if !defined(NDEBUG) || defined(DEBUG)
      assert(isCheckpoint);
#endif
      return checkpReader->hvData[(y * nx) + x];
    };

    float getBathymetry(float x, float y)
    {
#if !defined(NDEBUG) || defined(DEBUG)
      assert(!isCheckpoint);
#endif
      float bathy = bathyReader->sample(x, y, true);
      if(bathy <= 0 && bathy >= -20) bathy = -20;
      if(bathy >= 0 && bathy <= 20) bathy = 20;
      return bathy;
    };

    float getWaterHeight(float x, float y)
    { 
#if !defined(NDEBUG) || defined(DEBUG)
      assert(!isCheckpoint);
#endif
      return -min(bathyReader->sample(x, y), 0.0F) + dispReader->sample(x, y);
    };

	  float endSimulation()
    { 
        return simulationTime;
    };

    BoundaryType getBoundaryType(BoundaryEdge edge) 
    {
        switch(edge)
        {
          case BND_LEFT:
            return outflowConditions[0];
          case BND_RIGHT:
            return outflowConditions[1];
          case BND_BOTTOM:
            return outflowConditions[2];
          case BND_TOP:
            return outflowConditions[3];
        }
        return OUTFLOW;
    };

    float getBoundaryPos(BoundaryEdge i_edge) 
    {
       if (i_edge == BND_LEFT) return bathyReader->xMin;
       else if (i_edge == BND_RIGHT) return bathyReader->xMax;
       else if (i_edge == BND_BOTTOM) return bathyReader->yMin;
       else return bathyReader->yMax;
    };

    ~SWE_TsunamiScenario()
    {
      if(bathyReader != nullptr) delete bathyReader;
      if(dispReader != nullptr) delete dispReader;
      if(checkpReader != nullptr) delete checkpReader;
    };

};

#endif
