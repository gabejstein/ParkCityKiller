#include <stdio.h>
#include <stdlib.h>
#include "common.h"

#include "playState.h"
#include "titleState.h"

static void InitRayLib(void);
static void Unload(void);
static void InitGame(void);

int screenWidth = 1280;
int screenHeight = 720;

Game gGame = { 0 };

static RenderTexture2D target;

int main(int argc, char** argv)
{
    InitRayLib();
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

        //Update Logic Here

        if (gGame.update)
            gGame.update(deltaTime);

        //Rendering Logic Here
        BeginTextureMode(target);
            ClearBackground(RAYWHITE);

            if (gGame.render)
                gGame.render();

            if (gGame.fader.alpha > 0)
                DrawRectangle(0, 0, VIRTUAL_WINDOW_W, VIRTUAL_WINDOW_H, Fade(gGame.fader.color, gGame.fader.alpha));
        EndTextureMode();

        
        //Render Texture
        BeginDrawing();

        DrawTexturePro(
            target.texture,
            (Rectangle) {0.0f, 0.0f, (float)target.texture.width, (float)-target.texture.height},
            (Rectangle) {(GetScreenWidth() - ((float)VIRTUAL_WINDOW_W * scale)) * 0.5f,(GetScreenHeight() - ((float)VIRTUAL_WINDOW_H * scale)) * 0.5f,
                (float)VIRTUAL_WINDOW_W* scale, (float)VIRTUAL_WINDOW_H* scale},
            (Vector2) {0, 0}, 0.0f, WHITE);

        EndDrawing();

        deltaTime = GetFrameTime();
        deltaTime = MIN(deltaTime, 0.05);

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

    //SetTargetFPS(30);

    //Create render texture
    target = LoadRenderTexture(VIRTUAL_WINDOW_W, VIRTUAL_WINDOW_H);
    SetTextureFilter(target.texture, TEXTURE_FILTER_POINT);  // Texture scale filter to use

    InitAudioDevice();

    HideCursor();

    RES_Init();
}

static void InitGame(void)
{
    PlayState_Start();
}

static void Unload(void)
{
    if (gGame.unload)
        gGame.unload();

    RES_Unload();
    UnloadRenderTexture(target);
}