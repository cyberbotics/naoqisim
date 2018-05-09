#ifndef STRIKER_HPP
#define STRIKER_HPP

//-----------------------------------------------------------------------------
//  File:         Striker C++ class (to be used in a Webots controllers)
//  Description:  Striker, responsible for scoring goals.
//                Extremely simplified behavior with no coordination with
//                any teammates or opponents. Currently not able to know which
//                one is their goal and which one is the opponent's.
//  Project:      Robotstadium, the online robot soccer competition
//-----------------------------------------------------------------------------

#include "Player.hpp"

class Striker: public Player {
public:
  Striker(int playerID, int teamID, std::string naoIp, int naoPort);
  ~Striker();
  void run();

private:
  bool mCalibrating;
  bool mPreparing;
  bool mRandomKick;
  bool mReady;
  bool mVerification;
  int mIterRemaining;

  void resetProgress();
};

#endif
