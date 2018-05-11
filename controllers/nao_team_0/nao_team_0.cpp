//---------------------------------------------------------------------------------------
//  Description:  Instantiates the correct player type: FieldPlayer or GoalKeeper
//  Project:      Robotstadium, the online robot soccer competition
//---------------------------------------------------------------------------------------

#include <player_manager.h>
#include <alerror/alerror.h>
#include <qi/application.hpp>
#include <stdlib.h>
#include <cstdlib>
#include <iostream>

#ifdef _WIN32
#include <windows.h>
#define SEP "\\"
#define putenv _putenv
#else
#include <unistd.h>
#define SEP "/"
#endif

using namespace std;

// Run a different type of player according to playerID. The player sends
// naoqi commands which will be interpreted by the naoqisim process.
int runBehavior(int playerID, int teamID, int naoPort) {
  class Player *player = NULL;
  string naoIp("localhost");

  // At this point, some naoqi modules may still be launching
  while (player == NULL) {
    if (playerID == 0)
      player = create_player(GOALKEEPER, playerID, teamID, naoIp.c_str(), naoPort);
    else if (playerID <= 2)
      player = create_player(STRIKER, playerID, teamID, naoIp.c_str(), naoPort);
    else
      player = create_player(DEFENDER, playerID, teamID, naoIp.c_str(), naoPort);

    if (player == NULL) {
#ifdef _WIN32
      Sleep(2000);
#else
      usleep(2000000);
#endif
    }
  }

  try {
    run_player(player);
  } catch (exception &e) {
    cerr << e.what() << endl;
  }

  delete_player(player);

  return EXIT_SUCCESS;
}

#ifdef _WIN32
// naoqisim is linked with the Windows SubSystem (/SUBSYSTEM:WINDOWS) in order to avoid the console pop-up
// hence WinMain() is used instead of main() on Windows
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd) {
  int argc = __argc;
  char **argv = __argv;

  qi::Application *app = new qi::Application(argc, argv);
  app->run();

#else
int main(int argc, char *argv[]) {
#endif
  if (argc < 3) {
    cerr << "Error: could not find teamID and playerID in controllerArgs" << endl;
    return EXIT_FAILURE;
  }

  int playerID = atoi(argv[1]);
  int teamID = atoi(argv[2]);
  int naoPort = 9559 + playerID + (5 * teamID);

  string WEBOTS_NAOSIM_PATH(getenv("WEBOTS_NAOQISIM_PATH"));
  string path = WEBOTS_NAOSIM_PATH + SEP + "naoqisim";

#ifdef _WIN32
  path += ".exe";
  string arg = "\"" + path + "\" " + to_string(static_cast<long long>(naoPort));

  STARTUPINFO si;
  PROCESS_INFORMATION pi;

  ZeroMemory( &si, sizeof(si) );
  si.cb = sizeof(si);
  ZeroMemory( &pi, sizeof(pi) );

  // Start the child process.
  if(!CreateProcess((LPCTSTR) path.c_str(), (LPTSTR) arg.c_str(), NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi)) {
    printf( "CreateProcess failed (%d).\n", GetLastError() );
    return EXIT_FAILURE;
  }

  runBehavior(playerID, teamID, naoPort);

  // Wait until child process exits.
  WaitForSingleObject( pi.hProcess, INFINITE );

  // Close process and thread handles.
  CloseHandle( pi.hProcess );
  CloseHandle( pi.hThread );
#else
  int pid = fork();

  if (pid == -1) // Error when forking
    return EXIT_FAILURE;
  else if (pid == 0) // One of the process is naoqisim, which will receive the naoqi commands.
    execl(path.c_str(), path.c_str(), to_string(static_cast<long long>(naoPort)).c_str(), NULL);
  else // The other process sends the naoqi commands.
    runBehavior(playerID, teamID, naoPort);
#endif

  return EXIT_SUCCESS;
}
