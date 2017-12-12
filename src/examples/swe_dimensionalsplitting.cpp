/**
 * @file swe_dimensionalsplitting.cpp
 * @brief Main entry point for our version of SWE
 */

#include <cassert>
#include <cstdlib>
#include <string>
#include <iostream>
#include <thread>

#ifdef USE_OMP
#include <omp.h>
#endif

//Use these macros to select x, y or both dimensions for splitting
//USeful for demonstating the individual dimensions
#define DIMSPLIT_SELECT_X 1
#define DIMSPLIT_SELECT_Y 2
#define DIMSPLIT_SELECT_XY 4
#define DIMSPLIT_SELECT DIMSPLIT_SELECT_XY

#ifndef CUDA
#include "blocks/SWE_DimensionalSplittingBlock.hh"
#else
#include "blocks/cuda/SWE_DimensionalSplittingBlock.hh"
#endif

#ifdef WRITENETCDF
#include "writer/NetCdfWriter.hh"
#else
#include "writer/VtkWriter.hh"
#endif

#ifdef ASAGI
#include "scenarios/SWE_AsagiScenario.hh"
#else
#include "parser/CDLStreamParser.hh"
#include "scenarios/SWE_simple_scenarios.hh"
#include "scenarios/SWE_TsunamiScenario.hh"
#include "scenarios/SWE_ArtificialTsunamiScenario.hh"
#endif

#ifdef READXML
#include "tools/CXMLConfig.hpp"
#endif

#include "tools/args.hh"
#include "tools/help.hh"
#include "tools/Logger.hh"
#include "tools/ProgressBar.hh"


using namespace parser;

/**
 * @brief Adds an argument to the list of possible command line arguments
 * 
 * @param args The argument list
 * @param name The name
 * @param shortOption The flag name
 * @param description The long name
 * @param required Wether  this argument is required
 */
void addArgument(tools::Args& args, string name, 
  char shortOption, string description, bool required = false)
{
  if(required) args.addOption(name, shortOption, description);
  else args.addOption(name, shortOption, description, tools::Args::Argument::Optional, false);
}

/**
 * @brief Main program for the simulation on a single SWE_DimensionalSplittingBlock
 * 
 * @param argc Argument count
 * @param argv Argument buffer
 * 
 * @return The exit code
 */
int main(int argc, char** argv) 
{

  tools::Logger::logger.printString("\nThis is swe_dimensionalsplitting, using SWE_DimensionalSplittingBlock\n");

  // Parse command line parameters
  tools::Args args;
  
#ifndef READXML
  addArgument(args, "grid-size-x", 'x', "Number of cells in x direction");
  addArgument(args, "grid-size-y", 'y', "Number of cells in y direction");
  addArgument(args, "input-bathymetry", 'a', "Input bathymetry file name");
  addArgument(args, "input-displacement", 'c', "Input displacement file name");
  addArgument(args, "time-duration", 'd', "Time duration");
  addArgument(args, "checkpoint-amount", 'p', "Amount of checkpoints");
  addArgument(args, "boundary-condition-left", 'l', "Boundary condition left");
  addArgument(args, "boundary-condition-right", 'r', "Boundary condition right");
  addArgument(args, "boundary-condition-top", 't', "Boundary condition top");
  addArgument(args, "boundary-condition-bottom", 'b', "Boundary condition bottom");
  addArgument(args, "output-basepath", 'o', "Output base file name");
  addArgument(args, "input-checkpoint", 'm', "Input checkpoint file name");
  addArgument(args, "simulate-failure", 'f', "Simulate failure after n timesteps");
  addArgument(args, "output-scale", 's', "Scale for the output file cell sizes");
#ifdef USE_OMP
  addArgument(args, "limit-cores", 'z', "Maximum number of CPU cores used");
#endif
#endif
  tools::Args::Result ret = args.parse(argc, argv);

  switch (ret)
  {
    case tools::Args::Error: return 1;
    case tools::Args::Help: return 0;
    default: break;
  }

  bool isCheckpoint = false; 
  //number of grid cells in x- and y-direction.
  int l_nX, l_nY;
  //input file paths
  std::string l_ifile_baty, l_ifile_disp, l_ifile_checkp;
  //other parameters
  int l_time_dur;
  //number of checkpoints for visualization (at each checkpoint in time, an output file is written).
  int l_checkpoints;
  int l_timestep = 0;
  float l_timepos = 0.0;
  int l_failure = -1;
  int l_output_scale = 1;
  //boundary conditions
  BoundaryType* l_bound_types = new BoundaryType[4]; 
  //l_baseName of the plots.
  std::string l_baseName;

  //netcdf checkpoint reader
  io::NetCdfReader* checkp_reader;

  //read command line parameters
#ifndef READXML
  std::stringstream sstm;
  sstm << "\nGot parameters ";
  if(args.isSet("input-checkpoint"))
  {
    sstm << "from the checkpoint file:\n";
    l_ifile_checkp = args.getArgument<std::string>("input-checkpoint");
    sstm << "Input checkpoint file name:\t" << l_ifile_checkp << "\n";
    isCheckpoint = true;
    checkp_reader = new io::NetCdfReader(l_ifile_checkp);
    l_nX = checkp_reader->getGlobalIntAttribute("nx");
    l_nY = checkp_reader->getGlobalIntAttribute("ny");
    l_time_dur = checkp_reader->getGlobalFloatAttribute("timeduration");
    l_checkpoints = checkp_reader->getGlobalIntAttribute("checkpoints");
    l_bound_types = (BoundaryType*)checkp_reader->getGlobalIntPtrAttribute("outconditions", 4);
    l_baseName = checkp_reader->getGlobalTextAttribute("basename");
    l_timestep = checkp_reader->timeLength - 2;
    sstm << "Existing checkpoints:\t\t" << l_timestep << "\n";
    l_timepos = checkp_reader->timeMax;
    sstm << "Existing time:\t\t\t" << l_timepos << "\n\n";
  }
  else
  {
    sstm << "from the command line:\n";
    l_nX = args.getArgument<int>("grid-size-x");
    l_nY = args.getArgument<int>("grid-size-y");
    l_ifile_baty = args.getArgument<std::string>("input-bathymetry");
    l_ifile_disp = args.getArgument<std::string>("input-displacement");
    l_time_dur = args.getArgument<int>("time-duration");
    l_checkpoints = args.getArgument<int>("checkpoint-amount");
    l_bound_types[0] = static_cast<BoundaryType>(args.getArgument<int>("boundary-condition-left"));
    l_bound_types[1] = static_cast<BoundaryType>(args.getArgument<int>("boundary-condition-right"));
    l_bound_types[2] = static_cast<BoundaryType>(args.getArgument<int>("boundary-condition-bottom"));
    l_bound_types[3] = static_cast<BoundaryType>(args.getArgument<int>("boundary-condition-top"));
    l_baseName = args.getArgument<std::string>("output-basepath");
    l_output_scale = args.getArgument<int>("output-scale");
  }
  if(args.isSet("simulate-failure")) l_failure = args.getArgument<int>("simulate-failure");

  sstm << "Number of cells in x direction:\t" << l_nX << "\n"; 
  sstm << "Number of cells in y direction:\t" << l_nY << "\n";   
  sstm << "Input bathymetry file name:\t" << l_ifile_baty << "\n";
  sstm << "Input displacement file name:\t" << l_ifile_disp << "\n";
  sstm << "Time duration:\t\t\t" << l_time_dur << "\n";
  sstm << "Amount of checkpoints:\t\t" << l_checkpoints << "\n";
  sstm << "Boundary condition left:\t" << l_bound_types[0] << "\n";
  sstm << "Boundary condition right:\t" << l_bound_types[1] << "\n";
  sstm << "Boundary condition top:\t\t" << l_bound_types[2] << "\n";
  sstm << "Boundary condition bottom:\t" << l_bound_types[3] << "\n";
  sstm << "Output base file name:\t\t" << l_baseName << "\n";

#ifdef USE_OMP
  int l_limit_cpu = thread::hardware_concurrency();
  if(args.isSet("limit-cores")) l_limit_cpu = min(args.getArgument<int>("limit-cores"), l_limit_cpu);
  omp_set_num_threads(l_limit_cpu);
  sstm << "Number of CPU cores used:\t" << l_limit_cpu << "\n";
#endif

  tools::Logger::logger.printString(sstm.str());
#endif

if((l_timestep + 1) >= l_checkpoints)
{
  tools::Logger::logger.printString("This checkpoint file already has all its checkpoints computed!\n");
  return 0;
}

  // read xml file
#ifdef READXML
  assert(false); //TODO: not implemented.
  if(argc != 2) 
  {
    tools::Logger::logger.printString("Aborting. Please provide a proper input file.");
    tools::Logger::logger.printString("Example: ./SWE_gnu_debug_none_augrie config.xml");
    return 1;
  }
  tools::Logger::logger.printString("Reading xml-file.");
  std::string l_xmlFile = std::string(argv[1]);
  tools::Logger::logger.printString(l_xmlFile);
  CXMLConfig l_xmlConfig;
  l_xmlConfig.loadConfig(l_xmlFile.c_str());
#endif

#ifdef ASAGI
  /* Information about the example bathymetry grid (tohoku_gebco_ucsb3_500m_hawaii_bath.nc):
   *
   * Pixel node registration used [Cartesian grid]
   * Grid file format: nf = GMT netCDF format (float)  (COARDS-compliant)
   * x_min: -500000 x_max: 6500000 x_inc: 500 name: x nx: 14000
   * y_min: -2500000 y_max: 1500000 y_inc: 500 name: y ny: 8000
   * z_min: -6.48760175705 z_max: 16.1780223846 name: z
   * scale_factor: 1 add_offset: 0
   * mean: 0.00217145586762 stdev: 0.245563641735 rms: 0.245573241263
   */

  //simulation area
  float simulationArea[4];
  simulationArea[0] = -450000;
  simulationArea[1] = 6450000;
  simulationArea[2] = -2450000;
  simulationArea[3] = 1450000;
  SWE_AsagiScenario l_scenario( ASAGI_INPUT_DIR "tohoku_gebco_ucsb3_500m_hawaii_bath.nc",
    ASAGI_INPUT_DIR "tohoku_gebco_ucsb3_500m_hawaii_displ.nc",
    (float) 28800., simulationArea);
#else
  // create a scenario
  SWE_TsunamiScenario* l_scenario;
  if(!isCheckpoint) l_scenario =  new SWE_TsunamiScenario(l_ifile_disp, l_ifile_baty, l_bound_types, l_time_dur);
  else l_scenario = new SWE_TsunamiScenario(checkp_reader, l_nX, l_nY, l_time_dur);
  //SWE_RadialDamBreakScenario* l_scenario = new SWE_RadialDamBreakScenario(l_bound_types);
   //SWE_ArtificialTsunamiScenario* l_scenario = new SWE_ArtificialTsunamiScenario(l_bound_types);
#endif

  //! size of a single cell in x- and y-direction
  float l_dX, l_dY;
  if(isCheckpoint)
  {
    l_dX = checkp_reader->getGlobalFloatAttribute("dx");
    l_dY = checkp_reader->getGlobalFloatAttribute("dy");
  }
  else
  {
    // compute the size of a single cell
    l_dX = (l_scenario->getBoundaryPos(BND_RIGHT) - l_scenario->getBoundaryPos(BND_LEFT))/l_nX;
    l_dY = (l_scenario->getBoundaryPos(BND_TOP) - l_scenario->getBoundaryPos(BND_BOTTOM))/l_nY;
  }
  // create a single dimensional splitting block
#ifndef CUDA
  SWE_DimensionalSplittingBlock l_dimensionalSplittingBlock(l_nX,l_nY,l_dX,l_dY);
#else
  SWE_DimensionalSplittingBlockCuda l_dimensionalSplittingBlock(l_nX,l_nY,l_dX,l_dY);
#endif

  //origin of the simulation domain in x- and y-direction
  float l_originX, l_originY;
  if(isCheckpoint)
  {
    l_originX = checkp_reader->getGlobalFloatAttribute("originx");
    l_originY = checkp_reader->getGlobalFloatAttribute("originy");
  }
  else
  {
    // get the origin from the scenario
    l_originX = l_scenario->getBoundaryPos(BND_LEFT);
    l_originY = l_scenario->getBoundaryPos(BND_BOTTOM);
  }
  // initialize the dimensional splitting block
  l_dimensionalSplittingBlock.initScenario(l_originX, l_originY, *l_scenario);

  //time when the simulation ends.
  float l_endSimulation = l_scenario->endSimulation();
  //checkpoints when output files are written.
  float* l_checkPoints = new float[l_checkpoints+1];
  // compute the checkpoints in time
  for(int cp = 0; cp <= l_checkpoints; cp++) l_checkPoints[cp] = cp*(l_endSimulation/l_checkpoints);

  // Init fancy progressbar
  tools::ProgressBar progressBar(l_endSimulation);
  // write the output at time zero
  tools::Logger::logger.printOutputTime((float) 0.);
  progressBar.update(0.);
  std::string l_fileName = generateBaseFileName(l_baseName,0,0);

  //boundary size of the ghost layers
  io::BoundarySize l_boundarySize = {{1, 1, 1, 1}};
  if(isCheckpoint) for(int i=0; i<4; i++) l_boundarySize.boundarySize[i] = (checkp_reader->getGlobalIntPtrAttribute("boundarysize", 4))[i];

  if(isCheckpoint) delete checkp_reader;

#ifdef WRITENETCDF
  //construct a NetCdfWriter
  io::NetCdfWriter l_writer(l_fileName, l_baseName, l_dimensionalSplittingBlock.getBathymetry(),
    l_boundarySize, l_nX, l_nY, l_dX, l_dY, (int*)l_bound_types, l_time_dur, l_checkpoints, l_originX, l_originY, l_timestep, isCheckpoint, 1, false, l_output_scale);
  //construct a checkpoint writer
  io::NetCdfWriter l_cp_writer(l_fileName + "_cp", l_baseName, l_dimensionalSplittingBlock.getBathymetry(),
    l_boundarySize, l_nX, l_nY, l_dX, l_dY, (int*)l_bound_types, l_time_dur, l_checkpoints, l_originX, l_originY, l_timestep, isCheckpoint, 1, true, l_output_scale);
#else
  // consturct a VtkWriter
  io::VtkWriter l_writer(l_fileName, l_dimensionalSplittingBlock.getBathymetry(),
    l_boundarySize, l_nX, l_nY, l_dX, l_dY );
  io::VtkWriter l_cp_writer(l_fileName, l_dimensionalSplittingBlock.getBathymetry(),
    l_boundarySize, l_nX, l_nY, l_dX, l_dY );
#endif
  if(!isCheckpoint)
  {
    // Write zero time step
    l_writer.writeTimeStep(l_dimensionalSplittingBlock.getWaterHeight(),
      l_dimensionalSplittingBlock.getDischarge_hu(), l_dimensionalSplittingBlock.getDischarge_hv(), (float) 0.);
  }


  // print the start message and reset the wall clock time
  progressBar.clear();
  tools::Logger::logger.printStartMessage();
  tools::Logger::logger.initWallClockTime(time(NULL));

  //! simulation time.
  float l_t = l_timepos;
  progressBar.update(l_t);
  unsigned int l_iterations = 0;

  // loop over checkpoints
  for(int c=l_timestep; c<=l_checkpoints; c++) 
  { 
    // Write a checkpoint 
    if(l_failure > 0 && c >= l_failure)
    {
       tools::Logger::logger.printString("Simulating catastrophic failure\n");
       abort(); //rough termination, no cleanup, no io flushing
    }
    // do time steps until next checkpoint is reached
    while( l_t < l_checkPoints[c] )
    {
      // set values in ghost cells:
      l_dimensionalSplittingBlock.setGhostLayer();
      // reset the cpu clock
      tools::Logger::logger.resetClockToCurrentTime("Cpu");

#if DIMSPLIT_SELECT != DIMSPLIT_SELECT_Y
      //compute x (horizontal) sweep
      float l_maxWaveSpeedHorizontal = l_dimensionalSplittingBlock.computeNumericalFluxesHorizontal();
      //approximate max timestep using the max wavespeed in x direction
      l_dimensionalSplittingBlock.computeMaxTimestep(l_maxWaveSpeedHorizontal, true);
      //maximum allowed time step width.
      float l_maxTimeStepWidth = l_dimensionalSplittingBlock.getMaxTimestep();
      //update unknowns in x direction
      l_dimensionalSplittingBlock.updateUnknownsHorizontal(l_maxTimeStepWidth);

#if !defined(NDEBUG) || defined(DEBUG)
      //Check CFL condition for x sweep
      //assert(l_maxTimeStepWidth < 0.5 * (l_dX / l_maxWaveSpeedHorizontal));
#endif

#endif

#if DIMSPLIT_SELECT != DIMSPLIT_SELECT_X
      //compute y (vertical) sweep fluxes
      float l_maxWaveSpeedVertical = l_dimensionalSplittingBlock.computeNumericalFluxesVertical();

#if DIMSPLIT_SELECT == DIMSPLIT_SELECT_Y
      //approximate max timestep using the max wavespeed in y direction
      l_dimensionalSplittingBlock.computeMaxTimestep(l_maxWaveSpeedVertical, false);
      //maximum allowed time step width.
      float l_maxTimeStepWidth = l_dimensionalSplittingBlock.getMaxTimestep();
#endif

      //update unknowns in y direction, reeuse max time step
      l_dimensionalSplittingBlock.updateUnknownsVertical(l_maxTimeStepWidth);

#if !defined(NDEBUG) || defined(DEBUG)
      //Check CFL condition for y sweep
      //assert(l_maxTimeStepWidth < 0.5 * (l_dY / l_maxWaveSpeedVertical));
#endif

#endif
     
      // update the cpu time in the logger
      tools::Logger::logger.updateTime("Cpu");
      // update simulation time with time step width.
      l_t += l_maxTimeStepWidth;
      l_iterations++;
      // print the current simulation time
      progressBar.clear();
      tools::Logger::logger.printSimulationTime(l_t);
      progressBar.update(l_t);
    }

    // print current simulation time of the output
    progressBar.clear();
    tools::Logger::logger.printOutputTime(l_t);
    progressBar.update(l_t);
    // write output
    l_writer.writeTimeStep( l_dimensionalSplittingBlock.getWaterHeight(),
      l_dimensionalSplittingBlock.getDischarge_hu(), l_dimensionalSplittingBlock.getDischarge_hv(), l_t);
    // if we are in a step that requires a chekpoint we write this too
    if ( l_t >= l_checkPoints[c])
    {
      l_cp_writer.writeTimeStep( l_dimensionalSplittingBlock.getWaterHeight(),
      l_dimensionalSplittingBlock.getDischarge_hu(), l_dimensionalSplittingBlock.getDischarge_hv(), l_t);
    }
  }

  // write the statistics message
  progressBar.clear();
  tools::Logger::logger.printStatisticsMessage();
  // print the cpu time
  tools::Logger::logger.printTime("Cpu", "CPU time");
  // print the wall clock time (includes plotting)
  tools::Logger::logger.printWallClockTime(time(NULL));
  // printer iteration counter
  tools::Logger::logger.printIterationsDone(l_iterations);

  delete l_scenario;
  delete l_bound_types;

  return 0;
}
