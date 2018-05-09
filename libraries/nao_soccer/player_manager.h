#ifndef PLAYER_MANAGER_H
#define PLAYER_MANAGER_H

#define STRIKER 1
#define DEFENDER 2
#define GOALKEEPER 3

#ifdef __cplusplus
extern "C" {
#endif

typedef class Player Player;

Player* create_player(int type, int playerID, int teamID, const char* naoIp, int naoPort);
void run_player(Player* player);
void delete_player(Player* player);

#ifdef __cplusplus
}
#endif
#endif
