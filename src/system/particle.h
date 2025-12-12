#ifndef PARTICLE_H
#define PARTICLE_H

#include <raylib.h>

typedef enum
{
	EMIT_DEFAULT,
	EMIT_SPREAD_OUT
}EMIT_TYPE;

typedef struct
{
	int bAlive;
	Vector3 position;
	Vector3 velocity;
	Vector3 acceleration;
	float life;
	float lifeTime;
	float size,startSize,endSize;
	float rotation;
	Color color, startColor, endColor;
	Texture* texture;
}Particle;

typedef struct
{
	Vector3 position;
	Vector3 direction;
	Vector3 force;
	float speed;
	float radius;
	float time;
	float spawnInterval;
	Vector2 lifeRange;
	Vector2 sizeRange;
	Vector2 rotationRange;
	float endSize;
	Texture* texture;
	Color startColor;
	Color endColor;
	Vector2 emitCount;
	EMIT_TYPE emitType;
}ParticleEmitter;

void InitParticleSystem(Camera* camera);
void UnloadParticleSystem(void);
void UpdateParticleSystem(float  dt);
void RenderParticleSystem(const Camera* camera);
void EmitParticle(ParticleEmitter* emitter);
void UpdateParticleEmitter(ParticleEmitter* emitter, float dt);
void DebugRender_ParticleEmitter(ParticleEmitter* emitter);


#endif