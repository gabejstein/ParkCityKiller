#include "level.h"
#include "entity.h"
#include <stdlib.h>

typedef struct
{
	unsigned int levelId;
	Vector3 warpPos;
}LevelPortal;

//might be better just to malloc these things since they dont need to be pooled.
#define MAX_PORTALS 20
LevelPortal portals[MAX_PORTALS];

static void UnloadPortal(Entity* e)
{
	LevelPortal* data = (LevelPortal*)e->data;
	free(data);
}

//This ignores the above array and mallocs a portal.
Entity* NewPortalEntity(Entity* e, Vector3 pos, Vector3 size)
{
	e->unload = UnloadPortal;
	e->transform.position = pos;
	e->tag = ET_PORTAL;
	e->bStatic = 1;
	e->bActive = 1;
	e->bPassthrough = 1;
	e->collider.type = CT_BOX;
	//e->collider.box = (BoundingBox){};
	LevelPortal* portal = (LevelPortal*)malloc(sizeof(LevelPortal));
	portal->warpPos = (Vector3){ 0,0,0 };
	portal->levelId = 0;
	e->data = portal;

}

//These will just be used with function pointers to load entities and other values temporarily.
void LoadOverworld(Level* level)
{
	level->model = RES_LoadModel("assets/models/super_mega_world4.glb");
	level->collisionModel = level->model;
}

void LoadHotel(Level* level)
{
	level->model = RES_LoadModel("assets/models/hotel_01.glb");
	level->collisionModel = level->model;
}

Level levelDB[MAX_LEVELS] = {
	[LEVEL_OVERWORLD] = {
		.Load = LoadOverworld,
		.Unload = NULL
	},
	[LEVEL_HOTEL] = {
		.Load = LoadHotel,
		.Unload = NULL
	}
};

Level* GetLevel(LEVEL_DEF_ID id)
{
	if (id > 0 && id < MAX_LEVELS)
		return &levelDB[id];
}