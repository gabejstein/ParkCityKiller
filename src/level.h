#ifndef LEVEL_H
#define LEVEL_H

#include <raylib.h>
#include "system/resource.h"
#include "entity/entity.h"

typedef enum
{
	LEVEL_NULL = -1,
	LEVEL_OVERWORLD = 0,
	LEVEL_HOTEL,
	MAX_LEVELS
}LEVEL_DEF_ID;

typedef struct
{
	Vector3 position, size;
	BoundingBox box;
	unsigned int levelId;
	char* spawnId;
}LevelPortal;

typedef struct
{
	char* id;
	Vector3 pos;
	float rotY;
}SpawnPoint;

typedef enum
{
	SKY_FLAT_COLOR,
	SKY_STATIC_IMAGE,
	SKY_SKYBOX
}SKY_TYPE;

typedef struct
{
	float lightIntensity;
	Vector3 sunDirection;
	float fogDensity;
	Color fogColor;
	SKY_TYPE skyType;
	//TODO: union with either flat color or skybox struct
}LevelLighting;

//TODO: Level needs to be separated into a definition type and the object that's used at runtime.
typedef struct
{
	LEVEL_DEF_ID id; //Might replace with string name
	LevelPortal* portals;
	SpawnPoint* spawnPoints;
	ModelHandle model;
	ModelHandle collisionModel;
	TextureHandle background;
	SoundHandle music;
	void (*load)(struct Level*); //Note: may not need this, but could be useful if there's level-specific logic.
	void (*unload)(struct Level*);
	Entity* player;

	int portalCount;
	int spawnPointCount;

	LevelLighting lighting;
}Level;

Level* Level_GetLevel(LEVEL_DEF_ID id);
void Level_Load(Level* level);
void Level_Unload(Level* level);
void Level_Update(Level* level, float dt);
void Level_DebugRender(Level* level);
void Level_DebugRender2D(Level* level);
void Level_SetNext(LEVEL_DEF_ID levelId, char* spawnId);

#endif
