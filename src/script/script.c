#include "script.h"
#include "../gui/dialogueBox.h"
#include <stdbool.h>
#include <raymath.h>
#include "../system/resource.h"
#include "../entity/npc.h"
#include "../game.h"
#include "../const.h"
#include "../gui/dialogueBox.h"

extern Game gGame;

//script functions.
//could use function pointers instead and store them in an array.
static void Script_Dialogue(ScriptAction* a, float dt);
static void Script_Wait(ScriptAction* a, float dt);
static void Script_MoveCamera(ScriptAction* a, float dt);
static void Script_FadeIn(ScriptAction* a, float dt); //TODO: make both fades one action and allow color to be set.
static void Script_FadeOut(ScriptAction* a, float dt);
static void Script_PlaySound(ScriptAction* a, float dt);

void Script_Save(const char* filename, Script* s)
{

}

void Script_Load(const char* filename)
{

}

void Script_Reset(Script* script)
{
    script->curAction = 0;
    script->timer = 0;
    script->xReg = 0;

    for (int i = 0; i < script->actionsCount; i++)
        script->actions[i].bStarted = script->actions[i].bFinished = 0;
}

int Script_Update(Script* s, float dt)
{
    if (s->curAction < s->actionsCount)
    {
        ScriptAction* act = &s->actions[s->curAction];
        switch (act->type)
        {
        case SCRIPT_ACTION_DIALOGUE:   Script_Dialogue(act, dt); break;
        case SCRIPT_ACTION_FADE_IN: Script_FadeIn(act, dt); break;
        case SCRIPT_ACTION_FADE_OUT: Script_FadeOut(act, dt); break;
        case SCRIPT_ACTION_MOVE_CAMERA: Script_MoveCamera(act, dt); break;
        case SCRIPT_ACTION_PLAY_SOUND: Script_PlaySound(act, dt); break;
        case SCRIPT_ACTION_WAIT: Script_Wait(act, dt); break;
        case SCRIPT_ACTION_LABEL: break;
        case SCRIPT_ACTION_JMP:
            for (int i = 0; i < s->actionsCount; i++)
            {
                if (s->actions[i].type == SCRIPT_ACTION_LABEL && s->actions[i].params[0].iValue == act->params[0].iValue)
                    s->curAction = i;
            }
            break;
        case SCRIPT_ACTION_BEQ:
            if (s->xReg != act->params[0].iValue)break;
            for (int i = 0; i < s->actionsCount; i++)
            {
                if (s->actions[i].type == SCRIPT_ACTION_LABEL && s->actions[i].params[0].iValue == act->params[1].iValue)
                    s->curAction = i;
            }
            break;
        case SCRIPT_ACTION_LDX:
            s->xReg = act->params[0].iValue;
            break;
        }

        if(act->bFinished)
            s->curAction++;

        return 1;
    }
    else
    {
        return 0;
    }
}


void Script_EndCurrentAction(Script* s)
{
    s->actions[s->curAction].bFinished = 1;
    s->curAction++; //may not need this.
}

void Script_RunScript(Script* s)
{
    if (!s)return;
    Script_Reset(s);
}

//----------------------Script Helper Functions (Could be moved to macros instead)---------------------------------
ScriptAction Wait(float time)
{
    ScriptAction action = (ScriptAction){ .type = SCRIPT_ACTION_WAIT,.nParamsCount = 1 };
    action.params[0].type = PARAM_FLOAT;
    action.params[0].fValue = time;
    return action;
}

ScriptAction Label(int id)
{
    ScriptAction action = (ScriptAction){ .type = SCRIPT_ACTION_LABEL,.nParamsCount = 1 };
    action.params[0].type = PARAM_INT;
    action.params[0].iValue = id;
    return action;
}

ScriptAction JMP(int id)
{
    ScriptAction action = (ScriptAction){ .type = SCRIPT_ACTION_JMP,.nParamsCount = 1 };
    action.params[0].type = PARAM_INT;
    action.params[0].iValue = id;
    return action;
}

ScriptAction LDX(int id)
{
    ScriptAction action = (ScriptAction){ .type = SCRIPT_ACTION_LDX,.nParamsCount = 1 };
    action.params[0].type = PARAM_INT;
    action.params[0].iValue = id;
    return action;
}

ScriptAction BEQ(int val, int labelId)
{
    ScriptAction action = (ScriptAction){ .type = SCRIPT_ACTION_BEQ,.nParamsCount = 2 };
    action.params[0].type = PARAM_INT;
    action.params[0].iValue = val;

    action.params[1].type = PARAM_INT;
    action.params[1].iValue = labelId;
    return action;
}

ScriptAction PlaySoundEffect(int id)
{
    ScriptAction action = (ScriptAction){ .type = SCRIPT_ACTION_PLAY_SOUND,.nParamsCount = 1 };
    action.params[0].type = PARAM_INT;
    action.params[0].iValue = id;
    return action;
}

ScriptAction Say(const char* text)
{
    ScriptAction action = (ScriptAction){ .type = SCRIPT_ACTION_DIALOGUE,.nParamsCount = 1 };
    action.params[0].type = PARAM_TEXT;
    strcpy(action.params[0].text, text);
    return action;
}

ScriptAction FadeIn(float duration)
{
    ScriptAction action = (ScriptAction){ .type = SCRIPT_ACTION_FADE_IN,.nParamsCount = 1 };
    action.params[0].type = PARAM_FLOAT;
    action.params[0].fValue = MAX(0.001,duration); //clamp to prevent divide by zero errors.
    return action;
}

ScriptAction MoveCamera(Vector3 targetPos, Vector3 lookTarget)
{
    ScriptAction action = (ScriptAction){ .type = SCRIPT_ACTION_MOVE_CAMERA,.nParamsCount = 9 };
    action.params[0].type = PARAM_FLOAT;
    action.params[0].fValue = targetPos.x;

    action.params[1].type = PARAM_FLOAT;
    action.params[1].fValue = targetPos.y;

    action.params[2].type = PARAM_FLOAT;
    action.params[2].fValue = targetPos.z;

    //look target
    action.params[3].type = PARAM_FLOAT;
    action.params[3].fValue = lookTarget.x;

    action.params[4].type = PARAM_FLOAT;
    action.params[4].fValue = lookTarget.y;

    action.params[5].type = PARAM_FLOAT;
    action.params[5].fValue = lookTarget.z;
    return action;
}

void AddAction(Script* s, ScriptAction a)
{
    if (s->actionsCount >= MAX_ACTIONS) return;

    s->actions[s->actionsCount++] = a;
}

//-----------------------------Script Process Functions---------------------------------
static void Script_Dialogue(ScriptAction* a, float dt)
{
    if (!a->bStarted)
    {
        printf("Starting dialogue script.\n");
        a->bStarted = true;
        DialogueBox_AddText(a->params[0].text);
    }
    else
    {
        DialogueBox_Update(dt);
        if (DialogueBox_IsDone())
            a->bFinished = true;
    }
}

static void Script_Wait(ScriptAction* a, float dt)
{
    if (!a->bStarted)
    {
        printf("Starting wait.\n");
        a->params[1].fValue = 0;
        a->bStarted = true;
    }
    else
    {
        a->params[1].fValue += dt;
        float dur = a->params[0].fValue;
        if (a->params[1].fValue >= dur)
        {
            a->bFinished = true;
            printf("Ending wait.\n");
        }
            
    }
    
}

//Going to replace this with a camera_set_target, camera_wait action instead
static void Script_MoveCamera(ScriptAction* a, float dt)
{
    /*if (!a->bStarted)
    {
        a->bStarted = true;
    }
    else
    {
        Vector3 cameraTargetPos = (Vector3) {a->params[0].fValue,a->params[1].fValue ,a->params[2].fValue };
        Vector3 cameraLookTargetPos = (Vector3) { a->params[3].fValue,a->params[4].fValue ,a->params[5].fValue };
        gGame.camera.position = Vector3Lerp(gGame.camera.position, cameraTargetPos, dt * 3);
        gGame.camera.target = Vector3Lerp(gGame.camera.target, cameraLookTargetPos, dt * 3);
        if (Vector3Distance(gGame.camera.position, cameraTargetPos) < 0.01)
            a->bFinished = true;
    }*/
}

static void Script_FadeIn(ScriptAction* a, float dt)
{
    if (!a->bStarted)
    {
        printf("Starting fade-in script.\n");
        a->bStarted = true;
        a->params[1].fValue = 0;
    }
    else
    {
        a->params[1].fValue += dt;
        float progress = a->params[1].fValue / a->params[0].fValue;

        if (progress >= 1)
        {
            printf("Ending fade-in script\n");
            a->bFinished = true;
            gGame.fader.alpha = 0;
            return;
        }

        gGame.fader.alpha = MAX(0,1.0f - progress);
        
    }
}

static void Script_FadeOut(ScriptAction* a, float dt)
{

}

static void Script_PlaySound(ScriptAction* a, float dt)
{
    int sndId = a->params[0].iValue;
    //Resource_PlaySound(sndId);
    a->bFinished = true;
}