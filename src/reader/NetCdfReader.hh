#ifndef NETCDFREADER_HH_
#define NETCDFREADER_HH_

#include <cstring>
#include <string>
#include <vector>
#ifdef USEMPI
#include <mpi.h>
#ifndef MPI_INCLUDED
#define MPI_INCLUDED
#define MPI_INCLUDED_NETCDF
#endif
#endif
#include <netcdf.h>
#ifdef MPI_INCLUDED_NETCDF
#undef MPI_INCLUDED
#undef MPI_INCLUDED_NETCDF
#endif

#include "writer/Writer.hh"

using namespace std;

namespace io 
{

  class NetCdfReader;

}

class io::NetCdfReader  
{

private:

    int dataFile;
    int timeVar, xVar, yVar, bVar, hVar, huVar, hvVar, timeDim, xDim, yDim;

  public:

    size_t timeLength, xLength, yLength;
    float* bData; float* hData; float* huData; float* hvData;
    float xMin, xMax, yMin, yMax, timeMax;

    NetCdfReader(const string &i_fileName);
    string getGlobalTextAttribute(const string& name);
    int getGlobalIntAttribute(const string& name);
    int* getGlobalIntPtrAttribute(const string& name, int length);
    float getGlobalFloatAttribute(const string& name);
    ~NetCdfReader();

};

#endif /* NETCDFREADER_HH_ */
