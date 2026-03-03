#include "player.h"
#include <raymath.h>
#include <stdio.h>
#include "../playState.h"
#include "bullet.h"
#include "pickup.h"

#include "../system/animation.h"
#include "../const.h"
#include "../gui/msgBox.h"

#include "npc.h"

typedef struct
{
	Vector3 center;
	float radius;
}InteractionZone;

static InteractionZone playerInteractionZone;
Entity* interactable = NULL;

static const float speed = 15.0f;
static const float turnSpeed = 200.0f;
static float jumpForce = 0.0;
static const float jumpHeight = 6.0f;
static float hurtTimer = 0;

static void Player_Update(Entity* p, float dt);
static void Player_Render(Entity* p);
static void Player_Unload(Entity* p);
static void Player_DebugRender(Entity* p);
static void Player_OnWorldHit(Entity* p, RayCollision groundHit);
static void Player_OnCollision(Entity* p, Entity* other);

static ModelHandle model;

static Model shadow;
static Texture shadowTexture;

static CH_AnimationController animController;

static int handBone = -1;

static SoundHandle gunSilencer;
static SoundHandle gunEmpty;
static SoundHandle gunCock;

//currently the indexes of the glb file. should standardize
typedef enum
{
	ANIM_IDLE=1,
	ANIM_RUN,
	ANIM_FALL,
	ANIM_JUMP,
	ANIM_AIM,
	ANIM_DEAD
	
}PLAYER_ANIM_STATE;

static PLAYER_STATE playerState = PLAYER_STATE_MOVE;
PLAYER_STATE GetPlayerState(void)
{
	return playerState;
}

static void UpdatePlayerMove(Entity* p, float dt);
static void UpdatePlayerAim(Entity* p, float dt);

void Player_Common_Init(void)
{
	jumpForce = sqrtf(2 * GRAVITY * jumpHeight);
	printf("jump force: %f\n", jumpForce);

	model = RES_LoadModel("assets/models/Beautiful_Body_01.glb");
	CH_LoadAnimationController(&animController, "assets/models/Beautiful_Body_01.glb");

	shadow = LoadModelFromMesh(GenMeshPlane(2, 2, 1, 1));
	shadowTexture = LoadTexture("assets/textures/shadow_02.png");
	shadow.materials[0].maps[MATERIAL_MAP_ALBEDO].texture = shadowTexture;

	//Sounds
	gunSilencer = RES_LoadSound("assets/sounds/gun_silencer.wav");
	gunEmpty = RES_LoadSound("assets/sounds/gun_empty.wav");
	gunCock = RES_LoadSound("assets/sounds/gun_hammer.wav");

	handBone = CH_GetBoneId(model, "hand.r");

	if (handBone > -1)
		printf("Found hand bone.\n");
	else
		printf("Could not find hand bone.\n");
}

void Player_Common_Unload(void)
{
	CH_UnloadAnimationController(&animController);
}

void Player_New(Entity* e, Vector3 position)
{
	e->update = Player_Update;
	e->render = Player_Render;
	e->unload = Player_Unload;
	e->debugRender = Player_DebugRender;
	e->onWorldCollision = Player_OnWorldHit;
	e->onCollision = Player_OnCollision;
	e->transform.position = position;
	e->tag = ET_PLAYER;

	playerState = PLAYER_STATE_MOVE;

	//Set animation settings
	CH_SetClipLoopIndex(&animController, ANIM_IDLE, 1);
	CH_SetClipLoopIndex(&animController, ANIM_RUN, 1);
	CH_SetClipLoopIndex(&animController, ANIM_AIM, 1);

	//Collisions
	e->mass = 1;
	e->collider.type = CT_SPHERE;
	e->collider.center = position;
	e->collider.offset = (Vector3){ 0,0.8,0 };
	e->collider.radius = 0.9f;

	interactable = NULL;
}

static void Player_Update(Entity* p, float dt)
{
	int gamepad = 0;

	p->velocity.x = p->velocity.z = 0;

	if (playerState == PLAYER_STATE_DEAD)
	{
		CH_PlayAnimationByIndex(&animController, ANIM_DEAD);
		return;
	}

	if (playerState == PLAYER_STATE_HURT)
	{
		hurtTimer -= dt;
		if (hurtTimer <= 0)
			playerState = PLAYER_STATE_MOVE;
	}
	
	if (IsGamepadButtonDown(gamepad, GAMEPAD_BUTTON_LEFT_TRIGGER_2) && p->bGrounded)
	{
		if (playerState != PLAYER_STATE_AIM)
			RES_PlaySound(gunCock);
		playerState = PLAYER_STATE_AIM;
		UpdatePlayerAim(p, dt);
		return;
	}
	else
	{
		playerState = PLAYER_STATE_MOVE;
		UpdatePlayerMove(p, dt);
	}

	
}

static void UpdatePlayerMove(Entity* p, float dt)
{
	int gamepad = 0;
	float deadZone = 0.2;
	float horAxis = GetGamepadAxisMovement(gamepad, GAMEPAD_AXIS_LEFT_X);
	float vertAxis = GetGamepadAxisMovement(gamepad, GAMEPAD_AXIS_LEFT_Y);

	if (fabs(horAxis) < deadZone) horAxis = 0.0f;
	if (fabs(vertAxis) < deadZone) vertAxis = 0.0f;

	Vector2 dirVec = (Vector2){ -horAxis,-vertAxis };

	float magnitude = Vector2Length(dirVec);

	if (magnitude > 0)
	{
		dirVec = Vector2Normalize(dirVec);

		float angle = atan2f(dirVec.x, dirVec.y);
		p->transform.rotation.y = angle * RAD2DEG - gGame.mainCamera.transform.rotation.y - 90;

		float moveOffset = magnitude * speed;
		p->velocity = (Vector3){ p->transform.forward.x * moveOffset,p->velocity.y,p->transform.forward.z * moveOffset };

	}

	if (IsGamepadButtonPressed(gamepad, GAMEPAD_BUTTON_RIGHT_FACE_DOWN))
	{
		if (p->bGrounded)
		{
			p->bGrounded = 0;
			p->velocity.y += jumpForce;
		}

	}

	//Animation logic
	if (p->bGrounded)
	{
		if (magnitude > 0)
		{
			CH_PlayAnimationByIndex(&animController, ANIM_RUN);
			CH_UpdateAnimationController(&animController, dt * magnitude);
		}
		else
		{
			CH_PlayAnimationByIndex(&animController, ANIM_IDLE);
			CH_UpdateAnimationController(&animController, dt);
		}

	}
	else
	{
		//TODO: if velocity is positive, play jump anim, else play fall anim
		//Comment this out because its distracting for debugging the physics.
		if (p->velocity.y > 0)
			CH_PlayAnimationByIndex(&animController, ANIM_JUMP);
		else
			CH_PlayAnimationByIndex(&animController, ANIM_FALL);

		CH_UpdateAnimationController(&animController, dt);
	}

	playerInteractionZone.center = Vector3Add(p->transform.position, Vector3Scale(p->transform.forward, 5.3f));
	playerInteractionZone.center.y += 0.5f;
	playerInteractionZone.radius = 3.0f;

	interactable = Entity_QueryWorld_Sphere(playerInteractionZone.center, playerInteractionZone.radius);
	if (interactable)
	{
		if (interactable->tag == ET_NPC)
		{
			if(IsGamepadButtonPressed(gamepad,GAMEPAD_BUTTON_RIGHT_FACE_LEFT))
				NPC_Interact(interactable);
		}
		
	}
}

Entity* Player_GetInteractable(void) { return interactable; }

static void UpdatePlayerAim(Entity* p, float dt)
{
	int gamepad = 0;
	float deadZone = 0.2;
	float horAxis = GetGamepadAxisMovement(gamepad, GAMEPAD_AXIS_LEFT_X);
	float vertAxis = GetGamepadAxisMovement(gamepad, GAMEPAD_AXIS_LEFT_Y);

	if (fabs(horAxis) < deadZone) horAxis = 0.0f;
	if (fabs(vertAxis) < deadZone) vertAxis = 0.0f;

	float rightHorAxis = GetGamepadAxisMovement(gamepad, GAMEPAD_AXIS_RIGHT_X);
	float rightVertAxis = GetGamepadAxisMovement(gamepad, GAMEPAD_AXIS_RIGHT_Y);

	if (fabs(rightHorAxis) < deadZone) rightHorAxis = 0.0f;
	if (fabs(rightVertAxis) < deadZone) rightVertAxis = 0.0f;

	p->transform.rotation.y += -rightHorAxis * 100 * dt;

	/*Vector3 strafe = Vector3Scale(p->transform.right, -horAxis * dt * 10);
	strafe.y = p->velocity.y;
	p->velocity = strafe;*/

	if (IsGamepadButtonPressed(gamepad, GAMEPAD_BUTTON_RIGHT_FACE_LEFT))
	{
		if (gGame.playerStats.bullets > 0)
		{
			Vector3 firingPos = (Vector3){ 0,1.0f,0 };
			firingPos = Vector3Add(firingPos, Vector3Scale(p->transform.forward, 0.3f));
			firingPos = Vector3Add(p->transform.position, firingPos);
			Vector3 bulletVel = Vector3Scale(p->transform.forward, 40.0f);
			SpawnBullet(firingPos, bulletVel, p->tag);
			gGame.playerStats.bullets--;
			RES_PlaySound(gunSilencer);
			//SetGamepadVibration(gamepad, 1.0, 1.0, 1.0); //is currently unsupported for windows
		}
		else
		{
			RES_PlaySound(gunEmpty);
		}

	}

	CH_PlayAnimationByIndex(&animController, ANIM_AIM);
	CH_UpdateAnimationController(&animController, dt);

}

static void Player_Render(Entity* p)
{
	//draw shadow
	Vector3 shadowPos = p->groundPos;
	shadowPos.y += 0.1f;
	DrawModel(shadow, shadowPos, 1, WHITE);
	CH_UpdateModelAnimation(&animController, model);
	
	RES_DrawModelEx(model, p->transform.position, (Vector3) { 0, 1, 0 }, p->transform.rotation.y, Vector3One());
	
	//Update: This code now actually works, but only for position. The object's local rotation still needs to be worked out.
	/*Transform handBoneTrans = CH_GetBoneTransform(&animController, handBone);
	Matrix playerRotMat = MatrixRotate(UP, p->transform.rotation.y*DEG2RAD);
	Matrix playerTransMat = MatrixTranslate(p->transform.position.x, p->transform.position.y, p->transform.position.z);
	Matrix handLocalMat = MatrixMultiply(playerRotMat, playerTransMat);
	Vector3 handPos = Vector3Transform(handBoneTrans.translation, handLocalMat);

	DrawCube(handPos, 0.3, 0.3, 0.3, GREEN);*/
}

static void Player_DebugRender(Entity* p)
{
	Model* m = RES_GetModel(model);
	
	for (int i = 0; i < m->meshCount; i++)
	{
		/*Mesh* mesh = &m->meshes[i];
		for (int b = 0; b < mesh->boneCount; b++)
		{
			Vector3 bonePos =
		}*/
		//anim->framePoses;
		//Vector3 bonePos = Vector3Add(p->transform.position, m->bindPose[i].translation);
		//DrawSphereWires(bonePos, 0.08, 4, 4, GREEN);
	}

	DrawSphereWires(p->transform.position, 0.2f, 4, 4, ORANGE);

	//Draw Jump height
	DrawSphereWires((Vector3) { p->transform.position.x, p->groundPos.y + jumpHeight, p->transform.position.z }, 0.2, 4, 4, RED);

	//Note: This will probably jitter when drawn because of the collision system, but that's okay
	//since the purpose of this object is not visual.
	DrawSphereWires(playerInteractionZone.center, playerInteractionZone.radius, 4, 4, DARKPURPLE);
}

static void Player_Unload(Entity* p)
{
	printf("Unloading Player\n");
	
}

static void Player_OnWorldHit(Entity* p, RayCollision groundHit)
{

}

static void Player_OnCollision(Entity* p, Entity* other)
{
	if (playerState == PLAYER_STATE_DEAD)
		return;

	if (other->tag == ET_PICKUP)
	{
		PickupData* pickup = (PickupData*)other->data;
		char msg[256]; //TODO: make the formatter built into the message box.
		switch (pickup->type)
		{
		case PICKUP_AMMO:
			gGame.playerStats.bullets += pickup->amount;
			snprintf(msg, 256, "Picked up %d ammo",pickup->amount);
			PushMsgBox(msg);
			break;
		case PICKUP_HEALTH:
			gGame.playerStats.health += pickup->amount;
			PushMsgBox("Picked up a health pack");
			break;
		case PICKUP_MONEY:
			gGame.playerStats.money += pickup->amount;
			snprintf(msg, 256, "You got $%d", pickup->amount);
			PushMsgBox(msg);
			break;
		case PICKUP_EXP:
			snprintf(msg,256, "You got %d experience points", pickup->amount);
			PushMsgBox(msg);
			break;
		}

		other->bActive = 0;
	}

	if (other->tag == ET_BULLET && playerState!=PLAYER_STATE_HURT)
	{
		BulletData* bullet = (BulletData*)other->data;
		if (bullet->shooter == ET_ENEMY)
		{
			playerState = PLAYER_STATE_HURT;
			hurtTimer = 0.3f;
			gGame.playerStats.health--;
			if (gGame.playerStats.health <= 0)
			{
				playerState = PLAYER_STATE_DEAD;
				gGame.mainCamera.camera->position = Vector3Add(p->transform.position, (Vector3) { 5, 4, 0 });
			}
		}
	}
}