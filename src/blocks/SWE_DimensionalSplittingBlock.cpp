#include "SWE_DimensionalSplittingBlock.hh"

#include <cassert>
#include <string>
#include <limits>


SWE_DimensionalSplittingBlock::SWE_DimensionalSplittingBlock (int l_nx, int l_ny, float l_dx, float l_dy) :
	SWE_Block (l_nx, l_ny, l_dx, l_dy),
	hNetUpdatesLeft (nx + 1, ny),
	hNetUpdatesRight (nx + 1, ny),
	huNetUpdatesLeft (nx + 1, ny),
	huNetUpdatesRight (nx + 1, ny),

	hNetUpdatesBelow (nx, ny + 1),
	hNetUpdatesAbove (nx, ny + 1),
	hvNetUpdatesBelow (nx, ny + 1),
	hvNetUpdatesAbove (nx, ny + 1)
{
}

/**
 * Compute vertical net updates for the block.
 */
float SWE_DimensionalSplittingBlock::computeNumericalFluxesVertical()
{
	float maxWaveSpeed = (float) 0.;
	for (int i = 1; i < nx + 1; i++) 
	{
		for (int j = 1; j < ny + 2; j++) 
		{
			float maxEdgeSpeed;
			wavePropagationSolver.computeNetUpdates(
				h[i][j - 1], h[i][j], hv[i][j - 1], hv[i][j], b[i][j - 1], b[i][j],
				hNetUpdatesBelow[i - 1][j - 1], hNetUpdatesAbove[i - 1][j - 1],
				hvNetUpdatesBelow[i - 1][j - 1], hvNetUpdatesAbove[i - 1][j - 1],
				maxEdgeSpeed
			);
			maxWaveSpeed = std::max (maxWaveSpeed, maxEdgeSpeed);
		}
	}
	return maxWaveSpeed;
}

/**
 * Compute horizontal net updates for the block.
 */
float SWE_DimensionalSplittingBlock::computeNumericalFluxesHorizontal()
{
	float maxWaveSpeed = (float) 0.;
	for (int i = 1; i < nx + 2; i++) 
	{
		for (int j = 1; j < ny + 1; ++j) 
		{
			float maxEdgeSpeed;
			wavePropagationSolver.computeNetUpdates(
				h[i - 1][j], h[i][j], hu[i - 1][j], hu[i][j], b[i - 1][j], b[i][j],
				hNetUpdatesLeft[i - 1][j - 1], hNetUpdatesRight[i - 1][j - 1],
				huNetUpdatesLeft[i - 1][j - 1], huNetUpdatesRight[i - 1][j - 1],
				maxEdgeSpeed
			);
			maxWaveSpeed = std::max(maxWaveSpeed, maxEdgeSpeed);
		}
	}
	return maxWaveSpeed;
}

/**
 * Compute net updates for the block.
 * The member variable #maxTimestep will be updated with the 
 * maximum allowed time step size
 */
void SWE_DimensionalSplittingBlock::computeNumericalFluxes()
{
	float maxWaveSpeedHorizontal = computeNumericalFluxesHorizontal();
	float maxWaveSpeedVertical = computeNumericalFluxesVertical();
	computeMaxTimestep(std::max(maxWaveSpeedVertical, maxWaveSpeedHorizontal), dx < dy);
}

/*
 * The member variable #maxTimestep will be updated with the approxmated maximum allowed time step size
 */
void SWE_DimensionalSplittingBlock::computeMaxTimestep(float max, bool is_horizontal)
{
	if (max > 0.00001)
	{
		//TODO zeroTol
		//compute the time step width
		//CFL-Codition
		//(max. wave speed) * dt / dx < .5
		// => dt = .5 * dx/(max wave speed)
		if(is_horizontal)
		{
			maxTimestep = dx / max;
			maxTimestep *= (float) .4; //CFL-number = .5
		}
		else
		{
			maxTimestep = dy / max;
			maxTimestep *= (float) .4; //CFL-number = .5
		}
	} 
	else
	{
		//might happen in dry cells
		maxTimestep = std::numeric_limits<float>::max ();
	}
}

/**
 * Updates the unknowns horizontally with the already computed net-updates.
 *
 * @param dt time step width used in the update.
 */
void SWE_DimensionalSplittingBlock::updateUnknownsHorizontal(float dt)
{
	//update cell averages with the net-updates
	for (int i = 1; i < nx + 1; i++)
	{
		for (int j = 1; j < ny + 1; j++)
		{
			h[i][j] -= dt / dx * (hNetUpdatesRight[i - 1][j - 1] + hNetUpdatesLeft[i][j - 1]);
			hu[i][j] -= dt / dx * (huNetUpdatesRight[i - 1][j - 1] + huNetUpdatesLeft[i][j - 1]);
		}
	}
	//zeroSmallValues();
}

/**
 * Updates the unknowns vertically with the already computed net-updates.
 *
 * @param dt time step width used in the update.
 */
void SWE_DimensionalSplittingBlock::updateUnknownsVertical(float dt)
{
	//update cell averages with the net-updates
	for (int i = 1; i < nx + 1; i++)
	{
		for (int j = 1; j < ny + 1; j++)
		{
			h[i][j] -= dt / dy * (hNetUpdatesAbove[i - 1][j - 1] + hNetUpdatesBelow[i - 1][j]);
			hv[i][j] -= dt / dy * (hvNetUpdatesAbove[i - 1][j - 1] + hvNetUpdatesBelow[i - 1][j]);
		}
	}
	//zeroSmallValues();
}

/**
 * Updates the unknowns with the already computed net-updates.
 *
 * @param dt time step width used in the update.
 */
void SWE_DimensionalSplittingBlock::updateUnknowns(float dt)
{
	//update cell averages with the net-updates
	for (int i = 1; i < nx + 1; i++)
	{
		for (int j = 1; j < ny + 1; j++)
		{
			h[i][j] -= dt / dx * (hNetUpdatesRight[i - 1][j - 1] + hNetUpdatesLeft[i][j - 1]) + dt / dy * (hNetUpdatesAbove[i - 1][j - 1] + hNetUpdatesBelow[i - 1][j]);
			hu[i][j] -= dt / dx * (huNetUpdatesRight[i - 1][j - 1] + huNetUpdatesLeft[i][j - 1]);
			hv[i][j] -= dt / dy * (hvNetUpdatesAbove[i - 1][j - 1] + hvNetUpdatesBelow[i - 1][j]);
		}
	}
	//zeroSmallValues();
}

/**
 * Sets small values of h, hv and hu to zero
 */
void SWE_DimensionalSplittingBlock::zeroSmallValues()
{
	for (int i = 1; i < nx + 1; i++)
	{
		for (int j = 1; j < ny + 1; j++) 
		{
			if (h[i][j] < 0)
			{
				//TODO: dryTol
#ifndef NDEBUG
				// Only print this warning when debug is enabled
				// Otherwise we cannot vectorize this loop
				if (h[i][j] - b[i][j] < -0.1) 
				{
					std::cerr << "Warning, negative height-bathy: (i,j)=(" << i << "," << j << ")=" << h[i][j] << "-" << b[i][j] << std::endl;
				}
#endif
				//zero (small) negative depths
				h[i][j] = hu[i][j] = hv[i][j] = 0.;
			} 
			else if (h[i][j] < 0.1)
			{
				hu[i][j] = hv[i][j] = 0.; //no water, no speed!
			}
		}
	}
}