//---------------------------------------------------------------------------------------
//  Description:  Instantiates the correct player type: FieldPlayer or GoalKeeper
//  Project:      Robotstadium, the online robot soccer competition
//---------------------------------------------------------------------------------------

#include <player_manager.h>
#include <alerror/alerror.h>
#include <stdlib.h>
#include <cstdlib>
#include <iostream>

#ifdef _WIN32
#include <windows.h>
#include <qi/application.hpp>
#define SEP "\\"
#define putenv _putenv
#else
#include <unistd.h>
#define SEP "/"
#endif

using namespace std;

void runBehavior(int naoPort) {
  class Player *player = NULL;
  string naoIp("localhost");

  // At this point, some naoqi modules may still be launching
  while (player == NULL) {
    player = create_player(STRIKER, 10, 0, naoIp.c_str(), naoPort);

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
}

#ifdef _WIN32
// naoqisim is linked with the Windows SubSystem (/SUBSYSTEM:WINDOWS) in order to avoid the console pop-up
// hence WinMain() is used instead of main() on Windows
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd) {
  int argc = __argc;
  char **argv = __argv;

  qi::Application *app = new qi::Application(argc, argv);

#else
int main(int argc, char *argv[]) {
#endif

  int naoPort = 9560;
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

  runBehavior(naoPort);

  // Wait until child process exits.
  WaitForSingleObject( pi.hProcess, INFINITE );

  // Close process and thread handles.
  CloseHandle( pi.hProcess );
  CloseHandle( pi.hThread );
#else
  int pid = fork();

  if (pid == -1)
    return EXIT_FAILURE;
  else if (pid == 0)
    execl(path.c_str(), path.c_str(), to_string(naoPort).c_str(), NULL);
  else
    runBehavior(naoPort);
#endif

  return EXIT_SUCCESS;
}
