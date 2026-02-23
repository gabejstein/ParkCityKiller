#include "particle.h"
#include <raymath.h>
#include <rlgl.h>
#include <stdio.h>
#include <stdlib.h>

#define MAX_PARTICLES 1000

static Particle particlePool[MAX_PARTICLES];

static unsigned int nParticles = 0;

static Camera* gCamera = NULL;

static Shader shader = { 0 };

void InitParticleSystem(Camera* camera)
{
	gCamera = camera;
	nParticles = 0;
	memset(particlePool, 0, sizeof(particlePool));
	shader = LoadShader("assets/shaders/particle.vs", "assets/shaders/particle.fs");
	
}

void UnloadParticleSystem(void)
{
	UnloadShader(shader);
}

#ifndef UTILS_H
	static float GetRandomFloat(float min, float max)
	{
		int intMin = 0;
		int intMax = 10000; // Controls precision
		int r = GetRandomValue(intMin, intMax);
		float t = (float)r / (float)(intMax - intMin);
		return min + t * (max - min);
	}
#endif

static int particleDistComparator(const void* a, const void* b)
{
	Particle* pa = (Particle*)a;
	Particle* pb = (Particle*)b;

	float da = Vector3DistanceSqr(gCamera->position, pa->position);
	float db = Vector3DistanceSqr(gCamera->position, pb->position);

	return (da < db) ? 1: -1;
}

static Particle* GetParticle(void)
{
	if (nParticles >= MAX_PARTICLES)
		return NULL;

	return &particlePool[nParticles++];

}

static void KillParticle(unsigned int index)
{
	if (index < MAX_PARTICLES)
	{
		particlePool[index] = particlePool[--nParticles];
	}
	
}

void UpdateParticleSystem(float  dt)
{
	if (!nParticles)return;

	for (unsigned int i = 0; i < nParticles; i++)
	{
		Particle* p = &particlePool[i];

		if (p->bAlive)
		{
			p->life += dt;
			if (p->life >= p->lifeTime)
			{
				p->bAlive = 0;
				KillParticle(i);
				i--;
				continue;
			}
			
			float t = p->life / p->lifeTime;
			p->color = ColorLerp(p->startColor, p->endColor, t);
			p->size = Lerp(p->startSize,p->endSize,t);
			p->position = Vector3Add(p->position, Vector3Scale(p->velocity, dt));
			p->velocity = Vector3Add(p->velocity, Vector3Scale(p->acceleration, dt));

			//Normally you'd want to set acceleration back to zero, but we'll assume that there's a
			//force that's constantly being applied (ie gravity)
			
		}
		
	}
	
}

void RenderParticleSystem(const Camera* camera)
{
	if (!nParticles)return;

	qsort(particlePool, nParticles, sizeof(Particle), particleDistComparator);

	BeginShaderMode(shader);
	BeginBlendMode(BLEND_ADDITIVE);
	for (unsigned int i = 0; i < nParticles; i++)
	{
		Particle* p = &particlePool[i];

		if (p->bAlive)
		{
			if (p->texture)
			{
				Rectangle source = { 0.0f, 0.0f, (float)(*p->texture).width, (float)(*p->texture).height };
				Vector2 size = (Vector2){ p->size * fabsf((float)source.width / source.height), p->size };
				DrawBillboardPro(*camera, *p->texture, source, p->position,
					(Vector3) {	0, 1, 0	},
					size,
					Vector2Scale(size, 0.5),
					p->rotation, p->color);
			}
			else
			{
				//DrawCube(p->position, p->size, p->size, p->size, p->color);
				DrawSphere(p->position, p->size, p->color);
			}
			
		}
	}
	EndBlendMode();
	EndShaderMode();
	
}

void EmitParticle(ParticleEmitter* emitter)
{
	int count = GetRandomValue(emitter->emitCount.x, emitter->emitCount.y);

	for(int i = 0; i < count; i++)
	{
		Particle* p = GetParticle();
		if (p)
		{
			memset(p, 0, sizeof(Particle));
			float angle = GetRandomValue(0, 360);
			Vector3 offset = (Vector3){ cos(DEG2RAD * angle),0,sin(DEG2RAD * angle) };
			float radius = GetRandomFloat(0, emitter->radius);
			offset = Vector3Add(emitter->position, Vector3Scale(offset, radius));
			p->position = offset;
			p->rotation = GetRandomValue((int)emitter->rotationRange.x,(int) emitter->rotationRange.y);
			p->velocity = Vector3Scale(emitter->direction, emitter->speed);
			p->size = p->startSize = GetRandomFloat(emitter->sizeRange.x, emitter->sizeRange.y);
			p->endSize = emitter->endSize;
			p->texture = emitter->texture;
			p->life = 0.0f;
			p->lifeTime = GetRandomFloat(emitter->lifeRange.x, emitter->lifeRange.y);
			p->color = p->startColor = emitter->startColor;
			p->endColor = emitter->endColor;
			p->bAlive = 1;

			Vector3 dir;
			switch (emitter->emitType)
			{
			case EMIT_DEFAULT:
				p->acceleration = emitter->force;
				break;
			case EMIT_SPREAD_OUT:
				dir = Vector3Normalize(Vector3Subtract(p->position, emitter->position));
				p->acceleration = Vector3Multiply(dir, emitter->force);
				break;
			}

		}
	}
}

//Used when there's a need to update emitters each frame
void UpdateParticleEmitter(ParticleEmitter* emitter, float dt)
{
	emitter->time += dt;
	if (emitter->time >= emitter->spawnInterval)
	{
		emitter->time = 0.0f;
		EmitParticle(emitter);
	}
}

void DebugRender_ParticleEmitter(ParticleEmitter* emitter)
{
	//TODO: Get circles to face direction
	DrawCircle3D(emitter->position, 0.2f, (Vector3) { 1, 0, 0 }, 90, BLUE);
	DrawCircle3D(emitter->position, emitter->radius, (Vector3) { 1, 0, 0 }, 90, PURPLE);

	Vector3 dir = Vector3Add(emitter->position, Vector3Scale(emitter->direction, 0.3f));
	DrawLine3D(emitter->position, dir, RED);
}

//Under Construction
int SaveParticleEffect(const char* filePath, const ParticleEmitter* emitter)
{
	FILE* f = fopen(filePath, "wb");

	if (f)
	{

		return 1;
	}
	else
	{
		return 0;
	}
}