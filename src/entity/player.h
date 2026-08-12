#ifndef PLAYER_H
#define PLAYER_H

#include "entity.h"

Entity* Player_GetInteractable(void);

void Player_Common_Init(void);
void Player_Common_Unload(void);
void Player_New(Entity* e, Vector3 position);

#endif
