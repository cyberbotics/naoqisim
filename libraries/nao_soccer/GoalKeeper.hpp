#ifndef GOAL_KEEPER_HPP
#define GOAL_KEEPER_HPP

//-----------------------------------------------------------------------------
//  Description:  Goal keeper (not a field player !)
//  Project:      Robotstadium, the online robot soccer competition
//-----------------------------------------------------------------------------

#include "Player.hpp"

class GoalKeeper: public Player {
public:
  GoalKeeper(int playerID, int teamID, std::string naoIp, int naoPort);
  virtual ~GoalKeeper();
  virtual void run();
};

#endif
