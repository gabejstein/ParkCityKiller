#ifndef NPC_H
#define NPC_H

#include "entity.h"

typedef enum
{
	NPC_DEF_DEFAULT,
	NPC_DEF_ORANGE_GUY,
	NPC_DEF_BLUE_GUY,
	MAX_NPC_DEF
}NPC_DEF_TYPE;

void NPC_CommonInit(void);
void NPC_New(Entity* e, Vector3 pos, float rot, NPC_DEF_TYPE def);
void NPC_Interact(Entity* e); //Just using for testing.
void NPC_FacePosition(Entity* e, Vector3 target, float dt);

//temporary. might get rid of.
void NPC_SetPlayer(Entity* e);

#endif
