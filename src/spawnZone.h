#ifndef SPAWN_ZONE_H
#define SPAWN_ZONE_H

#include <raylib.h>

void InitSpawnZones(void);
void UpdateSpawnZones(float dt, const Vector3* playerPos);
void Debug_RenderSpawnZones(void);

#endif
