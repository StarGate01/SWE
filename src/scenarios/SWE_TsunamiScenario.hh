#ifndef __SWE_TSUNAMI_SCENARIO_H
#define __SWE_TSUNAMI_SCENARIO_H

#include <string>

#include "reader/NetCdfDataReader.hh"
#include "SWE_Scenario.hh"

class SWE_TsunamiScenario : public SWE_Scenario 
{

  private:

    BoundaryType* outflowConditions;
    int simulationTime;
    io::NetCdfDataReader* bathyReader = nullptr;
    io::NetCdfDataReader* dispReader = nullptr;

  public:
    SWE_TsunamiScenario(std::string dispFilePath, std::string bathyFilePath, BoundaryType* outConditions, int time)
      : outflowConditions(outConditions), 
        simulationTime(time)
    {
      bathyReader = new io::NetCdfDataReader(bathyFilePath);
      dispReader = new io::NetCdfDataReader(dispFilePath);
    };

    float getBathymetry(float x, float y)
    {
        float bathy = bathyReader->sample(x, y, true);
        if(bathy <= 0 && bathy >= -20) bathy = -20;
        if(bathy >= 0 && bathy <= 20) bathy = 20;
        return bathy;
    };

    float getWaterHeight(float x, float y)
    { 
      return -min(bathyReader->sample(x, y), 0.0F) + dispReader->sample(x, y);
    };

	  virtual float endSimulation()
    { 
        return simulationTime;
    };

    virtual BoundaryType getBoundaryType(BoundaryEdge edge) 
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
      delete bathyReader;
      delete dispReader;
    }

};

#endif
