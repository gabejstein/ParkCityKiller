#include "npc.h"
#include <raymath.h>
#include <stdio.h>
#include <stdlib.h>
#include "../system/animation.h"
#include "../gui/dialogueBox.h"

static Model shadow;
static Texture shadowTexture;

//Just for testing.
static Entity* player;

void NPC_SetPlayer(Entity* e)
{
	player = e;
}

typedef enum
{
	ANIM_IDLE = 1,
	ANIM_RUN,
	ANIM_FALL,
	ANIM_JUMP,
	ANIM_AIM,
	ANIM_DEAD,
	ANIM_ATTACK_SWORD,
	ANIM_BOARD,
	ANIM_SPEAK
}ANIM_STATE;

typedef enum
{
	NPC_STATE_IDLE,
	NPC_STATE_MOVE,
	NPC_STATE_SPEAK
}NPC_STATE;

//TODO: create some sort of interactable object that's separate
//from the npc so it can be reused for things like signs, doors,etc.
//Maybe also create an actor object for cutscene-related stuff.
typedef struct
{
	int id;
	Vector3 headPos;
	CH_AnimationController animController;
	ANIM_STATE animState;
	NPC_STATE npcState;
}NPC_Data;

void NPC_CommonInit(void)
{
	shadow = LoadModelFromMesh(GenMeshPlane(2, 2, 1, 1));
	shadowTexture = LoadTexture("assets/textures/shadow_02.png");
	shadow.materials[0].maps[MATERIAL_MAP_ALBEDO].texture = shadowTexture;
}

void NPC_FacePosition(Entity* e, Vector3 target, float dt)
{
	Vector3 distVect = Vector3Subtract(target, e->transform.position);

	float targetAngle = atan2f(distVect.x, distVect.z) * RAD2DEG - e->transform.rotation.y;
	targetAngle = (int)(targetAngle + 180) % 360 - 180;

	e->transform.rotation.y += targetAngle * dt * 15;

}

static void NPC_Update(Entity* e, float dt)
{
	NPC_Data* npc = (NPC_Data*) e->data;
	switch (npc->npcState)
	{
		case NPC_STATE_IDLE:
			CH_PlayAnimationByIndex(&npc->animController, npc->animState);
			break;
		case NPC_STATE_MOVE:
			break;
		case NPC_STATE_SPEAK:
			NPC_FacePosition(e, player->transform.position, dt*0.3f);
			CH_PlayAnimationByIndex(&npc->animController, ANIM_SPEAK);
			if (CH_AnimationFinished(&npc->animController))
				npc->npcState = NPC_STATE_IDLE;
			break;
	}
	
	CH_UpdateAnimationController(&npc->animController, dt);
}

//Just using this to test animations for now.
void NPC_Interact(Entity* e)
{
	NPC_Data* npc = (NPC_Data*)e->data;
	npc->npcState = NPC_STATE_SPEAK;
	DialogueBox_AddText("The ol' shim sham!!!");
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

	data->npcState = NPC_STATE_IDLE;

	e->data = data;
}

