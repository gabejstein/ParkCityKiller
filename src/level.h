#ifndef LEVEL_H
#define LEVEL_H

#include <raylib.h>
#include "system/resource.h"

typedef struct
{
	ModelHandle model;
	ModelHandle collisionModel;
	void (*Load)(struct Level*); //Note: may not need this, but could be useful if there's level-specific logic.
	void (*Unload)(struct Level*);
}Level;

typedef enum
{
	LEVEL_OVERWORLD,
	LEVEL_HOTEL,
	MAX_LEVELS
}LEVEL_DEF_ID;

Level* GetLevel(LEVEL_DEF_ID id);

#endif
