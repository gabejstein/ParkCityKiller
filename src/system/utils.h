#ifndef UTILS_H
#define UTILS_H

#include <raylib.h>

float GetRandomFloat(float min, float max);
int CheckPointInBox(const Vector3* point, const BoundingBox* box);

#endif
