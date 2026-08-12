#pragma once

#include <raylib.h>

typedef enum
{
	EFFECT_IMPACT,
	EFFECT_EXPLOSION
}EFFECT_TYPE;

void SpawnImpactEffect(Vector3 position, Vector3 rotation);
void SpawnExplosion(Vector3 position);
void Effect_Init_Common(Camera* camera);
void Effect_Update_Common(float dt);
void Effect_Render_Common(void);