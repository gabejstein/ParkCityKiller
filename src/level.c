#include "level.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "playState.h"
#include "system/utils.h"
#include "entity/player.h"
#include "entity/npc.h"
#include "const.h"

//These will just be used with function pointers to load entities and other values temporarily.
//Eventually level data should be loaded from files.
static void LoadOverworld(Level* level)
{
	printf("Loading Overworld\n");
	level->model = RES_LoadModel("assets/models/super_mega_world4.glb");
	level->collisionModel = level->model;

	level->background = RES_LoadTexture("assets/textures/skybox_03.png");

	level->player = NewEntity();
	Player_New(level->player, (Vector3) { 4.0f, 3.0f, 4.0f });

	Entity* npc1 = NewEntity();
	NPC_New(npc1, (Vector3) { -0.98f, 0.1f, 22.8f }, 0);

	Entity* npc2 = NewEntity();
	NPC_New(npc2, (Vector3) { 18.13f, 0.1f, 6.38f }, -90);

	level->portalCount = 1;
	level->portals = (LevelPortal*) malloc(sizeof(LevelPortal) * level->portalCount);
	memset(level->portals, 0, sizeof(LevelPortal)* level->portalCount);

	level->portals[0] = (LevelPortal){
		.levelId = LEVEL_HOTEL,
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
	memset(level->spawnPoints, 0, sizeof(SpawnPoint)* level->spawnPointCount);

	level->spawnPoints[0] = (SpawnPoint) {
		.id = "entrance",
		.pos = (Vector3){42.7f,5.1f,-43.35f},
		.rotY = 0
	};
}

static void LoadHotel(Level* level)
{
	printf("Loading Hotel");
	level->model = RES_LoadModel("assets/models/hotel_01.glb");
	level->collisionModel = level->model;

	level->background = RES_LoadTexture("assets/textures/skybox_hotel_test.png");

	level->player = NewEntity();
	Player_New(level->player, (Vector3) { 0.0f, 0.0f, 0.0f });

	level->spawnPointCount = 2;
	level->spawnPoints = (SpawnPoint*) malloc(sizeof(SpawnPoint) * level->spawnPointCount);
	memset(level->spawnPoints, 0, sizeof(SpawnPoint)* level->spawnPointCount);

	level->spawnPoints[0] = (SpawnPoint) {
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
	memset(level->portals, 0, sizeof(LevelPortal)* level->portalCount);

	level->portals[0] = (LevelPortal){
		.levelId = LEVEL_OVERWORLD,
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

//This really shouldn't be an array of Levels, but LevelDefinitions
static Level levelDB[MAX_LEVELS] = {
	[LEVEL_OVERWORLD] = {
		.load = LoadOverworld,
		.unload = NULL
	},
	[LEVEL_HOTEL] = {
		.load = LoadHotel,
		.unload = NULL
	}
};

Level* Level_GetLevel(LEVEL_DEF_ID id)
{
	if (id >= 0 && id < MAX_LEVELS)
		return &levelDB[id];
}

void Level_Load(Level* level)
{
	printf("Loading Level\n");
	//Currently using a callback until a file format has been worked out.
	//May still use it for scripted logic.
	if (level->load)
		level->load(level);

	SpawnPoint* end = level->spawnPoints + level->spawnPointCount;
	for (SpawnPoint* cur = level->spawnPoints; cur < end; cur++)
	{
		if (strcmp(gGame.nextSpawn, cur->id) == 0)
		{
			level->player->transform.position = cur->pos;
			level->player->transform.rotation.y = cur->rotY;
			break;
		}
	}

	printf("Level Load Complete\n");
}

void Level_Unload(Level* level)
{
	if (level->unload)
		level->unload(level);

	if (level->portalCount)
	{		
		free(level->portals);
	}
		
	if (level->spawnPointCount)
	{
		free(level->spawnPoints);
	}
		
}

void Level_Update(Level* level, float dt)
{
	//Check to see if player should warp.
	for (int i = 0; i < level->portalCount; i++)
	{
		LevelPortal* portal = &level->portals[i];
		if (CheckPointInBox(&level->player->transform.position, &portal->box))
		{
			Level_SetNext(portal->levelId, portal->spawnId);
		}
	}
}

void Level_SetNext(LEVEL_DEF_ID levelId, char* spawnId)
{
	gGame.nextLevel = levelId;

	if (!spawnId)return;

	char* p = spawnId;
	char* out = gGame.nextSpawn;
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
