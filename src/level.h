#ifndef LEVEL_H
#define LEVEL_H

#include <raylib.h>
#include "system/resource.h"
#include "entity/entity.h"
#include <stdint.h>

typedef struct
{
	Vector3 position, size;
	BoundingBox box;
	char* levelName;
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
	SKY_NONE,
	SKY_FLAT_COLOR,
	SKY_STATIC_IMAGE,
	SKY_SKYBOX
}SKY_TYPE;

typedef struct
{
	Color lightColor;
	Vector3 sunDirection;
	float fogDensity;
	Color fogColor;
	SKY_TYPE skyType;
	union
	{
		Color skyColor;
		char* skyTexturePath;
	};
}LevelLighting;

typedef enum
{
	LIGHTING_DAYTIME,
	LIGHTING_NIGHT,
	LIGHTING_SUNSET,
	LIGHTING_INDOORS,
	LIGHTING_MAX
}LIGHTING_DEF;

typedef Vector3 Billboard; //Just storing position until system is worked out

typedef struct
{
	uint32_t portalCount;
	uint32_t spawnPointCount;
	uint32_t billboardCount;
	LevelPortal* portals;
	SpawnPoint* spawnPoints;
	Billboard* billboards;
	ModelHandle model;
	ModelHandle collisionModel;
	Music music;
	void (*load)(struct Level*); //Note: may not need this, but could be useful if there's level-specific logic.
	void (*unload)(struct Level*);
	void (*onStart)(struct Level*, void* data); //For running specialized level scripts when starting a level.
	void (*onUpdate)(struct Level*, void* data);
	Entity* player;
	Entity* hoverbike;
	bool bOutdoors; //Whether or not to swap day/night lighting
	LevelLighting lighting;
}Level;

//Note: If this needs to be external, JSON would be useful for this.
typedef struct
{
	char* id;
	char* name;
	char* filePath;
	char* modelPath;
	char* collisionModelPath; //Should be same as model if null.
	LIGHTING_DEF lightingType;
	char* musicPath;
	void (*onStart)(void* data); //might just be integers into a table of function pointers
	void (*onUpdate)(void* data); //Could handle things like mission objectives, etc.
	//TODO: should also designate materials for each slot.
}LevelDef;

LevelDef* GetLevelDef(const char* id);
void Level_Load(const char* id);
void Level_Unload(Level* level);
void Level_Update(Level* level, float dt);
void Level_Render(Level* level);
void Level_DebugRender(Level* level);
void Level_DebugRender2D(Level* level);
void Level_SetNext(char* levelName, char* spawnId);

#endif
