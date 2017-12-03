#include "NetCdfDataReader.hh"
#include <string>
#include <vector>
#include <iostream>
#include <cassert>

io::NetCdfDataReader::NetCdfDataReader(const std::string &i_fileName)
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

	nc_inq_varid(dataFile, "z", &zVar);
	nc_inq_dimid(dataFile, "x", &xDim);
	nc_inq_dimlen(dataFile, xDim, &xLength);
	nc_inq_dimid(dataFile, "y", &yDim);
	nc_inq_dimlen(dataFile, yDim, &yLength);
}

/**
 * Destructor of a netCDFdata-reader.
 */
io::NetCdfDataReader::~NetCdfDataReader() 
{
	nc_close(dataFile);
}

void getZValues(const Float2D &i_z)
{
	float zdata[xLength][yLength];
};