#ifndef DEFENDER_HPP
#define DEFENDER_HPP

//-----------------------------------------------------------------------------
//  File:         Defender C++ class (to be used in a Webots controllers)
//  Description:  Defender, responsible for preventing the opponent from scoring.
//                Extremely simplified behavior with no coordination with
//                any teammates or opponents.
//  Project:      Robotstadium, the online robot soccer competition
//-----------------------------------------------------------------------------

#include "Player.hpp"

class Defender: public Player {
public:
  Defender(int playerID, int teamID, std::string naoIp, int naoPort);
  ~Defender();
  void run();
};

#endif
