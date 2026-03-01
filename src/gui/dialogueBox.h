#ifndef DIALOGUE_BOX_H
#define DIALOGUE_BOX_H

#include <stdbool.h>
#include <stdint.h>
#include <raylib.h>

#define MAX_DIALOGUE 512

typedef enum DialogueBoxState
{
    DBOX_STATE_TYPING,
    DBOX_STATE_WAIT_NEXT,
    DBOX_STATE_END
}DialogueBoxState;

typedef void(*DialogueCallback)(void* data);

typedef struct
{
    char dialogueText[MAX_DIALOGUE];
    char renderText[MAX_DIALOGUE]; //may be used to expand text symbols.
    int textPos;
    char* curPage;
    int nChoices; //How many choices to allow a user to select.
    Rectangle rec;
    int timer;
    int textDelay;
    Color textColor;
    DialogueBoxState state;

    uint16_t bFinished : 1;
    uint16_t bImmediate : 1; //Determines whether or not to draw all characters instantly.
    uint16_t bIgnoreInput : 1; //Ignores input. Used in cutscenes.
    uint16_t bDontSkip : 1;

    DialogueCallback endCallback;
    void* callbackData;
}DialogueBox;

void DialogueBox_Init(void);
void DialogueBox_Render(void);
void DialogueBox_Reset(void);
void DialogueBox_AddTextEx(char* text, DialogueCallback callback, void* data);
void DialogueBox_AddText(char* text);
void DialogueBox_Hide(void);
void DialogueBox_Unload(void);
void DialogueBox_Update(float dt);
bool DialogueBox_IsDone(void);

#endif
