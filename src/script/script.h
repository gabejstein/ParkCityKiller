#ifndef SCRIPT_H
#define SCRIPT_H

typedef struct ScriptAction ScriptAction;

#define MAX_SCRIPT_TEXT 256
#define MAX_PARAM 12
#define MAX_ACTIONS 256

#include <raylib.h>

typedef enum
{
    ACT_SAY,
    ACT_PLAY_SOUND,
    ACT_WAIT,
    ACT_MOVE,
    ACT_CHANGE_MAP,
    ACT_MOVE_CAMERA,
    ACT_FADE_IN,
    ACT_FADE_OUT,
    ACT_LABEL,
    ACT_JMP, //Unconditional Goto
    ACT_LDX, //Set x register
    ACT_BEQ, //Branch on equals
}ACTION_TYPE;

typedef enum
{
    PARAM_INT,
    PARAM_FLOAT,
    PARAM_TEXT
}PARAM_TYPE;

typedef struct
{
    int type;
    union
    {
        int IO_Value;
        int iValue;
        float fValue;
    };
    char text[MAX_SCRIPT_TEXT];
}Value;

struct ScriptAction
{
    int type;
    int nParams;
    Value params[MAX_PARAM];
    int bStarted;
    int bFinished;
    int bBlocking;
};

typedef struct
{
    ScriptAction actions[MAX_ACTIONS];
    int actionsCount;
    int curAction;
    //script context variables:
    float timer;
    int xReg;
    void* owner; //may not keep this in the end, but could be useful for NPCs to perform actions on themselves.
}Script;

void SaveScript(const char* filename, Script* s);
void LoadScript(const char* filename);
void ResetScript(Script* script);
int UpdateScript(Script* s, float dt);
void EndCurrentAction(Script* s);
void EnterScriptState(Script* s);

//Helper functions
ScriptAction Wait(float time);
ScriptAction Label(int id);
ScriptAction JMP(int id);
ScriptAction LDX(int id);
ScriptAction BEQ(int val, int labelId);
//Domain-specific helpers
ScriptAction Say(const char* text);
ScriptAction FadeIn(float duration);
ScriptAction MoveCamera(Vector3 targetPos, Vector3 lookTarget);
ScriptAction PlaySoundEffect(int id);
void AddAction(Script* s, ScriptAction a);


#endif
