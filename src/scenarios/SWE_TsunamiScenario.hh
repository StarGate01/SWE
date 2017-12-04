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
    bool isCheckpoint = false;
    io::NetCdfDataReader* bathyReader = nullptr;
    io::NetCdfDataReader* dispReader = nullptr;
    io::NetCdfReader* checkpReader = nullptr;
    int nx, ny;
    int simulationTime;

  public:

    SWE_TsunamiScenario(std::string dispFilePath, std::string bathyFilePath, BoundaryType* outConditions, int time)
      : outflowConditions(outConditions), 
        simulationTime(time)
    {
      bathyReader = new io::NetCdfDataReader(bathyFilePath);
      dispReader = new io::NetCdfDataReader(dispFilePath);
      isCheckpoint = false;
    };

    SWE_TsunamiScenario(io::NetCdfReader* checkpReader, int nx, int ny, int time)
      : checkpReader(checkpReader),
        nx(nx), ny(ny),
        simulationTime(time)
    {
      isCheckpoint = true;
    };

    bool providesRawData() 
    {
      return isCheckpoint; 
    };

    float getB(int x, int y) 
    {
      assert(isCheckpoint);
      if(x < 0) x = 0;
      if(x >= nx) x = nx-1;
      if(y < 0) y = 0;
      if(y >= ny) y = ny -1;
      return checkpReader->bData[(y * nx) + x];
    };

    float getH(int x, int y) 
    {
      assert(isCheckpoint);
      return checkpReader->hData[(y * nx) + x];
    };

    float getHu(int x, int y) 
    {
      assert(isCheckpoint);
      return checkpReader->huData[(y * nx) + x];
    };

    float getHv(int x, int y)
    { 
      assert(isCheckpoint);
      return checkpReader->hvData[(y * nx) + x];
    };

    float getBathymetry(float x, float y)
    {
      assert(!isCheckpoint);
      float bathy = bathyReader->sample(x, y, true) + dispReader->sample(x, y);
      if(bathy <= 0 && bathy >= -20) bathy = -20;
      if(bathy >= 0 && bathy <= 20) bathy = 20;
      return bathy;
    };

    float getWaterHeight(float x, float y)
    { 
      assert(!isCheckpoint);
      float bathy = bathyReader->sample(x, y, true);
      if(bathy <= 0 && bathy >= -20) bathy = -20;
      if(bathy >= 0 && bathy <= 20) bathy = 20;
      return -min(bathy, 0.0F);
    };

	  float endSimulation()
    { 
        return simulationTime;
    };

    BoundaryType getBoundaryType(BoundaryEdge edge) 
    {
      if(isCheckpoint) 
      {
        int32_t* vptr = checkpReader->getGlobalIntPtrAttribute("outconditions", 4);
        return (BoundaryType)vptr[(int)edge];
      }
      else return outflowConditions[(int)edge];
    };

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

    ~SWE_TsunamiScenario()
    {
      if(bathyReader != nullptr) delete bathyReader;
      if(dispReader != nullptr) delete dispReader;
    };

};

#endif
