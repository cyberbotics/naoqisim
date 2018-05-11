#include <alcommon/albroker.h>
#include <alerror/alerror.h>
#include <alproxies/almotionproxy.h>
#include <alproxies/alrobotpostureproxy.h>
#include <iostream>
#include <cstring>
#include "TaskManager.hpp"

#ifdef _WIN32
#include <windows.h>
#define SEP "\\"
#define putenv _putenv
#else
#include <fstream>
#include <dirent.h>
#include <signal.h>
#include <sstream>
#include <sys/wait.h>
#define SEP "/"
#endif

using namespace std;

void runManager(int naoPort, string naoIp) {
  bool start = false;
  TaskManager *taskManager = NULL;

  //At this point, some naoqi modules may still be launching
  while (!start) {
    try {
      taskManager = new TaskManager(naoIp, naoPort);
      start = true;
    } catch (AL::ALError &e) {
#ifdef _WIN32
      Sleep(2000);
#else
      usleep(2000000);
#endif
    }
  }

  try {
    taskManager->run();
    cout << "All the tasks have been correctly executed." << endl;
  } catch (exception &e) {
    cerr << e.what() << endl;
    cerr << "An error occured during the resolution of the challenge." << endl;
  }

  delete taskManager;
}

#ifdef _WIN32
// naoqisim is linked with the Windows SubSystem (/SUBSYSTEM:WINDOWS) in order to avoid the console pop-up
// hence WinMain() is used instead of main() on Windows
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd) {
  int argc = __argc;
  char **argv = __argv;
#else
int main(int argc, char *argv[]) {
#endif
  string naoIp("localhost");
  char naoPort[] = "9559";

  string WEBOTS_NAOSIM_PATH(getenv("WEBOTS_NAOQISIM_PATH"));
  string path = WEBOTS_NAOSIM_PATH + SEP + "naoqisim";

#ifdef _WIN32
  path += ".exe";
  string arg = "\"" + path + "\" " + naoPort;

  STARTUPINFO si;
  PROCESS_INFORMATION pi;

  ZeroMemory( &si, sizeof(si) );
  si.cb = sizeof(si);
  ZeroMemory( &pi, sizeof(pi) );

  // Start the other process, naoqisim.
  if(!CreateProcess((LPCTSTR) path.c_str(), (LPTSTR) arg.c_str(), NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi)) {
    printf( "CreateProcess failed (%d).\n", GetLastError() );
    return EXIT_FAILURE;
  }

  // sends the commans to naoqi.
  runManager(atoi(naoPort), naoIp);

  /** Do we really want to close naoqisim when runManager exits ? */
  // Wait until the other process exits.
  WaitForSingleObject( pi.hProcess, INFINITE );

  // Close process and thread handles.
  CloseHandle( pi.hProcess );
  CloseHandle( pi.hThread );
#else
  int pid = fork();

  if (pid == -1) { // Error when forking
    return EXIT_FAILURE;
  } else if (pid == 0) { // The child process is the one sending the command to naoqi.
    runManager(atoi(naoPort), naoIp);
  } else { // The parent process is naoqisim, which executes hal and naoqi-bin.
    execl(path.c_str(), path.c_str(), naoPort, NULL);
  }
#endif

  return EXIT_SUCCESS;
}
