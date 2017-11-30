#include <cxxtest/TestSuite.h>
#include "../parser/CDLStreamParser.hh"
#include "../parser/CDLData.hh"
#include <fstream>

#include <string>

using namespace std;
using namespace parser;

namespace swe_tests
{
    class SWECDLTestsSuite;
}

class swe_tests::SWECDLTestsSuite : public CxxTest::TestSuite
{
   
    public:
        void testStringParse()
        {
            string cdltext = R"(
                netcdf foo { // example netCDF specification in CDL
                dimensions:
                    lat = 10, lon = 5, time = unlimited;
                variables:
                    int lat(lat), lon(lon), time(time);
                    float z(time,lat,lon), t(lat,lon);
                    lat:units = "degrees_north";
                    lon:units = "degrees_east";
                    time:units = "seconds";
                    z:valid_range = 0., 5000.;
                    :test = "global value";
                data:
                    lat = 0, 10, 20, 30, 40, 50, 60, 70, 80, 90;
                }
             )";
             CDLData cdldata = {
                 name: "foo",
                 globalAttributes: {
                    { "test", { name: "test", values: { "global value" } }}
                 },
                 dimensions: {
                    { "lat", { name: "lat", length: 10, unlimited: false }},
                    { "lon", { name: "lon", length: 5, unlimited: false }},
                    { "time", { name: "time", length: 0, unlimited: true }}
                 },
                 variables: {
                     { "lat", { 
                         name: "lat", type: "int", components: { "lat" }, 
                         attributes: { { "units", { name: "units", values: { "degrees_north" }  } } },
                         data: { "0", "10", "20", "30", "40", "50", "60", "70", "80", "90" }
                     }},
                     { "lon", { 
                         name: "lon", type: "int", components: { "lon" }, 
                         attributes: { { "units", { name: "units", values: { "degrees_east" } } } }, data: {}
                     }},
                     { "time", { 
                         name: "time", type: "int", components: { "time" }, 
                         attributes: { { "units", { name: "units", values: { "seconds" } } } }, data: {}
                     }},
                     { "z", { 
                         name: "z", type: "float", components: { "time", "lat", "lon" }, 
                         attributes: { { "valid_range", { name: "valid_range", values: { "0.", "5000." } } } }, data: {}
                     }},
                     { "t", { 
                         name: "t", type: "float", components: { "lat", "lon" }, attributes: {}, data: {}
                     }}
                 }
             };

             // ## Test 1 (Parse string) ##
             TS_ASSERT(CDLStreamParser::CDLStringToData(cdltext) == cdldata);

             // ## Test 2 (Parse file) ##
             std::ifstream file;
             file.open("testfile.cdl", std::ifstream::in);

             //Print warning if file could not be openend and fail
             if(!file.is_open())
                 TS_WARN("Stream could not read file in TestStringParse: Test 2: \t");
             TS_ASSERT(file.is_open());

             //Compare
             //TS_ASSERT(CDLStreamParser::CDLStringToData(file) == cdldata);
             file.close();
        }

};
