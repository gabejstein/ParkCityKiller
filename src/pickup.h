#ifndef PICKUP_H
#define PICKUP_H

#include <raylib.h>
#include "entity.h"

typedef enum
{
	PICKUP_NULL,
	PICKUP_HEALTH,
	PICKUP_AMMO,
	PICKUP_MONEY,
	PICKUP_EXP,
	MAX_PICKUP_TYPES
}PICKUP_TYPE;

typedef struct
{
	PICKUP_TYPE type;
	int amount;
	Entity* entity;
}PickupData;

void InitPickupPool(void);
void SpawnPickup(Vector3 position, PICKUP_TYPE type, int amount);
void SpawnRandomPickup(Vector3 position);

#endif
