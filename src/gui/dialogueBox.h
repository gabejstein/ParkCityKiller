#ifndef DIALOGUE_BOX_H
#define DIALOGUE_BOX_H

#include <stdbool.h>
#include <stdint.h>
#include <raylib.h>

#define MAX_DIALOGUE 512

typedef void(*DialogueCallback)(void* data);

typedef struct
{
    char curDialogue[MAX_DIALOGUE];
    int curLine;
    Rectangle rec;
    float timer;
    unsigned int index;
    Color textColor;
    uint16_t bFinished : 1;
    uint16_t bPageEnd : 1;
    DialogueCallback endCallback;
    void* callbackData;
}DialogueBox;


void DialogueBox_Init(void);
void DialogueBox_Render(void);
void DialogueBox_Reset(void);
void DialogueBox_AddTextEx(char* text, DialogueCallback callback, void* data);
void DialogueBox_AddText(char* text);
void DialogueBox_Unload(void);
void DialogueBox_Update(float dt);
bool DialogueBox_IsDone(void);

#endif
