#include "NetCdfReader.hh"
#include <string>
#include <vector>
#include <iostream>
#include <cassert>

io::NetCdfReader::NetCdfReader(const std::string &i_fileName)
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

	nc_inq_varid(dataFile, "time", &timeVar);
	nc_inq_dimid(dataFile, "time", &timeDim);
	nc_inq_dimlen(dataFile, timeDim, &timeLength);

	nc_inq_varid(dataFile, "x", &xVar);
	nc_inq_dimid(dataFile, "x", &xDim);
	nc_inq_dimlen(dataFile, xDim, &xLength);

	nc_inq_varid(dataFile, "y", &yVar);
	nc_inq_dimid(dataFile, "y", &yDim);
	nc_inq_dimlen(dataFile, yDim, &yLength);

	nc_inq_varid(dataFile, "b", &bVar);
	nc_inq_varid(dataFile, "h", &hVar);
	nc_inq_varid(dataFile, "hu", &huVar);
	nc_inq_varid(dataFile, "hv", &hvVar);
}

/**
 * Destructor of a netCDF-writer.
 */
io::NetCdfReader::~NetCdfReader() 
{
	nc_close(dataFile);
}

// void io::NetCdfReader::getLatestTimeStep(const Float2D &i_h,
// 	const Float2D &i_hu, const Float2D &i_hv) 
// {
// 	int timeVarId;
// 	//länge = nX * nY
// 	int last_timestep;
// 	nc_get_att_int(dataFile, timeVarId, "timestep", &last_timestep);

// 	//Get dim ids
// 	int h_id, hu_id, hv_id, b_id;
// 	nc_inq_dimid(dataFile, "h", &h_id);
// 	nc_inq_dimid(dataFile, "hu", &hu_id);
// 	nc_inq_dimid(dataFile, "hv", &hv_id);
// 	nc_inq_dimid(dataFile, "b", &b_id);
	
// 	//start = (anzahl - 1) * länge
// 	// nc_get_var_float , besser nc_get_vara_float
// 	//nc_get_vara_float(dataFile, )
// }
