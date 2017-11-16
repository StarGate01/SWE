/**
 * TODO: Add documentation
*/

#ifndef _SWE_DIMENSIONAL_SPLITTING_HPP
#define _SWE_DIMENSIONAL_SPLITTING_HPP

#include "blocks/SWE_Block.hh"

class SWE_DimensionalSplitting : public SWE_Block {
    
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

  public:
    //constructor of a SWE_DimensionalSplitting.
    SWE_DimensionalSplitting(int l_nx, int l_ny,
    					float l_dx, float l_dy);

    //computes the net-updates for the block
    void computeNumericalFluxes();

    //update the cells
    void updateUnknowns(float dt);
    void updateUnknownsRow(float dt, int i);

    /**
     * Destructor of a SWE_DimensionalSplitting.
     *
     * In the case of a hybrid solver (NDEBUG not defined) information about the used solvers will be printed.
     */
    virtual ~SWE_DimensionalSplitting() {}
};

#endif