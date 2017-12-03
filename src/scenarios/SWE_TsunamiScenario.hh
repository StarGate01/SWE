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

    float getDisplacement(float x, float y)
    {
      int xindex = round(((x - dispReader->xMin) / (dispReader->xMax - dispReader->xMin)) * dispReader->xLength);
      int yindex = round(((y - dispReader->yMin) / (dispReader->yMax - dispReader->yMin)) * dispReader->yLength);
      if(xindex < 0 || xindex > dispReader->xLength - 1 || yindex < 0 || yindex > dispReader->yLength - 1) return 0;
      return (*(dispReader->zData2D))[xindex][yindex];
    };

  public:
    SWE_TsunamiScenario(std::string dispFilePath, std::string bathyFilePath, BoundaryType* outConditions, int time)
      : outflowConditions(outConditions), 
        simulationTime(time)
      {
        bathyReader = new io::NetCdfDataReader(bathyFilePath);
        dispReader = new io::NetCdfDataReader(dispFilePath);
      };

    float getBathymetry(float xpos, float ypos)
    {
        int xindex = round(((xpos - bathyReader->xMin) / (bathyReader->xMax - bathyReader->xMin)) * bathyReader->xLength);
        int yindex = round(((ypos - bathyReader->yMin) / (bathyReader->yMax - bathyReader->yMin)) * bathyReader->yLength);
        if(xindex < 0) xindex = 0;
        if(xindex > bathyReader->xLength - 1) xindex = bathyReader->xLength - 1;
        if(yindex < 0) xindex = 0;
        if(yindex > bathyReader->yLength - 1) yindex = bathyReader->yLength - 1;
        float bathy = (*(bathyReader->zData2D))[xindex][yindex] + getDisplacement(xpos, ypos);
        if(bathy <= 0 && bathy >= -20) bathy = -20;
        if(bathy >= 0 && bathy <= 20) bathy = 20;
        return bathy;
    };

    float getWaterHeight(float x, float y)
    { 
      return getDisplacement(x, y);
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
        //Default
        return WALL;
    };

    float getBoundaryPos(BoundaryEdge i_edge) 
    {
       if (i_edge == BND_LEFT) return bathyReader->xMin;
       else if (i_edge == BND_RIGHT) return bathyReader->xMax;
       else if (i_edge == BND_BOTTOM) return bathyReader->yMax;
       else return bathyReader->yMin;
    };

    ~SWE_TsunamiScenario()
    {
      delete bathyReader;
      delete dispReader;
    }

};
#endif
