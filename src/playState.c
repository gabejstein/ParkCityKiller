#include "playState.h"
#include "system/resource.h"
#include "system/particle.h"
#include <raymath.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "entity/entity.h"
#include "entity/bullet.h"
#include "entity/player.h"
#include "entity/enemy.h"
#include "entity/pickup.h"

#include "spawnZone.h"
#include "gui/selectionList.h"
#include "gui/msgBox.h"
#include "gui/dialogueBox.h"
#include "pauseMenuState.h"

#include "const.h"

static const Color bgColor = { 0,0,0,255 };

static void PlayState_Start(void);
static void PlayState_Update(float dt);
static void PlayState_Render(void);
static void UpdatePlayerCamera(float dt);
static void UpdateDeathCamera(float dt);
static void PlayState_Unload(void);
static void ResolveSphereCollision(Entity* e1, Entity* e2);

//entities
#define MAX_ENTITY 1000
static Entity entities[MAX_ENTITY];
static int curEntity = 0;
static Entity* player;
static Vector3 startPos;

static void UpdateEntities(float dt);
static void RenderEntities(void);
static void DebugRenderEntities(void);

static ModelHandle levelModel;
static ModelHandle levelCollider;

static Camera camera = { 0 };

TextureHandle background; //TODO: make this dependent on the level.
static TextureHandle crossHair;

static void DrawHud(void)
{
	int x = 10, y = VIRTUAL_WINDOW_H-20;
	DrawText(TextFormat("Health: %d Bullets: %d Money: $%d", gGame.playerStats.health, gGame.playerStats.bullets, gGame.playerStats.money), x, y, 12, WHITE);
	
}

GameState GetPlayState(void)
{
	GameState state = {0};

	state.start = PlayState_Start;
	state.update = PlayState_Update;
	state.render = PlayState_Render;
	state.unload = PlayState_Unload;

	return state;
}

static void PlayState_Start(void)
{
	curEntity = 0;
	memset(entities, 0, sizeof(entities));

	InitBulletPool();
	InitEnemyPool();
	InitPickupPool();
	InitSpawnZones();
	InitParticleSystem(&camera);

	ResetMsgBox();

	//just for testing purposes. delete later.
	//SpawnEnemy((Vector3) { 40, 0, -10 });

	for (int i = 0; i < 15; i++)
	{
		Vector3 pos;
		pos.x = GetRandomValue(-130, 260);
		pos.y = 0;
		pos.z = GetRandomValue(-176, 158);
		SpawnRandomPickup(pos);
	}

	Level_Load(gGame.curLevel);

	player = gGame.curLevel->player;
	//NewPlayer(player, (Vector3) { 4.0f, 3.0f, 4.0f });
	startPos = player->transform.position;

	Enemy_SetPlayer(player);
	
	printf("Entites in scene: %d\n", curEntity);

	levelModel = gGame.curLevel->model;
	levelCollider = gGame.curLevel->collisionModel;

	camera.position = (Vector3){ 3,3,3 };
	camera.target = (Vector3){ 0.0f, 0.8f, 0.0f };
	camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };
	camera.fovy = 45.0f;
	camera.projection = CAMERA_PERSPECTIVE;
	
	gGame.mainCamera.camera = &camera;
	gGame.mainCamera.transform.rotation = (Vector3){ 0.0,0.0,0.0 };

	background = RES_LoadTexture("assets/textures/skybox_03.png");
	crossHair = RES_LoadTexture("assets/textures/Crosshair_01.png");

}

static void PlayState_Unload(void)
{
	printf("Unloading play state.\n");

	for (int i = 0; i < curEntity; i++)
	{
		Entity* e = &entities[i];
		if (e->unload)
			e->unload(e);
	}

	curEntity = 0;

	UnloadParticleSystem();
}


static void PlayState_Update(float dt)
{
	if (gGame.fader.alpha > 0)
	{
		gGame.fader.alpha -= dt;
		if (gGame.fader.alpha < 0)gGame.fader.alpha = 0;
	}
	Level_Update(gGame.curLevel, dt);
	UpdateCamera(&camera, CAMERA_PERSPECTIVE);
	Vector3 cameraDir = Vector3Subtract(camera.position, camera.target);
	RES_UpdateShader(&camera.position, &cameraDir);
	UpdateParticleSystem(dt);
	UpdateMsgBox(dt);

	if (!DialogueBox_IsDone())
		DialogueBox_Update(dt);

	UpdateEntities(dt);

	if (GetPlayerState() == PLAYER_STATE_DEAD)
		UpdateDeathCamera(dt);
	else
		UpdatePlayerCamera(dt);

	UpdateSpawnZones(dt, &player->transform.position);

	if (IsKeyPressed(KEY_X))
		gGame.bDebugMode = !gGame.bDebugMode;

	if (IsKeyPressed(KEY_M))
		PushMsgBox("This is a message");

	if (IsKeyPressed(KEY_C))
	{
		char* text = "Hello, my name is\x05\x02 John Coxworth\x06.\x05\x01\nYo mama is\x05\x04 spa music\x05\x01!!!\nYou all over!!!";
		DialogueBox_AddText(text);
	}
		

	if (IsKeyPressed(KEY_J))
		Level_SetNext(LEVEL_HOTEL, NULL);

	if (IsKeyPressed(KEY_P))
		printf("Player Pos: %f %f %f\n", player->transform.position.x, player->transform.position.y, player->transform.position.z);


	if (IsKeyPressed(KEY_L))
	{
		player->transform.position = startPos;
		player->velocity = Vector3Zero();
	}

	if (IsGamepadButtonPressed(0, GAMEPAD_BUTTON_MIDDLE_RIGHT))
	{
		
		PushGameState(GetPauseMenuState());
	}
		
}

static void PlayState_Render(void)
{
	ClearBackground(bgColor);

	RES_DrawTexture(background, 0, 0);

	BeginMode3D(camera);

	RES_DrawModel(levelModel, Vector3Zero(), 1);
	RenderEntities();
	RenderParticleSystem(&camera);

	if (gGame.bDebugMode)
	{
		DebugRenderEntities();

		//World origin gizmo
		Vector3 origin = (Vector3){ 0, 2, 0 };
		DrawLine3D(origin, Vector3Add(origin, (Vector3) { 1, 0, 0 }), RED); //X
		DrawLine3D(origin, Vector3Add(origin, (Vector3) { 0, 1, 0 }), GREEN); //Y
		DrawLine3D(origin, Vector3Add(origin, (Vector3) { 0, 0, 1 }), BLUE); //Z

		//Level Collider
		// currently doesnt work because of the custom shader.
		//RES_DrawModelWiresEx(levelCollider, Vector3Zero(),Vector3Zero(),0,Vector3One(),GREEN);
		Level_DebugRender(gGame.curLevel);

		Debug_RenderSpawnZones();
	}

	EndMode3D();

	DrawHud();
	DrawMsgBox();
	if (!DialogueBox_IsDone())
		DialogueBox_Render();

	if (GetPlayerState() == PLAYER_STATE_AIM)
		RES_DrawTexture(crossHair, VIRTUAL_WINDOW_W * 0.5 - crossHair->width * 0.5, VIRTUAL_WINDOW_H * 0.5 - crossHair->height);

	if (GetPlayerState() == PLAYER_STATE_DEAD)
		DrawText("HEALTHY GONE!!\nYOU ALL OVER!!",60,50,24,WHITE);

	//printf("Vel y: %f\n", player->velocity.y);
}

static RayCollision HandleCameraCollisions(Vector3 target)
{
	Model* level = RES_GetModel(levelCollider);

	//TODO: remember that we want to set target position (not position) later.
	Ray ray;
	ray.position = player->transform.position;
	ray.direction = Vector3Normalize(Vector3Subtract(gGame.mainCamera.camera->position, target));
	RayCollision hit = { 0 };
	hit.distance = INFINITY;

	//Check ground
	for (int i = 0; i < level->meshCount; i++)
	{
		RayCollision newHit = GetRayCollisionMesh(ray, level->meshes[i], level->transform);
		if (newHit.hit && newHit.distance < 10.0f && newHit.distance < hit.distance)
		{
			hit = newHit;
		}
	}

	return hit;
}

static void UpdatePlayerCamera(float dt)
{

	//INPUT
	float mouseSensitivity = 100.0f;
	float minPitch = -20.0f;
	float maxPitch = 89.0f;
	GameCamera* cam = &gGame.mainCamera;
	int gamepad = 0;
	float horAxis = GetGamepadAxisMovement(gamepad, GAMEPAD_AXIS_RIGHT_X);
	float vertAxis = GetGamepadAxisMovement(gamepad, GAMEPAD_AXIS_RIGHT_Y);
	cam->transform.rotation.y += horAxis * mouseSensitivity * dt; //yaw
	cam->transform.rotation.x += vertAxis * mouseSensitivity * dt; //pitch
	cam->transform.rotation.x = Clamp(cam->transform.rotation.x, minPitch, maxPitch);

	//calculate camera forward
	Vector3 forward;
	forward.x = cosf(DEG2RAD * cam->transform.rotation.y) * cosf(DEG2RAD * cam->transform.rotation.x);
	forward.y = sinf(DEG2RAD * cam->transform.rotation.x);
	forward.z = sinf(DEG2RAD * cam->transform.rotation.y) * cosf(DEG2RAD * cam->transform.rotation.x);
	cam->transform.forward = forward;

	//TODO: have player's speed affect camera distance as well.
	float followDist = 10.0f;
	float minDist = 1.5f;

	//we don't really want the camera placed right at the player, since that would be at his feet.
	Vector3 playerTarget =Vector3Add(player->transform.position, (Vector3){ 0,1.2f,0.0f});
	Vector3 lookOffset = (Vector3){ 0,1.2,0 };

	if (GetPlayerState() == PLAYER_STATE_MOVE)
	{
		RayCollision wallHit = HandleCameraCollisions(playerTarget);
		if (wallHit.hit && wallHit.distance < followDist) //we only want collisions to affect things if they're closer, not further than the default
		{
			followDist = wallHit.distance;
		}

		followDist = MAX(minDist, followDist);

		Vector3 scaledForward = Vector3Scale(forward, followDist);

		cam->targetPos = Vector3Add(playerTarget, scaledForward);

		
	}
	else if (GetPlayerState()==PLAYER_STATE_AIM)
	{
		followDist = -2.0f;
		playerTarget.y = player->transform.position.y + 1.3f;
		playerTarget = Vector3Add(playerTarget,Vector3Scale(player->transform.right,-0.3f));
		lookOffset = Vector3Add(lookOffset, Vector3Scale(player->transform.right, -0.3f));

		Vector3 behindPlayer = Vector3Scale(player->transform.forward, followDist);
		cam->targetPos = Vector3Add(playerTarget, behindPlayer);
	}	

	//cam->camera->target = Vector3Add(player->transform.position, lookOffset);
	float smooth = MIN(1, 9.0 * dt);
	//If I dont smooth the target too then the camera follows the player too tightly.
	//If the player jitters too much, the camera jitters too much.
	Vector3 newTarget = Vector3Add(player->transform.position, lookOffset);
	cam->camera->target = Vector3Lerp(cam->camera->target, newTarget, smooth);
	
	smooth = MIN(1, 8.0 * dt);
	cam->camera->position = Vector3Lerp(cam->camera->position, cam->targetPos, smooth);
}

static void UpdateDeathCamera(float dt)
{
	UpdateCamera(gGame.mainCamera.camera, CAMERA_ORBITAL);
	gGame.mainCamera.camera->target = player->transform.position;
	
}

static RayCollision GetClosestLevelCollision(Vector3 position, Vector3 direction)
{
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
	if(e->velocity.y < 0)
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

static void UpdateEntities(float dt)
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
			if (!HAS_FLAG(e->flags,ET_FLAG_FLOAT))
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
				HandleWorldCollisions_Sphere(e,dt);
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
	
	Vector3 start = Vector3Subtract(e2->collider.center,Vector3Scale(normal, e2->collider.radius));
	Vector3 end = Vector3Add(e1->collider.center, Vector3Scale(normal, e1->collider.radius));
	float depth = Vector3Length(Vector3Subtract(end, start));

	Vector3 pushFactor = Vector3Scale(normal, depth);
	
	e2->transform.position = Vector3Add(e2->transform.position, pushFactor);

	if (e1->collider.center.y < e2->collider.center.y)
		e2->bGrounded = 1;
}

static void RenderEntities(void)
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

static void DebugRenderEntities(void)
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

Entity* GetPlayer(void)
{
	return player;
}

