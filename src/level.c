#include "level.h"
#include <stdio.h>
#include <stdlib.h>
#include "playState.h"
#include "system/utils.h"
#include "entity/player.h"

//These will just be used with function pointers to load entities and other values temporarily.
//Eventually level data should be loaded from files.
static void LoadOverworld(Level* level)
{
	printf("Loading Overworld\n");
	level->model = RES_LoadModel("assets/models/super_mega_world4.glb");
	level->collisionModel = level->model;

	level->player = NewEntity();
	NewPlayer(level->player, (Vector3) { 4.0f, 3.0f, 4.0f });

	level->portalCount = 1;
	level->portals = (LevelPortal*) malloc(sizeof(LevelPortal) * level->portalCount);
	memset(level->portals, 0, sizeof(LevelPortal)* level->portalCount);

	level->portals[0] = (LevelPortal){
		.levelId = LEVEL_HOTEL,
		.position = (Vector3){10,0,10},
		.size = (Vector3){5,5,5},
		.spawnId = NULL
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

static void LoadHotel(Level* level)
{
	printf("Loading Hotel");
	level->model = RES_LoadModel("assets/models/hotel_01.glb");
	level->collisionModel = level->model;

	level->player = NewEntity();
	NewPlayer(level->player, (Vector3) { 0.0f, 0.0f, 0.0f });
}

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

	printf("Level Load Complete\n");
}

void Level_Unload(Level* level)
{
	if (level->unload)
		level->unload(level);

	if (level->portalCount)
		free(level->portals);
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

void Level_DebugRender(Level* level)
{
	if (!level->portalCount)return;

	for (int i = 0; i < level->portalCount; i++)
		DrawBoundingBox(level->portals[i].box, BLUE);
}

void Level_SetNext(LEVEL_DEF_ID levelId, char* spawnId)
{
	gGame.nextLevel = levelId;

	/*char* p = spawnId;
	char* out = gGame.nextSpawn;
	while (*p)
		*out = *(p++);*/
}