#include "Player.hpp"
#include "Striker.hpp"
#include "Defender.hpp"
#include "GoalKeeper.hpp"
#include "player_manager.h"
#include <alerror/alerror.h>
#include <iostream>
#include <string>

class Player *player = NULL;

extern "C" {
  Player* create_player(int type, int playerID, int teamID, const char* naoIp, int naoPort) {
  try {
    switch (type) {
      case 1: player = new Striker(playerID, teamID, naoIp, naoPort); break;
      case 2: player = new Defender(playerID, teamID, naoIp, naoPort); break;
      case 3: player = new GoalKeeper(playerID, teamID, naoIp, naoPort); break;
      default: std::cerr << "Unknown player type" << std::endl; break;
    }
  } catch (AL::ALError &) {
    player = NULL;
  }

  return player;
  }

  void run_player(Player* player) {
    player->run();
  }

  void delete_player(Player* player) {
    delete player;
  }
}
