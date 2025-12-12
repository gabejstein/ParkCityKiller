#ifndef ENEMY_H
#define ENEMY_H

#include "common.h"
#include "entity.h"

typedef enum
{
	ENEMY_STATE_NEUTRAL,
	ENEMY_STATE_ATTACK,
	ENEMY_STATE_PURSUE,
	ENEMY_STATE_ESCAPE,
	ENEMY_STATE_HURT,
	ENEMY_STATE_DEAD
}ENEMY_STATE;

typedef struct
{
	ENEMY_STATE state;
	ENEMY_STATE prevState;
	float waitTimer;
	Entity* entity;
}Enemy;

void InitEnemyPool(void);
Enemy* SpawnEnemy(Vector3 pos);
void Enemy_SetPlayer(Entity* p);

#endif
