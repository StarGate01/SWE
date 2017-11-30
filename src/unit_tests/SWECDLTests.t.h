/**
 * @file SWECDLTests.t.h
 * @brief Unit tests for the CDL parser
 */

#include <cxxtest/TestSuite.h>
#include "../parser/CDLStreamParser.hh"
#include "../parser/CDLData.hh"
#include <fstream>

#include <string>

using namespace std;
using namespace parser;

/**
 * @brief Unit tests for SWE and the CDL parser
 */
namespace swe_tests
{
    class SWECDLTestsSuite;
}

/**
 * @brief Implements several tests for the CDL parser
 */
class swe_tests::SWECDLTestsSuite : public CxxTest::TestSuite
{
   
    public:

        /**
         * @test Verify parsed CDL string equals a NetCDF data object in memory
         */
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
        
            map<string, ICDLVariable*> vars;

            CDLVariable<int32_t>* latd = new CDLVariable<int32_t>({0, 10, 20, 30, 40, 50, 60, 70, 80, 90}); 
            vars["lat"] = latd; vars["lat"]->assign(
                "int", false, "lat", { "lat" }, 
                { { "units", { name: "units", values: { "degrees_north" }  } } }
            );
            CDLVariable<int32_t>* lond = new CDLVariable<int32_t>; vars["lon"] = lond; vars["lon"]->assign(
                "int", false, "lon", { "lon" }, 
                { { "units", { name: "units", values: { "degrees_east" } } } }
            );
            CDLVariable<int32_t>* timed = new CDLVariable<int32_t>; vars["time"] = timed; vars["time"]->assign(
                "int", false, "time", { "time" }, 
                { { "units", { name: "units", values: { "seconds" } } } }
            );
            CDLVariable<float>* zd = new CDLVariable<float>; vars["z"] = zd; vars["z"]->assign(
                "float", false, "z", { "time", "lat", "lon" }, 
                { { "valid_range", { name: "valid_range", values: { "0.", "5000." } } } }
            );
            CDLVariable<float>* td = new CDLVariable<float>; vars["t"] = td; vars["t"]->assign(
                "float", false, "t", { "lat", "lon" }, {}
            );

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

             // ## Test 2 (Parse valid file) ##
             std::ifstream file;
             file.open("testfile.cdl", std::ifstream::in);

             //Print warning if file could not be openend and fail
             if(!file.is_open())
                 TS_WARN("Stream could not read file in TestStringParse: Test 2: \t");
             TS_ASSERT(file.is_open());

             //Compare
             //TODO: Check interface between Parser and Test
             CDLData newdata;
             //TODO: Parse data
             //TODO: Assert data against reference
             file.close();

             // ## Test 3 (Parse invalid file) ##
             //TODO: Implement
        }

};
