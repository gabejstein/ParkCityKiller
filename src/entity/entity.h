#ifndef ENTITY_H
#define ENTITY_H

#include <raylib.h>
#include "../system/transform.h"
#include "../system/resource.h"
#include "../system/utils.h"
#include <stdio.h>

#define MAX_ENTITY_NAME 256

typedef struct Entity Entity;

enum ET_FLAG
{
	ET_FLAG_STATIC = (1 << 0),
	ET_FLAG_FLOAT = (1 << 1),
	ET_FLAG_PASSTHROUGH = (1 << 2)
};

typedef enum
{
	ET_NULL,
	ET_PLAYER,
	ET_LEVEL,
	ET_NPC,
	ET_ENEMY,
	ET_BULLET,
	ET_PICKUP,
	ET_PORTAL,
	ET_AI_VEHICLE
}ENT_TAG;

typedef enum
{
	CT_NULL,
	CT_SPHERE,
	CT_BOX,
	CT_MESH
}COLLIDER_TYPE;

typedef struct
{
	ModelHandle model;
	float bounds; //radius for broadphase testing
}MeshCollider;

typedef struct
{
	Vector3 center;
	Vector3 offset;
	COLLIDER_TYPE type;
	int timeStep;
	union
	{
		float radius;
		BoundingBox box;
		MeshCollider meshCollider;
	};
}CH_Collider;

typedef struct Entity Entity;

typedef struct ContactInfo
{
	Vector3 point, normal;
	Entity* other;
	float distance, penetrationDepth;
	struct ContactInfo* next;
}ContactInfo;

typedef void(*EntityUpdate)(Entity*, float);
typedef void(*EntityRender)(Entity*);
typedef void(*EntityCollision)(Entity*, ContactInfo*);

struct Entity
{
	unsigned int id;
	char name[MAX_ENTITY_NAME];
	int health;
	int bActive;
	int flags;
	int bGrounded;
	CH_Transform transform;
	Vector3 velocity;
	Vector3 acceleration;
	Vector3 groundPos;
	float mass;
	void* data;
	EntityUpdate update;
	EntityRender render;
	EntityRender debugRender;
	EntityCollision onCollision;
	void (*onWorldCollision)(Entity* self, RayCollision groundHit);
	void (*unload)(Entity* self);
	ENT_TAG tag;
	CH_Collider collider;
	ModelHandle model;
};


void Entity_Init(void);
void Entity_Unload(void);
Entity* NewEntity(void);
Entity* LoadEntity(FILE* f);
Entity* Entity_GetById(unsigned int id);
Entity* Entity_GetByName(const char* name);
void UpdateEntities(float dt);
void RenderEntities(void);
void DebugRenderEntities(void);

Entity* Entity_QueryWorld_Sphere(Vector3 center, float radius);

#endif
