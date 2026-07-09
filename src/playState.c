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
#include "entity/npc.h"

#include "spawnZone.h"
#include "gui/selectionList.h"
#include "gui/msgBox.h"
#include "gui/dialogueBox.h"
#include "pauseMenuState.h"
#include "system/debug.h"

#include "const.h"

#include "script/script.h"

static const Color bgColor = { 0,0,0,255 };

static void PlayState_Start(void);
static void PlayState_Update(float dt);
static void PlayState_Render(void);
static void UpdatePlayerCamera(float dt);
static void UpdateDeathCamera(float dt);
static void PlayState_Unload(void);

static Entity* player;
static Vector3 startPos;

static ModelHandle levelModel;
static ModelHandle levelCollider;

static Camera camera = { 0 };

static TextureHandle crossHair;

static Script testScript;
static bool scriptRunning = false;

static void DrawHud(void)
{
	int x = 10, y = VIRTUAL_WINDOW_H-20;
	DrawText(TextFormat("Health: %d Bullets: %d Money: $%d", gGame.playerStats.health, gGame.playerStats.bullets, gGame.playerStats.money), x, y, 12, WHITE);
	
	Entity* interactable = Player_GetInteractable();

	//Context Pop-up for interactable objects
	if (interactable)
	{
		if (interactable->tag == ET_NPC)
		{
			//TODO: Actually measure size of text. Also, change text based on context.
			DrawRectangle(8, 8, 72, 14, GRAY);
			DrawText("SPEAK TO", 10, 10, 12, BLACK);
		}
	}
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
	Entity_Init();
	InitBulletPool();
	InitEnemyPool();
	InitPickupPool();
	InitSpawnZones();
	InitParticleSystem(&camera);

	ResetMsgBox();

	Level_Load(gGame.nextLevel);

	player = gGame.curLevel->player;
	startPos = player->transform.position;

	Enemy_SetPlayer(player);
	NPC_SetPlayer(player);

	levelModel = gGame.curLevel->model;
	levelCollider = gGame.curLevel->collisionModel;

	camera.position = (Vector3){ 3,3,3 };
	camera.target = (Vector3){ 0.0f, 0.8f, 0.0f };
	camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };
	camera.fovy = 45.0f;
	camera.projection = CAMERA_PERSPECTIVE;
	
	gGame.mainCamera.camera = &camera;
	gGame.mainCamera.transform.rotation = (Vector3){ 0.0,0.0,0.0 };

	crossHair = RES_LoadTexture("assets/textures/Crosshair_01.png");

	Script* s = &testScript;
	AddAction(s, Say("Bob: Yo\x05\x46 Rick\x05\x41! What's goin' on?\x04You still detectiving?"));
	AddAction(s, Wait(0.8f));
	AddAction(s, Say("Rick: Not much, Bobby!\x04Just tryin' to solve this case."));
	AddAction(s, Wait(0.8f));
	AddAction(s, Say("Bob: Okay."));
}

static void PlayState_Unload(void)
{
	printf("Unloading play state.\n");

	Entity_Unload();

	UnloadParticleSystem();
}

static void PlayState_Update(float dt)
{
	UpdateCamera(&camera, CAMERA_PERSPECTIVE);
	Vector3 cameraDir = Vector3Subtract(camera.position, camera.target);
	RES_UpdateShader(&camera.position, &cameraDir);
	UpdateParticleSystem(dt);
	UpdateMsgBox(dt);

	if (scriptRunning)
	{
		scriptRunning = Script_Update(&testScript, dt);
	}
	else
	{
		if (IsKeyPressed(KEY_S))
		{
			scriptRunning = true;
			Script_RunScript(&testScript);
		}
			
	}

	if (gGame.fader.alpha > 0)
	{
		gGame.fader.alpha -= dt;
		if (gGame.fader.alpha < 0)gGame.fader.alpha = 0;
	}

	if (!DialogueBox_IsDone())
		DialogueBox_Update(dt);

	Level_Update(gGame.curLevel, dt);
	UpdateEntities(dt);

	if (GetPlayerState() == PLAYER_STATE_DEAD)
		UpdateDeathCamera(dt);
	else
		UpdatePlayerCamera(dt);

	UpdateSpawnZones(dt, &player->transform.position);

#if DEBUG_TOOLS
	if (IsKeyPressed(KEY_X))
		gGame.bDebugMode = !gGame.bDebugMode;

	if (IsKeyPressed(KEY_P))
		Debug_PingConsole("Console Test!");

	Debug_PrintOverlay("FPS: %d", GetFrameTime());
	Debug_PrintOverlay("Player Pos: %f %f %f", player->transform.position.x, player->transform.position.y, player->transform.position.z);
#endif

	if (IsKeyPressed(KEY_C))
	{

		char* text = "The \x05\x46president\x05\x41 has been\nkidnapped by ninjas.\x04"
			"Are you a bad enough dude to\nrescue her?";

		char* text2 = "I'm sick of that other \x05\x46text\x05\x41.";

		DialogueBox_AddText(text);
	}
		

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

	BeginMode3D(camera);

	RES_DrawModel(levelModel, Vector3Zero(), 1);
	RenderEntities();
	RenderParticleSystem(&camera);

#if DEBUG_TOOLS
	if (gGame.bDebugMode)
	{
		DebugRenderEntities();

		//World origin gizmo
		Vector3 origin = (Vector3){ 0, 0.1, 0 };
		DrawLine3D(origin, Vector3Add(origin, (Vector3) { 1, 0, 0 }), RED); //X
		DrawLine3D(origin, Vector3Add(origin, (Vector3) { 0, 1, 0 }), GREEN); //Y
		DrawLine3D(origin, Vector3Add(origin, (Vector3) { 0, 0, 1 }), BLUE); //Z

		//Level Collider
		// currently doesnt work because of the custom shader.
		//RES_DrawModelWiresEx(levelCollider, Vector3Zero(),Vector3Zero(),0,Vector3One(),GREEN);
		Level_DebugRender(gGame.curLevel);

		Debug_RenderSpawnZones();
		
	}
#endif
	EndMode3D();

	//----------------HUD/GUI stuff here----------------
	DrawHud();
	DrawMsgBox();
	if (!DialogueBox_IsDone())
		DialogueBox_Render();

	if (GetPlayerState() == PLAYER_STATE_AIM)
		RES_DrawTexture(crossHair, VIRTUAL_WINDOW_W * 0.5 - crossHair->width * 0.5, VIRTUAL_WINDOW_H * 0.5 - crossHair->height);

	if (GetPlayerState() == PLAYER_STATE_DEAD)
		DrawText("HEALTHY GONE!!\nYOU ALL OVER!!",60,50,24,WHITE);

#if DEBUG_TOOLS
	if (gGame.bDebugMode)
	{
		Level_DebugRender2D(gGame.curLevel);
	}
#endif

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
	float smooth = MIN(1, 12.0 * dt);
	//If I dont smooth the target too then the camera follows the player too tightly.
	//If the player jitters too much, the camera jitters too much.
	Vector3 newTarget = Vector3Add(player->transform.position, lookOffset);
	cam->camera->target = Vector3Lerp(cam->camera->target, newTarget, smooth);
	
	smooth = MIN(1, 8.0 * dt);
	cam->camera->position = Vector3Lerp(cam->camera->position, cam->targetPos, smooth);
	cam->transform.position = cam->camera->position; //This is not very elegant. Should fix.
}

static void UpdateDeathCamera(float dt)
{
	UpdateCamera(gGame.mainCamera.camera, CAMERA_ORBITAL);
	gGame.mainCamera.camera->target = player->transform.position;
	
}

Entity* GetPlayer(void)
{
	return player;
}

