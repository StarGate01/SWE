/**
 * @file
 * This file is part of SWE.
 *
 * @author Michael Bader, Kaveh Rahnema, Tobias Schnabel
 * @author Sebastian Rettenberger (rettenbs AT in.tum.de, http://www5.in.tum.de/wiki/index.php/Sebastian_Rettenberger,_M.Sc.)
 *
 * @section LICENSE
 *
 * SWE is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * SWE is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with SWE.  If not, see <http://www.gnu.org/licenses/>.
 *
 *
 * @section DESCRIPTION
 *
 * TODO
 */

#ifndef __SWE_SIMPLE_SCENARIOS_H
#define __SWE_SIMPLE_SCENARIOS_H

#include <cmath>
#include <math.h>

#include "SWE_Scenario.hh"

#define SIMULATION_TIME 80

// #define BATHY_OBSTACLE

/**
 * Scenario "Radial Dam Break":
 * elevated water in the center of the domain
 */
class SWE_RadialDamBreakScenario : public SWE_Scenario 
{

  public:

    float getBathymetry(float x, float y)
    {
#ifdef BATHY_OBSTACLE
      return ((x > 300 && x < 500 && y > 200 && y < 400) ? -10.f : 0.f)
        + ((x > 400 && x < 700 && y > 600 && y < 800) ? 20.f : 0.f);
#else
      return 0;
#endif
    };

    float getWaterHeight(float x, float y)
    { 
      return ((sqrt((x - 500.f) * (x - 500.f) + (y - 500.f) * (y - 500.f)) < 100.f) ? 25.f : 10.0f);
    };

	  virtual float endSimulation() 
    { 
      return (float) SIMULATION_TIME; 
    };

    virtual BoundaryType getBoundaryType(BoundaryEdge edge) 
    {
       return OUTFLOW; 
    };

    /** Get the boundary positions
     *
     * @param i_edge which edge
     * @return value in the corresponding dimension
     */
    float getBoundaryPos(BoundaryEdge i_edge) 
    {
       if (i_edge == BND_LEFT) return (float)0;
       else if (i_edge == BND_RIGHT) return (float)1000;
       else if (i_edge == BND_BOTTOM) return (float)0;
       else return (float)1000;
    };

};

/**
 * Scenario "Bathymetry Dam Break":
 * uniform water depth, but elevated bathymetry in the centre of the domain
 */
class SWE_BathymetryDamBreakScenario : public SWE_Scenario 
{

  public:

    float getBathymetry(float x, float y) 
    { 
       return (std::sqrt((x - 500.f) * (x - 500.f) + (y - 500.f) * (y - 500.f)) < 50.f) ? -255.f : -260.f;
    };

    float getWaterHeight(float x, float y)
    {
      return (float) 260;
    }
    
	  virtual float endSimulation()
    {
       return (float) SIMULATION_TIME; 
    };

    virtual BoundaryType getBoundaryType(BoundaryEdge edge)
    {
       return OUTFLOW; 
    };

    /** Get the boundary positions
     *
     * @param i_edge which edge
     * @return value in the corresponding dimension
     */
    float getBoundaryPos(BoundaryEdge i_edge) 
    {
       if (i_edge == BND_LEFT) return (float)0;
       else if (i_edge == BND_RIGHT) return (float)1000;
       else if (i_edge == BND_BOTTOM) return (float)0;
       else return (float)1000;
    };

};

/**
 * Scenario "Sea at Rest":
 * flat water surface ("sea at rest"), 
 * but non-uniform bathymetry (id. to "Bathymetry Dam Break")
 * test scenario for "sea at rest"-solution 
 */
class SWE_SeaAtRestScenario : public SWE_Scenario
{

  public:

    float getWaterHeight(float x, float y)
    { 
       return (sqrt((x - 0.5) * (x - 0.5) + (y - 0.5) * (y - 0.5)) < 0.1f) ? 9.9f : 10.0f;
    };

    float getBathymetry(float x, float y)
    { 
       return (sqrt((x - 0.5) * (x - 0.5) + (y - 0.5) * (y - 0.5)) < 0.1f) ? 0.1f : 0.0f;
    };

};

/**
 * Scenario "Splashing Pool":
 * intial water surface has a fixed slope (diagonal to x,y)
 */
class SWE_SplashingPoolScenario : public SWE_Scenario
{

  public:

    float getBathymetry(float x, float y) 
    {
       return -250.f;
    };

    float getWaterHeight(float x, float y) 
    {
    	return 250.0f + (5.0f - (x + y) / 200);
    };

    virtual float endSimulation() 
    { 
      return (float) SIMULATION_TIME; 
    };

    /** Get the boundary positions
     *
     * @param i_edge which edge
     * @return value in the corresponding dimension
     */
    float getBoundaryPos(BoundaryEdge i_edge) 
    {
       if (i_edge == BND_LEFT) return (float)0;
       else if (i_edge == BND_RIGHT) return (float)1000;
       else if (i_edge == BND_BOTTOM) return (float)0;
       else return (float)1000;
    };

};

/**
 * Scenario "Splashing Cone":
 * bathymetry forms a circular cone
 * intial water surface designed to form "sea at rest"
 * but: elevated water region in the centre (similar to radial dam break)
 */
class SWE_SplashingConeScenario : public SWE_Scenario
{

  public:

    float getWaterHeight(float x, float y)
    { 
       float r = sqrt((x - 0.5f) * (x - 0.5f) + (y - 0.5f) * (y - 0.5f));
       float h = 4.0f - 4.5f * (r / 0.5f);
       if (r < 0.1f) h = h + 1.0f;
       return (h > 0.0f) ? h : 0.0f;
    };

    float getBathymetry(float x, float y) 
    { 
       float r = sqrt((x - 0.5f) * (x - 0.5f) + (y - 0.5f) * (y - 0.5f));
       return 1.0f + 9.0f * ((r < 0.5f) ? r : 0.5f);
    };
    
    float waterHeightAtRest()
    {
       return 4.0f; 
    };

    float endSimulation() 
    { 
      return 0.5f; 
    };

    virtual BoundaryType getBoundaryType(BoundaryEdge edge)
    { 
      return OUTFLOW;
    };

};


/**
 * Scenario "Sine wave artifical tusnami:
 * uniform water and bathymetry level with a sine wave shaped elevation in the middle
 */
class SWE_ArtificialTsunamiScenario : public SWE_Scenario 
{

  public:

    float getBathymetry(float x, float y) 
    { 

      if (abs(x-5000) <= 500 && abs(y-5000) <= 500)
      {
        return (float) (-100 + (5 * (sin((((x-5000)/500)+1)*M_PI)) * (-(((y-5000)/500)*((y-5000)/500)+1))));
      }
       else
       {
          return (float) -100;
       }
    };

    float getWaterHeight(float x, float y)
    {
      return (float) 0;
    };
    
	  virtual float endSimulation()
    {
       return (float) SIMULATION_TIME; 
    };

    virtual BoundaryType getBoundaryType(BoundaryEdge edge)
    {
       return WALL; 
    };

    /** Get the boundary positions
     *
     * @param i_edge which edge
     * @return value in the corresponding dimension
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
