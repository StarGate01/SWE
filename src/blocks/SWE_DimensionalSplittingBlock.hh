/**
 * TODO: Add documentation
*/

#ifndef _SWE_DIMENSIONAL_SPLITTING_HPP
#define _SWE_DIMENSIONAL_SPLITTING_HPP

#include "blocks/SWE_Block.hh"
#include "tools/help.hh"

#include <string>

#define SUPPRESS_SOLVER_DEBUG_OUTPUT
// #define SOLVER_DEBUG_OUTPUT_ONLYNONZERO

//which wave propagation solver should be used
//  0: Hybrid
//  1: f-Wave
//  2: Approximate Augmented Riemann solver
#if WAVE_PROPAGATION_SOLVER==0
#include "../submodules/solvers/src/solver/Hybrid.hpp"
#elif WAVE_PROPAGATION_SOLVER==1
#include "../submodules/solvers/src/solver/FWave.hpp"
#elif WAVE_PROPAGATION_SOLVER==2
#include "../submodules/solvers/src/solver/AugRie.hpp"
#else
#warning SWE_DimensionalSplittingBlock should only be used with Riemann solvers 0, 1, and 2 (FWave, AugRie or Hybrid)
#endif

/**
 * SWE_DimensionalSplittingBlock is an implementation of the SWE_Block abstract class.
 * It uses a wave propagation solver which is defined with the pre-compiler flag WAVE_PROPAGATION_SOLVER (see above).
 *
 * Possible wave propagation solvers are:
 *  F-Wave, Apprximate Augmented Riemann, Hybrid (f-wave + augmented).
 *  (details can be found in the corresponding source files)
 */
class SWE_DimensionalSplittingBlock : public SWE_Block 
{
    
private:

    solver::FWave<float> wavePropagationSolver;

    //! net-updates for the heights of the cells on the left sides of the vertical edges.
    Float2D hNetUpdatesLeft;
    //! net-updates for the heights of the cells on the right sides of the vertical edges.
    Float2D hNetUpdatesRight;

    //! net-updates for the x-momentums of the cells on the left sides of the vertical edges.
    Float2D huNetUpdatesLeft;
    //! net-updates for the x-momentums of the cells on the right sides of the vertical edges.
    Float2D huNetUpdatesRight;

    //! net-updates for the heights of the cells below the horizontal edges.
    Float2D hNetUpdatesBelow;
    //! net-updates for the heights of the cells above the horizontal edges.
    Float2D hNetUpdatesAbove;

    //! net-updates for the y-momentums of the cells below the horizontal edges.
    Float2D hvNetUpdatesBelow;
    //! net-updates for the y-momentums of the cells above the horizontal edges.
    Float2D hvNetUpdatesAbove;

    void zeroSmallValues();

  public:

    //constructor of a SWE_DimensionalSplittingBlock.
    SWE_DimensionalSplittingBlock(int l_nx, int l_ny, float l_dx, float l_dy);

    //computes the net-updates for the block
    float computeNumericalFluxesHorizontal();
    float computeNumericalFluxesVertical();
    void computeNumericalFluxes();

    //compute the maximum time step
    void computeMaxTimestep(float max, bool is_horizontal);

    //update the cells
    void updateUnknownsHorizontal(float dt);
    void updateUnknownsVertical(float dt);
    void updateUnknowns(float dt);

    /**
     * Destructor of a SWE_DimensionalSplittingBlock.
     *
     * In the case of a hybrid solver (NDEBUG not defined) information about the used solvers will be printed.
     */
    virtual ~SWE_DimensionalSplittingBlock() {}

};

#endif