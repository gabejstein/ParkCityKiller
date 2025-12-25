#ifndef MGS_BOX_H
#define MSG_BOX_H

void InitMsgBox(void);
void ResetMsgBox(void);
void UpdateMsgBox(float dt);
void PushMsgBox(const char* msg);
void DrawMsgBox(void);

#endif
