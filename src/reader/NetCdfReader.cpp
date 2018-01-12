/**
 * @file NetCdfReader.cpp
 * @brief Implements the functionality defined in NetCdfReader.hh
 */

#include "NetCdfReader.hh"
#include <string>
#include <vector>
#include <iostream>
#include <cassert>

using namespace std;

io::NetCdfReader::NetCdfReader(const string &i_fileName, bool noassert)
{
	int status;

	//create a netCDF-file, an existing file will be replaced
	status = nc_open(i_fileName.c_str(), NC_NOWRITE, &dataFile);

    //check if the netCDF-file open constructor succeeded.
	if (status != NC_NOERR) 
	{
		if(!noassert) assert(false);
		return;
	}
	success = true;

	nc_inq_varid(dataFile, "time", &timeVar);
	nc_inq_dimid(dataFile, "time", &timeDim);
	nc_inq_dimlen(dataFile, timeDim, &timeLength);
	float timeData[timeLength];
	nc_get_var_float(dataFile, timeVar, timeData);
	timeMax = timeData[timeLength - 1];

	nc_inq_varid(dataFile, "x", &xVar);
	nc_inq_dimid(dataFile, "x", &xDim);
	nc_inq_dimlen(dataFile, xDim, &xLength);
	float xdata[xLength];
	nc_get_var_float(dataFile, xVar, xdata);
	xMin = xdata[0];
	xMax = xdata[xLength - 1];

	nc_inq_varid(dataFile, "y", &yVar);
	nc_inq_dimid(dataFile, "y", &yDim);
	nc_inq_dimlen(dataFile, yDim, &yLength);
	float ydata[yLength];
	nc_get_var_float(dataFile, yVar, ydata);
	yMin = ydata[0];
	yMax = ydata[yLength - 1];

	nc_inq_varid(dataFile, "b", &bVar);
	bData = new float[xLength * yLength];
	nc_get_var_float(dataFile, bVar, bData);

	size_t hs_indices[3] = {(timeLength - 1), 0, 0};
	size_t hs_counts[3] = {1, yLength, xLength};

	nc_inq_varid(dataFile, "h", &hVar);
	hData = new float[xLength * yLength];
	nc_get_vara_float(dataFile, hVar, hs_indices, hs_counts, hData);

	nc_inq_varid(dataFile, "hu", &huVar);
	huData = new float[xLength * yLength];
	nc_get_vara_float(dataFile, huVar, hs_indices, hs_counts, huData);

	nc_inq_varid(dataFile, "hv", &hvVar);
	hvData = new float[xLength * yLength];
	nc_get_vara_float(dataFile, hvVar, hs_indices, hs_counts, hvData);
}

io::NetCdfReader::~NetCdfReader() 
{
	delete[] bData;
	delete[] hData;
	delete[] huData;
	delete[] hvData;
	nc_close(dataFile);
}

string io::NetCdfReader::getGlobalTextAttribute(const string& name)
{
	size_t slen;
	nc_inq_attlen(dataFile, NC_GLOBAL, name.c_str(), &slen);
	char sval[slen+1];
	nc_get_att(dataFile, NC_GLOBAL, name.c_str(), &sval);
	sval[slen] = '\0';
	return string(sval);
}

int io::NetCdfReader::getGlobalIntAttribute(const string& name)
{
	int32_t val;
	nc_get_att_int(dataFile, NC_GLOBAL, name.c_str(), &val);
	return (int)val;
}

int32_t* io::NetCdfReader::getGlobalIntPtrAttribute(const string& name, int length)
{
	int32_t* val = new int32_t[length];
	nc_get_att_int(dataFile, NC_GLOBAL, name.c_str(), val);
	return val;
}

float io::NetCdfReader::getGlobalFloatAttribute(const string& name)
{
	float val;
	nc_get_att_float(dataFile, NC_GLOBAL, name.c_str(), &val);
	return val;
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
