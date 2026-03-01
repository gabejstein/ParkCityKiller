#include "entity.h"
#include <raymath.h>
#include "..\const.h"
#include "..\game.h"

//entities
#define MAX_ENTITY 1000
static Entity entities[MAX_ENTITY];
static int curEntity = 0;

static void ResolveSphereCollision(Entity* e1, Entity* e2);

void Entity_Init(void)
{
	curEntity = 0;
	memset(entities, 0, sizeof(entities));
}

void Entity_Unload(void)
{
	for (int i = 0; i < curEntity; i++)
	{
		Entity* e = &entities[i];
		if (e->unload)
			e->unload(e);
	}

	curEntity = 0;
}

Entity* NewEntity(void)
{
	if (curEntity >= MAX_ENTITY)
		return NULL;

	Entity* e = &entities[curEntity++];
	memset(e, 0, sizeof(Entity));
	e->bActive = 1;
	e->transform.rotation = Vector3Zero();
	e->collider.offset = Vector3Zero();
	e->collider.timeStep = 1;

	return e;
}

static RayCollision GetClosestLevelCollision(Vector3 position, Vector3 direction)
{
	ModelHandle levelCollider = gGame.curLevel->collisionModel;
	Model* level = RES_GetModel(levelCollider);

	Ray ray;
	ray.position = position;
	ray.direction = direction;
	RayCollision hit = { 0 };
	hit.distance = INFINITY;

	for (int i = 0; i < level->meshCount; i++)
	{
		RayCollision newHit = GetRayCollisionMesh(ray, level->meshes[i], level->transform);
		if (newHit.hit && newHit.distance < hit.distance)
		{
			hit = newHit;
		}
	}
	return hit;

}

static void HandleWorldCollisions_Sphere(Entity* e, float dt)
{
	if (e->velocity.y < 0)
		e->bGrounded = 0;

	e->collider.center = Vector3Add(e->transform.position, e->collider.offset);

	RayCollision hit = GetClosestLevelCollision(e->collider.center, (Vector3) { 0, -1, 0 });

	if (hit.hit)
	{
		int notSlope = hit.normal.y > 0.6;
		if (hit.distance <= e->collider.radius)
		{

			if (e->onWorldCollision)
				e->onWorldCollision(e, hit);

			if (notSlope)
			{
				e->bGrounded = 1;
				e->velocity.y = 0;
			}

			e->collider.center.y -= hit.normal.y * (hit.distance - e->collider.radius);
			if (!notSlope)
			{
				e->collider.center.x -= hit.normal.x * (hit.distance - e->collider.radius);
				e->collider.center.z -= hit.normal.z * (hit.distance - e->collider.radius);
			}


		}

		//2nd check with slight step buffer for going down slopes.
		if (!e->bGrounded && hit.distance <= e->collider.radius + 0.4f && e->velocity.y < 0)
		{
			if (notSlope)
			{
				e->collider.center.y -= hit.normal.y * (hit.distance - e->collider.radius);
				e->bGrounded = 1;
			}
		}

		e->groundPos = hit.point;
	}

	//get wall collisions
	Vector3 direction = Vector3Normalize(e->velocity);
	//direction.y = 0;

	hit = GetClosestLevelCollision(e->collider.center, direction);

	if (hit.hit)
	{
		if (hit.distance <= e->collider.radius)
		{
			if (e->onWorldCollision)
				e->onWorldCollision(e, hit);

			int notSlope = hit.normal.y > 0.6;
			if (notSlope)
			{
				e->bGrounded = 1;
				e->velocity.y = 0;
			}

			e->collider.center.y -= hit.normal.y * (hit.distance - e->collider.radius);
			if (!notSlope)
			{
				e->collider.center.x -= hit.normal.x * (hit.distance - e->collider.radius);
				e->collider.center.z -= hit.normal.z * (hit.distance - e->collider.radius);
			}

		}
	}

	e->transform.position = Vector3Subtract(e->collider.center, e->collider.offset);
	//doing this for now because the shadow needs to keep up with the player
	e->groundPos.x = e->transform.position.x;
	e->groundPos.z = e->transform.position.z;
}

void UpdateEntities(float dt)
{
	for (int i = 0; i < curEntity; i++)
	{
		Entity* e = &entities[i];
		if (!e->bActive)
			continue;

		//Note: -z is forward in opengl, but my models have z forward. Because of this, sin/cos are flipped for now.
		e->transform.forward = (Vector3){
			sinf(e->transform.rotation.y * DEG2RAD) * cosf(e->transform.rotation.x * DEG2RAD),
			-sinf(e->transform.rotation.x * DEG2RAD),
			cosf(e->transform.rotation.y * DEG2RAD) * cosf(e->transform.rotation.x * DEG2RAD) };

		e->transform.right = Vector3CrossProduct((Vector3) { 0, 1, 0 }, e->transform.forward);
		e->transform.up = Vector3CrossProduct(e->transform.forward, e->transform.right);

		if (e->update)
			e->update(e, dt);

		if (!HAS_FLAG(e->flags, ET_FLAG_STATIC))
		{
			if (!HAS_FLAG(e->flags, ET_FLAG_FLOAT))
			{
				e->acceleration.y += -GRAVITY;
			}

			e->velocity = Vector3Add(e->velocity, Vector3Scale(e->acceleration, dt));
			e->velocity.y = MAX(e->velocity.y, MAX_FALL);
			e->velocity.x = MAX(MIN(MAX_HOR_VEL, e->velocity.x), -MAX_HOR_VEL);
			e->velocity.z = MAX(MIN(MAX_HOR_VEL, e->velocity.z), -MAX_HOR_VEL);

			e->transform.position = Vector3Add(e->transform.position, Vector3Scale(e->velocity, dt));

			e->acceleration = Vector3Zero();

			if (e->collider.type == CT_SPHERE)
			{
				HandleWorldCollisions_Sphere(e, dt);
			}


		}
	}

	//Object-to-Object collisions
	for (int i = 0; i < curEntity; i++)
	{
		Entity* e1 = &entities[i];
		if (!e1->bActive) continue;

		for (int j = i + 1; j < curEntity; j++)
		{
			Entity* e2 = &entities[j];
			if (!e2->bActive) continue;

			if (e1->collider.type == CT_SPHERE && e2->collider.type == CT_SPHERE)
			{
				if (CheckCollisionSpheres(e1->collider.center, e1->collider.radius, e2->collider.center, e2->collider.radius))
				{
					if (e1->onCollision)
						e1->onCollision(e1, e2);

					if (e2->onCollision)
						e2->onCollision(e2, e1);

					if (!HAS_FLAG(e1->flags, ET_FLAG_PASSTHROUGH) && !HAS_FLAG(e2->flags, ET_FLAG_PASSTHROUGH))
					{
						if (e1->mass > e2->mass)
							ResolveSphereCollision(e1, e2);
						else
							ResolveSphereCollision(e2, e1);
					}

				}
			}

		}
	}
}

static void ResolveSphereCollision(Entity* e1, Entity* e2)
{
	e2->velocity = Vector3Zero();
	//Should push position by overlap amount
	Vector3 normal = Vector3Subtract(e2->collider.center, e1->collider.center);
	normal = Vector3Normalize(normal);

	Vector3 start = Vector3Subtract(e2->collider.center, Vector3Scale(normal, e2->collider.radius));
	Vector3 end = Vector3Add(e1->collider.center, Vector3Scale(normal, e1->collider.radius));
	float depth = Vector3Length(Vector3Subtract(end, start));

	Vector3 pushFactor = Vector3Scale(normal, depth);

	e2->transform.position = Vector3Add(e2->transform.position, pushFactor);

	if (e1->collider.center.y < e2->collider.center.y)
		e2->bGrounded = 1;
}

void RenderEntities(void)
{
	for (int i = 0; i < curEntity; i++)
	{
		Entity* e = &entities[i];

		if (!e->bActive)
			continue;

		if (e->render)
			e->render(e);
	}
}

void DebugRenderEntities(void)
{
	for (int i = 0; i < curEntity; i++)
	{
		Entity* e = &entities[i];

		if (!e->bActive)
			continue;

		if (e->debugRender)
			e->debugRender(e);

		Vector3 origin = e->transform.position;
		DrawSphereWires(origin, 0.1f, 4, 4, BLUE);

		if (e->collider.type == CT_SPHERE)
		{
			DrawSphereWires(e->collider.center, e->collider.radius, 4, 6, GREEN);
		}

	}

}