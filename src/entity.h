#ifndef ENTITY_H
#define ENTITY_H

#include <raylib.h>
#include "main.h"
#include "system/resource.h"

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

typedef struct Entity Entity;

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
	union
	{
		float radius;
		BoundingBox box;
	};
}CH_Collider;

struct Entity
{
	int health;
	int bActive; 
	int bStatic; //TODO: use bit flags for all of these.
	int bFloat;
	int bGrounded;
	int bPassthrough;
	CH_Transform transform;
	Vector3 velocity;
	float mass;
	void* data;
	void (*update)(Entity* self, float);
	void (*render)(Entity* self);
	void (*debugRender)(Entity* self);
	void (*onCollision)(Entity* self, Entity* other);
	void (*onWorldCollision)(Entity* self, RayCollision groundHit);
	void (*unload)(Entity* self);
	ENT_TAG tag;
	CH_Collider collider;
	ModelHandle model;
};

#endif
