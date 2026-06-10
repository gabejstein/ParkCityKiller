#include "animation.h"
#include "resource.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//Emulate a low frame-rate using this value.
#define FRAME_SKIP 10

void CH_UpdateAnimationClip(CH_AnimationClip* anim, float dt)
{
    if (!anim->bLoop && anim->bFinished) return;

    anim->timer += dt;
    if (anim->timer > anim->frameRate)
    {
        anim->timer = 0;
        anim->curFrame++;

        if (anim->curFrame >= anim->frameCount)
        {
            if (anim->bLoop)
            {
                anim->curFrame = 0;
            }
            else
            {
                anim->bFinished = 1;
                anim->curFrame = anim->frameCount;
            }
        }
        
        //anim->curFrame = (anim->curFrame + 1 ) % anim->frameCount; //Assuming looping for now.
    }
}

void CH_LoadAnimationController(CH_AnimationController* controller, const char* fileName)
{
    controller->modelAnimations = LoadModelAnimations(fileName, &controller->animCount);

    controller->anims = (CH_AnimationClip*) malloc(sizeof(CH_AnimationClip) * controller->animCount);
    memset(controller->anims, 0, sizeof(CH_AnimationClip) * controller->animCount);

    printf("Creating animation controller: %d animations\n", controller->animCount);

    for (int i = 0; i < controller->animCount; i++)
    {
        
        CH_AnimationClip* anim = &controller->anims[i];
        
        anim->frameCount = controller->modelAnimations[i].keyframeCount;
        
        anim->name = controller->modelAnimations[i].name;
        anim->frameRate = 0.008f;
        anim->animIndex = i;
        printf("Successfully created animation clip: %s\n", anim->name);
    }

    controller->curAnim = 3;
}

void CH_UnloadAnimationController(CH_AnimationController* controller)
{
    free(controller->anims);
    UnloadModelAnimations(controller->modelAnimations, controller->animCount);
    
}

void CH_PlayAnimationByName(CH_AnimationController* controller, const char* name)
{
    if (controller->modelAnimations == NULL)
        return;

    for (int i = 0; i < controller->animCount; i++)
    {
        if (strcmp(name, controller->anims[i].name) == 0)
            controller->curAnim = i;
    }
}

void CH_PlayAnimationByIndex(CH_AnimationController* controller, const int index)
{
    if (controller->modelAnimations == NULL)
        return;

    if (controller->curAnim == index)
        return;

    if (index >= 0 && index < controller->animCount)
    {
        controller->curAnim = index;
        controller->anims[controller->curAnim].bFinished = 0; //this doesnt work if multiple anims are set per frame
        controller->anims[controller->curAnim].curFrame = 0;
        controller->anims[controller->curAnim].timer = 0;
    }
        
}

int CH_AnimationFinished(CH_AnimationController* controller)
{
    return controller->anims[controller->curAnim].bFinished;
}

void CH_UpdateAnimationController(CH_AnimationController* controller, float dt)
{
    CH_UpdateAnimationClip(&controller->anims[controller->curAnim], dt);
}

//Currently to only be used just before drawing. May change in the future.
//May have this take in a model handle instead.
void CH_UpdateModelAnimation(CH_AnimationController* controller, const ModelHandle m)
{
    unsigned int curFrame = controller->anims[controller->curAnim].curFrame;
    RES_UpdateModelAnimation(m, controller->modelAnimations[controller->curAnim], curFrame);
}

void CH_SetClipLoopIndex(CH_AnimationController* controller, const int index, int bLoop)
{
    controller->anims[index].bLoop = bLoop;
}

void CH_SetClipLoopName(CH_AnimationController* controller, const char* name, int bLoop)
{
    for (int i = 0; i < controller->animCount; i++)
    {
        if (strcmp(name, controller->anims[i].name) == 0)
            controller->anims[i].bLoop = bLoop;
    }
}

//TODO: consider moving this to the resources module instead.
//the model's skeleton is now part of the model.
int CH_GetBoneId(const ModelHandle m, const char* name)
{
    Model* model = RES_GetModel(m);
    ModelSkeleton skeleton = model->skeleton;
    for (int i = 0; i < skeleton.boneCount; i++)
    {
        if (strcmp(skeleton.bones[i].name, name) == 0)
            return i;
    }
    return -1;
}

Transform CH_GetBoneTransform(const CH_AnimationController* controller, int boneId)
{
    ModelAnimation* anim = &controller->modelAnimations[controller->curAnim];
    unsigned int curFrame = controller->anims[controller->curAnim].curFrame;

    if (curFrame >= anim->keyframeCount)
        curFrame = anim->keyframeCount - 1;

    Transform transform = anim->keyframePoses[curFrame][boneId];

    return transform;
}