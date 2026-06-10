#ifndef TRANSFORM_H
#define TRANSFORM_H

#include <raylib.h>

typedef struct CH_Transform
{
    Vector3 position;
    Vector3 rotation;
    Vector3 forward;
    Vector3 right;
    Vector3 up;
    Vector3 scale;
    struct CH_Transform* parent;
}CH_Transform;

#endif
