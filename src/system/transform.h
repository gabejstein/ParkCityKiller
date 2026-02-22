#ifndef TRANSFORM_H
#define TRANSFORM_H

#include <raylib.h>

typedef struct
{
    Vector3 position;
    Vector3 rotation;
    Vector3 forward;
    Vector3 right;
    Vector3 up;
    Vector3 scale;
}CH_Transform;

#endif
