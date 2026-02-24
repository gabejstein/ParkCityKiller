#ifndef DIALOGUE_BOX_H
#define DIALOGUE_BOX_H

#include <stdbool.h>

void DialogueBox_Init(void);
void DialogueBox_Render(void);
void DialogueBox_Reset(void);
void DialogueBox_AddText(char* text);
void DialogueBox_Unload(void);
void DialogueBox_Update(float dt);
bool DialogueBox_IsDone(void);

#endif
