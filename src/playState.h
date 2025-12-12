#ifndef PLAY_STATE_H
#define PLAY_STATE_H

#include "entity.h"

void PlayState_Start(void);
Entity* NewEntity(void);

Entity* GetPlayer(void);

#endif
