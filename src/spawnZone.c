#include "spawnZone.h"
#include "common.h"
#include "enemy.h"

typedef struct
{
	int bActive;
	Vector3 position;
	Vector3 size;
	BoundingBox bounds;
	int spawnMin, spawnMax;
	Color color; //just using to test collisions.
}SpawnZone;

#define MAX_SPAWN_ZONES 20
#define MIN_DIST_FROM_PLAYER 16 //squared distance

SpawnZone zones[MAX_SPAWN_ZONES];

static float timer = 0.0f;

void InitSpawnZones(void)
{
	//Just hardcoding some in for now.
	zones[0].bActive = 1;
	zones[0].position = (Vector3){ 80,0,0 };
	zones[0].spawnMin = 2;
	zones[0].spawnMax = 4;
	zones[0].size = (Vector3){ 30,3,30 };

	zones[1].bActive = 1;
	zones[1].position = (Vector3){ 80,0,80 };
	zones[1].spawnMin = 3;
	zones[1].spawnMax = 6;
	zones[1].size = (Vector3){ 50,3,50 };

	for (int i = 0; i < MAX_SPAWN_ZONES; i++)
	{
		SpawnZone* z = &zones[i];
		if (!z->bActive)continue;
		
		z->color = RED;
		z->bounds = (BoundingBox)
		{
			(Vector3) 
			{
				z->position.x - z->size.x / 2,z->position.y,z->position.z - z->size.z / 2
			},
			(Vector3) 
			{
				z->position.x + z->size.x / 2,z->position.y + z->size.y,z->position.z + z->size.z / 2
			}
		};
	}

	timer = 1.8f;
}

void UpdateSpawnZones(float dt,const Vector3* playerPos)
{
	
	for (int i = 0; i < MAX_SPAWN_ZONES; i++)
	{
		SpawnZone* z = &zones[i];
		if (!z->bActive)continue;

		if (CheckPointInBox(playerPos, &z->bounds))
		{
			z->color = ORANGE;

			if (timer >= 0)
			{
				timer -= dt;
				continue;
			}
			timer = 1.8f;

			printf("Spawning enemies.\n");
			int spawns = GetRandomValue(z->spawnMin, z->spawnMax);
			int nSpawned = 0;

			for (int s = 0; s < spawns; s++)
			{
				int spawnAttempts = 30;
				while (spawnAttempts > 0)
				{
					float xpos =(float) GetRandomValue((int)z->bounds.min.x,(int) z->bounds.max.x);
					float zpos = GetRandomValue((int)z->bounds.min.z,(int) z->bounds.max.z);
					float ypos = z->position.y;

					Vector3 pos = (Vector3){ xpos, ypos, zpos };

					float dist = Vector3DistanceSqr(*playerPos,pos);

					if (dist > MIN_DIST_FROM_PLAYER)
					{
						Vector3 camForward = Vector3Subtract(gGame.mainCamera.camera->target, gGame.mainCamera.camera->position);
						Vector3 posVector = Vector3Subtract(pos, gGame.mainCamera.camera->position);
						float inCameraView = Vector3DotProduct(camForward, posVector);

						if (inCameraView <= 0)
						{
							SpawnEnemy(pos);
							nSpawned++;
							break;
						}
						
					}

					spawnAttempts--;
					
				}

			}

			if (nSpawned)PushMsgBox("Hold it right there, Mack!");

			z->bActive = 0;
		}
		else
		{
			z->color = RED;
		}
	}
}

void Debug_RenderSpawnZones(void)
{
	for (int i = 0; i < MAX_SPAWN_ZONES; i++)
	{
		SpawnZone* z = &zones[i];
		if (!z->bActive)continue;

		DrawBoundingBox(z->bounds, z->color);
	}
}