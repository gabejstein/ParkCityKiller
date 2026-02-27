#include "dialogueBox.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../game.h"
#include "../const.h"

#define MAX_DISPLAY_LINES 4
#define CHAR_SPACING 8
#define LINE_SPACING 12
#define DEFAULT_TEXT_SPEED 0.08f

#define CCODE_END_PAGE 0x04
#define CCODE_CHANGE_COLOR 0x05
#define CCODE_PLAY_SOUND 0x06
#define CCODE_PLAYER_NAME 0x07
#define CCODE_TEXT_SPEED 0x08
#define CCODE_ICON 0x09

typedef enum TextColorCode
{
    TEXT_COLOR_WHITE = 0x01,
    TEXT_COLOR_RED = 0x02,
    TEXT_COLOR_BLUE = 0x03,
    TEXT_COLOR_YELLOW = 0x04,
    TEXT_COLOR_PURPLE = 0x05
}TextColorCode;

//global variables
static DialogueBox gDialogueBox;
static float dialogueSpeed = 0.1f; //TODO: Move to a settings struct.
static const int defaultBoxWidth = VIRTUAL_WINDOW_W*0.9;
static const int defaultBoxHeight = VIRTUAL_WINDOW_H*0.3;
static int lineSpacing = 4;
static  Font font;
static Texture2D nPatchTexture;
static NPatchInfo nPatchInfo;
static bool bPlayedSFX = false; //flag to prevent sound from being replayed.

SoundHandle typingSFX;
//Delete this later
SoundHandle sfx;

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
        .height = defaultBoxHeight + 5
        },
    .textColor = BLACK,
    .textSpeed = DEFAULT_TEXT_SPEED
    };

    font = LoadFontEx("assets/fonts/ignore/PressStart2P-Regular.ttf", 8, 0, 128);
    nPatchTexture = LoadTexture("assets/textures/gui/panel_04.png");
    //For raylib npatch's: the first param is the area of src texture, 2nd,3rd is the top and right of the middle, and then top,right of bottom-right corner.
    nPatchInfo =(NPatchInfo) { (Rectangle) { 0,0,24,24 },8,8,16,16,NPATCH_NINE_PATCH };

    sfx = RES_LoadSound("assets/sounds/ignore/name.wav");
    typingSFX = RES_LoadSound("assets/sounds/select_007.ogg");
}

void ChangeTextColor(TextColorCode colorCode)
{
    switch (colorCode)
    {
    case TEXT_COLOR_WHITE:
        gDialogueBox.textColor = WHITE;
        break;
    case TEXT_COLOR_RED:
        gDialogueBox.textColor = RED;
        break;
    case TEXT_COLOR_BLUE:
        gDialogueBox.textColor = BLUE;
        break;
    case TEXT_COLOR_YELLOW:
        gDialogueBox.textColor = YELLOW;
        break;
    case TEXT_COLOR_PURPLE:
        gDialogueBox.textColor = PURPLE;
        break;
    }
}

void DialogueBox_Render(void)
{
    //TODO delete later:
    if (!*gDialogueBox.dialogueText)return;

    float padding = 10;

    //draw backing
    DrawTextureNPatch(nPatchTexture, nPatchInfo, gDialogueBox.rec, (Vector2) { 0, 0 }, 0.0f, WHITE);

    //draw text
    int x = (int)gDialogueBox.rec.x + padding;
    int y = (int)gDialogueBox.rec.y + padding;

    gDialogueBox.textColor = WHITE;
    for (int i = 0; i < gDialogueBox.textPos; i++)
    {
        
        int codepointByteCount = 0;
        int codepoint = GetCodepointNext(&gDialogueBox.curPage[i], &codepointByteCount);

        switch (codepoint) {
        case '\n':
            x = (int)gDialogueBox.rec.x + padding;
            y += LINE_SPACING;
            break;
        case '\0':
            gDialogueBox.bPageEnd = true;
            break;
        case CCODE_CHANGE_COLOR:
            codepoint = GetCodepointNext(&gDialogueBox.curPage[++i], &codepointByteCount);
            ChangeTextColor(codepoint);
            break;
        case CCODE_PLAY_SOUND:
            if (!bPlayedSFX)
            {
                //TODO: Allow for different sounds to played.
                RES_PlaySound(sfx);
                bPlayedSFX = true;
            }
            break;
        case CCODE_END_PAGE:
          
            gDialogueBox.bPageEnd = true;
            break;
        default:
            DrawTextCodepoint(font, codepoint, (Vector2) { x, y }, font.baseSize, gDialogueBox.textColor);
            x += CHAR_SPACING;

            //the typing sound should only be played for the last character typed,
            //otherwise, it will just keep playing each frame.
            if(!gDialogueBox.bPageEnd && i==gDialogueBox.textPos-1)
                RES_PlaySound(typingSFX);
        }
        
    }

    //Draw 'more text' charet
    if (gDialogueBox.bPageEnd)
    {
        if (!gDialogueBox.bFinished)
        {
            int recX = gDialogueBox.rec.x + gDialogueBox.rec.width - 14;
            int recY = gDialogueBox.rec.y + gDialogueBox.rec.height - 14;
            DrawRectangle(recX, recY, 8, 8, WHITE);
        }
    }
    
}

void DialogueBox_AddTextEx(char* text, DialogueCallback callback, void* data)
{
    DialogueBox_Reset();
    bPlayedSFX = false;
    
    char* p = text;
    char* out = gDialogueBox.dialogueText;
    while ((*out++ = *p++));

    gDialogueBox.curPage = gDialogueBox.dialogueText;
    gDialogueBox.textPos = 0;

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
    gDialogueBox.bFinished = 0;
    gDialogueBox.bPageEnd = 0;
    gDialogueBox.textColor = WHITE;
}

void DialogueBox_Update(float dt)
{
    //One problem with this is that there will be a delay for the command codes too even if there's no text.
    gDialogueBox.timer += dt;
    if (!gDialogueBox.bPageEnd && gDialogueBox.timer > gDialogueBox.textSpeed)
    {
        gDialogueBox.timer = 0;
        gDialogueBox.textPos++;
    }

    if (gDialogueBox.bPageEnd)
    {
        if (IsGamepadButtonPressed(0, GAMEPAD_BUTTON_RIGHT_FACE_DOWN))
        {
            gDialogueBox.curPage += gDialogueBox.textPos;
            gDialogueBox.textPos = 0;
            gDialogueBox.bPageEnd = false;
        }
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

//void CompileText(char* text)
//{
//    char* p = text;
//    char* out = gDialogueBox.dialogueText;
//
//    while (*p)
//    {
//        if (*p == '<')
//        {
//            if (strncmp(p, CCODE_BLUE_TEXT, strlen(CCODE_BLUE_TEXT) == 0))
//            {
//                *out++ = 5;
//                p += strlen(CCODE_BLUE_TEXT);
//            }
//            else
//            {
//                p++;
//            }
//        }
//        else
//        {
//            *out++ = *p++;
//        }
//    }
//}

