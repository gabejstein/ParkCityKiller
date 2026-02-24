#include "dialogueBox.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <raylib.h>
#include "../game.h"
#include "../const.h"

#define MAX_LINE 100
#define MAX_CHAR 60
#define MAX_DISPLAY_LINES 4

typedef struct
{
    char textQueue[MAX_LINE][MAX_CHAR];
    int curLine;
    int rearLine;
    Rectangle rec;
    float timer;
    unsigned int index;
    bool bFinished;
    int choice;
}DialogueBox;

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
        .height = defaultBoxHeight
        }
    };

    font = LoadFontEx("assets/fonts/ignore/PressStart2P-Regular.ttf", 8, 0, 128);
    nPatchTexture = LoadTexture("assets/textures/gui/panel_04.png");
    //For raylib npatch's: the first param is the area of src texture, 2nd,3rd is the top and right of the middle, and then top,right of bottom-right corner.
    nPatchInfo =(NPatchInfo) { (Rectangle) { 0,0,24,24 },8,8,16,16,NPATCH_NINE_PATCH };
}

void DialogueBox_Render(void)
{
    float padding = 10;
    char displayText[MAX_CHAR];
    char* text = NULL;

    //draw backing
    DrawTextureNPatch(nPatchTexture, nPatchInfo, gDialogueBox.rec, (Vector2) { 0, 0 }, 0.0f, WHITE);

    //draw text
    int start = gDialogueBox.curLine;
    int end = MIN(gDialogueBox.curLine + MAX_DISPLAY_LINES, gDialogueBox.rearLine);

    int y = (int)gDialogueBox.rec.y + padding;
    for (int i = start; i < end; i++)
    {
        //just draw the lines for now without typewriter.

        DrawTextEx(font, gDialogueBox.textQueue[i], (Vector2) { (int)gDialogueBox.rec.x + padding, y }, 8, 2, WHITE);
        y += 8+lineSpacing;
    }
   
    /*text = gDialogueBox.textQueue[gDialogueBox.curLine];
    strncpy(displayText, text, gDialogueBox.index);
    displayText[gDialogueBox.index] = '\0';*/
    
}

void DialogueBox_AddText(char* text)
{
    if (gDialogueBox.rearLine >= MAX_LINE)return;

    strcpy(gDialogueBox.textQueue[gDialogueBox.rearLine++], text);
}

void DialogueBox_Reset(void)
{
    gDialogueBox.timer = 0.0f;
    gDialogueBox.index = 0;
    gDialogueBox.bFinished = 0;
    gDialogueBox.curLine = 0;
    gDialogueBox.rearLine = 0;
}

void DialogueBox_Update(float dt)
{
    /*int doneTyping = gDialogueBox.index >= strlen(gDialogueBox.textQueue[gDialogueBox.curLine]);

    if (!doneTyping)
    {
        gDialogueBox.timer += dt;
        if (gDialogueBox.timer > dialogueSpeed)
        {
            gDialogueBox.timer = 0;
            gDialogueBox.index++;
        }

    }*/
    
    if (IsKeyPressed(KEY_SPACE))
    {
        /*if (!doneTyping)
            gDialogueBox.index = strlen(gDialogueBox.textQueue[gDialogueBox.curLine]);
        else
        {*/
            gDialogueBox.curLine+=MAX_DISPLAY_LINES;
            if(gDialogueBox.curLine >= gDialogueBox.rearLine)
                gDialogueBox.bFinished = 1;
            else
            {
                gDialogueBox.timer = 0.0f;
                gDialogueBox.index = 0;
            }
        //}
            
       
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

