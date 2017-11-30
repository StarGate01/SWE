/**
 * @file SWECDLTests.t.h
 * @brief Unit tests for the CDL parser
 */

#include <cxxtest/TestSuite.h>
#include "../parser/CDLStreamParser.hh"
#include "../parser/CDLData.hh"

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
            // ## Test 1 (valid string) ##
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
                variables: vars
            };

            CDLData newdata;

            CDLStreamParser::CDLStringToData(cdltext, newdata);

            TS_ASSERT(newdata == cdldata);

            // ## Test 2 (invalid string) ##
            cdltext = R"(
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
                invalid token
            )";

            //Expect this operation to fail
            bool exceptionCatched = false;
            try{
                CDLStreamParser::CDLStringToData(cdltext, newdata);
            }catch(exception& e)
            {
                exceptionCatched = true;
            }
            TS_ASSERT(exceptionCatched);
        }
};
