#ifndef PLAYER_H
#define PLAYER_H

#include "entity.h"

typedef enum
{
	PLAYER_STATE_MOVE,
	PLAYER_STATE_AIM,
	PLAYER_STATE_HURT,
	PLAYER_STATE_DEAD
}PLAYER_STATE;

PLAYER_STATE GetPlayerState(void);

void Player_Common_Init(void);
void Player_Common_Unload(void);
void Player_New(Entity* e, Vector3 position);

#endif
