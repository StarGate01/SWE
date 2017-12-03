#include "NetCdfReader.hh"
#include <string>
#include <vector>
#include <iostream>
#include <cassert>

io::NetCdfReader::NetCdfReader(const std::string &i_baseName,
		const Float2D &i_b,
		const BoundarySize &i_boundarySize,
		int i_nX, int i_nY,
		float i_dX, float i_dY,
		float i_originX, float i_originY)
{

}

/**
 * Destructor of a netCDF-writer.
 */
io::NetCdfReader::~NetCdfReader() 
{
	nc_close(dataFile);
}

void io::NetCdfReader::getLatestTimeStep(const Float2D &i_h,
	const Float2D &i_hu, const Float2D &i_hv) 
{

}
