/**
 * @file SWE_DimensionalSplittingBlock.hh
 * @brief Defines a system for dimensional splitting
 */

#ifndef _SWE_DIMENSIONAL_SPLITTING_HPP
#define _SWE_DIMENSIONAL_SPLITTING_HPP

#include "blocks/SWE_Block.hh"
#include "tools/help.hh"

#include <string>

#define SUPPRESS_SOLVER_DEBUG_OUTPUT
// #define SOLVER_DEBUG_OUTPUT_ONLYNONZERO

#include "../submodules/solvers/src/solver/FWave.hpp"

/**
 * @brief This is an implementation of the SWE_Block abstract class.
 *  It uses a f-wave solver.
 */
class SWE_DimensionalSplittingBlock : public SWE_Block 
{
    
private:

    //! The actual f-wave solver itself
    solver::FWave<float> wavePropagationSolver;

    //! Net-updates for the heights of the cells on the left sides of the vertical edges.
    Float2D hNetUpdatesLeft;
    //! Net-updates for the heights of the cells on the right sides of the vertical edges.
    Float2D hNetUpdatesRight;

    //! Net-updates for the x-momentums of the cells on the left sides of the vertical edges.
    Float2D huNetUpdatesLeft;
    //! Net-updates for the x-momentums of the cells on the right sides of the vertical edges.
    Float2D huNetUpdatesRight;

    //! Net-updates for the heights of the cells below the horizontal edges.
    Float2D hNetUpdatesBelow;
    //! Net-updates for the heights of the cells above the horizontal edges.
    Float2D hNetUpdatesAbove;

    //! Net-updates for the y-momentums of the cells below the horizontal edges.
    Float2D hvNetUpdatesBelow;
    //! Net-updates for the y-momentums of the cells above the horizontal edges.
    Float2D hvNetUpdatesAbove;

    //! Amount of threads used
    int numThreads;
    //! Amount ofrows/cols per thread
    int workPerThread_horizontal, workPerThread_vertical;

    /**
     * @brief Converts small values to zero
     * 
     * Sets small values of h, hv and hu to zero
     * This is not used currently
     */
    void zeroSmallValues();

  public:

    /**
     * @brief Constructor
     * 
     * @param l_nx Amount of cells in x dimension
     * @param l_ny Amount of cells in y dimension
     * @param l_dx Width of a cell
     * @param l_dy Height of a cell
     * @param numthreads The amount of parallel threads to use
     */
    SWE_DimensionalSplittingBlock(int l_nx, int l_ny, float l_dx, float l_dy, int numthreads);

    /**
     * @brief Computes the horizontal fluxes
     */
    float computeNumericalFluxesHorizontal();

    /**
     * @brief Computes the vertical fluxes
     */
    float computeNumericalFluxesVertical();

    /**
     * @brief Computes all fluxes
     * 
     * Compute net updates for the block.
     * The member variable maxTimestep will be updated with the maximum allowed time step size
     */
    void computeNumericalFluxes();

    /**
     * @brief Computes the maximum timestep
     * 
     * The member variable maxTimestep will be updated with the approxmated maximum allowed time step size
     * 
     * @param max Maximum wave speed
     * @param is_horizontal Wether the speed is horizontal or not
     */
    void computeMaxTimestep(float max, bool is_horizontal);

    /**
     * @brief Updates the cells horizontal values
     * 
     * @param dt delta time
     */
    void updateUnknownsHorizontal(float dt);

     /**
     * @brief Updates the cells horizontal values
     * 
     * @param dt delta time
     */
    void updateUnknownsVertical(float dt);

    /**
     * @brief Updates all the cells values
     * 
     * @param dt delta time
     */
    void updateUnknowns(float dt);

    /**
     * @brief Destructor
     *
     * In the case of a hybrid solver (NDEBUG not defined) information about the used solvers will be printed.
     */
    virtual ~SWE_DimensionalSplittingBlock() {}

};

#endif