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

#ifndef __SWE_ARTIFICIAL_TSUNAMI_SCENARIO_H
#define __SWE_ARTIFICIAL_TSUNAMI_SCENARIO_H

#include <cmath>
#include <math.h>

#include "SWE_Scenario.hh"

#define SIMULATION_TIME 80

/**
 * Scenario "Sine wave artifical tusnami:
 * uniform water and bathymetry level with a sine wave shaped elevation in the middle
 */
class SWE_ArtificialTsunamiScenario : public SWE_Scenario 
{

  private:

    float getDisp(float x, float y)
    {
        if (std::abs(x-5000) <= 500 && std::abs(y-5000) <= 500)
        {
          return (float) (5 * (sin((((x-5000)/500)+1)*M_PI)) * (-((((y-5000)/500)*((y-5000)/500)))+1));
        }
        else return 0;
    }

  public:

    float getBathymetry(float x, float y) 
    { 
      return -100 + getDisp(x, y);
    };

    float getWaterHeight(float x, float y)
    {
      return 100;
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