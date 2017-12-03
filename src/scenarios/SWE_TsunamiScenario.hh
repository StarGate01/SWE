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
 */

#ifndef __SWE_TSUNAMI_SCENARIO_H
#define __SWE_TSUNAMI_SCENARIO_H

#include <string>


#include "parser/CDLStreamParser.hh"
#include "parser/CDLData.hh"
#include "SWE_Scenario.hh"

class SWE_TsunamiScenario : public SWE_Scenario 
{
  private:
    BoundaryType* outflowConditions;
    int simulationTime;
    parser::CDLData bathydata;
    parser::CDLData dispdata;
    std::string bathyFile;
    std::string dispFile;
    vector<float> ybathyvalues {0};
    vector<float> xbathyvalues {0};
    vector<float> zbathyvalues {0};

    vector<float> ydispvalues {0};
    vector<float> xdispvalues {0};
    vector<float> zdispvalues {0};

  public:
    SWE_TsunamiScenario(std::string dispFilePath, std::string bathyFilePath, BoundaryType* outConditions, int time)
      :outflowConditions(outConditions),
      simulationTime(time),
      bathyFile(bathyFilePath),
      dispFile(dispFilePath)
      {
        ifstream filebathy(bathyFile);
        parser::CDLStreamParser::CDLStreamToData(filebathy, bathydata);
        vector<float>& ybathyvaluestest = ((dynamic_cast<parser::CDLVariable<float>*>(bathydata.variables["y"]))->data);
        vector<float>& xbathyvaluestest = ((dynamic_cast<parser::CDLVariable<float>*>(bathydata.variables["x"]))->data);
        zbathyvalues = ((dynamic_cast<parser::CDLVariable<float>*>(bathydata.variables["z"]))->data);

        ifstream filedisp(dispFile);
        parser::CDLStreamParser::CDLStreamToData(filedisp, bathydata);
        ydispvalues = ((dynamic_cast<parser::CDLVariable<float>*>(bathydata.variables["y"]))->data);
        xdispvalues = ((dynamic_cast<parser::CDLVariable<float>*>(bathydata.variables["x"]))->data);
        zdispvalues = ((dynamic_cast<parser::CDLVariable<float>*>(bathydata.variables["z"]))->data);

      };

    float getBathymetry(float xpos, float ypos)
    {
        for(unsigned int y = 0; y<ydispvalues.size() ; y++)
        { // iterate first dimension of displacement
          for(unsigned int x = 0; x<xdispvalues.size() ; y++)
          { // iterate second dimension of displacement
            // find the displacement
            float displacement = zdispvalues.at((y*ydispvalues.size())+x);
            // find the position in bathymetry as displacement only contains fields with offset
            for(unsigned int yb = 0; yb<ybathyvalues.size() ; yb++){
              for(unsigned int xb = 0; xb<xbathyvalues.size() ; xb++){
                if (ybathyvalues.at(yb)-ydispvalues.at(y)<ZERO_PRECISION && xbathyvalues.at(xb)-xdispvalues.at(x)<ZERO_PRECISION)
                { // same field, add displacement
                  float tempbathy = zbathyvalues.at(yb*ybathyvalues.size()+xb);
                  zbathyvalues.erase(zbathyvalues.begin()+(yb*ybathyvalues.size()+xb));
                  zbathyvalues.insert(zbathyvalues.begin()+(yb*ybathyvalues.size()+xb), tempbathy + displacement);
                }
              }
            }
          }
        }
        // change bathymetry values from 20 and -20
        for(unsigned int yb = 0; yb<ybathyvalues.size() ; yb++){
          for(unsigned int xb = 0; xb<xbathyvalues.size() ; xb++){
            if(zbathyvalues.at(yb*ybathyvalues.size()+xb)<20 && zbathyvalues.at(yb*ybathyvalues.size()+xb)>=0)
            {
              zbathyvalues.erase(zbathyvalues.begin()+(yb*ybathyvalues.size()+xb));
              zbathyvalues.insert(zbathyvalues.begin()+(yb*ybathyvalues.size()+xb), 20);
            }
            else if(zbathyvalues.at(yb*ybathyvalues.size()+xb)<0 && zbathyvalues.at(yb*ybathyvalues.size()+xb)>-20)
            {
              zbathyvalues.erase(zbathyvalues.begin()+(yb*ybathyvalues.size()+xb));
              zbathyvalues.insert(zbathyvalues.begin()+(yb*ybathyvalues.size()+xb), -20);
            }
          }
        }
        // search for the position in bathymetry and get the closest value
        float distancex = 10000 ;
        unsigned int xpoint;
        float distancey = 10000;
        unsigned int ypoint;
        for(unsigned int yb = 0; yb<ybathyvalues.size() ; yb++){
          for(unsigned int xb = 0; xb<xbathyvalues.size() ; xb++){
            if (abs(ybathyvalues.at(yb)-ypos) < distancey)
            {
              distancey = abs(ybathyvalues.at(yb)-ypos);
              ypoint = yb;
            }
            if (abs(xbathyvalues.at(xb)-xpos) < distancex)
            {
              distancey = abs(xbathyvalues.at(xb)-xpos);
              xpoint = xb;
            }
          }
        }
        // finally return closest bathymetry
        return zbathyvalues.at(ypoint*ybathyvalues.size()+xpoint);

    };

    float getWaterHeight(float x, float y)
    { 
      float minbathy = 0;
      for(int i = zbathyvalues.size(); i>0 ; i--)
      {
        if(zbathyvalues.at(i) < minbathy){
          minbathy = zbathyvalues.at(i);
        }
      }
       return -minbathy;
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
       if (i_edge == BND_LEFT) return xbathyvalues.front();
       else if (i_edge == BND_RIGHT) return xbathyvalues.back();
       else if (i_edge == BND_BOTTOM) xbathyvalues.back();
       else return xbathyvalues.front();
    };

};
#endif
