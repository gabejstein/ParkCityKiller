#ifndef INTERACTABLE_H
#define INTERACTABLE_H

//Basically a trigger object that does something (anything)
//whenever the player presses the interaction button.

//Note: I don't know if I'll use this struct. It might be enough just to check
//an entity tag and call the entity's oninteraction method for now.

#include "entity.h"

typedef enum
{
	INTERACTION_TYPE_TALK,
}Interactable_Type;

typedef void (*Interactable_Callback)(void* data);

typedef struct Interactable
{
	Entity* entity; //may change this to a numbered id system later.
	Interactable_Type type;
	Interactable_Callback onInteract;
	void* data;
}Interactable;

Interactable* Interactable_Get(char* id);

#endif
