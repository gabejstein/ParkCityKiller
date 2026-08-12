#pragma once

#include "system/resource.h"
#include "entity/entity.h"
#include "system/transform.h"

typedef enum
{
    CAMERA_STATE_PLAYER,
    CAMERA_STATE_AIM,
    CAMERA_STATE_DEATH,
    CAMERA_STATE_VEHICLE,
    CAMERA_STATE_CUTSCENE,
}CAMERA_STATE;

typedef struct
{
    Camera* camera;
    CH_Transform transform;
    Vector3 followDistance;
    Vector3 targetRot;
    Vector3 targetPos;
    Vector3 newLookTarget;
    float time;
    CAMERA_STATE state;
}GameCamera;

void InitGameCamera(Entity* _player, ModelHandle _levelCollider);
void UpdateGameCamera(float dt);