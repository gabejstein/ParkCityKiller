#include <stdio.h>
#include <stdlib.h>
#include <raylib.h>
#include <raymath.h>

#include "const.h"
#include "game.h"

#include "playState.h"
#include "titleState.h"

#include "gui/msgBox.h"
#include "gui/dialogueBox.h"
#include "entity/player.h"
#include "entity/npc.h"

static void InitRayLib(void);
static void Unload(void);
static void InitGame(void);
static int CheckNextLevel(void);

int screenWidth = 1280;
int screenHeight = 720;

Game gGame = { 0 };

static RenderTexture2D target;

int main(int argc, char** argv)
{
    InitRayLib();
    InitMsgBox();
    DialogueBox_Init();
    InitGame();

    float deltaTime = 0.0f;
    float scale = 0.0f;

    while (!WindowShouldClose())
    {
        scale = MIN((float)GetScreenWidth() / VIRTUAL_WINDOW_W, (float)GetScreenHeight() / VIRTUAL_WINDOW_H);

        //Virtual mouse that adapts to render texture
        Vector2 mouse = GetMousePosition();
        gGame.virtualMouse.x = (mouse.x - (GetScreenWidth() - (VIRTUAL_WINDOW_W * scale)) * 0.5f) / scale;
        gGame.virtualMouse.y = (mouse.y - (GetScreenHeight() - (VIRTUAL_WINDOW_H * scale)) * 0.5f) / scale;
        gGame.virtualMouse = Vector2Clamp(gGame.virtualMouse,(Vector2) { 0, 0 },(Vector2) {VIRTUAL_WINDOW_W, VIRTUAL_WINDOW_H });

        deltaTime = GetFrameTime();
        deltaTime = MIN(deltaTime, SECS_PER_FRAME);

        if (CheckNextLevel())
        {
            continue;
        }
            

        //Update Logic Here
     
        if (gGame.stateStack[gGame.stateStackTop-1].update)
            gGame.stateStack[gGame.stateStackTop-1].update(deltaTime);

        //Rendering Logic Here
        BeginTextureMode(target);
            ClearBackground(RAYWHITE);

            for(int i=0;i<gGame.stateStackTop;i++)
                if (gGame.stateStack[i].render)
                    gGame.stateStack[i].render();

            if (gGame.fader.alpha > 0)
                DrawRectangle(0, 0, VIRTUAL_WINDOW_W, VIRTUAL_WINDOW_H, Fade(gGame.fader.color, gGame.fader.alpha));
        EndTextureMode();

        
        //Render Texture
        BeginDrawing();
        ClearBackground(BLACK);

        DrawTexturePro(
            target.texture,
            (Rectangle) {0.0f, 0.0f, (float)target.texture.width, (float)-target.texture.height},
            (Rectangle) {(GetScreenWidth() - ((float)VIRTUAL_WINDOW_W * scale)) * 0.5f,(GetScreenHeight() - ((float)VIRTUAL_WINDOW_H * scale)) * 0.5f,
                (float)VIRTUAL_WINDOW_W* scale, (float)VIRTUAL_WINDOW_H* scale},
            (Vector2) {0, 0}, 0.0f, WHITE);

        EndDrawing();

        //printf("Frame Time: %f\n", deltaTime);
    }

    CloseWindow();
    Unload();

    return 0;
}

static void InitRayLib(void)
{

    SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_VSYNC_HINT /* | FLAG_FULLSCREEN_MODE*/);
    InitWindow(screenWidth, screenHeight, "Park City Killer");
    SetWindowMinSize(VIRTUAL_WINDOW_W, VIRTUAL_WINDOW_H);

    SetTargetFPS(FPS);

    //Create render texture
    target = LoadRenderTexture(VIRTUAL_WINDOW_W, VIRTUAL_WINDOW_H);
    SetTextureFilter(target.texture, TEXTURE_FILTER_POINT);  // Texture scale filter to use

    InitAudioDevice();

    HideCursor();

    RES_Init();
}

static void InitGame(void)
{
    memset(gGame.stateStack, 0, sizeof(gGame.stateStack));
    gGame.stateStackTop = 0;
    gGame.bDebugMode = 0;
    gGame.curLevel = NULL;
    strcpy(gGame.nextLevel, "overworld");
    strcpy(gGame.nextSpawn, "start");
    PlayerStats_Init();
    Player_Common_Init();
    NPC_CommonInit();

    PushGameState(GetPlayState());
}

static void Unload(void)
{
    if (gGame.stateStack[gGame.stateStackTop - 1].unload)
        gGame.stateStack[gGame.stateStackTop - 1].unload();

    Player_Common_Unload();
    DialogueBox_Unload();
    RES_Unload();
    UnloadRenderTexture(target);
}

void PushGameState(GameState state)
{
    if (gGame.stateStackTop >= MAX_STATE_STACK) return;

    gGame.stateStack[gGame.stateStackTop] = state;
    if(gGame.stateStack[gGame.stateStackTop].start)
        gGame.stateStack[gGame.stateStackTop].start();

    gGame.stateStackTop++;
}

void PopGameState(void)
{
    if (gGame.stateStackTop <= 0) return;

    if(gGame.stateStack[gGame.stateStackTop - 1].unload)
        gGame.stateStack[gGame.stateStackTop-1].unload();

    gGame.stateStackTop--;
}

void ChangeGameState(GameState newState)
{
    if (gGame.stateStack[gGame.stateStackTop - 1].unload)
        gGame.stateStack[gGame.stateStackTop - 1].unload();

    gGame.stateStackTop = 0;

    gGame.stateStack[gGame.stateStackTop] = newState;
    if (gGame.stateStack[gGame.stateStackTop].start)
        gGame.stateStack[gGame.stateStackTop].start();

    gGame.stateStackTop++;
}

static int CheckNextLevel(void)
{
    if (gGame.nextLevel[0] == '\0')
        return 0;

    gGame.fader.alpha += 0.05f;
    if (gGame.fader.alpha < 1)
        return 0;
    else
        gGame.fader.alpha = 1;

    if (gGame.curLevel)
        Level_Unload(gGame.curLevel);

    ChangeGameState(GetPlayState());

    return 1;
}