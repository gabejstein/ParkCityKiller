#include "script.h"
#include "../gui/dialogueBox.h"
#include <stdbool.h>
#include <raymath.h>
#include "../system/resource.h"
#include "../entity/npc.h"
#include "../entity/entity.h"
#include "../game.h"
#include "../const.h"
#include "../gui/dialogueBox.h"

#define SCRIPT_PATH "assets/scripts/"
#define MAGIC_NUMBER 0x48534F42 //B-O-S-H
#define SCRIPT_VER 1

extern Game gGame;

//script functions.
//could use function pointers instead and store them in an array.
static void Script_MoveCamera(ScriptAction* a, float dt);
static void Script_FadeIn(ScriptAction* a, float dt); //TODO: make both fades one action and allow color to be set.
static void Script_FadeOut(ScriptAction* a, float dt);

static Script* sCurScript = NULL;

static int Script_ActionStart(Script* s, ScriptAction* act, float dt);
static int Script_ActionUpdate(Script* s, ScriptAction* act, float dt);

//Should do text-to-binary conversion
void Script_Compile(const char* filename, Script* s)
{

}

void Script_Save(const char* filename, Script* s)
{
    FILE* f = fopen(filename,"wb"); //TODO: have dedicated directory for this
    if (!f) {
        printf("ERROR: Could not create script file.\n");
        return;
    }

    fwrite(MAGIC_NUMBER, 4, 1, f);
    fwrite(SCRIPT_VER, 4, 1, f);
    fwrite(s->actionsCount, 4, 1, f);
    for (int i = 0; i < s->actionsCount; i++)
    {
        ScriptAction* act = &s->actions[i];
        fwrite(&act->nParamsCount, 4, 1, f);
        for (int j = 0; j < act->nParamsCount; j++)
        {
            Value* p = &act->params[j];
            fwrite(&p->type, 1, 1, f);
            fwrite(&p->IO_Value,1,1,f);
            //TODO: also need to do text size to not waste space
            if (p->type == PARAM_TEXT)
            {
                fwrite(p->text, MAX_SCRIPT_TEXT, 1, f);
            }
        }
    }

    printf("Script successfully saved!\n");

    fclose(f);
}

Script* Script_Load(const char* filename)
{
    Script* s = (Script*)malloc(sizeof(Script));
    if (!s) {
        printf("Could not allocate script.\n");
        return NULL;
    }

    struct Header {
        int magicNum;
        int version;
    };
    struct Header header;

    FILE* f = fopen(filename, "rb"); //TODO: have dedicated directory for this
    if (!f) {
        printf("ERROR: Could not open script file.\n");
        return NULL;
    }

    fread(&header, sizeof(struct Header), 1, f);
    if (header.magicNum != MAGIC_NUMBER)
    {
        printf("Incorrect script header.\n");
        exit(1);
    }

    fread(&s->actionsCount, 4, 1, f);
    for (int i = 0; i < s->actionsCount; i++)
    {
        ScriptAction* act = &s->actions[i];
        fread(act->nParamsCount, 4, 1, f);
        for (int j = 0; j < act->nParamsCount; j++)
        {
            Value* p = &act->params[j];
            fread(&p->type, 1, 1, f);
            switch (p->type)
            {
            case PARAM_INT:
            case PARAM_FLOAT:
            case PARAM_BOOL:
                fread(&p->IO_Value, 1, 1, f);
            }
       
            //TODO: also need to do text size to not waste space
            if (p->type == PARAM_TEXT)
            {
                fread(p->text, MAX_SCRIPT_TEXT, 1, f);
            }
        }
    }

    printf("Script successfully Loaded!\n");

    fclose(f);

    return s;
}

void Script_Reset(Script* script)
{
    script->bFinished = false;
    script->curAction = 0;
    script->timer = 0;
    script->xReg = 0;

    for (int i = 0; i < script->actionsCount; i++)
        script->actions[i].bStarted = script->actions[i].bFinished = false;
}

bool Script_Update(float dt)
{
    Script* s = sCurScript;
    if (!s || s->bFinished) return false;

    ScriptAction* act = &s->actions[sCurScript->curAction];

    if (!act->bStarted)
    {
        Script_ActionStart(s,act, dt);
    }
    else //Note that this causes it to be updated on the next frame. This may not be desirable.
    {
        Script_ActionUpdate(s, act, dt);
    }

    if (act->bFinished)
        if (++s->curAction >= s->actionsCount)
        {
            s->bFinished = true;
            s = NULL;
            return false;
        }
            
    return true;
}

static int Script_ActionStart(Script* s, ScriptAction* act, float dt)
{
    
    switch (act->type)
    {
    case SCRIPT_ACTION_DIALOGUE:  DialogueBox_AddText(act->params[0].text); break;
    case SCRIPT_ACTION_FADE_IN: Script_FadeIn(act, dt); break;
    case SCRIPT_ACTION_FADE_OUT: Script_FadeOut(act, dt); break;
    case SCRIPT_ACTION_MOVE_CAMERA: Script_MoveCamera(act, dt); break;
    case SCRIPT_ACTION_PLAY_SOUND: 
        RES_PlaySound(act->params[0].iValue);
        act->bFinished = true; 
        break;
    case SCRIPT_ACTION_WAIT: 
        printf("Starting wait.\n");
        act->params[1].fValue = 0;
        break;
    case SCRIPT_ACTION_LABEL: act->bFinished = true;  break;
    case SCRIPT_ACTION_JMP: 
        for (int i = 0; i < s->actionsCount; i++)
        {
            if (s->actions[i].type == SCRIPT_ACTION_LABEL && s->actions[i].params[0].iValue == act->params[0].iValue)
                s->curAction = i;
        }
        act->bFinished = true;
        break;
    case SCRIPT_ACTION_ACTIVATE_ENTITY:
        {
            Entity* e = Entity_GetByName(act->params[0].text);
            if (e)
            {
                e->bActive = act->params[1].bValue;

            }
            else
                printf("ERROR: Script accessing name that doesn't exist.");
           
            act->bFinished = true;
            break;
        }
    case SCRIPT_ACTION_BEQ:
        if (s->xReg != act->params[0].iValue)break;
        for (int i = 0; i < s->actionsCount; i++)
        {
            if (s->actions[i].type == SCRIPT_ACTION_LABEL && s->actions[i].params[0].iValue == act->params[1].iValue)
                s->curAction = i;
        }
        act->bFinished = true;
        break;
    case SCRIPT_ACTION_LDX:
        s->xReg = act->params[0].iValue;
        act->bFinished = true;
        break;
    }

    act->bStarted = true;

    return 1;
}

static int Script_ActionUpdate(Script* s, ScriptAction* act, float dt)
{
    switch (act->type)
    {
    case SCRIPT_ACTION_DIALOGUE:    
        DialogueBox_Update(dt);
        act->bFinished = DialogueBox_IsDone(); 
        break;
    case SCRIPT_ACTION_FADE_IN: Script_FadeIn(act, dt); break;
    case SCRIPT_ACTION_FADE_OUT: Script_FadeOut(act, dt); break;
    case SCRIPT_ACTION_MOVE_CAMERA: 
        if (Vector3Distance(gGame.mainCamera.transform.position, gGame.mainCamera.targetPos) < 0.01)
            act->bFinished = true;
            break;
    case SCRIPT_ACTION_WAIT:  
        act->params[1].fValue += dt;
        if (act->params[1].fValue >= act->params[0].fValue)
        {
            act->bFinished = true;
            printf("Wait over\n");
        } 
        break;
   
    }

    return 1;
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

    sCurScript = s;
}



//-----------------------------Script Process Functions---------------------------------

//Going to replace this with a camera_set_target, camera_wait action instead
static void Script_MoveCamera(ScriptAction* a, float dt)
{
    
    gGame.mainCamera.targetPos = (Vector3) { a->params[0].fValue, a->params[1].fValue, a->params[2].fValue };
    gGame.mainCamera.newLookTarget = (Vector3){ a->params[3].fValue,a->params[4].fValue ,a->params[5].fValue };
    
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
    action.params[0].fValue = MAX(0.001, duration); //clamp to prevent divide by zero errors.
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

ScriptAction ActivateEntity(const char* name, bool isActive)
{
    ScriptAction action = (ScriptAction){ .type = SCRIPT_ACTION_ACTIVATE_ENTITY,.nParamsCount = 2 };
    action.params[0].type = PARAM_TEXT;
    strcpy(action.params[0].text, name);
    action.params[1].type = PARAM_BOOL;
    action.params[1].bValue = isActive;
    return action;
}

void AddAction(Script* s, ScriptAction a)
{
    if (s->actionsCount >= MAX_ACTIONS) return;

    s->actions[s->actionsCount++] = a;
}