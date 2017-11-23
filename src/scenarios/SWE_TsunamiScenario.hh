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

#ifndef __SWE_TSUNAMI_SCENARIO_H
#define __SWE_TSUNAMI_SCENARIO_H

#include <cmath>

#include "SWE_Scenario.hh"

// #define BATHY_OBSTACLE

/**
 * Scenario "Radial Dam Break":
 * elevated water in the center of the domain
 */
class SWE_TsunamiScenario : public SWE_Scenario 
{
  private:
    BoundaryType outflowConditions;
    int simulationTime;

  public:
    SWE_TsunamiScenario(BoundaryType outConditions, int time)
      :outflowConditions(outConditions),
      simulationTime(time)
      {
          
      };

    float getBathymetry(float x, float y)
    {
        //TODO: Implement
    };

    float getWaterHeight(float x, float y)
    { 
        //TODO: Implement
    };

	  virtual float endSimulation(int seconds)
    { 
        //TODO: Implement
    };

    virtual BoundaryType getBoundaryType(BoundaryEdge edge) 
    {
        //TODO: Implement
    };

    float getBoundaryPos(BoundaryEdge i_edge) 
    {
       //TODO: Implement
    };

};
#endif
