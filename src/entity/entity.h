#ifndef ENTITY_H
#define ENTITY_H

#include <raylib.h>
#include "../system/transform.h"
#include "../system/resource.h"
#include "../system/utils.h"

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
	ET_PORTAL
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
	Vector3 center;
	Vector3 offset;
	COLLIDER_TYPE type;
	int timeStep;
	union
	{
		float radius;
		BoundingBox box;
	};
}CH_Collider;

typedef void(*EntityUpdate)(Entity*, float);
typedef void(*EntityRender)(Entity*);

struct Entity
{
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
	void (*onCollision)(Entity* self, Entity* other);
	void (*onWorldCollision)(Entity* self, RayCollision groundHit);
	void (*unload)(Entity* self);
	ENT_TAG tag;
	CH_Collider collider;
	ModelHandle model;
};

void Entity_Init(void);
void Entity_Unload(void);
Entity* NewEntity(void);
void UpdateEntities(float dt);
void RenderEntities(void);
void DebugRenderEntities(void);

#endif
