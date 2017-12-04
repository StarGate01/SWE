/**
 * @file SWE_ArtificialTsunamiScenario.hh
 * @brief Implements an artificial scenario
 */
#ifndef __SWE_ARTIFICIAL_TSUNAMI_SCENARIO_H
#define __SWE_ARTIFICIAL_TSUNAMI_SCENARIO_H

#include <cmath>
#include <math.h>

#include "SWE_Scenario.hh"

#define SIMULATION_TIME 80

/**
 * @brief Scenario "Sine wave" artifical tsunami:
 *  uniform water and bathymetry level with a sine wave shaped elevation in the middle
 */
class SWE_ArtificialTsunamiScenario : public SWE_Scenario 
{

  private:

    //! The outflow conditions
    BoundaryType* outflowConditions;

    /**
     * @brief Get displacement data at a position
     * 
     * @param x X position
     * @param y Y position
     * 
     * @return The displacement
     */
    float getDisp(float x, float y)
    {
        if (std::abs(x-5000) <= 500 && std::abs(y-5000) <= 500)
        {
          return (float) (5 * (sin((((x-5000)/500)+1)*M_PI)) * (-((((y-5000)/500)*((y-5000)/500)))+1));
        }
        else return 0;
    }

  public:

    /**
     * @brief Constructor
     * 
     * @param outConditions The outflow conditions
     */
    SWE_ArtificialTsunamiScenario(BoundaryType* outConditions)
      : outflowConditions(outConditions)
    {};

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
      return -100 + getDisp(x, y);
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
      return 100;
    };
    
    /**
     * @brief Get the maximum simulation time
     * 
     * @return the maximum scenario time
     */
    float endSimulation()
    {
       return (float) SIMULATION_TIME; 
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
      return outflowConditions[(int)edge];
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
       if (i_edge == BND_LEFT) return (float)0;
       else if (i_edge == BND_RIGHT) return (float)10000;
       else if (i_edge == BND_BOTTOM) return (float)0;
       else return (float)10000;
    };

};

#endif