#ifndef BULLET_H
#define BULLET_H

#include "entity.h"

typedef struct
{
	ENT_TAG shooter;
	int damage;
	int life;
}BulletData;

void InitBulletPool(void);
void SpawnBullet(Vector3 pos, Vector3 vel, ENT_TAG shooter);

#endif
