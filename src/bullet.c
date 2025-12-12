#include "bullet.h"
#include "common.h"
#include "playState.h"

//bullet pool
#define MAX_BULLETS 100
#define BULLET_LIFE 50
#define BULLET_SIZE 0.1f

static Entity* bulletPool[MAX_BULLETS];
static BulletData bulletData[MAX_BULLETS];
static int curBullet = 0;

ParticleEmitter NewImpactEffect(Vector3 pos)
{
	ParticleEmitter emitter = (ParticleEmitter)
	{
		.position = pos,
		.direction = (Vector3){0,1.0f,0},
		.force = (Vector3){2.0,-9.0f,2.0f},
		.speed = 0.2f,
		.radius = 0.5f,
		.time = 0.0f,
		.spawnInterval = 0.8f,
		.lifeRange = {0.2f,0.5f},
		.sizeRange = {0.1f,0.2f},
		.endSize = 0.4f,
		.startColor = RED,
		.endColor = ORANGE,
		.emitCount = {5,10},
		.emitType = EMIT_DEFAULT
	};

	//emitter.endColor.a = 0;

	return emitter;
}

ParticleEmitter impact;

static void RenderBullet(Entity* e)
{
	DrawSphere(e->transform.position, BULLET_SIZE, ORANGE);
}

static void DebugRender(Entity* e)
{
	DebugRender_ParticleEmitter(&impact);
}

static void UpdateBullet(Entity* e, float dt)
{
	BulletData* b = (BulletData*)e->data;

	b->life--;
	if (b->life <= 0)
	{
		e->bActive = 0;
	}
}

static void OnCollision(Entity* self, Entity* other)
{
	BulletData* b = (BulletData*)self->data;
	if (other->tag != b->shooter)
	{
		self->bActive = 0;
		impact.position = self->transform.position;
		impact.direction = Vector3Normalize(self->velocity);
		EmitParticle(&impact);
	}
	
}

void InitBulletPool(void)
{
	curBullet = 0;

	for (int i = 0; i < MAX_BULLETS; i++)
	{
		Entity* e = NewEntity();
		if (e)
		{
			e->render = RenderBullet;
			e->debugRender = DebugRender;
			e->update = UpdateBullet;
			e->onCollision = OnCollision;
			e->tag = ET_BULLET;
			e->bActive = 0;
			e->bFloat = 1;
			e->bPassthrough = 1;
			BulletData* b = &bulletData[i];
			b->shooter = ET_NULL;
			b->damage = 1;
			b->life = BULLET_LIFE;

			e->data = b;

			e->collider.type = CT_SPHERE;
			e->collider.radius = BULLET_SIZE;

			bulletPool[i] = e;
		}
	}

	impact = NewImpactEffect((Vector3) { 5, 0, 5 });
}

void SpawnBullet(Vector3 pos, Vector3 vel, ENT_TAG shooter)
{
	Entity* b = bulletPool[curBullet];
	curBullet = (curBullet + 1) % MAX_BULLETS;

	b->bActive = 1;
	b->transform.position = pos;
	b->velocity = vel;
	((BulletData*)b->data)->shooter = shooter;
	((BulletData*)b->data)->life = BULLET_LIFE;

}