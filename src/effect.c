#include "effect.h"
#include <stdbool.h>
#include <stdio.h>
#include <raymath.h>
#include "system/resource.h"
#include <assert.h>

//The KM stands for 'Kuma'
typedef struct KM_BillboardAnimation
{
	bool loop, isFinished;
	int curFrame, curRow;
	float time, frameDur;
	int rows, cols;
	float frameW, frameH;
	Rectangle drawRect;
}KM_BillboardAnimation;

bool KM_BillboardAnimation_IsFinished(KM_BillboardAnimation* anim) { return anim->isFinished; }

void KM_BillboardAnimation_Init(KM_BillboardAnimation* anim,Texture* texture, int cols, int rows)
{
	assert((rows != 0)&&(cols!=0));

	anim->isFinished = false;
	anim->loop = false;
	anim->time = 0;
	anim->frameDur = 0.05f;
	anim->rows = rows;
	anim->cols = cols;
	anim->frameW = (float)texture->width / cols;
	anim->frameH = (float)texture->height / rows;
	anim->curFrame = 0;
	anim->curRow = 0;
	anim->drawRect = (Rectangle) { 0,0,anim->frameW,anim->frameH };

}

void KM_BillboardAnimation_Update(KM_BillboardAnimation* anim, float dt)
{
	if (anim->isFinished)return;

	anim->time += dt;
	if (anim->time >= anim->frameDur)
	{
		anim->time = 0;

		if (++anim->curFrame >= anim->cols)
		{
			anim->curFrame = 0;
			if (++anim->curRow >= anim->rows)
			{
				if (anim->loop)
					anim->curRow = 0;
				else
					anim->isFinished = true;
			}
		}

		anim->drawRect.x = anim->frameW * anim->curFrame;
		anim->drawRect.y = anim->frameH * anim->curRow;

	}
}

void KM_BillboardAnimation_Render(const KM_BillboardAnimation* anim,const Camera* camera,Vector3 pos,const Texture* texture)
{
	DrawBillboardPro(*camera, *texture, anim->drawRect, pos, 
		(Vector3) { 0, 1, 0 }, 
		(Vector2) { 4, 4 }, 
		(Vector2) {2,2 }, 0, WHITE);
}

typedef struct Effect
{
	bool isActive;
	Vector3 position, rotation;
	float startScale, endScale;
	float timer;
	float lifeTime;
	EFFECT_TYPE type;
	void (*update)(struct Effect* effect, float dt);
	void (*render)(struct Effect* effect);
	union{
		KM_BillboardAnimation billboardAnim;
	};
}Effect;

#define MAX_EFFECTS 256
static Effect effectPool[MAX_EFFECTS];
static unsigned int curEffect;
static SoundHandle explosionSFX;
static Texture explosionTexture;
static Camera* sCamera = NULL;

//TODO: use a queue so you dont have to loop through everything
Effect* GetEffect(void)
{
	Effect* e = &effectPool[curEffect];
	curEffect = (curEffect + 1) % MAX_EFFECTS;
	return e;
}

static void Impact_Update(Effect* e, float dt)
{
	e->timer += dt;
	if (e->timer > e->lifeTime)
	{
		e->isActive = false;
	}
}

static void Impact_Render(Effect* e)
{
	DrawSphere(e->position, e->startScale, ORANGE);
}

static void Explosion_Update(Effect* e, float dt)
{
	/*e->startScale = Lerp(e->startScale,e->endScale,20.0f*dt);

	e->timer += dt;
	if (e->timer > e->lifeTime)
	{
		e->isActive = false;
	}*/
	KM_BillboardAnimation_Update(&e->billboardAnim, dt);
	e->isActive = !KM_BillboardAnimation_IsFinished(&e->billboardAnim);
}

static void Explosion_Render(Effect* e)
{
	//DrawSphere(e->position, e->startScale, RED);
	KM_BillboardAnimation_Render(&e->billboardAnim, sCamera, e->position, &explosionTexture);
}

void SpawnImpactEffect(Vector3 position, Vector3 rotation)
{
	Effect* e = GetEffect();
	e->isActive = true;
	e->position = position;
	e->rotation = rotation;
	e->update = Impact_Update;
	e->render = Impact_Render;
	e->timer = 0;
	e->lifeTime = 1.0f;
	e->startScale = 0.2;
}

void SpawnExplosion(Vector3 position)
{
	Effect* e = GetEffect();
	e->isActive = true;
	e->position = position;
	e->rotation = Vector3Zero();
	e->update = Explosion_Update;
	e->render = Explosion_Render;
	e->timer = 0;
	e->lifeTime = 0.5f;
	e->startScale = 0.4f;
	e->endScale = 4.0f;
	RES_PlaySound(explosionSFX);

	KM_BillboardAnimation_Init(&e->billboardAnim, &explosionTexture, 5, 3);
}

void Effect_Init_Common(Camera* camera)
{
	sCamera = camera;
	explosionSFX = RES_LoadSound("assets/sounds/explosion-6055.mp3");
	explosionTexture = LoadTexture("assets/textures/effect/explosion_sheet_01.png");
	SetTextureFilter(explosionTexture, TEXTURE_FILTER_BILINEAR);
}


void Effect_Update_Common(float dt)
{
	for (int i = 0; i < MAX_EFFECTS; i++)
	{
		Effect* e = &effectPool[i];
		if (!e->isActive)continue;

		if(e->update)
			e->update(e, dt);
	}
}

void Effect_Render_Common(void)
{
	for (int i = 0; i < MAX_EFFECTS; i++)
	{
		Effect* e = &effectPool[i];
		if (!e->isActive)continue;

		if (e->render)
			e->render(e);
	}
}