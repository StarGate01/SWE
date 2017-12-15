#include <cxxtest/TestSuite.h>

#include "tools/help.hh"                    //Float1D, Float2D
#include "../CoarseComputation.hh"          //BoundarySize
include "../submodule/solver/FConst.hpp"    //ZERO_PRECISION

namespace swe_tests
{
    class SWECoarseTests;
}

class swe_tests::SWECoarseTests : public Cxxtest::TestSuite
{
    public:
        void testAverageCalculation()
        {
            // ### Test 1 (Scale by 2) ###
            Float2D field(20, 20);

            //Initialize field with chess pattern of zeros and twos
            for(int y = 0; y < field.getRows(); y++)
            {
                for(int x = 0; x < field.getCols(); x++)
                {
                    field[x][y] = (x % 2 == 0 && y % 2 == 0) ? 2 : 0;
                }
            }

            //Initialize CoarseComputation
            int scale = 2;
            io::BoundarySize bs();
            for(i = 0; i < 4; i++)
                bs[i] = 0;
            CoarseComputation cc(scale, bs, field.getCols(), field.getRows());

            //Execute average calculation
            cc::updateAverages(field);
            
            //Assert dimensions
            TS_ASSERT(field.getCols() == 10);
            TS_ASSERT(field.getRows() == 10);

            //Assert averages
            for(int y = 0; y < field.getRows(); y++)
            {
                for(int x = 0; x < field.getCols(); x++)
                {
                    TS_ASSERT_DELTA(field[x][y], 1, ZERO_PRECISION);
                }
            }

            // ### Test 2 (Scale 1) ###
            //Init test
            scale = 1;
            Float2D comp = field;
            cc(scale, bs, field.getCols(), field.getRows());
            
            //Execute average calculation
            cc::updateAverages(field);

            //Assert dimensions
            TS_ASSERT_EQUALS(field.getCols(), comp.getCols());
            TS_ASSERT_EQUALS(field.getRows(), comp.getRows());

            //Assert values did not change
            for(int y = 0; y < field.getRows(); y++)
            {
                for(int x = 0; x < field.getCols(); x++)
                {
                    TS_ASSERT_EQUALS(field[x][y], comp[x][y]);
                }
            }

            // ### Test 3 (Invalid case) ###
            comp = field;
            scale = -3
            cc(scale, bs, field.getCols(), field.getRows);

            //Execute average calculation
            cc.updateAverages(field);

            //Assert dimensions
            TS_ASSERT_EQUALS(field.getCols(), comp.getCols());
            TS_ASSERT_EQUALS(field.getRows(), comp.getRows());

            //Assert values did not change
            for(int y = 0; y < field.getRows(); y++)
            {
                for(int x = 0; x < field.getCols(); x++)
                {
                    TS_ASSERT_EQUALS(field[x][y], comp[x][y]);
                }
            }
        }
}