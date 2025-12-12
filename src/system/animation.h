#ifndef ANIMATION_H
#define ANIMATION_H

#include <raylib.h>
#include "resource.h"

//animation timing and state info
typedef struct
{
    const char* name;
    unsigned int curFrame;
    unsigned int animIndex;
    unsigned int frameCount;
    int bLoop;
    int bFinished;
    float timer;
    float frameRate;
}CH_AnimationClip;

//The thing that holds all the animation clips and controls the playback of the current.
//Note that it doesn't hold a reference to the model. It only takes it in when updating.
typedef struct
{
    unsigned int animCount;
    unsigned int curAnim;
    CH_AnimationClip* anims;
    ModelAnimation* modelAnimations;
}CH_AnimationController;

void CH_LoadAnimationController(CH_AnimationController* controller, const char* fileName);
void CH_UnloadAnimationController(CH_AnimationController* controller);
void CH_PlayAnimationByName(CH_AnimationController* controller, const char* name);
void CH_PlayAnimationByIndex(CH_AnimationController* controller, const int index);
int CH_AnimationFinished(CH_AnimationController* controller);

void CH_UpdateAnimationController(CH_AnimationController* controller, float dt);
void CH_UpdateModelAnimation(CH_AnimationController* controller, const ModelHandle m);

void CH_SetClipLoopIndex(CH_AnimationController* controller, const int index, int bLoop);
void CH_SetClipLoopName(CH_AnimationController* controller, const char* name, int bLoop);

void CH_UpdateAnimationClip(CH_AnimationClip* anim, float dt);

int CH_GetBoneId(const ModelHandle m, const char* name);
Transform CH_GetBoneTransform(const CH_AnimationController* controller, int boneId);

#endif