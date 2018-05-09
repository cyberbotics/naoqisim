// Description: Interface between Webots and Nao simulation SDK

#include <webots/robot.h>
#include <iostream>
#include <stdlib.h>
#include <cstring>
#include <qi/application.hpp>
#include "Nao.hpp"
#include "Singletons.hpp"
#include "Mutex.hpp"

#ifdef _WIN32
#include <windows.h>
#endif

using namespace std;

static void printUsage() {
  cerr << "Please specify the NAOQI_PORT_NUMBER in the 'controllerArgs' field of the Nao robot.\n";
  cerr << "Usage: controllerArgs \"[-nocam] NAOQI_PORT_NUMBER\"\n";
  cerr << "Note that each Nao robot should use a different port number.\n";
  cerr << "Options: -nocam, disable the simulated camera\n";
}

Mutex *halInitializationMutex=NULL;

#ifdef _WIN32 // TODO: remove this case when Aldebaran release the fixed simulator-sdk for Mac and Linux 64
static void halInitializedCallback();
#endif

#ifdef _WIN32
// naoqisim is linked with the Windows SubSystem (/SUBSYSTEM:WINDOWS) in order to avoid the console pop-up
// hence WinMain() is used instead of main() on Windows
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd) {
  int argc = __argc;
  char **argv = __argv;
#else
int main(int argc, char *argv[]) {
#endif
  if (argc < 2 || argc > 3) {
    printUsage();
    return EXIT_FAILURE;
  }

  int naoqiPort = atoi(argv[argc - 1]);
  if (naoqiPort == 0) {
    cerr << "Error: invalid NAOQI_PORT_NUMBER specified in 'controllerArgs'\n";
    printUsage();
    return EXIT_FAILURE;
  }

  bool useCameras = true;
  if (argc == 3) {
    if (strcmp(argv[1], "-nocam") == 0)
      useCameras = false;
    else {
      cerr << "invalid argument: " << argv[1] << "\n";
      printUsage();
      return EXIT_FAILURE;
    }
  }

  // initialize qi::os
  qi::Application(argc, argv);

  // initialize webots
  wb_robot_init();

  // get WorldInfo.basicTimeStep
  int timeStep = (int)wb_robot_get_basic_time_step();

  cout << "===== naoqisim controller started =====\n" << flush;
  cout << "Press the real-time button [>] to start the simulation, then you can connect Choregraphe.\n" << flush;

  // wait for simulation to start or revert
  if (wb_robot_step(timeStep) == -1) {
    wb_robot_cleanup();
    return EXIT_SUCCESS;
  }

  // the robot model is hidden in the Robot.name field in the .proto file
  string robotModel(wb_robot_get_model());

#ifndef _WIN32
  // TODO: remove this case when Aldebaran release the fixed simulator-sdk for Mac and Linux 64
  if (! Singletons::initialize(robotModel, naoqiPort, NULL))
     return EXIT_FAILURE;
  sleep(1);
#else
  // Setup a Mutex to wait for the end of the initialization of the HAL
  halInitializationMutex = new Mutex();
  halInitializationMutex->lock();

  // initialize HAL
  if (! Singletons::initialize(robotModel, naoqiPort, halInitializedCallback))
    return EXIT_FAILURE;

  halInitializationMutex->lock();
#endif

  // create/run/destroy Nao
  Nao *nao = new Nao(timeStep, useCameras);
  nao->run();
  delete nao;

  // cleanup
  Singletons::shutdown();
  wb_robot_cleanup();

  return EXIT_SUCCESS;
}

#ifdef _WIN32 // TODO: remove this case when Aldebaran release the fixed simulator-sdk for Mac and Linux 64
static void halInitializedCallback() {
  halInitializationMutex->unlock();
}
#endif
