/**
 * @file NetCdfWriter.cpp
 * @brief Implements the functionality from NetCdfWriter.hh
 */

#include "NetCdfWriter.hh"
#include <string>
#include <vector>
#include <iostream>
#include <cassert>
#include <sstream>

io::NetCdfWriter::NetCdfWriter(const std::string &i_baseName,
	const std::string &i_filebaseName,
	const Float2D &i_b,
	const BoundarySize &i_boundarySize,
	int i_nX, int i_nY,
	float i_dX, float i_dY,
	int* i_outConditions,
	float i_timeDuration,
	int i_checkpoints,
	float i_originX, float i_originY,
	size_t timestep,
	const bool append,
	const unsigned int i_flush,
	const bool ischeckpoint,
	const int outscale) 
	//const bool  &i_dynamicBathymetry : //!TODO
		: flush(i_flush),
		  is_checkpoint(ischeckpoint),
		  scale(outscale),
  		  io::Writer(i_baseName + ".nc", i_b, i_boundarySize, i_nX, i_nY, timestep)
{
	int status;
	
	if(append)
	{
		//open an existing file
		status = nc_open(fileName.c_str(), NC_WRITE, &dataFile);
		if (status != NC_NOERR)
		{
			assert(false);
			return;
		}
	}
	else
	{
		//create a netCDF-file, an existing file will be replaced
		status = nc_create(fileName.c_str(), NC_NETCDF4, &dataFile);
		if (status != NC_NOERR)
		{
			assert(false);
			return;
		}
	}

	//initialize coarse computation
	coarse = new CoarseComputation(scale, boundarySize, nX, nY);
	nx_a = coarse->newWidth;
	ny_a = coarse->newHeight;
	dx_a = i_dX * scale;
	dy_a = i_dY * scale;

	//dimensions
	int l_timeDim, l_xDim, l_yDim;
	if(append)
	{
		nc_inq_dimid(dataFile, "time", &l_timeDim);
		nc_inq_dimid(dataFile, "x", &l_xDim);
		nc_inq_dimid(dataFile, "x", &l_yDim);
	}
	else
	{
		nc_def_dim(dataFile, "time", NC_UNLIMITED, &l_timeDim);
		nc_def_dim(dataFile, "x", nx_a, &l_xDim);
		nc_def_dim(dataFile, "y", ny_a, &l_yDim);
	}

	//variables (TODO: add rest of CF-1.5)
	int l_xVar, l_yVar;
	if(append)
	{
		nc_inq_varid(dataFile, "time", &timeVar);
		nc_inq_varid(dataFile, "x", &l_xVar);
		nc_inq_varid(dataFile, "y", &l_yVar);
	}
	else
	{
		nc_def_var(dataFile, "time", NC_FLOAT, 1, &l_timeDim, &timeVar);
		ncPutAttText(timeVar, "long_name", "Time");
		ncPutAttText(timeVar, "units", "seconds since simulation start"); // the word "since" is important for the paraview reader
		nc_def_var(dataFile, "x", NC_FLOAT, 1, &l_xDim, &l_xVar);
		nc_def_var(dataFile, "y", NC_FLOAT, 1, &l_yDim, &l_yVar);
	}


	//variables, fastest changing index is on the right (C syntax), will be mirrored by the library
	int dims[] = {l_timeDim, l_yDim, l_xDim};
	if(append)
	{
		nc_inq_varid(dataFile, "h", &hVar);
		nc_inq_varid(dataFile, "hu", &huVar);
		nc_inq_varid(dataFile, "hv", &hvVar);
		nc_inq_varid(dataFile, "b", &bVar);
	}
	else
	{
		nc_def_var(dataFile, "h",  NC_FLOAT, 3, dims, &hVar);
		nc_def_var(dataFile, "hu", NC_FLOAT, 3, dims, &huVar);
		nc_def_var(dataFile, "hv", NC_FLOAT, 3, dims, &hvVar);
		nc_def_var(dataFile, "b",  NC_FLOAT, 2, &dims[1], &bVar);
	}
	
	if(!append)
	{
		//set attributes to match CF-1.5 convention
		ncPutAttText(NC_GLOBAL, "Conventions", "CF-1.5");
		ncPutAttText(NC_GLOBAL, "title", "Computed tsunami solution");
		ncPutAttText(NC_GLOBAL, "history", "SWE");
		ncPutAttText(NC_GLOBAL, "institution", "Technische Universitaet Muenchen, Department of Informatics, Chair of Scientific Computing");
		ncPutAttText(NC_GLOBAL, "source", "Bathymetry and displacement data.");
		ncPutAttText(NC_GLOBAL, "references", "http://www5.in.tum.de/SWE");
		ncPutAttText(NC_GLOBAL, "comment", "SWE is free software and licensed under the GNU General Public License. Remark: In general this does not hold for the used input data.");
	}

	//save checkpoint state
	ncPutAttText(NC_GLOBAL, "basename", i_filebaseName.c_str());
	nc_put_att_int(dataFile, NC_GLOBAL, "nx", NC_INT, 1, &nx_a);
	nc_put_att_int(dataFile, NC_GLOBAL, "ny", NC_INT, 1, &ny_a);
	nc_put_att_float(dataFile, NC_GLOBAL, "timeduration", NC_FLOAT, 1, &i_timeDuration);
	nc_put_att_int(dataFile, NC_GLOBAL, "checkpoints", NC_INT, 1, &i_checkpoints);
	nc_put_att_float(dataFile, NC_GLOBAL, "dx", NC_FLOAT, 1, &dx_a);
	nc_put_att_float(dataFile, NC_GLOBAL, "dy", NC_FLOAT, 1, &dy_a);
	nc_put_att_float(dataFile, NC_GLOBAL, "originx", NC_FLOAT, 1, &i_originX);
	nc_put_att_float(dataFile, NC_GLOBAL, "originy", NC_FLOAT, 1, &i_originY);
	nc_put_att_int(dataFile, NC_GLOBAL, "boundarysize", NC_INT, 4, boundarySize.boundarySize);
	nc_put_att_int(dataFile, NC_GLOBAL, "outconditions", NC_INT, 4, i_outConditions);

	if(!append)
	{
		//setup grid size
		float gridPosition = i_originX + (float).5 * dx_a;
		for(size_t i = 0; i < (size_t)nx_a; i++) {
			nc_put_var1_float(dataFile, l_xVar, &i, &gridPosition);
			gridPosition += dx_a;
		}

		gridPosition = i_originY + (float).5 * dy_a;
		for(size_t j = 0; j < (size_t)ny_a; j++) {
			nc_put_var1_float(dataFile, l_yVar, &j, &gridPosition);
			gridPosition += dy_a;
		}
	}

	nc_sync(dataFile);
}

io::NetCdfWriter::~NetCdfWriter() 
{
	nc_close(dataFile);
}

void io::NetCdfWriter::writeVarTimeDependent(const Float2D i_matrix, int i_ncVariable) 
{
	//boundary stripping is now done by coarse computer
	//storage in Float2D is col wise
	//read carefully, the dimensions are confusing
	size_t start[] = {timeStep, 0, 0};
	size_t count[] = {1, (size_t)ny_a, 1};
	for(unsigned int col = 0; col < (size_t)nx_a; col++) 
	{
		start[2] = col; //select col (dim "x")
		nc_put_vara_float(dataFile, i_ncVariable, start, count, &i_matrix[col][0]); //write col
  	}
}


void io::NetCdfWriter::writeVarTimeIndependent(const Float2D& i_matrix, int i_ncVariable)
{
	//boundary stripping is now done by coarse computer
	//storage in Float2D is col wise
	//read carefully, the dimensions are confusing
	size_t start[] = {0, 0};
	size_t count[] = {(size_t)ny_a, 1};
	for(unsigned int col = 0; col < (size_t)nx_a; col++) 
	{
		start[1] = col; //select col (dim "x")
		nc_put_vara_float(dataFile, i_ncVariable, start, count, &i_matrix[col][0]); //write col
  	}
}


void io::NetCdfWriter::writeTimeStep(const Float2D& i_h, const Float2D& i_hu,
	const Float2D& i_hv, float i_time) 
{
	// Write bathymetry
	if (timeStep == 0) writeVarTimeIndependent(b, bVar);

	//write i_time
	nc_put_var1_float(dataFile, timeVar, &timeStep, &i_time);

	//write water height
	coarse->updateAverages(i_h);
	writeVarTimeDependent(*(coarse->averages), hVar);

	//write momentum in x-direction
	coarse->updateAverages(i_hu);
	writeVarTimeDependent(*(coarse->averages), huVar);

	//write momentum in y-direction
	coarse->updateAverages(i_hv);
	writeVarTimeDependent(*(coarse->averages), hvVar);

	// Increment timeStep for next call
	timeStep++;

	if (flush > 0 && timeStep % flush == 0) nc_sync(dataFile);
}
