#include "pickup.h"
#include "playState.h"
#include "common.h"

#define MAX_PICKUPS 100

static PickupData PickupPool[MAX_PICKUPS];
static int curPickup = 0;

static Entity* NewPickupEntity(Entity* e);

static ModelHandle model[MAX_PICKUP_TYPES];

void InitPickupPool(void)
{
	curPickup = 0;
	model[PICKUP_NULL] = RES_LoadModel("assets/models/pickup_01.glb");
	model[PICKUP_AMMO] = RES_LoadModel("assets/models/pickup_01.glb");
	model[PICKUP_MONEY] = RES_LoadModel("assets/models/pickup_money.glb");
	model[PICKUP_EXP] = RES_LoadModel("assets/models/pickup_exp.glb");
	model[PICKUP_HEALTH] = RES_LoadModel("assets/models/pickup_health.glb");

	for (int i = 0; i < MAX_PICKUPS; i++)
	{
		Entity* e = NewEntity();
		if (e)
		{
			NewPickupEntity(e);
			PickupData* pickup = &PickupPool[i];
			pickup->type = PICKUP_AMMO;
			pickup->amount = 20;
			pickup->entity = e;
			e->data = pickup;
		}
		else
		{
			printf("Could not create pickup entity.\n");
			return;
		}
	}

}

static void UpdatePickup(Entity* e, float dt)
{
	e->transform.rotation.y += dt * 300; //spin pickup
}

static void RenderPickup(Entity* e)
{
	RES_DrawModelEx(e->model, e->transform.position,(Vector3) { 0,1,0 }, e->transform.rotation.y, Vector3One());
}

static Entity* NewPickupEntity(Entity* e)
{
	e->bActive = 0; //should not be active until spawned.
	e->update = UpdatePickup;
	e->render = RenderPickup;

	e->model = model;

	//all collision logic will be handled by the player
	e->tag = ET_PICKUP;
	e->collider.type = CT_SPHERE;
	e->collider.radius = 0.5f;
	e->bFloat = 1;
	e->bPassthrough = 1;
	return e;
}

void SpawnPickup(Vector3 position, PICKUP_TYPE type, int amount)
{
	PickupData* pickup = &PickupPool[curPickup];
	pickup->type = type;
	pickup->amount = amount;
	pickup->entity->bActive = 1;
	pickup->entity->transform.position = position;

	curPickup = (curPickup + 1) % MAX_PICKUPS;

	pickup->entity->model = model[type];

}

//the position should be decided by the spawn zone
void SpawnRandomPickup(Vector3 position)
{
	PICKUP_TYPE type = GetRandomValue(PICKUP_NULL + 1, MAX_PICKUP_TYPES - 1);
	int amount = 10;
	if (type == PICKUP_AMMO)
	{
		amount = GetRandomValue(20, 30);
	}

	SpawnPickup(position, type, amount);
}
