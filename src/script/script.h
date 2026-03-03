#ifndef SCRIPT_H
#define SCRIPT_H

typedef struct ScriptAction ScriptAction;

#define MAX_SCRIPT_TEXT 512
#define MAX_PARAM 12
#define MAX_ACTIONS 256

#include <raylib.h>
#include <stdbool.h>

typedef enum
{
    SCRIPT_ACTION_DIALOGUE,
    SCRIPT_ACTION_DIALOGUE_CONTINUE, //Does not wait for dismissal.
    SCRIPT_ACTION_SET_FLAG, //Sets a story flag.
    SCRIPT_ACTION_GET_FLAG, //should store in a register or something.
    SCRIPT_ACTION_GIVE_ITEM,
    SCRIPT_ACTION_REMOVE_ITEM,
    SCRIPT_ACTION_PLAY_SOUND,
    SCRIPT_ACTION_WAIT,
    SCRIPT_ACTION_WAIT_CAMERA,
    SCRIPT_ACTION_MOVE,
    SCRIPT_ACTION_CHANGE_MAP,
    SCRIPT_ACTION_MOVE_CAMERA,
    SCRIPT_ACTION_ANIMATE_NPC,
    SCRIPT_ACTION_FADE_IN,
    SCRIPT_ACTION_FADE_OUT,
    SCRIPT_ACTION_LABEL,
    SCRIPT_ACTION_JMP, //Unconditional Goto
    SCRIPT_ACTION_LDX, //Set x register
    SCRIPT_ACTION_BEQ, //Branch on equals
}SCRIPT_ACTION_TYPE;

typedef enum
{
    PARAM_INT,
    PARAM_FLOAT,
    PARAM_TEXT
}PARAM_TYPE;

typedef struct
{
    PARAM_TYPE type;
    union
    {
        int IO_Value;
        int iValue;
        float fValue;
        bool bValue;
    };
    char text[MAX_SCRIPT_TEXT]; //Should either allocate or maybe just have index into a text table instead.
}Value;

struct ScriptAction
{
    SCRIPT_ACTION_TYPE type;
    int nParamsCount;
    Value params[MAX_PARAM];
    bool bStarted;
    bool bFinished;
    bool bBlocking;
};

typedef struct
{
    ScriptAction actions[MAX_ACTIONS];
    unsigned int actionsCount;
    unsigned int curAction;
    //script context variables:
    float timer;
    int xReg;
    unsigned int dataSize;
    void* data; //may not keep this in the end, but could be useful for NPCs to perform actions on themselves.
}Script;

void Script_Save(const char* filename, Script* s);
void Script_Load(const char* filename);
void Script_Reset(Script* script);
int Script_Update(Script* s, float dt);
void Script_EndCurrentAction(Script* s);
//void EnterScriptState(Script* s); //old project
void Script_RunScript(Script* s);

//Helper functions for building in code.
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
