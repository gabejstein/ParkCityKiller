#include "npc.h"
#include <raymath.h>
#include <stdio.h>
#include <stdlib.h>
#include "../system/animation.h"

static Model shadow;
static Texture shadowTexture;

typedef enum
{
	ANIM_IDLE = 1,
	ANIM_RUN,
	ANIM_FALL,
	ANIM_JUMP,
	ANIM_AIM,
	ANIM_DEAD

}ANIM_STATE;

//TODO: create some sort of interactable object that's separate
//from the npc so it can be reused for things like signs, doors,etc.
typedef struct
{
	int id;
	Vector3 headPos;
	CH_AnimationController animController;
	ANIM_STATE animState;
}NPC_Data;

void NPC_CommonInit(void)
{
	shadow = LoadModelFromMesh(GenMeshPlane(2, 2, 1, 1));
	shadowTexture = LoadTexture("assets/textures/shadow_02.png");
	shadow.materials[0].maps[MATERIAL_MAP_ALBEDO].texture = shadowTexture;
}

static void NPC_Update(Entity* e, float dt)
{
	NPC_Data* data = (NPC_Data*) e->data;
	CH_PlayAnimationByIndex(&data->animController, ANIM_IDLE);
	CH_UpdateAnimationController(&data->animController, dt);
}

static void NPC_Render(Entity* e)
{
	NPC_Data* data = (NPC_Data*)e->data;
	Vector3 shadowPos = e->groundPos;
	shadowPos.y += 0.1f;
	DrawModel(shadow, shadowPos, 1, WHITE);

	CH_UpdateModelAnimation(&data->animController, e->model);
	RES_DrawModelEx(e->model, e->transform.position, (Vector3) { 0, 1, 0 }, e->transform.rotation.y, Vector3One());
}

static void NPC_Unload(Entity* e)
{
	printf("Unloading NPC\n");

	NPC_Data* data = (NPC_Data*)e->data;
	CH_UnloadAnimationController(&data->animController);
	free(e->data);
}

void NPC_New(Entity* e, Vector3 pos, float rot)
{
	e->transform.position = pos;
	e->transform.rotation.y = rot;
	e->tag = ET_NPC;
	e->model = RES_LoadModel("assets/models/Beautiful_Body_01_Blue.glb");
	e->render = NPC_Render;
	e->update = NPC_Update;
	e->unload = NPC_Unload;

	SET_FLAG(e->flags, ET_FLAG_FLOAT);

	e->collider.type = CT_SPHERE;
	e->collider.center = pos;
	e->collider.offset = (Vector3){ 0,0.8,0 };
	e->collider.radius = 0.9f;

	NPC_Data* data = (NPC_Data*)malloc(sizeof(NPC_Data));
	memset(data, 0, sizeof(NPC_Data));

	data->headPos = (Vector3){ 0,1.5,0 };
	CH_LoadAnimationController(&data->animController, "assets/models/Beautiful_Body_01_Blue.glb");
	CH_SetClipLoopIndex(&data->animController, ANIM_IDLE, 1);
	CH_SetClipLoopIndex(&data->animController, ANIM_RUN, 1);
	CH_SetClipLoopIndex(&data->animController, ANIM_AIM, 1);
	data->animState = ANIM_IDLE;

	e->data = data;
}

