#ifndef PLAY_STATE_H
#define PLAY_STATE_H

#include "entity.h"

GameState GetPlayState(void);
Entity* NewEntity(void);

Entity* GetPlayer(void);

#endif
