#include "dialogueBox.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../game.h"
#include "../const.h"

#define MAX_DISPLAY_LINES 4
#define CHAR_SPACING 8
#define LINE_SPACING 12
#define DEFAULT_TEXT_DELAY 3

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
    TEXT_COLOR_PURPLE = 0x05,
    TEXT_COLOR_GREEN = 0x06,
    TEXT_COLOR_BLACK = 0x07
}TextColorCode;

typedef enum TextSpeed
{
    TEXT_SPEED_SLOW = 0x01,
    TEXT_SPEED_NORMAL = 0x02,
    TEXT_SPEED_FAST = 0x03,
    TEXT_SPEED_SUPER_SLOW = 0x04,
}TextSpeed;

//global variables
static DialogueBox gDialogueBox;
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
static void DialogueBox_RenderText(void);

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
    .textDelay = DEFAULT_TEXT_DELAY
    };

    font = LoadFontEx("assets/fonts/ignore/PressStart2P-Regular.ttf", 8, 0, 128);
    nPatchTexture = LoadTexture("assets/textures/gui/panel_04.png");
    //For raylib npatch's: the first param is the area of src texture, 2nd,3rd is the top and right of the middle, and then top,right of bottom-right corner.
    nPatchInfo =(NPatchInfo) { (Rectangle) { 0,0,24,24 },8,8,16,16,NPATCH_NINE_PATCH };

    sfx = RES_LoadSound("assets/sounds/ignore/name.wav");
    typingSFX = RES_LoadSound("assets/sounds/select_007.ogg");
}

static void ChangeTextColor(TextColorCode colorCode)
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
    case TEXT_COLOR_GREEN:
        gDialogueBox.textColor = GREEN;
        break;
    case TEXT_COLOR_BLACK:
        gDialogueBox.textColor = BLACK;
        break;
    }
}

static void ChangeTextSpeed(TextSpeed code)
{
    switch (code)
    {
    case TEXT_SPEED_SLOW:
        gDialogueBox.textDelay = 8;
        break;
    case TEXT_SPEED_NORMAL:
        gDialogueBox.textDelay = DEFAULT_TEXT_DELAY;
        break;
    case TEXT_SPEED_FAST:
        gDialogueBox.textDelay = 1;
        break;
    case TEXT_SPEED_SUPER_SLOW:
        gDialogueBox.textDelay = 15;
        break;
    }
}

void DialogueBox_Render(void)
{
    DialogueBox* dBox = &gDialogueBox; //thought I'd future-proof this code a bit in case there are multiple boxes.

    //TODO delete later:
    if (!*dBox->dialogueText)return;

    //draw backing
    DrawTextureNPatch(nPatchTexture, nPatchInfo, dBox->rec, (Vector2) { 0, 0 }, 0.0f, WHITE);

    DialogueBox_RenderText();

    //Draw 'more text' charet
    if (dBox->state == DBOX_STATE_WAIT_NEXT)
    {
        int recX = dBox->rec.x + dBox->rec.width - 14;
        int recY = dBox->rec.y + dBox->rec.height - 14;
        DrawRectangle(recX, recY, 8, 8, WHITE);
    }
    else if (dBox->state == DBOX_STATE_END)
    {
        int recX = dBox->rec.x + dBox->rec.width - 14;
        int recY = dBox->rec.y + dBox->rec.height - 14;
        DrawRectangle(recX, recY, 8, 8, DARKGREEN);
    }
}

static void DialogueBox_RenderText(void)
{
    DialogueBox* dBox = &gDialogueBox; //thought I'd future-proof this code a bit in case there are multiple boxes.
    //TODO delete later:
    if (!*dBox->dialogueText)return;

    float padding = 10;
    int i;
    int x = (int)dBox->rec.x + padding;
    int y = (int)dBox->rec.y + padding;
    int xOffset = 0, yOffset = 0;

    //draw text
    dBox->textColor = WHITE;
    for (i = 0; i < dBox->textPos; i++)
    {
        
        int codepointByteCount = 0;
        int codepoint = GetCodepointNext(&dBox->curPage[i], &codepointByteCount);

        switch (codepoint) {
        case '\n':
            xOffset = 0;
            yOffset += LINE_SPACING;
            break;
        case '\0':
            dBox->state = DBOX_STATE_END;
            break;
        case ' ':
            xOffset += CHAR_SPACING;
            break;
        case CCODE_CHANGE_COLOR:
            codepoint = GetCodepointNext(&dBox->curPage[++i], &codepointByteCount);
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
        case CCODE_TEXT_SPEED:
            codepoint = GetCodepointNext(&dBox->curPage[++i], &codepointByteCount);
            ChangeTextSpeed(codepoint);
            break;
        case CCODE_END_PAGE:
            if (dBox->state == DBOX_STATE_TYPING)
            {
                dBox->state = DBOX_STATE_WAIT_NEXT;
            }
            return;
        default:
            DrawTextCodepoint(font, codepoint, (Vector2) { x+xOffset, y+yOffset }, font.baseSize, dBox->textColor);
            xOffset += CHAR_SPACING;

            //the typing sound should only be played for the last character typed,
            //otherwise, it will just keep playing each frame.
            if(!dBox->state==DBOX_STATE_TYPING && i==dBox->textPos-1 && dBox->timer == dBox->textDelay)
                RES_PlaySound(typingSFX);

            break;
        }
        
    }

    if (dBox->timer == 0)
    {
        dBox->timer = dBox->textDelay;
        dBox->textPos = i+1;
    }
    else
    {
        dBox->timer--;
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

    gDialogueBox.state = DBOX_STATE_TYPING;
}

void DialogueBox_AddText(char* text)
{
    DialogueBox_AddTextEx(text, NULL, NULL);
}

void DialogueBox_Reset(void)
{
    gDialogueBox.timer = 0;
    gDialogueBox.bFinished = 0;
}

void DialogueBox_Update(float dt)
{
    
    if (gDialogueBox.state==DBOX_STATE_WAIT_NEXT)
    {
        if (IsGamepadButtonPressed(0, GAMEPAD_BUTTON_RIGHT_FACE_RIGHT))
        {
            gDialogueBox.curPage += gDialogueBox.textPos;
            gDialogueBox.textPos = 0;
            gDialogueBox.timer = gDialogueBox.textDelay;
            gDialogueBox.state = DBOX_STATE_TYPING;
        }
    }
    else if (gDialogueBox.state == DBOX_STATE_END)
    {
        if (IsGamepadButtonPressed(0, GAMEPAD_BUTTON_RIGHT_FACE_RIGHT))
        {
            gDialogueBox.bFinished = true;
            if (gDialogueBox.endCallback)
                gDialogueBox.endCallback(gDialogueBox.callbackData);
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

