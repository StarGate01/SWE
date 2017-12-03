#include "NetCdfDataReader.hh"
#include <string>
#include <cassert>
#include <cmath>

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

	nc_close(dataFile);
}

io::NetCdfDataReader::~NetCdfDataReader() 
{
	delete[] zData;
}

float io::NetCdfDataReader::sample(float x, float y, bool extend, float fallback)
{
	long int xindex = round(((x - xMin) / (xMax - xMin)) * xLength);
	long int yindex = round(((y - yMin) / (yMax - yMin)) * yLength);
	if(extend)
	{
		if(xindex < 0) xindex = 0;
        if(xindex > (long int)(xLength - 1)) xindex = xLength - 1;
        if(yindex < 0) yindex = 0;
        if(yindex > (long int)(yLength - 1)) yindex = yLength - 1;
	}
	else
	{
		if (xindex < 0 || xindex > (long int)(xLength - 1) 
			|| yindex < 0 || yindex > (long int)(yLength - 1)) return fallback;
	}
	return zData[(yindex * xLength) + xindex];
}