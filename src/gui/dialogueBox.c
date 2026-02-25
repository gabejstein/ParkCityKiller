#include "dialogueBox.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../game.h"
#include "../const.h"

#define MAX_DISPLAY_LINES 4

#define CCODE_BLUE_TEXT "<color blue>"
#define CCODE_RED_TEXT "<color red>"
#define CCODE_WHITE_TEXT "<color white>"
#define CCODE_PLAY_SOUND "<playsfx>"
#define CCODE_END_PAGE "<eop>"
#define CCODE_PLAYER_NAME "<player>"

//global variables
static DialogueBox gDialogueBox;
static float dialogueSpeed = 0.1f; //TODO: Move to a settings struct.
static const int defaultBoxWidth = VIRTUAL_WINDOW_W*0.9;
static const int defaultBoxHeight = VIRTUAL_WINDOW_H*0.3;
static int lineSpacing = 4;
static  Font font;
static Texture2D nPatchTexture;
static NPatchInfo nPatchInfo;

//static functions
static void UpdateChoice(float dt);

void DialogueBox_Init(void)
{
    memset(&gDialogueBox, 0, sizeof(gDialogueBox));

    gDialogueBox = (DialogueBox){
    .rec = {
        .x = VIRTUAL_WINDOW_W * 0.5 - defaultBoxWidth * 0.5,
        .y = VIRTUAL_WINDOW_H - defaultBoxHeight - 10,
        .width = defaultBoxWidth,
        .height = defaultBoxHeight+5
        },
    .textColor = BLACK,
    };

    font = LoadFontEx("assets/fonts/ignore/PressStart2P-Regular.ttf", 8, 0, 128);
    nPatchTexture = LoadTexture("assets/textures/gui/panel_04.png");
    //For raylib npatch's: the first param is the area of src texture, 2nd,3rd is the top and right of the middle, and then top,right of bottom-right corner.
    nPatchInfo =(NPatchInfo) { (Rectangle) { 0,0,24,24 },8,8,16,16,NPATCH_NINE_PATCH };
}

void DialogueBox_Render(void)
{
    float padding = 10;

    //draw backing
    DrawTextureNPatch(nPatchTexture, nPatchInfo, gDialogueBox.rec, (Vector2) { 0, 0 }, 0.0f, WHITE);

    //draw text
    int y = (int)gDialogueBox.rec.y + padding;

    DrawTextEx(font, "This is some dummy\ntext. You can read this!\nLine break here.\nAnd line break here.", (Vector2) { (int)gDialogueBox.rec.x + padding, y }, 8, 2, gDialogueBox.textColor);
    
}

void CompileText(char* text)
{
    char* p = text;
    char* out = gDialogueBox.curDialogue;

    while (*p)
    {
        if (*p == '<')
        {
            if (strncmp(p, CCODE_BLUE_TEXT, strlen(CCODE_BLUE_TEXT) == 0))
            {
                *out++ = 5;
                p+= strlen(CCODE_BLUE_TEXT);
            }
            else
            {
                p++;
            }
        }
        else
        {
            *out++ = *p++;
        }
    }
}

void DialogueBox_AddTextEx(char* text, DialogueCallback callback, void* data)
{
    DialogueBox_Reset();
    CompileText(text);

    gDialogueBox.endCallback = callback;
    gDialogueBox.callbackData = data;
}

void DialogueBox_AddText(char* text)
{
    DialogueBox_AddTextEx(text, NULL, NULL);
}

void DialogueBox_Reset(void)
{
    gDialogueBox.timer = 0.0f;
    gDialogueBox.index = 0;
    gDialogueBox.bFinished = 0;
    gDialogueBox.bPageEnd = 0;
    gDialogueBox.curLine = 0;
    gDialogueBox.textColor = WHITE;
}

void DialogueBox_Update(float dt)
{
    if (IsGamepadButtonPressed(0, GAMEPAD_BUTTON_RIGHT_FACE_DOWN))
    {
        gDialogueBox.bFinished = true;
    }
}

void DialogueBox_Unload(void)
{
    UnloadFont(font);
    UnloadTexture(nPatchTexture);
}

bool DialogueBox_IsDone(void)
{
    return gDialogueBox.bFinished;
}

static void UpdateChoice(float dt)
{

}

