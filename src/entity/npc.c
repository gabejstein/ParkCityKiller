#include "npc.h"

//TODO: create some sort of interactable object that's separate
//from the npc so it can be reused for things like signs, doors,etc.
typedef struct
{
	int id;
	Vector3 headPos;
}NPC_Data;

static void NPC_Update(Entity* e, float dt)
{

}

static void NPC_Render(void)
{

}

void NPC_New(Entity* e)
{
	e->tag = ET_NPC;
}