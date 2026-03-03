#ifndef NPC_H
#define NPC_H

#include "entity.h"

void NPC_CommonInit(void);
void NPC_New(Entity* e, Vector3 pos, float rot);
void NPC_Interact(Entity* e); //Just using for testing.

#endif
