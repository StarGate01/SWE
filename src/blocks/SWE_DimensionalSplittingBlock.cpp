/**
 * @file SWE_DimensionalSplittingBlock.cpp
 * @brief Implements the functionality defined in DimensionalSplittingBlock.hh
 */

#include "SWE_DimensionalSplittingBlock.hh"

#include <cassert>
#include <string>
#include <limits>

#ifdef USE_OMP
#include <omp.h>
#endif


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

float SWE_DimensionalSplittingBlock::computeNumericalFluxesVertical()
{
	float maxWaveSpeed = (float) 0.;
#ifdef USE_OMP
	#pragma omp parallel for reduction(max: maxWaveSpeed)
#endif
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

float SWE_DimensionalSplittingBlock::computeNumericalFluxesHorizontal()
{
	float maxWaveSpeed = (float) 0.;
#ifdef USE_OMP
	#pragma omp parallel for reduction(max: maxWaveSpeed)
#endif
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

void SWE_DimensionalSplittingBlock::computeNumericalFluxes()
{
	float maxWaveSpeedHorizontal = computeNumericalFluxesHorizontal();
	float maxWaveSpeedVertical = computeNumericalFluxesVertical();
	computeMaxTimestep(std::max(maxWaveSpeedVertical, maxWaveSpeedHorizontal), dx < dy);
}

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