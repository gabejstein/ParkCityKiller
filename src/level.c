#include "level.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "playState.h"
#include "system/utils.h"
#include "entity/player.h"
#include "entity/npc.h"
#include "const.h"

#define MAGIC_NUMBER 0x48534F42 //B-O-S-H

static void LoadOverworld(Level* level)
{
	printf("Loading Overworld\n");

	level->player = NewEntity();
	Player_New(level->player, (Vector3) { 4.0f, 3.0f, 4.0f });

	Entity* npc1 = NewEntity();
	NPC_New(npc1, (Vector3) { -0.98f, 0.1f, 22.8f }, 0, NPC_DEF_ORANGE_GUY);

	Entity* npc2 = NewEntity();
	NPC_New(npc2, (Vector3) { 18.13f, 0.1f, 6.38f }, -90, NPC_DEF_BLUE_GUY);

	level->portalCount = 1;
	level->portals = (LevelPortal*)malloc(sizeof(LevelPortal) * level->portalCount);
	memset(level->portals, 0, sizeof(LevelPortal) * level->portalCount);

	level->portals[0] = (LevelPortal){
		.levelName = "hotel",
		.position = (Vector3){42.8f,5.0f,-46.89f},
		.size = (Vector3){5,5,5},
		.spawnId = "lobby"
	};

	level->portals[0].box = (BoundingBox)
	{
		(Vector3)
		{
			level->portals[0].position.x - level->portals[0].size.x / 2,level->portals[0].position.y,level->portals[0].position.z - level->portals[0].size.z / 2
		},
		(Vector3)
		{
			level->portals[0].position.x + level->portals[0].size.x / 2,level->portals[0].position.y + level->portals[0].size.y,level->portals[0].position.z + level->portals[0].size.z / 2
		}
	};

	level->spawnPointCount = 1;
	level->spawnPoints = (SpawnPoint*)malloc(sizeof(SpawnPoint) * level->spawnPointCount);
	memset(level->spawnPoints, 0, sizeof(SpawnPoint) * level->spawnPointCount);

	level->spawnPoints[0] = (SpawnPoint){
		.id = "entrance",
		.pos = (Vector3){42.7f,5.1f,-43.35f},
		.rotY = 0
	};

	//Random pickups.
	for (int i = 0; i < 15; i++)
	{
		Vector3 pos;
		pos.x = GetRandomValue(-130, 260);
		pos.y = 0;
		pos.z = GetRandomValue(-176, 158);
		SpawnRandomPickup(pos);
	}

}

static void LoadHotel(Level* level)
{
	printf("Loading Hotel");

	level->player = NewEntity();
	Player_New(level->player, (Vector3) { 0.0f, 0.0f, 0.0f });

	level->spawnPointCount = 2;
	level->spawnPoints = (SpawnPoint*)malloc(sizeof(SpawnPoint) * level->spawnPointCount);
	memset(level->spawnPoints, 0, sizeof(SpawnPoint) * level->spawnPointCount);

	level->spawnPoints[0] = (SpawnPoint){
		.id = "lobby",
		.pos = (Vector3){0,0,0},
		.rotY = 0
	};

	level->spawnPoints[1] = (SpawnPoint){
		.id = "counter",
		.pos = (Vector3){0.25f,0.5f,-20.6f},
		.rotY = 90
	};

	level->portalCount = 1;
	level->portals = (LevelPortal*)malloc(sizeof(LevelPortal) * level->portalCount);
	memset(level->portals, 0, sizeof(LevelPortal) * level->portalCount);

	level->portals[0] = (LevelPortal){
		.levelName = "overworld",
		.position = (Vector3){0.49f,0.1f,5.45f},
		.size = (Vector3){3,3,3},
		.spawnId = "entrance"
	};

	level->portals[0].box = (BoundingBox)
	{
		(Vector3)
		{
			level->portals[0].position.x - level->portals[0].size.x / 2,level->portals[0].position.y,level->portals[0].position.z - level->portals[0].size.z / 2
		},
		(Vector3)
		{
			level->portals[0].position.x + level->portals[0].size.x / 2,level->portals[0].position.y + level->portals[0].size.y,level->portals[0].position.z + level->portals[0].size.z / 2
		}
	};

}

LevelLighting levelLightDefs[LIGHTING_MAX] =
{
	[LIGHTING_DAYTIME] = {
		.fogColor = {122,112,102,255},
		.fogDensity = 0.002f,
		.lightColor = {255,255,240,255},
		.skyType = SKY_SKYBOX,
		.skyTexturePath = "assets/textures/skybox_01.png",
		.sunDirection = {0.5f,1.0f,0.5f} //TODO: convert from eulers instead.
	},
	[LIGHTING_NIGHT] = {
		.fogColor = {52,42,90,255},
		.fogDensity = 0.002f,
		.lightColor = {120,100,190,255},
		.skyType = SKY_SKYBOX,
		.skyTexturePath = "assets/textures/skybox_night_01.png",
		.sunDirection = {0.5f,1.0f,0.5f} //TODO: convert from eulers instead.
	},
	[LIGHTING_SUNSET] = {
		.fogColor = {52,42,90,255},
		.fogDensity = 0.002f,
		.lightColor = {255,130,90,255},
		.skyType = SKY_SKYBOX,
		.skyTexturePath = "assets/textures/skybox_night_01.png",
		.sunDirection = {0.5f,1.0f,0.5f} //TODO: convert from eulers instead.
	}
};

LevelDef levelDefs[] = {
	{
		.id = "overworld",
		.name = "Mega City",
		.filePath = "assets/levels/overworld.level",
		.modelPath = "assets/models/levels/super_mega_world4.glb",
		.collisionModelPath = NULL,
		.lightingType = LIGHTING_DAYTIME,
		.musicPath = NULL,
		.onStart = NULL,
		.onUpdate = NULL
	},
	{
		.id = "hotel",
		.name = "Hotel",
		.filePath = NULL,
		.modelPath = "assets/models/levels/hotel_01.glb",
		.collisionModelPath = NULL,
		.lightingType = LIGHTING_DAYTIME,
		.musicPath = NULL,
		.onStart = NULL,
		.onUpdate = NULL
	},
	{
		.id = "highway_01",
		.name = "Highway",
		.filePath = NULL,
		.modelPath = "assets/models/levels/inagi_Corridor_01.glb.glb",
		.collisionModelPath = NULL,
		.lightingType = LIGHTING_NIGHT,
		.musicPath = NULL,
		.onStart = NULL,
		.onUpdate = NULL
	}
};

#define MAX_LEVEL_DEFS (sizeof(levelDefs)/sizeof(LevelDef))

LevelDef* GetLevelDef(const char* id)
{
	for (int i = 0; i < MAX_LEVEL_DEFS; i++)
	{
		if (strcmp(levelDefs[i].id, id) == 0)
			return &levelDefs[i];
	}

	return NULL;
}

static void MakePlayer(Level* level, Vector3 position)
{
	level->player = NewEntity();
	Player_New(level->player, position);
}

static void Level_LoadLevelFile(Level* level, const char* filepath)
{
	if (!filepath)return;

	typedef struct
	{
		uint32_t magicNum;
		uint32_t entityCount;
		uint32_t portalCount;
		uint32_t spawnPointCount;
		uint32_t billboardCount;
	}LevelFileHeader;

	LevelFileHeader header;
	char nameBuffer[128];
	uint32_t nameBufferCount;
	Vector3 position;
	float rotY = 0;

	printf("Loading level file: %s\n", filepath);

	FILE* f = fopen(filepath, "rb");
	if (!f) { printf("Could not open level file: %s\n", filepath); return; }

	fread(&header, sizeof(LevelFileHeader), 1, f);
	if (header.magicNum != MAGIC_NUMBER) { printf("Not a valid level file."); return; }

	printf("Level header:\n");
	printf("\tEntity Count: %d\n", header.entityCount);
	printf("\tPortal Count: %d\n", header.portalCount);
	printf("\tSpawnpoint Count: %d\n", header.spawnPointCount);
	printf("\tBillboard Count: %d\n", header.billboardCount);

	level->portalCount = header.portalCount;
	level->portals = (LevelPortal*)malloc(sizeof(LevelPortal) * level->portalCount);

	level->spawnPointCount = header.spawnPointCount;
	level->spawnPoints = (SpawnPoint*)malloc(sizeof(SpawnPoint) * level->spawnPointCount);

	level->billboardCount = header.billboardCount;
	level->billboards = (Billboard*)malloc(sizeof(Billboard) * level->billboardCount);

	for (int i = 0; i < level->portalCount; i++)
	{
		fread(&nameBufferCount, 4, 1, f);
		level->portals[i].levelName = (char*)malloc(nameBufferCount + 1);
		fread(level->portals[i].levelName, nameBufferCount, 1, f);
		level->portals[i].levelName[nameBufferCount] = '\0';

		fread(&nameBufferCount, 4, 1, f);
		level->portals[i].spawnId = (char*)malloc(nameBufferCount + 1);
		fread(level->portals[i].spawnId, nameBufferCount, 1, f);
		level->portals[i].spawnId[nameBufferCount] = '\0';

		fread(&level->portals[i].position, 4, 3, f);
		fread(&level->portals[i].size, 4, 3, f);

		level->portals[i].box = (BoundingBox)
		{
			(Vector3)
			{
				level->portals[i].position.x - level->portals[i].size.x / 2,level->portals[i].position.y,level->portals[i].position.z - level->portals[i].size.z / 2
			},
			(Vector3)
			{
				level->portals[i].position.x + level->portals[i].size.x / 2,level->portals[i].position.y + level->portals[i].size.y,level->portals[i].position.z + level->portals[i].size.z / 2
			}
		};
	}

	SpawnPoint* lastSpawnPoint = level->spawnPoints + level->spawnPointCount;

	for (SpawnPoint* cur = level->spawnPoints; cur < lastSpawnPoint; cur++)
	{
		fread(&nameBufferCount, 4, 1, f);
		cur->id = (char*)malloc(nameBufferCount + 1);
		fread(cur->id, nameBufferCount, 1, f);
		cur->id[nameBufferCount] = '\0';

		fread(&cur->pos, sizeof(Vector3), 1, f);
		fread(&cur->rotY, 4, 1, f);
	}

	Billboard* lastBillboard = level->billboards + level->billboardCount;

	for (Billboard* cur = level->billboards; cur < lastBillboard; cur++)
	{
		uint32_t bType;
		fread(&bType, 4, 1, f);
		fread(&cur, sizeof(Vector3), 1, f);
	}

	for (int i = 0; i < header.entityCount; i++)
	{
		fread(&nameBufferCount, 4, 1, f);
		fread(nameBuffer, 1, nameBufferCount, f);
		nameBuffer[nameBufferCount] = '\0';
		printf("Found class of type: %s\n", nameBuffer);

		fread(&position, sizeof(Vector3), 1, f);
		printf("Position: %f %f %f\n", position.x, position.y, position.z);

		fread(&rotY, 4, 1, f);
		printf("Rotation: %f\n", rotY);

		if (strcmp(nameBuffer, "npc") == 0)
		{
			fread(&nameBufferCount, 4, 1, f);
			fread(nameBuffer, 1, nameBufferCount, f);
			nameBuffer[nameBufferCount] = '\0';
			if (strcmp(nameBuffer, "blue_guy") == 0) //TODO: currently doesnt actually set blue guy
				NPC_New(NewEntity(), position, rotY * RAD2DEG + 90, 0);
		}

	}

	fclose(f);

	printf("Level File: %s successfully loaded.\n", filepath);
}

void Level_Load(const char* id)
{
	printf("Loading Level\n");

	LevelDef* levelDef = GetLevelDef(id);
	if (!levelDef) {
		printf("Could not find level in database.\n");
		exit(1);
	}

	gGame.curLevel = (Level*)malloc(sizeof(Level));
	memset(gGame.curLevel, 0, sizeof(Level));

	Level_LoadLevelFile(gGame.curLevel, levelDef->filePath);

	//TODO: The following needs to be loaded as well, but I'm hardcoding it for now.
	gGame.curLevel->model = RES_LoadModel(levelDef->modelPath);
	if (!levelDef->collisionModelPath)
		gGame.curLevel->collisionModel = gGame.curLevel->model;
	else
		gGame.curLevel->collisionModel = levelDef->collisionModelPath;

	gGame.curLevel->lighting = levelLightDefs[levelDef->lightingType];
	//RES_SetShaderValues(gGame.curLevel->lighting.fogDensity, gGame.curLevel->lighting.fogColor, gGame.curLevel->lighting.sunDirection, gGame.curLevel->lighting.lightColor);
	MakePlayer(gGame.curLevel, (Vector3) { 4.0f, 5.0f, 4.0f });

	gGame.curLevel->onStart = levelDef->onStart;
	gGame.curLevel->onUpdate = levelDef->onUpdate;

	//level->music = LoadMusicStream("assets/music/apple_market.mp3");
	//level->music.looping = true;
	//Note: there is a flaw in the resource handling here.
	//Handles default to 0, but 0 can be the first item in the asset, so there's no way to check for null.
	//PlayMusicStream(level->music); 

	//Position player at spawnpoint
	SpawnPoint* end = gGame.curLevel->spawnPoints + gGame.curLevel->spawnPointCount;
	for (SpawnPoint* cur = gGame.curLevel->spawnPoints; cur < end; cur++)
	{
		if (strcmp(gGame.nextSpawn, cur->id) == 0)
		{
			gGame.curLevel->player->transform.position = cur->pos;
			gGame.curLevel->player->transform.rotation.y = cur->rotY;
			break;
		}
	}

	gGame.nextLevel[0] = '\0';
	gGame.nextSpawn[0] = '\0';

	printf("Level Load Complete\n");
}

void Level_Unload(Level* level)
{
	if (level->unload)
		level->unload(level);

	for (int i = 0; i < level->portalCount; i++)
	{
		free(level->portals[i].levelName);
		free(level->portals[i].spawnId);
	}

	free(level->portals);

	for (int i = 0; i < level->spawnPointCount; i++)
		free(level->spawnPoints[i].id);

	free(level->spawnPoints);

	free(level->billboards);

	free(level);
	level = NULL;

}

void Level_Update(Level* level, float dt)
{
	if (level->onUpdate)
		level->onUpdate(level, NULL);

	//UpdateMusicStream(level->music);
	//Check to see if player should warp.
	for (int i = 0; i < level->portalCount; i++)
	{
		LevelPortal* portal = &level->portals[i];
		if (CheckPointInBox(&level->player->transform.position, &portal->box))
		{
			Level_SetNext(portal->levelName, portal->spawnId);
		}
	}
}

void Level_Render(Level* level)
{
	RES_DrawModel(level->model, (Vector3) { 0, 0, 0 }, 1);

	Texture texture;
	for (int i = 0; i < level->billboardCount; i++)
	{
		Billboard b = level->billboards[i];
		if (Vector3DistanceSqr(b, gGame.mainCamera.transform.position) > MAX_CAMERA_DIST_SQR) continue;
		//DrawBillboard(*gGame.mainCamera.camera, texture, b, 1, WHITE);
	}

}

void Level_SetNext(char* levelName, char* spawnId)
{
	//Copy level name
	char* p = levelName;
	char* out = gGame.nextLevel;
	while ((*out++ = *p++));

	if (!spawnId)return;

	//Copy spawn name
	p = spawnId;
	out = gGame.nextSpawn;
	while ((*out++ = *p++));
}

#if DEBUG_TOOLS
void Level_DebugRender(Level* level)
{
	if (!level->portalCount)return;

	//TODO: Draw overlay of id as well
	for (int i = 0; i < level->portalCount; i++)
	{
		DrawBoundingBox(level->portals[i].box, BLUE);
		DrawSphereWires(level->portals[i].position, 0.4, 4, 4, PURPLE);
	}

	if (!level->spawnPointCount)return;
	//TODO: Draw overlay of id as well
	for (int i = 0; i < level->spawnPointCount; i++)
		DrawSphereWires(level->spawnPoints[i].pos, 0.4, 4, 4, ORANGE);
}

void Level_DebugRender2D(Level* level)
{
	for (int i = 0; i < level->portalCount; i++)
		DrawTextOverlay(level->portals[i].spawnId, level->portals[i].position, 8, RED);

	for (int i = 0; i < level->spawnPointCount; i++)
		DrawTextOverlay(level->spawnPoints[i].id, level->spawnPoints[i].pos, 8, YELLOW);
}
#endif
