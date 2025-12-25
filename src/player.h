#ifndef PLAYER_H
#define PLAYER_H

#include <raylib.h>
#include "entity.h"

//typedef struct
//{
//
//}Player;

typedef enum
{
	PLAYER_STATE_MOVE,
	PLAYER_STATE_AIM,
	PLAYER_STATE_HURT,
	PLAYER_STATE_DEAD
}PLAYER_STATE;

PLAYER_STATE GetPlayerState(void);

void NewPlayer(Entity* e, Vector3 position);

#endif
