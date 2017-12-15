#include <cxxtest/TestSuite.h>
#include "tools/help.hh"                                        //Float1D, Float2D
#include "../writer/CoarseComputation.hh"                       //BoundarySize
#include "../../submodules/solvers/src/solver/FConst.hpp"       //ZERO_PRECISION

using namespace std;
using namespace io;

namespace swe_tests
{
    class SWECoarseTests;
}

class swe_tests::SWECoarseTests : public CxxTest::TestSuite
{
    private:
        float createChessPattern(int x, int y)
        {
            return (x % 2 == 0 && y % 2 == 0) ? 2 : (x % 2 == 1 && y % 2 == 1 ? 2 : 0);
        }

    public:

        /**
         * @test Test coarse computation
         */
        void testAverageCalculation()
        {
            // ### Test 1 (Scale by 2) ###
            Float2D field(20, 20);

            //Initialize field with chess pattern of zeros and twos
            for(int y = 0; y < field.getRows(); y++)
            {
                for(int x = 0; x < field.getCols(); x++)
                {
                    field[x][y] = createChessPattern(x, y);
                }
            }

            //Initialize CoarseComputation
            int scale = 2;
            io::BoundarySize bs;
            for(int i = 0; i < 4; i++)
                bs[i] = 0;
            CoarseComputation* cc = new CoarseComputation(scale, bs, field.getCols(), field.getRows());

            //Execute average calculation
            cc->updateAverages(field);

            //Assert dimensions
            TS_ASSERT(cc->averages->getCols() == 10);
            TS_ASSERT(cc->averages->getRows() == 10);

            //Assert averages
            for(int y = 0; y < cc->averages->getRows(); y++)
            {
                for(int x = 0; x < cc->averages->getCols(); x++)
                {
                    TS_ASSERT_DELTA((*(cc->averages))[x][y], 1, ZERO_PRECISION);
                }
            }

            // ### Test 2 (Scale 1) ###
            //Init test
            scale = 1;
            cc = new CoarseComputation(scale, bs, field.getCols(), field.getRows());
            
            //Execute average calculation
            cc->updateAverages(field);

            //Assert dimensions
            TS_ASSERT_EQUALS(field.getCols(), 20);
            TS_ASSERT_EQUALS(field.getRows(), 20);

            //Assert values did not change
            for(int y = 0; y < field.getRows(); y++)
            {
                for(int x = 0; x < field.getCols(); x++)
                {
                    TS_ASSERT_EQUALS(field[x][y], createChessPattern(x, y));
                }
            }

            // ### Test 3 (Invalid case) ###
            scale = -3;
            cc = new CoarseComputation(scale, bs, field.getCols(), field.getRows());

            //Execute average calculation
            cc->updateAverages(field);

            //Assert dimensions
            TS_ASSERT_EQUALS(field.getCols(), 20);
            TS_ASSERT_EQUALS(field.getRows(), 20);

            //Assert values did not change
            for(int y = 0; y < field.getRows(); y++)
            {
                for(int x = 0; x < field.getCols(); x++)
                {
                    TS_ASSERT_EQUALS(field[x][y], createChessPattern(x, y));
                }
            }
        }
};