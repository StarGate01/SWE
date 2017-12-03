#include "NetCdfDataReader.hh"
#include <string>
#include <vector>
#include <iostream>
#include <cassert>

io::NetCdfDataReader::NetCdfDataReader(const std::string& i_fileName)
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

	nc_inq_varid(dataFile, "x", &xVar);
	nc_inq_varid(dataFile, "y", &yVar);
	nc_inq_varid(dataFile, "z", &zVar);
	nc_inq_dimid(dataFile, "x", &xDim);
	nc_inq_dimlen(dataFile, xDim, &xLength);
	nc_inq_dimid(dataFile, "y", &yDim);
	nc_inq_dimlen(dataFile, yDim, &yLength);

	float xdata[xLength];
	nc_get_var_float(dataFile, xVar, xdata);
	xMin = xdata[0];
	xMax = xdata[xLength - 1];

	float ydata[yLength];
	nc_get_var_float(dataFile, yVar, ydata);
	yMin = ydata[0];
	yMax = ydata[yLength - 1];

	zData = new float[xLength * yLength];
	nc_get_var_float(dataFile, zVar, zData);
	zData2D = new Float2D(xLength, yLength, zData);

	nc_close(dataFile);
}

io::NetCdfDataReader::~NetCdfDataReader() 
{
	delete zData2D;
	delete[] zData;
}