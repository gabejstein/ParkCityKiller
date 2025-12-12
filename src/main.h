#ifndef MAIN_H
#define MAIN_H

#include <raylib.h>
#include "level.h"
#include "gameWorld.h"

typedef struct
{
    float timer;
    float duration;
    float alpha;
    Color color;
}Fader;

typedef enum
{
    GAME_STATE_TITLE,
    GAME_STATE_PLAY,
    GAME_STATE_GAMEOVER,
    GAME_STATE_HIGHSCORE
}GAME_STATE;

typedef struct
{
    Vector3 position;
    Vector3 rotation;
    Vector3 forward;
    Vector3 right;
    Vector3 up;
    Vector3 scale;
}CH_Transform;

typedef struct
{
    Camera* camera;
    CH_Transform transform;
    Vector3 followDistance;
    Vector3 targetRot;
    Vector3 targetPos;
    float time;
}GameCamera;

typedef struct
{
    int bFullscreen;
}Settings;

typedef struct
{
    GAME_STATE state;
    void (*update)(float);
    void (*render)(void);
    void (*unload)(void);
    int score;
    int lives;
    Fader fader;
    Vector2 virtualMouse;
    GameCamera mainCamera;
    Settings settings;
    int bDebugMode;
    int bRunning;
    PlayerStats playerStats;
    Level* curLevel;
}Game;

extern Game gGame;

#endif
