#include "NetCdfReader.hh"
#include <string>
#include <vector>
#include <iostream>
#include <cassert>

io::NetCdfReader::NetCdfReader(const std::string &i_fileName,
                 const BoundarySize &i_boundarySize,
                 int i_nX, int i_nY)
{
	int status;

	//create a netCDF-file, an existing file will be replaced
	status = nc_open(i_fileName.c_str(), NC_NOWRITE, &dataFile);

    //check if the netCDF-file open constructor succeeded.
	if (status != NC_NOERR) 
	{
		assert(false);
		return;
	}

	nc_inq_varid(dataFile, "h", &hVar);
	nc_inq_varid(dataFile, "hu", &huVar);
	nc_inq_varid(dataFile, "hv", &hvVar);
	nc_inq_varid(dataFile, "b", &bVar);
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
