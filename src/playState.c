#include "playState.h"
#include "system/resource.h"
#include "system/particle.h"
#include "system/skybox.h"
#include "system/debug.h"
#include <raymath.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "effect.h"
#include "camera.h"

#include "entity/enemy.h"
#include "entity/entity.h"
#include "entity/bullet.h"
#include "entity/player.h"
#include "entity/npc.h"
#include "entity/pickup.h"

#include "gui/selectionList.h"
#include "gui/msgBox.h"
#include "gui/dialogueBox.h"
#include "pauseMenuState.h"

#include "const.h"

#include "script/script.h"

static const Color bgColor = { 0,0,0,255 };

static void PlayState_Start(void);
static void PlayState_Update(float dt);
static void PlayState_Render(void);
static void PlayState_Unload(void);

static Entity* player;
static Vector3 startPos;

static ModelHandle levelModel;
static ModelHandle levelCollider;

static Camera camera = { 0 };

static TextureHandle crossHair;

static Skybox skybox;
static TextureHandle backgroundTexture;

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
	InitPickupPool();
	InitParticleSystem(&camera);
	Effect_Init_Common(&camera);
	//Skybox_CommonInit();

	ResetMsgBox();

	Level_Load(gGame.nextLevel);

	if (gGame.curLevel->lighting.skyType == SKY_SKYBOX)
	{
		skybox.texturePath = gGame.curLevel->lighting.skyTexturePath;
		Skybox_Init(&skybox);
	}
	else if (gGame.curLevel->lighting.skyType == SKY_STATIC_IMAGE)
		backgroundTexture = RES_LoadTexture(gGame.curLevel->lighting.skyTexturePath);

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

	InitGameCamera(player, gGame.curLevel->collisionModel);

	crossHair = RES_LoadTexture("assets/textures/Crosshair_01.png");

}

static void PlayState_Unload(void)
{
	printf("Unloading play state.\n");

	Entity_Unload();

	UnloadParticleSystem();
	if(gGame.curLevel->lighting.skyType==SKY_SKYBOX)
		Skybox_Unload(&skybox);
	//Skybox_CommonUnload();
}

static void PlayState_Update(float dt)
{
	UpdateCamera(&camera, CAMERA_PERSPECTIVE);
	Vector3 cameraDir = Vector3Subtract(camera.position, camera.target);
	RES_UpdateShader(&camera.position, &cameraDir);
	UpdateParticleSystem(dt);
	Skybox_Update(&camera, dt);
	UpdateMsgBox(dt);
	Effect_Update_Common(dt);


	if (gGame.fader.alpha > 0)
	{
		gGame.fader.alpha -= dt;
		if (gGame.fader.alpha < 0)gGame.fader.alpha = 0;
	}

	/*if (!DialogueBox_IsDone())
		DialogueBox_Update(dt);*/
	gGame.bInCutscene = Script_Update(dt);

	//TODO: yeah, this is a crap hack.
	if (!gGame.bInCutscene)
	{
		gGame.mainCamera.state = CAMERA_STATE_PLAYER;
	}
	else
	{
		gGame.mainCamera.state = CAMERA_STATE_CUTSCENE;
	}
		

	Level_Update(gGame.curLevel, dt);
	UpdateEntities(dt);
	UpdateGameCamera(dt);


#if DEBUG_TOOLS
	if (IsKeyPressed(KEY_X))
		gGame.bDebugMode = !gGame.bDebugMode;

	Debug_PrintOverlay("Player speed: %f", Vector3Length(player->velocity));
	Debug_PrintOverlay("FPS: %d", GetFPS());
	Debug_PrintOverlay("Player's Pos: %f %f %f", player->transform.position.x, player->transform.position.y, player->transform.position.z);
#endif

	if (IsKeyPressed(KEY_C))
	{

		char* text = "The \x05\x46president\x05\x41 has been\nkidnapped by ninjas.\x04"
			"Are you a bad enough dude to\nrescue her?";

		char* text2 = "I'm sick of that other \x05\x46text\x05\x41.";

		DialogueBox_AddText(text);
	}
		
	if (IsKeyPressed(KEY_L))
	{
		player->transform.position = startPos;
		player->velocity = Vector3Zero();
	}

	if (IsKeyPressed(KEY_E))
	{
		SpawnExplosion((Vector3) {0,2,0});
	}
	if (IsKeyPressed(KEY_P))
	{
		SpawnRandomPickup((Vector3) { 0, 2, 0 });
	}


	if (IsGamepadButtonPressed(0, GAMEPAD_BUTTON_MIDDLE_RIGHT))
	{
		
		PushGameState(GetPauseMenuState());
	}

		
}

static void PlayState_Render(void)
{
	ClearBackground(bgColor);

	if(gGame.curLevel->lighting.skyType==SKY_STATIC_IMAGE)
		RES_DrawTexture(backgroundTexture, 0, 0);
	
	BeginMode3D(camera);
	if(gGame.curLevel->lighting.skyType==SKY_SKYBOX)
		Skybox_Draw(&skybox);
	Level_Render(gGame.curLevel);
	RenderEntities();
	Effect_Render_Common();
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
		
	}
#endif
	EndMode3D();

	//----------------HUD/GUI stuff here----------------
	DrawHud();
	DrawMsgBox();
	if (!DialogueBox_IsDone())
		DialogueBox_Render();

	if (gGame.playerStats.playerState == PLAYER_STATE_AIM)
		RES_DrawTexture(crossHair, VIRTUAL_WINDOW_W * 0.5 - crossHair->width * 0.5, VIRTUAL_WINDOW_H * 0.5 - crossHair->height);

	if (gGame.playerStats.playerState == PLAYER_STATE_DEAD)
		DrawText("HEALTHY GONE!!\nYOU ALL OVER!!",60,50,24,WHITE);

#if DEBUG_TOOLS
	if (gGame.bDebugMode)
	{
		Level_DebugRender2D(gGame.curLevel);
	}
#endif

}

Entity* GetPlayer(void)
{
	return player;
}

